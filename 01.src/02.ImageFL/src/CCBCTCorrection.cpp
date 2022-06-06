/*
 * CCBCTCorrection.cpp
 *
 *  Created on: 2021/04/12
 *      Author: 10097663
 */

#include "CCBCTCorrection.h"
#include "COverlapCorrection.h"

#include <math.h>
#include <iostream>
#include <vector>
using namespace std;

#include "imageData.h"
#include "CTDAT.h"
#include "CTTMA.h"
#include "CSIMARSF.h"

#define DCORRECT_LEN	10				//段差を左右それぞれ何画素補正するか
#define STEPCORRECT_SUBMED_SIZE	67		//段差補正の副走査メディアンフィルタのサイズ
#define INPUT_MAX_QL	60000			//入力最大QL値
#define LR_DIFF_QL		3				//段差補正時の右端と左端のQL差の最大値パーセント
#define PROFILE_DIFF_QL	300				//段差補正時にピーク判別するときの認識QL

CCBCTCorrection::CCBCTCorrection() {

}

CCBCTCorrection::CCBCTCorrection(SCBCTCorrectData in_SCBCTCorrectData){

	m_SCBCTCorrectData = in_SCBCTCorrectData;

}

CCBCTCorrection::~CCBCTCorrection() {
	// TODO Auto-generated destructor stub
}
void CCBCTCorrection::SetCorrectData(SCBCTCorrectData in_SCBCTCorrectData){

	m_SCBCTCorrectData = in_SCBCTCorrectData;

}
//欠陥補正データ生成
void CCBCTCorrection::MakeDefectMap(string inBetaFile, string outDefectMapFile){

	TTMA BetaImg;
	TTMA DefectImg;
	TTMA MedianImg;

	const int OffsetQL = 1000;
	const int DefectThre = 2000;

	BetaImg.ReadTma(inBetaFile);
	MedianImg = BetaImg;
	MedianImg.Median(3,1);		//欠陥を消すためにメディアンをかける
	DefectImg = BetaImg;

//	MedianImg.SaveTmaTdt("tempImg/median33.tma");

	for(int i=0;i<BetaImg.SubPixel;i++){
		for(int j=0;j<BetaImg.MainPixel;j++){
			//欠陥だけを残すために、メディアンを掛けたものから元画像を引く
			BetaImg.Dt[i][j] = OffsetQL + MedianImg.Dt[i][j] - BetaImg.Dt[i][j];
		}
	}


//	BetaImg.SaveTmaTdt("tempImg/beta33.tma");

	for(int i=0;i<BetaImg.SubPixel;i++){
		for(int j=0;j<BetaImg.MainPixel;j++){
			if (BetaImg.Dt[i][j] > (OffsetQL + DefectThre)){
				DefectImg.Dt[i][j] = BetaImg.DtMax;
			} else {
				DefectImg.Dt[i][j] = 0;
			}
		}
	}

	DefectImg.SaveTmaTdt(outDefectMapFile);


}

//全補正処理実施
bool CCBCTCorrection::CorrectImg(bool *bCorrect){

	if (!loadImage()){					//画像読込
		printf("loadImage Error!!!\n");
		return false;
	}

	if (!exeOffsetAndGain(bCorrect[0], bCorrect[1])){			//オフセット＆ゲイン補正
		printf("Offset and Gain Correction Error!!!\n");
		return false;
	}


	if (!correctDefectImage(bCorrect[2])){			//欠陥補正
		printf("Defect Correction Error!!!\n");
		return false;
	}


	if (!RotateImage()){				//右画像回転(180°回転)
		printf("Rotate Right Image Error!!!\n");
	}

	if (!bCorrect[3]){		//つなぎ処理、トリミングをしない場合

		string tmpOutFileName = m_SCBCTCorrectData.OutFileName;
		int findTma = tmpOutFileName.find(".tma");

		if (findTma != string::npos){
			tmpOutFileName.erase(findTma);
		}

		string OutL_FileName = tmpOutFileName + "_L.tma";
		string OutR_FileName = tmpOutFileName + "_R.tma";

		m_LCorrectImg.SaveTmaTdt(OutL_FileName);
		m_RCorrectImg.SaveTmaTdt(OutR_FileName);


		return true;
	}

	SOverlapCorrectParam overlapParam;
	overlapParam.diffAngle = m_SCBCTCorrectData.diffAngle;
	overlapParam.diffStartPos = m_SCBCTCorrectData.diffStartPos;
	overlapParam.lowP_Mag = m_SCBCTCorrectData.lowP_Mag;
	overlapParam.overlayAngle = m_SCBCTCorrectData.overlayAngle;
	overlapParam.overlayX = m_SCBCTCorrectData.overlayX;
	overlapParam.overlayY = m_SCBCTCorrectData.overlayY;

	COverlapCorrection overlapCorrection(overlapParam);
	overlapCorrection.ExecuteOverlap(m_LCorrectImg, m_RCorrectImg, m_CombinationImg);


	//右側画像左端の拡大
//	scaleRImage();

//	m_LCorrectImg.SaveTmaTdt("MiddleCorrectL.tma");
//	m_RCorrectImg.SaveTmaTdt("MiddleCorrectR.tma");

	if (!adjustOverlapSensitivity()){	//オーバラップ部分の感度補正
//	if (!adjustOverlapSensitivity2()){	//オーバラップ部分の感度補正
		printf("adujst Overlap Sensitivity Error!!!\n");
		return false;
	}

//	if (!correctStepImage3()){			//段差補正
//	if (!correctStepImage2()){			//段差補正
	if (!correctStepImage()){			//段差補正
		printf("Step Correction Error!!!\n");
		return false;
	}

	if (!overlapImage()){				//オーバラップ処理
		printf("Overlap Image Error!!!\n");
		return false;
	}


	if (!trimmingImage()){				//トリミング処理
		printf("trimming Image Error!!!\n");
		return false;
	}

	return true;
}

//処理対象画像の読み込み
bool CCBCTCorrection::loadImage(){

	bool bRet = true;

	bRet = m_LCorrectImg.ReadTma(m_SCBCTCorrectData.L_InFileName);

	if (!bRet){
		return false;
	}

	m_LCorrectImg.Div(2,2);

	bRet = m_RCorrectImg.ReadTma(m_SCBCTCorrectData.R_InFileName);

	m_RCorrectImg.Div(2,2);

	return bRet;
}

//右側画像回転処理
bool CCBCTCorrection::RotateImage(){

	m_RCorrectImg.Rotate(180);

	return true;
}

//オフセット&ゲイン補正
bool CCBCTCorrection::exeOffsetAndGain(bool bOffset, bool bGain){

	TTMA offsetImg;
	TTMA gainImg;
	TTMA *pCorrectImg;

	string offsetFilename;
	string gainFilename;
	float sensitivity;

	for(int h=0;h<2;h++){
		switch (h){
		case 0:			//左側を行う
			offsetFilename = m_SCBCTCorrectData.L_OffsetFileName;
			gainFilename = m_SCBCTCorrectData.L_GainFileName;
			pCorrectImg = &m_LCorrectImg;
			sensitivity = 4 /m_SCBCTCorrectData.LSensitivity;
			break;
		case 1:			//右側を行う
			offsetFilename = m_SCBCTCorrectData.R_OffsetFileName;
			gainFilename = m_SCBCTCorrectData.R_GainFileName;
			pCorrectImg = &m_RCorrectImg;
			sensitivity = 4 / m_SCBCTCorrectData.RSensitivity;
			break;
		}


		if (!offsetImg.ReadTma(offsetFilename)){
			return false;
		}

		if (m_SCBCTCorrectData.bOffsetOrgSize){		//オフセットが縮小前の場合には縮小する
			offsetImg.Div(2,2);
		}

		if (!gainImg.ReadTma(gainFilename)){
			return false;
		}

		if (h==1){
			offsetImg.SaveTmaTdt("tempImg/offsetR.tma");
			gainImg.SaveTmaTdt("tempImg/GainR.tma");
			pCorrectImg->SaveTmaTdt("tempImg/OrgImgR.tma");
			printf("Sensitivity:%3.4lf\n",sensitivity);
		}

		for(int i=0;i<pCorrectImg->SubPixel;i++){
			for(int j=0;j<pCorrectImg->MainPixel;j++){
				float tmpGain = (float)gainImg.Dt[i][j]/8192 * sensitivity;
				int offsetData = bOffset ? offsetImg.Dt[i][j] : 0;
				//int tmpData = pCorrectImg->Dt[i][j] - offsetImg.Dt[i][j];
				int tmpData = pCorrectImg->Dt[i][j] - offsetData;
				if (tmpData > INPUT_MAX_QL){
					pCorrectImg->Dt[i][j] = INPUT_MAX_QL;
				}else {
					if (bGain){
						tmpData *= tmpGain;
					}
					if (tmpData > INPUT_MAX_QL){
						pCorrectImg->Dt[i][j] = INPUT_MAX_QL;
					} else if (tmpData < 0){
						pCorrectImg->Dt[i][j] = 0;
					} else {
						pCorrectImg->Dt[i][j] = tmpData;
					}
				}
			}
		}
	}

	m_LCorrectImg.SaveTmaTdt("tempImg/GainOffsetL.tma");
	m_RCorrectImg.SaveTmaTdt("tempImg/GainOffsetR.tma");

	return true;
}

//欠陥補正処理
bool CCBCTCorrection::correctDefectImage(bool defect){

	if (!defect){
		return true;
	}

	TTMA DefImg;
	TTMA *pCorrectImg;

	string defFilename;

	bool bRet = true;

	for(int i=0;i<2;i++){
		switch (i){
		case 0:			//左側を行う
			defFilename = m_SCBCTCorrectData.L_DefFileName;
			pCorrectImg = &m_LCorrectImg;
			break;
		case 1:			//右側を行う
			defFilename = m_SCBCTCorrectData.R_DefFileName;
			pCorrectImg = &m_RCorrectImg;
			break;
		}

		if (!DefImg.ReadTma(defFilename)){		//欠陥マップのロード
			return false;
		}

		//線欠陥補正
		vector<CSimars::SLineDefect> lineDefect;
		CSimars::SLineDefect addLineDefData;
		int defLineWidth = 0;
		for(int i=0;i<pCorrectImg->SubPixel;i++){
			bool bLineDefect = true;
			for(int j=0;j<10;j++){	//10連続で欠陥があれば、線欠陥とみなす
				if (DefImg.Dt[i][j] == 0){
					bLineDefect = false;
				}
			}
			if (bLineDefect){
				defLineWidth++;
				if (defLineWidth == 1){
					addLineDefData.type = 2;	//横タイプ
					addLineDefData.lineNum = i;
				}
			} else {
				if (defLineWidth > 0 ){
					//printf("YOKO %d LineDef Width:%d\n",i,defLineWidth);
					addLineDefData.width = defLineWidth;
					lineDefect.push_back(addLineDefData);
					defLineWidth = 0;
				}
			}
			if (i == pCorrectImg->SubPixel-1 && defLineWidth > 0){		//最終ラインまで線欠陥があった場合の対応
				addLineDefData.width = defLineWidth;
				lineDefect.push_back(addLineDefData);
				defLineWidth = 0;
			}
		}

		for(int j=0;j<pCorrectImg->SubPixel;j++){
			bool bLineDefect = true;
			for(int i=0;i<10;i++){	//10連続で欠陥があれば、線欠陥とみなす
				if (DefImg.Dt[i][j] == 0){
					bLineDefect = false;
				}
			}
			if (bLineDefect){
				defLineWidth++;
				if (defLineWidth == 1){
					addLineDefData.type = 1;	//縦タイプ
					addLineDefData.lineNum = j;
				}
			} else {
				if (defLineWidth > 0 ){
					//printf("TATE %d LineDef Width:%d\n",j, defLineWidth);
					addLineDefData.width = defLineWidth;
					lineDefect.push_back(addLineDefData);
					defLineWidth = 0;
				}
			}
			if (i == pCorrectImg->SubPixel-1 && defLineWidth > 0){		//最終ラインまで線欠陥があった場合の対応
				addLineDefData.width = defLineWidth;
				lineDefect.push_back(addLineDefData);
				defLineWidth = 0;
			}
		}

		CSimars simarsObj(pCorrectImg);
		int iGD = 0;
		float fHon = 0;
		double dA1 = 0.0;
		double dA2 = 0.0;
		double dA3 = 0.0;
		double dA4 = 0.0;
		double dTh = 4.0;
		simarsObj.bDefectCorrection6a(lineDefect, false, iGD, fHon, dA1, dA2, dA3, dA4,dTh, false);

		//点欠陥補正
		//最初に線欠陥については正常画素として置換する。
		for(int i=0;i<lineDefect.size();i++){
			if (lineDefect[i].type == 1){			//縦タイプ
				for(int j=0;j<DefImg.SubPixel;j++){
					for(int k=0;k<lineDefect[i].width;k++){	//太さ対応
						DefImg.Dt[j][lineDefect[i].lineNum + k] = 0;
					}
				}
			} else if (lineDefect[i].type == 2){	//横タイプ
				for(int j=0;j<DefImg.MainPixel;j++){
					for(int k=0;k<lineDefect[i].width;k++){	//太さ対応
						DefImg.Dt[lineDefect[i].lineNum + k][j] = 0;
					}
				}
			}
		}

		simarsObj.bDefectCorrection7(DefImg, 2, false, 0, 0, false);

	}

	return true;
}
//右側画像左端拡大処理
bool CCBCTCorrection::scaleRImage(){

	//段差位置の右端を求める
	float diffStart = m_SCBCTCorrectData.diffStartPos;
	float diffEnd = diffStart + sin(m_SCBCTCorrectData.overlayAngle * M_PI / 180.0);
	int startMag = (diffStart > diffEnd) ? diffEnd : diffStart;		//拡大開始点は段差の右端(小さいほう)

	//オーバーラップ開始点を求める
	int startOverlap = m_RCorrectImg.MainPixel - m_SCBCTCorrectData.overlayX;

	if (startOverlap <= startMag){		//オーバラップ開始点よりも段差位置が左にあるのはあり得ない！！！
		printf("scaleRImage() ERROR!!!\n");
		printf("startOverlap:%d  startMag:%d\n", startOverlap, startMag);
		return false;
	}

	TTMA OverlapImgObj = m_RCorrectImg;
	TTMA MedImgObj = m_RCorrectImg;

	OverlapImgObj.Format(m_RCorrectImg.SubPixel, (int)m_SCBCTCorrectData.overlayX,
			0, (int)m_SCBCTCorrectData.overlayX - m_RCorrectImg.MainPixel);
	MedImgObj.Format(m_RCorrectImg.SubPixel, startOverlap - startMag,
			0, 0 - startMag);

	//切り取った画像を拡大する
	float lowP_mag = m_SCBCTCorrectData.lowP_Mag;
	OverlapImgObj.Scaling(OverlapImgObj.MainPixel/2, OverlapImgObj.SubPixel/2, 1.0, lowP_mag, 0);
	MedImgObj.ScalingLR(lowP_mag, 1.0, 0);

//	OverlapImgObj.SaveTmaTdt("overlapimg.tma");			//TODO
//	MedImgObj.SaveTmaTdt("medImg.tma");

	//拡大画像に置き換える
	m_RCorrectImg.CopyFormat(0, startOverlap, OverlapImgObj, OverlapImgObj.SubPixel, OverlapImgObj.MainPixel, 0, 0);
	m_RCorrectImg.CopyFormat(0, startMag, MedImgObj, MedImgObj.SubPixel, MedImgObj.MainPixel, 0, 0);

//	m_RCorrectImg.SaveTmaTdt("afterR.tma");

	return true;
}
//段差補正処理
bool CCBCTCorrection::correctStepImage(){

	TTMA RAveimgObj = m_RCorrectImg;

	//段差部分を切り出して、副走査方向にメディアンをかける
	RAveimgObj.Format(m_RCorrectImg.SubPixel, DCORRECT_LEN*2+1, 0, -1*((int)m_SCBCTCorrectData.diffStartPos-DCORRECT_LEN),0);
	RAveimgObj.Median(STEPCORRECT_SUBMED_SIZE,1);

	TTMA DiffImgObj = RAveimgObj;

	//TODO 参考のため、高周波成分を抽出。実際には不要。
	for(int i=0;i < DiffImgObj.SubPixel;i++){
		for(int j=0;j < DiffImgObj.MainPixel;j++){
			DiffImgObj.Dt[i][j] = 10000 * ((float)RAveimgObj.Dt[i][j] / m_RCorrectImg.Dt[ i ][j+ ((int)m_SCBCTCorrectData.diffStartPos-DCORRECT_LEN)]);
		}
	}
	DiffImgObj.SaveTmaTdt("tempImg/DiffStep.tma");

	//開始と終了を結んだ直線を理想的なQL値として、低周波数成分において、その差分を補正する。
	for(int i=0; i < DiffImgObj.SubPixel; i++){
		int StartQL = RAveimgObj.Dt[i][0];
		int EndQL = RAveimgObj.Dt[i][DiffImgObj.MainPixel-1];
		float deltaQL = ((float)EndQL - StartQL)/(2*DCORRECT_LEN);		//1画素ずれた時のΔQL
		for(int j=0; j < DiffImgObj.MainPixel; j++){
			float idealQL = StartQL + deltaQL * j;
			float ratio = idealQL /RAveimgObj.Dt[i][j];
			m_RCorrectImg.Dt[i][j+ ((int)m_SCBCTCorrectData.diffStartPos-DCORRECT_LEN)] *= ratio;
		}
	}

	m_LCorrectImg.SaveTmaTdt("tempImg/resL.tma");			//TODO
	m_RCorrectImg.SaveTmaTdt("tempImg/resR.tma");			//TODO

	RAveimgObj.SaveTmaTdt("tempImg/ave.tma");

	return true;
}
//段差補正処理2
bool CCBCTCorrection::correctStepImage2(){

	TTMA RAveimgObj = m_RCorrectImg;

	//段差部分を切り出して、副走査方向にメディアンをかける
	RAveimgObj.Format(m_RCorrectImg.SubPixel, DCORRECT_LEN*2+1, 0, -1*((int)m_SCBCTCorrectData.diffStartPos-DCORRECT_LEN),0);
	TTMA DiffImgObj = RAveimgObj;

	RAveimgObj.Median(STEPCORRECT_SUBMED_SIZE,1);

	DiffImgObj.SaveTmaTdt("tempImg/DiffStepB.tma");

	//高周波成分を抽出。実際には不要。
	for(int i=0;i < DiffImgObj.SubPixel;i++){
		for(int j=0;j < DiffImgObj.MainPixel;j++){
			//DiffImgObj.Dt[i][j] = 10000 * ((float)RAveimgObj.Dt[i][j] / m_RCorrectImg.Dt[ i ][j+ ((int)m_SCBCTCorrectData.diffStartPos-DCORRECT_LEN)]);
			int QL = 10000 + DiffImgObj.Dt[i][j] - RAveimgObj.Dt[i][j];
			if (QL > 65535){
				DiffImgObj.Dt[i][j] = 65535;
			} else if (QL < 0){
				DiffImgObj.Dt[i][j] = 0;
			} else {
				DiffImgObj.Dt[i][j] = QL;
			}
		}
	}
	DiffImgObj.SaveTmaTdt("tempImg/DiffStep.tma");

	//開始と終了を結んだ直線を理想的なQL値として、低周波数成分において、その差分を補正する。
	for(int i=0; i < DiffImgObj.SubPixel; i++){
		int StartQL = RAveimgObj.Dt[i][0];
		int EndQL = RAveimgObj.Dt[i][DiffImgObj.MainPixel-1];
		float deltaQL = ((float)EndQL - StartQL)/(2*DCORRECT_LEN);		//1画素ずれた時のΔQL
		for(int j=0; j < DiffImgObj.MainPixel; j++){
			float idealQL = StartQL + deltaQL * j;
			int outQL = idealQL + DiffImgObj.Dt[i][j] - 10000;
			if (outQL > 65535){
				outQL = 65535;
			} else if (outQL < 0){
				outQL = 0;
			}
			m_RCorrectImg.Dt[i][j+ ((int)m_SCBCTCorrectData.diffStartPos-DCORRECT_LEN)] = outQL;
		}
	}

	m_LCorrectImg.SaveTmaTdt("tempImg/resL.tma");			//TODO
	m_RCorrectImg.SaveTmaTdt("tempImg/resR.tma");			//TODO

	RAveimgObj.SaveTmaTdt("tempImg/ave.tma");

	return true;

}
//段差補正処理3
bool CCBCTCorrection::correctStepImage3(){

	TTMA RMeadianImg = m_RCorrectImg;

	//段差部分を切り出して、副走査方向にメディアンをかける
	RMeadianImg.Format(m_RCorrectImg.SubPixel, DCORRECT_LEN*2+1, 0, -1*((int)m_SCBCTCorrectData.diffStartPos-DCORRECT_LEN),0);
	TTMA DiffImgObj = RMeadianImg;
	TTMA AveImgOrg = RMeadianImg;
	TTMA AveImgObj = RMeadianImg;

	RMeadianImg.Median(STEPCORRECT_SUBMED_SIZE,1);

	RMeadianImg.SaveTmaTdt("tempImg/DiffStepB.tma");

	vector<bool>	vbUseDiffSub(DiffImgObj.SubPixel, false);		//プロファイルを使用するかフラグ
	for(int i=0; i<RMeadianImg.SubPixel; i++){
		float LRRatio = (float)RMeadianImg.Dt[i][0] / RMeadianImg.Dt[i][DiffImgObj.MainPixel-1];
		//左右差が大きすぎる場合は、不採用
		if (LRRatio*100 > (float)(100 + LR_DIFF_QL) || LRRatio*100 < (float)(100 - LR_DIFF_QL)){
			vbUseDiffSub[i] = false;
		} else {	//プロファイルに山が2つ以上ある場合は不採用
			bool bCheckStart = false;
			bool bDetectPeak = false;
			bool bUp = false;			//増加傾向か？
			bool bRes = true;			//最終結果
			for(int j=1; j<RMeadianImg.MainPixel; j++){
				//PROFILE_DIFF_QL
				int DiffQL = RMeadianImg.Dt[i][j]-RMeadianImg.Dt[i][j-1];
				if (DiffQL > PROFILE_DIFF_QL){
					if (bCheckStart == false){
						bCheckStart = true;
						bUp = true;
					} else if (bDetectPeak == true){
						if  (bUp == false){
							bRes = false;
							break;
						}
					} else {
						if (bUp == false){
							bDetectPeak = true;
							bUp = true;
						}
					}
				} else if (DiffQL < -1 * PROFILE_DIFF_QL){
					if (bCheckStart == false){
						bCheckStart = true;
						bUp = false;
					} else if (bDetectPeak == true){
						if  (bUp == true){
							bRes = false;
							break;
						}
					} else {
						if (bUp == true){
							bDetectPeak = true;
							bUp = false;
						}
					}
				}
			}
			vbUseDiffSub[i] = bRes;
		}
	}

	TTMA useDiffSub = RMeadianImg;
	for(int i=0;i<useDiffSub.SubPixel;i++){
		for(int j=0;j<useDiffSub.MainPixel;j++){
			if (vbUseDiffSub[i] == true){
				useDiffSub.Dt[i][j] = 30000;
			} else {
				useDiffSub.Dt[i][j] = 0;
			}
		}
	}
	useDiffSub.SaveTmaTdt("tempImg/useDiffSub.tma");


	int useProfileSub = 0;
	for(int i=0;i < DiffImgObj.SubPixel;i++){		//TODO	vBUseDiffSub[]がすべてfalseだった場合は？？
		if (vbUseDiffSub[i] == true){
			useProfileSub = i;
			break;
		}
	}
	//高周波成分を抽出。
	for(int i=0;i < DiffImgObj.SubPixel;i++){
		if (vbUseDiffSub[i] == true){
			useProfileSub = i;
		}
		//useProfileSub = 100;
		for(int j=0;j < DiffImgObj.MainPixel;j++){
			int QL = 30000 + DiffImgObj.Dt[i][j] - RMeadianImg.Dt[useProfileSub][j];
			//int QL = 10000 + DiffImgObj.Dt[i][j] - RMeadianImg.Dt[i][j];
			if (QL > 65535){
				DiffImgObj.Dt[i][j] = 65535;
			} else if (QL < 0){
				DiffImgObj.Dt[i][j] = 0;
			} else {
				DiffImgObj.Dt[i][j] = QL;
			}
		}
	}
	DiffImgObj.SaveTmaTdt("tempImg/DiffStep.tma");



	//副走査方向に平均をとる
	for(int j=0;j < AveImgOrg.MainPixel;j++){
		int SumQL = 0;			//加算QL
		int SumNum = 0;			//加算数
		for(int i=0; i < AveImgOrg.SubPixel; i++){
			if (i <= AveImgOrg.SubPixel -1 ){
				SumQL += AveImgOrg.Dt[i][j];
				SumNum++;
			}
			if (i-DCORRECT_LEN >= 0){
				SumQL -= AveImgOrg.Dt[i-DCORRECT_LEN][j];
				SumNum--;
			}
			AveImgObj.Dt[i][j] = SumQL/SumNum;
		}
	}

	AveImgObj.SaveTmaTdt("tempImg/DiffStepAve.tma");



	for(int i=0;i < DiffImgObj.SubPixel;i++){
		if (vbUseDiffSub[i] == true){
			useProfileSub = i;
			break;
		}
	}
	//開始と終了を結んだ直線を理想的なQL値として、低周波数成分において、その差分を補正する。
	for(int i=0; i < DiffImgObj.SubPixel; i++){
		if (vbUseDiffSub[i] == true){
			useProfileSub = i;
		}
		//useProfileSub = 100;
		int StartQL = RMeadianImg.Dt[useProfileSub][0];
		int EndQL = RMeadianImg.Dt[useProfileSub][DiffImgObj.MainPixel-1];
		float deltaQL = ((float)EndQL - StartQL)/(2*DCORRECT_LEN);		//1画素ずれた時のΔQL
		for(int j=0; j < DiffImgObj.MainPixel; j++){
			float idealQL = StartQL + deltaQL * j;
			int outQL = idealQL + DiffImgObj.Dt[i][j] - 30000;
			if (outQL > 65535){
				outQL = 65535;
			} else if (outQL < 0){
				outQL = 0;
			}
			m_RCorrectImg.Dt[i][j+ ((int)m_SCBCTCorrectData.diffStartPos-DCORRECT_LEN)] = outQL;
		}
	}

	m_LCorrectImg.SaveTmaTdt("tempImg/resL.tma");			//TODO
	m_RCorrectImg.SaveTmaTdt("tempImg/resR.tma");			//TODO

	RMeadianImg.SaveTmaTdt("tempImg/ave.tma");

	return true;

}
//シンチ段差補正
bool CCBCTCorrection::correctSintiEdge(){


	return true;
}
/*	つなぎ部感度調整
 *	[概要]
 *
 *	重なり部のQL比をとり、主方向にメディアンを取り、副走査ライン毎の感度補正量を求める。
 */
bool CCBCTCorrection::adjustOverlapSensitivity(){

	TTMA rotRImg = m_RCorrectImg;

	//回転させて、左画像と比較できるようにする
//	rotRImg.RotateAnyAngle(m_SCBCTCorrectData.overlayAngle, m_RCorrectImg.MainPixel-1, 0);
//	rotRImg.SaveTmaTdt("tempImg/rotRImg.tma");

	TTMA diffImg(m_RCorrectImg.SubPixel, m_RCorrectImg.MainPixel - m_SCBCTCorrectData.diffStartPos+1, 16);	//重なり部分のQL比の10000倍値

	//右画像と左画像のQL比をとり、diffImgへ代入(値は10000倍して保持)
	float tempData;
	for(int i=0;i<diffImg.SubPixel;i++){
		for(int j=0;j<diffImg.MainPixel;j++){
			int Rimg_X = (int)(m_SCBCTCorrectData.diffStartPos + j);
			int Rimg_Y = i;
			int Limg_X = j -(m_RCorrectImg.MainPixel - (int)m_SCBCTCorrectData.diffStartPos - m_SCBCTCorrectData.overlayX);
			int Limg_Y = i - m_SCBCTCorrectData.overlayY;
//			int Limg_Y = m_SCBCTCorrectData.overlayY + i;
			if (m_RCorrectImg.bCheckPos(Rimg_Y, Rimg_X) && m_LCorrectImg.bCheckPos(Limg_X,Limg_Y) && m_LCorrectImg.Dt[Limg_Y][Limg_X] != 0){
				tempData = (float)m_RCorrectImg.Dt[Rimg_Y][Rimg_X] / m_LCorrectImg.Dt[Limg_Y][Limg_X];
			} else {
				tempData = 1.0;
			}
			diffImg.Dt[i][j] = tempData * 10000;
		}
	}


	diffImg.SaveTmaTdt("tempImg/diff.tma");

	//主走査方向へメディアンをかける
	diffImg.Median(1,5);

	int offsetMain = (m_RCorrectImg.MainPixel - (int)m_SCBCTCorrectData.diffStartPos - m_SCBCTCorrectData.overlayX);
	printf("diffImgMain:%d offsetMain:%d\n",diffImg.MainPixel,offsetMain);
	for(int i=0;i<diffImg.SubPixel;i++){
		//ライン毎の感度を決める。
		int Sum = 0;
		for(int j=0;j < diffImg.MainPixel - offsetMain;j++){
			Sum += diffImg.Dt[i][offsetMain+j];
		}
		float tempSens = (float)Sum/(diffImg.MainPixel - offsetMain) / 10000;
//		printf("tempSens: %04d %3.3lf\n",i,tempSens);
		tempSens = 1 / tempSens;
		//重なり＆単独右画像部のデータを書き換える
		//int calcStartMain = getDiffCenterPos(m_SCBCTCorrectData.diffAngle, m_SCBCTCorrectData.diffStartPos, i) + 2;
		int calcStartMain = m_SCBCTCorrectData.diffStartPos + 2;
		if (i %100 == 0){
			//printf("i:%d tempSens:%lf\n",i, tempSens);
		}
		for(int j=calcStartMain; j < m_RCorrectImg.MainPixel; j++){
			m_RCorrectImg.Dt[i][j] = m_RCorrectImg.Dt[i][j] * tempSens;
		}
	}

	//m_RCorrectImg.SaveTmaTdt("tempImg/R_SensC.tma");

	return true;
}
/*	つなぎ部感度調整2
 *	[概要]
 *
 *
 */
bool CCBCTCorrection::adjustOverlapSensitivity2(){

	TTMA rotRImg = m_RCorrectImg;


	vector<int>	R_QL;	//右側のQL値
	vector<int> L_QL;	//左側のQL値

	TTMA RMedImg = rotRImg;
	TTMA LMedImg = m_LCorrectImg;

//	RMedImg.Median(3,3);		//3*3メディアンをかける
//	LMedImg.Median(3,3);

	const int startCheckPos = 0;
	const int endCheckPos = 10;

	FILE *fp;
//	fp = fopen("QLSET.txt","w");

	float tempData;
	for(int j=startCheckPos;j<endCheckPos;j++){
	//for(int j=3;j<m_SCBCTCorrectData.overlayX;j++){

		for(int i=10;i<RMedImg.SubPixel-10;i++){
			int R_Ypos = i+ m_SCBCTCorrectData.overlayY;
			int R_Xpos = RMedImg.MainPixel-m_SCBCTCorrectData.overlayX+j;
			if (LMedImg.bCheckPos(i,j) && RMedImg.bCheckPos(R_Ypos,R_Xpos)){
				if ((float)RMedImg.Dt[R_Ypos][R_Xpos]/LMedImg.Dt[i][j] < 2.0 && (float)RMedImg.Dt[R_Ypos][R_Xpos]/LMedImg.Dt[i][j] > 0.5){
					R_QL.push_back(RMedImg.Dt[R_Ypos][R_Xpos]);
					L_QL.push_back(LMedImg.Dt[i][j]);
					//fprintf(fp,"%d, %d, %d, %d, %5.3f\n",i, j, RMedImg.Dt[R_Ypos][R_Xpos],LMedImg.Dt[i][j], (float)RMedImg.Dt[R_Ypos][R_Xpos]/LMedImg.Dt[i][j]);
				}
			}
		}
	}

//	fclose(fp);

	//画像の位置が一致しているかの確認用
#if 0
	TTMA compImg = m_LCorrectImg;
	for(int i=0;i<compImg.SubPixel;i++){
		for(int j=0;j<compImg.MainPixel;j++){
			compImg.Dt[i][j] = 0;
		}
	}
	for(int i=10;i<RMedImg.SubPixel-10;i++){
		for(int j=5;j<m_SCBCTCorrectData.overlayX;j++){
			int R_Ypos = i+ m_SCBCTCorrectData.overlayY;
			int R_Xpos = RMedImg.MainPixel-m_SCBCTCorrectData.overlayX+j;
			if (LMedImg.bCheckPos(i,j) && RMedImg.bCheckPos(R_Ypos,R_Xpos)){
				compImg.Dt[i][j+m_SCBCTCorrectData.overlayX] = LMedImg.Dt[i][j];
				compImg.Dt[i][j] = RMedImg.Dt[R_Ypos][R_Xpos];
			}
		}
	}
	compImg.SaveTmaTdt("compLR.tma");
#endif


	float a,b,a0,b0;
	float avgX = 0;
	float avgY = 0;
	int addNum = 0;
	for(int i=0;i<R_QL.size();i++){
		avgX += R_QL[i];
		avgY += L_QL[i];
		addNum++;
	}
	avgX /= addNum;
	avgY /= addNum;

	printf("avgX:%lf avgY:%lf\n",avgX,avgY);

	float SumDenom = 0;		//分母
	float SumNumera = 0;	//分子
	for(int i=0;i<R_QL.size();i++){
		SumDenom += (R_QL[i] - avgX)*(R_QL[i] - avgX);
		SumNumera += (R_QL[i] - avgX)*(L_QL[i] - avgY);
	}

	a0 = SumNumera / SumDenom;
	b0 = avgY - a0 * avgX;
	a = a0;
	b = b0;

	printf("a:%6.3f b:%6.3f\n",a0,b0);
	//↓お試し
	//a = 1.111;
	//b = -6387 + 1000;



	float angleR = m_SCBCTCorrectData.diffAngle / 180 * M_PI;		//回転角度(ラジアン)

	int offsetMain = m_SCBCTCorrectData.diffStartPos -2 ;
//		int offsetMain = m_SCBCTCorrectData.diffStartPos + 1 + i * tanf(angleR);
		//printf("offset:%d\n",offsetMain);

	float theta = atanf(a);
	int N = m_RCorrectImg.MainPixel - m_SCBCTCorrectData.diffStartPos
			- m_SCBCTCorrectData.overlayX + startCheckPos+1;		//何分割するか
	printf("theta:%5.3lf   N:%d\n",atanf(a),N);


	for(int j=offsetMain; j < m_RCorrectImg.MainPixel; j++){
		//aa,bbを再計算する。
		float X,Y;		//y=xとy=ax+bの交点
		X = b / (1-a);	//XとYは同じ値なので、片方だけ計算

		float theta_i = (theta - M_PI*0.25) / N * (j - offsetMain) + M_PI*0.25;
		float aa = tanf(theta_i);
		float bb = X*(1 - aa);

		//修正
		aa = a;
		bb = b;

		if (j%20 == 0){
			printf("j:%d  aa:%5.3lf  b:%5.3lf theta_i:%5.3lf\n",j,aa,bb,theta_i);
		}

		for(int i=0;i<m_RCorrectImg.SubPixel;i++){
			float QL = m_RCorrectImg.Dt[i][j] * aa + bb;
			if (QL < 0){
				m_RCorrectImg.Dt[i][j] = 0;
			} else if (QL > 65535){
				m_RCorrectImg.Dt[i][j] = 65535;
			} else {
				m_RCorrectImg.Dt[i][j] = (unsigned short)QL;
			}
		}
	}

	m_RCorrectImg.SaveTmaTdt("tempImg/R_SensC.tma");

	return true;
}
//重ね合わせ処理
bool CCBCTCorrection::overlapImage(){

	int shift_x = m_SCBCTCorrectData.overlayX;
	int shift_y = m_SCBCTCorrectData.overlayY;

	m_CombinationImg = m_RCorrectImg;

	m_CombinationImg.Format(m_RCorrectImg.SubPixel,m_RCorrectImg.MainPixel*2-shift_x, 0,m_RCorrectImg.MainPixel-shift_x,0);

	for(int i=0;i<m_LCorrectImg.SubPixel;i++){
		for(int j=0;j<m_LCorrectImg.MainPixel;j++){
			m_CombinationImg.Dt[i][j+m_LCorrectImg.MainPixel-shift_x] = m_LCorrectImg.Dt[i][j];
		}
	}


	m_RCorrectImg.RotateAnyAngle((double)m_SCBCTCorrectData.overlayAngle, m_RCorrectImg.MainPixel-1, 0);

	for(int i=0;i<m_RCorrectImg.SubPixel+shift_y;i++){
		for(int j=0;j<m_RCorrectImg.MainPixel-shift_x;j++){
			if (m_RCorrectImg.bCheckPos(i+shift_y,j) && m_LCorrectImg.bCheckPos(i,j)){
				m_CombinationImg.Dt[i][j] = m_RCorrectImg.Dt[i+shift_y][j];
			}
		}
	}


	printf("Width:%d Height:%d bit:%d\n",m_CombinationImg.MainPixel, m_CombinationImg.SubPixel, m_CombinationImg.Bit);


	m_CombinationImg.SaveTmaTdt("tempImg/Combib.tma");

	return true;
}

//トリミング処理
bool CCBCTCorrection::trimmingImage(){

	int UpperCut = 7;
	int LowerCut = 7;
	int RightCut = 42;

	m_CombinationImg.Format(m_RCorrectImg.SubPixel - UpperCut - LowerCut,
							m_RCorrectImg.MainPixel*2 - RightCut,
							-1*UpperCut,
							-1*(m_CombinationImg.MainPixel -(m_RCorrectImg.MainPixel*2 - RightCut)));

	printf("Width:%d Height:%d bit:%d\n",m_CombinationImg.MainPixel, m_CombinationImg.SubPixel, m_CombinationImg.Bit);

	m_CombinationImg.SaveTmaTdt(m_SCBCTCorrectData.OutFileName);

	return true;
}

//段差中心Pixel座標を求める
float CCBCTCorrection::getDiffCenterPos(float angleD, float topPix, int iSub){

	//角度が大きすぎる場合は、そのまま返す(通常は、1°未満)
	if (angleD < -40 || angleD > 40){
		return topPix;
	}

	float angleR = angleD / 180 * M_PI;		//回転角度(ラジアン)

	return topPix + tanf(angleR)*iSub;

}


