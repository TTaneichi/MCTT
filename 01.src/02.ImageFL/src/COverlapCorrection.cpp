/*
 * COverlapCorrection.cpp
 *
 *  Created on: 2021/09/22
 *      Author: 10097663
 */

#include "COverlapCorrection.h"
#include "CUtil.h"
#include "CLeastSquare.h"
#include "math.h"
#include "CTFIMG.h"
#include <vector>

using namespace std;

#define MAX_SUB_SHIFT	13		//左右画像の最大の上下ずれ量(pix/2画素結合後)
#define DCORRECT_LEN	4		//段差を左右それぞれ何画素補正するか
#define STEPCORRECT_SUBMED_SIZE	67		//段差補正の副走査メディアンフィルタのサイズ
COverlapCorrection::COverlapCorrection(SOverlapCorrectParam inParam) {

	m_inParam = inParam;

}

COverlapCorrection::~COverlapCorrection() {
	// TODO Auto-generated destructor stub
}
//重ね合わせ実施
bool COverlapCorrection::ExecuteOverlap(TTMA in_LImg, TTMA in_RImg, TTMA &out_CombImg) {

	bool bRet = true;

	//右画像回転処理
	in_RImg.RotateAnyAngle((double)m_inParam.overlayAngle, in_RImg.MainPixel-1, 0);

	SOverlapDetailParam overlapParam;
	detectPosDiff(in_LImg,in_RImg, overlapParam);
#if 0
	//左と右の感度違いを検出(LP)
	LPAdjustEndPanelSensitivity2(in_LImg, in_RImg, out_CombImg);

	//右画像のオーバラップ部分の感度調整処理
	TTMA out_RImg = in_RImg;
//	bRet = changeSensitivity3(in_LImg, in_RImg, out_RImg);

	//濃度段差補正処理
	TTMA afterStepCorrectionImg;
	if (bRet){
		bRet = correctStepImage(out_RImg, afterStepCorrectionImg);
	}

	//画像の重ね合わせ
	if (bRet){
		bRet = overlapLR(in_LImg, afterStepCorrectionImg, out_CombImg);
	}
#endif
	return bRet;
}
//LP用オーバーラップ処理
bool COverlapCorrection::changeSensitivityLP(TTMA in_LImg, TTMA in_RImg, TTMA &out_RImg){

	bool bRet = true;

	//左画像と右画像の感度の違いを検出

	//1.ヒストグラムの取得

	//2.ThMaxQLCenterの計算

	//3.QL比の計算

	//4.左画像と右画像の主プロファイルの計算


	return true;
}
//オーバラップ部の感度調整
bool COverlapCorrection::changeSensitivity(TTMA in_LImg, TTMA in_RImg, TTMA &out_RImg){

	bool bRet = true;
	int overlapX = m_inParam.overlayX;
	int subPix = in_LImg.SubPixel;			//コードサイズ短縮のための代入

	in_LImg.SaveTmaTdt("tempImg/resLImg.tma");
	in_RImg.SaveTmaTdt("tempImg/RImg.tma");

	TTMA R_TanzakuImg = in_RImg;
	R_TanzakuImg.Format(subPix, overlapX, 0, overlapX - R_TanzakuImg.MainPixel, 0);

	//R_TanzakuImg.SaveTmaTdt("tempImg/Tanzaku.tma");

	TTMA DiffImg = R_TanzakuImg;
	const int iKasa = 10000;		//倍率

	for(int i=0;i<subPix;i++){
		for(int j=0;j<R_TanzakuImg.MainPixel;j++){
			if (R_TanzakuImg.Dt[i][j] > 20){
				DiffImg.Dt[i][j] = ((double)in_LImg.Dt[i][j] / R_TanzakuImg.Dt[i][j]) * iKasa;
			} else {
				DiffImg.Dt[i][j] = 1.0 * iKasa;
			}
		}
	}

	DiffImg.SaveTmaTdt("tempImg/DiffImg.tma");

	//DiffImg.Median(5,5);
	DiffImg.Median(25,1);
	DiffImg.Median(1,5);
	DiffImg.SaveTmaTdt("tempImg/DiffMedImg.tma");

	out_RImg = in_RImg;

	TTMA DiffImgSpread = DiffImg;
	int lowImgMain = in_RImg.MainPixel - m_inParam.diffStartPos + 1;		//感度低下領域の主方向サイズ
	DiffImgSpread.Format(subPix, lowImgMain, 0, lowImgMain - overlapX, 0);
	//DiffImgSpread.SaveTmaTdt("tempImg/DiffImgSpread.tma");

	for(int i=0;i<subPix;i++){
		for(int j=0;j<(lowImgMain - overlapX);j++){
			DiffImgSpread.Dt[i][j] = DiffImgSpread.Dt[i][lowImgMain - overlapX];
		}
	}
	DiffImgSpread.Median(7,1);

	//ななめエッジ判断
	bool *bDiagonalEdge;	//このまま延長した感度補正値を採用するかフラグ(true:採用しない／false:採用)
	bDiagonalEdge = (bool*)malloc(sizeof(bool)*subPix);
	for(int i=0;i<subPix;i++){
		bDiagonalEdge[i] = true;
	}

	double *doutData = (double*)malloc(sizeof(double)*subPix);
	for(int i=0;i<subPix;i++){
		doutData[i] = log((double)DiffImgSpread.Dt[i][lowImgMain - overlapX]);
	}

	//ファイル出力
	CUtil utilObj;
	utilObj.MakeDoubleFile("tempImg/SubSensD.txt", doutData, DiffImgSpread.SubPixel);

//	const double edgeTh = 0.04;		//エッジ判断閾値(辻さんのThreadは比で1.1なので、log(1.1)=0.04で大体あっている)
//	for(int i=1;i<subPix-1;i++){
//		if (abs(doutData[i-1] - doutData[i])>edgeTh || abs(doutData[i] - doutData[i+1])>edgeTh){
//			//エッジを検出した
//			bDiagonalEdge[i] = true;
//		}
//	}

	//ななめエッジの場合に感度を変更
	calcSensitivityInDiagonalEdge(bDiagonalEdge, in_RImg, DiffImgSpread, overlapX);
	DiffImgSpread.SaveTmaTdt("tempImg/DiffMedSpread.tma");

	free(doutData);
	free(bDiagonalEdge);

	//感度係数を元画像に加算
	for(int i=0;i<out_RImg.SubPixel;i++){
		for(int j=out_RImg.MainPixel - lowImgMain;j<out_RImg.MainPixel;j++){
			out_RImg.Dt[i][j] = out_RImg.Dt[i][j] * (double)DiffImgSpread.Dt[i][j-(out_RImg.MainPixel - lowImgMain)] / iKasa;
		}
	}
	out_RImg.SaveTmaTdt("tempImg/R_SensChg.tma");


	return bRet;

}
//オーバラップ部の感度調整2
//感度の掛け算を足し算に変更
bool COverlapCorrection::changeSensitivity2(TTMA in_LImg, TTMA in_RImg, TTMA &out_RImg){

	bool bRet = true;
	int overlapX = m_inParam.overlayX;
	int subPix = in_LImg.SubPixel;			//コードサイズ短縮のための代入

	in_LImg.SaveTmaTdt("tempImg/resLImg.tma");
	in_RImg.SaveTmaTdt("tempImg/RImg.tma");

	TTMA R_TanzakuImg = in_RImg;
	R_TanzakuImg.Format(subPix, overlapX, 0, overlapX - R_TanzakuImg.MainPixel, 0);

	//R_TanzakuImg.SaveTmaTdt("tempImg/Tanzaku.tma");

	TTMA DiffImg = R_TanzakuImg;
	const int iKasa = 20000;		//マイナス防止のかさあげ

	DiffImg.Median(5,5);
	TTMA LMedImg = in_LImg;
	LMedImg.Median(5,5);

	for(int i=0;i<subPix;i++){
		for(int j=0;j<R_TanzakuImg.MainPixel;j++){
			DiffImg.Dt[i][j] = LMedImg.Dt[i][j] - R_TanzakuImg.Dt[i][j] + iKasa;
		}
	}

	DiffImg.SaveTmaTdt("tempImg/DiffImg.tma");

	//DiffImg.Median(5,5);
	DiffImg.Median(25,1);
	DiffImg.Median(1,5);
	DiffImg.SaveTmaTdt("tempImg/DiffMedImg.tma");

	out_RImg = in_RImg;

	TTMA DiffImgSpread = DiffImg;
	int lowImgMain = in_RImg.MainPixel - m_inParam.diffStartPos + 1;		//感度低下領域の主方向サイズ
	DiffImgSpread.Format(subPix, lowImgMain, 0, lowImgMain - overlapX, 0);
	//DiffImgSpread.SaveTmaTdt("tempImg/DiffImgSpread.tma");

	for(int i=0;i<subPix;i++){
		for(int j=0;j<(lowImgMain - overlapX);j++){
			DiffImgSpread.Dt[i][j] = DiffImgSpread.Dt[i][lowImgMain - overlapX];
		}
	}
	DiffImgSpread.Median(7,1);

	//ななめエッジ判断
	bool *bDiagonalEdge;	//このまま延長した感度補正値を採用するかフラグ(true:採用しない／false:採用)
	bDiagonalEdge = (bool*)malloc(sizeof(bool)*subPix);
	for(int i=0;i<subPix;i++){
		bDiagonalEdge[i] = true;
	}

	double *doutData = (double*)malloc(sizeof(double)*subPix);
	for(int i=0;i<subPix;i++){
		doutData[i] = log((double)DiffImgSpread.Dt[i][lowImgMain - overlapX]);
	}

	//ファイル出力
	CUtil utilObj;
	utilObj.MakeDoubleFile("tempImg/SubSensD.txt", doutData, DiffImgSpread.SubPixel);

//	const double edgeTh = 0.04;		//エッジ判断閾値(辻さんのThreadは比で1.1なので、log(1.1)=0.04で大体あっている)
//	for(int i=1;i<subPix-1;i++){
//		if (abs(doutData[i-1] - doutData[i])>edgeTh || abs(doutData[i] - doutData[i+1])>edgeTh){
//			//エッジを検出した
//			bDiagonalEdge[i] = true;
//		}
//	}

	//ななめエッジの場合に感度を変更
	calcSensitivityInDiagonalEdge(bDiagonalEdge, in_RImg, DiffImgSpread, overlapX);
	DiffImgSpread.SaveTmaTdt("tempImg/DiffMedSpread.tma");

	free(doutData);
	free(bDiagonalEdge);

	//感度係数を元画像に加算
	for(int i=0;i<out_RImg.SubPixel;i++){
		for(int j=out_RImg.MainPixel - lowImgMain;j<out_RImg.MainPixel;j++){
			out_RImg.Dt[i][j] = out_RImg.Dt[i][j] + DiffImgSpread.Dt[i][j-(out_RImg.MainPixel - lowImgMain)] - iKasa;
		}
	}
	out_RImg.SaveTmaTdt("tempImg/R_SensChg.tma");


	return bRet;

}
//オーバラップ部の感度調整3
bool COverlapCorrection::changeSensitivity3(TTMA in_LImg, TTMA in_RImg, TTMA &out_RImg){

	bool bRet = true;
	int overlapX = m_inParam.overlayX;
	int subPix = in_LImg.SubPixel;			//コードサイズ短縮のための代入

	in_LImg.SaveTmaTdt("tempImg/resLImg.tma");
	in_RImg.SaveTmaTdt("tempImg/RImg.tma");

	TTMA R_TanzakuImg = in_RImg;
	R_TanzakuImg.Format(subPix, overlapX, 0, overlapX - R_TanzakuImg.MainPixel, 0);

	//R_TanzakuImg.SaveTmaTdt("tempImg/Tanzaku.tma");

	TTMA DiffImg = R_TanzakuImg;
	TTMA LMedImg = in_LImg;

	R_TanzakuImg.Median(5,5);
	LMedImg.Median(5,5);

	int *pLQL = (int*)malloc(sizeof(int)*subPix*R_TanzakuImg.MainPixel);
	int *pRQL = (int*)malloc(sizeof(int)*subPix*R_TanzakuImg.MainPixel);

	const int iTimes = 10000;		//倍率
	const double coef_a = 1.454;
	//const double coef_b = -7271;
	const double coef_b = -14545;
	for(int i=0;i<subPix;i++){
		for(int j=0;j<R_TanzakuImg.MainPixel;j++){
			double newQL = coef_a + coef_b / R_TanzakuImg.Dt[i][j];
			if (newQL < 0.1){
				newQL = 0.1;
			} else if (newQL > 5.0){
				newQL = 5.0;
			}
			DiffImg.Dt[i][j] = newQL * iTimes;
//			if (R_TanzakuImg.Dt[i][j] > 20){
//				DiffImg.Dt[i][j] = ((double)LMedImg.Dt[i][j] / R_TanzakuImg.Dt[i][j]) * iTimes;
//			} else {
//				DiffImg.Dt[i][j] = 1.0 * iTimes;
//			}
//			pLQL[i*R_TanzakuImg.MainPixel + j] = LMedImg.Dt[i][j];
//			pRQL[i*R_TanzakuImg.MainPixel + j] = R_TanzakuImg.Dt[i][j];
		}
	}

	DiffImg.SaveTmaTdt("tempImg/DiffImg.tma");

	//DiffImg.Median(5,5);
//	DiffImg.Median(25,1);
//	DiffImg.Median(1,5);
	DiffImg.SaveTmaTdt("tempImg/DiffMedImg.tma");

	out_RImg = in_RImg;

	TTMA DiffImgSpread = DiffImg;
	int lowImgMain = in_RImg.MainPixel - m_inParam.diffStartPos + 1;		//感度低下領域の主方向サイズ
	DiffImgSpread.Format(subPix, lowImgMain, 0, lowImgMain - overlapX, 0);
	//DiffImgSpread.SaveTmaTdt("tempImg/DiffImgSpread.tma");

	for(int i=0;i<subPix;i++){
		for(int j=0;j<(lowImgMain - overlapX);j++){
			DiffImgSpread.Dt[i][j] = DiffImgSpread.Dt[i][lowImgMain - overlapX];
		}
	}
	DiffImgSpread.Median(7,1);

	//ななめエッジ判断
	bool *bDiagonalEdge;	//このまま延長した感度補正値を採用するかフラグ(true:採用しない／false:採用)
	bDiagonalEdge = (bool*)malloc(sizeof(bool)*subPix);
	for(int i=0;i<subPix;i++){
		bDiagonalEdge[i] = true;
	}

	double *doutData = (double*)malloc(sizeof(double)*subPix);
	for(int i=0;i<subPix;i++){
		doutData[i] = log((double)DiffImgSpread.Dt[i][lowImgMain - overlapX]);
	}

	//ファイル出力
	CUtil utilObj;
	utilObj.MakeDoubleFile("tempImg/SubSensD.txt", doutData, DiffImgSpread.SubPixel);
	utilObj.MakeIntFile("tempImg/LQL.txt",pLQL,subPix*R_TanzakuImg.MainPixel );
	utilObj.MakeIntFile("tempImg/RQL.txt",pRQL,subPix*R_TanzakuImg.MainPixel );

//	const double edgeTh = 0.04;		//エッジ判断閾値(辻さんのThreadは比で1.1なので、log(1.1)=0.04で大体あっている)
//	for(int i=1;i<subPix-1;i++){
//		if (abs(doutData[i-1] - doutData[i])>edgeTh || abs(doutData[i] - doutData[i+1])>edgeTh){
//			//エッジを検出した
//			bDiagonalEdge[i] = true;
//		}
//	}

	//ななめエッジの場合に感度を変更
	calcSensitivityInDiagonalEdge(bDiagonalEdge, in_RImg, DiffImgSpread, overlapX);
	DiffImgSpread.SaveTmaTdt("tempImg/DiffMedSpread.tma");

	free(doutData);
	free(bDiagonalEdge);
	free(pLQL);
	free(pRQL);

	//感度係数を元画像に加算
	for(int i=0;i<out_RImg.SubPixel;i++){
		for(int j=out_RImg.MainPixel - lowImgMain;j<out_RImg.MainPixel;j++){
			out_RImg.Dt[i][j] = out_RImg.Dt[i][j] * (double)DiffImgSpread.Dt[i][j-(out_RImg.MainPixel - lowImgMain)] / iTimes;
		}
	}
	out_RImg.SaveTmaTdt("tempImg/R_SensChg.tma");


	return bRet;

}
//段差補正処理
bool COverlapCorrection::correctStepImage(TTMA in_RImg, TTMA &out_RImg){

	bool bRet = true;

	TTMA RAveimgObj = in_RImg;
	out_RImg = in_RImg;

	//段差部分を切り出して、副走査方向にメディアンをかける
	RAveimgObj.Format(in_RImg.SubPixel, DCORRECT_LEN*2+1, 0, -1*((int)m_inParam.diffStartPos-DCORRECT_LEN),0);
	RAveimgObj.Median(STEPCORRECT_SUBMED_SIZE,1);

	TTMA DiffImgObj = RAveimgObj;

//	//TODO 参考のため、高周波成分を抽出。実際には不要。
//	for(int i=0;i < DiffImgObj.SubPixel;i++){
//		for(int j=0;j < DiffImgObj.MainPixel;j++){
//			DiffImgObj.Dt[i][j] = 10000 * ((float)RAveimgObj.Dt[i][j] / in_RImg.Dt[ i ][j+ ((int)m_inParam.diffStartPos-DCORRECT_LEN)]);
//		}
//	}
//	DiffImgObj.SaveTmaTdt("tempImg/DiffStep.tma");

	//開始と終了を結んだ直線を理想的なQL値として、低周波数成分において、その差分を補正する。
	for(int i=0; i < RAveimgObj.SubPixel; i++){
		int StartQL = RAveimgObj.Dt[i][0];
		int EndQL = RAveimgObj.Dt[i][RAveimgObj.MainPixel-1];
		float deltaQL = ((float)EndQL - StartQL)/(2*DCORRECT_LEN);		//1画素ずれた時のΔQL
		for(int j=0; j < DiffImgObj.MainPixel; j++){
			float idealQL = StartQL + deltaQL * j;
			float ratio = idealQL /RAveimgObj.Dt[i][j];
			out_RImg.Dt[i][j+ ((int)m_inParam.diffStartPos-DCORRECT_LEN)] = ratio * in_RImg.Dt[i][j + ((int)m_inParam.diffStartPos-DCORRECT_LEN)];
		}
	}

	out_RImg.SaveTmaTdt("tempImg/resR.tma");			//TODO

	RAveimgObj.SaveTmaTdt("tempImg/ave.tma");


	return bRet;
}

//LR画像の重ね合わせ処理
bool COverlapCorrection::overlapLR(TTMA in_LImg, TTMA in_RImg, TTMA &out_CombImg){

	bool bRet = true;

	int shift_x = m_inParam.overlayX;
	int shift_y = m_inParam.overlayY;

	out_CombImg = in_RImg;

	out_CombImg.Format(in_RImg.SubPixel,in_RImg.MainPixel*2-shift_x, 0,in_RImg.MainPixel-shift_x,0);

	//回転処理は感度補正前に移動
	//in_RImg.RotateAnyAngle((double)m_inParam.overlayAngle, in_RImg.MainPixel-1, 0);

	for(int i=0;i<in_RImg.SubPixel+shift_y;i++){
		for(int j=0;j<in_RImg.MainPixel-shift_x;j++){
			if (in_RImg.bCheckPos(i+shift_y,j) && in_RImg.bCheckPos(i,j)){
				out_CombImg.Dt[i][j] = in_RImg.Dt[i+shift_y][j];
			}
		}
	}


	for(int i=0;i<in_LImg.SubPixel;i++){
		for(int j=0;j<in_LImg.MainPixel;j++){
			out_CombImg.Dt[i][j+in_LImg.MainPixel-shift_x] = in_LImg.Dt[i][j];
		}
	}


	printf("Width:%d Height:%d bit:%d\n",out_CombImg.MainPixel, out_CombImg.SubPixel, out_CombImg.Bit);


	out_CombImg.SaveTmaTdt("tempImg/Combib.tma");


	return bRet;
}

//ななめエッジ検出時の感度補正値計算
bool COverlapCorrection::calcSensitivityInDiagonalEdge(bool *bEdge, TTMA in_RImg, TTMA &inout_Sens, int overlapSize){

	bool bRet = true;
	int SpreadSize = inout_Sens.MainPixel - overlapSize;
	int SubPix = inout_Sens.SubPixel;
//	printf("mainPix:%d\n",inout_Sens.MainPixel);
//	printf("overlap:%d\n",overlapSize);
//	printf("SpreadSize:%d\n",SpreadSize);
//	printf("SubPix:%d\n",SubPix);

	if (SpreadSize <= 0){
		printf("OverlapSize is too large!!!\n");
		return false;
	}

	int startMainPix = in_RImg.MainPixel - inout_Sens.MainPixel;
	for(int i=1;i<SubPix-1;i++){
		if (bEdge[i]){	//置き換え対象のみ行う
			for(int j=0;j<SpreadSize;j++){
				//重なり領域の範囲内で、2乗和誤差が最小になる座標を見つける
				double minSum = 99999999999.0;
				int min_y_pos;
				int min_x_pos;
				for(int iSub=1; iSub < SubPix-1; iSub++){
					for(int iMain=SpreadSize; iMain < inout_Sens.MainPixel-1; iMain++){
						double Sum = 0.0;
						for(int x=-1;x<2;x++){
							for(int y=-1;y<2;y++){
								double d_diff = (double)in_RImg.Dt[iSub+y][iMain+startMainPix+x] - in_RImg.Dt[i+y][j+startMainPix+x];
								Sum += d_diff * d_diff;
							}
						}
						if (Sum < minSum){
							//printf("minSum update %7.4lf\n",Sum);
							minSum = Sum;
							min_y_pos = iSub;
							min_x_pos = iMain;
						}
					}
				}
				//x,yが確定したので、その点の感度を採用
				//printf("i:%d j:%d min_y:%d min_x:%d\n",i,j,min_y_pos,min_x_pos);
				inout_Sens.Dt[i][j] = inout_Sens.Dt[min_y_pos][min_x_pos];
			}
		}
	}

	inout_Sens.Median(3,3);

	return bRet;
}

//オーバーラップ部の詳細の位置ずれ検出
bool COverlapCorrection::detectPosDiff(TTMA in_LImg, TTMA in_RImg, SOverlapDetailParam &out_detailParam){

	bool bRet = true;

	int overlapX = m_inParam.overlayX;
	int subPix = in_LImg.SubPixel;			//コードサイズ短縮のための代入

	in_LImg.SaveTmaTdt("tempImg/resLImg.tma");
	in_RImg.SaveTmaTdt("tempImg/RImg.tma");

	TTMA R_TanzakuImg = in_RImg;
	R_TanzakuImg.Format(subPix, overlapX, m_inParam.overlayY, overlapX - R_TanzakuImg.MainPixel, 0);

	TTMA Log_LImg = R_TanzakuImg;
	TTMA Log_RImg = R_TanzakuImg;

	//Logデータを作成する
	for(int i=0;i<R_TanzakuImg.SubPixel;i++){
		for(int j=0;j<R_TanzakuImg.MainPixel;j++){
//			Log_LImg.Dt[i][j] = (unsigned int)(log(in_LImg.Dt[i][j]+1)*10000);
//			Log_RImg.Dt[i][j] = (unsigned int)(log(R_TanzakuImg.Dt[i][j]+1)*10000);
			Log_LImg.Dt[i][j] = in_LImg.Dt[i][j];
			Log_RImg.Dt[i][j] = R_TanzakuImg.Dt[i][j];
		}
	}

	//マッチング確認
	float fitX = 0.0;
	float fitY = 0.0;
	float fitMag = 1.0;
	float minSum = 1.0e+20;
	TTMA Log_RFinalImg;

	for(int mag = -10; mag < 10; mag++){
		float tempMag = 1.0 + (float)mag * 0.0002;
		TTMA MagRImg = Log_RImg;	//拡大画像
		MagRImg.Scaling(Log_RImg.MainPixel/2,Log_RImg.SubPixel/2,1.0,tempMag,0);
		for(int x_shift = -10; x_shift < 10; x_shift++){
			for(int y_shift = -10; y_shift < 10; y_shift++){
				float tempX = (float)x_shift * 0.2;
				float tempY = (float)y_shift * 0.2;
				TTMA ShiftRImg = MagRImg;
				ShiftRImg.ShiftXY(tempX,tempY,0);
				float sum = 0.0;
				for(int i = 2;i<subPix-2;i++){		//周囲2画素は対象外とする
					for(int j=2;j<Log_RImg.MainPixel-2;j++){
						int diff = Log_LImg.Dt[i][j] - ShiftRImg.Dt[i][j];
						sum += diff*diff;
					}
				}
				if (sum < minSum){
					fitX = tempX;
					fitY = tempY;
					fitMag = tempMag;
					minSum = sum;
					Log_RFinalImg = ShiftRImg;
					//printf("Update sum X:%7.4lf Y:%7.4lf Mag:%7.4lf sum:%7.4lf\n",tempX, tempY, fitMag,log(minSum));
				}
			}
		}
	}


	Log_LImg.SaveTmaTdt("tempImg/Log_LImg.tma");
	Log_RFinalImg.SaveTmaTdt("tempImg/Log_RFinalImg.tma");

	out_detailParam.overlayX = fitX + overlapX;
	out_detailParam.overlayY = fitY + m_inParam.overlayY;
	out_detailParam.lowP_Mag = 1.0 + fitMag;


	return bRet;
}


//左画像と右画像の感度違いを検出して左画像の感度を修正する(C)
bool COverlapCorrection::LPAdjustEndPanelSensitivity2(TTMA in_LImg, TTMA in_RImg, TTMA &out_RImg){

	bool bRet = true;

	//（ImageUpの下端のQL値の副方向の傾きと合致するCenterパネルのシンチ段差下方のｙ座標を計算）
	const int iMainMoveAve = 11;	//主と副のサイズを入れ替える
	const int iSubMoveAve = 51;		//主と副のサイズを入れ替える
	const int iMainShiftMax = 30;
	const int iMCut = iMainShiftMax;
	const int iUeOffset = 2;
	TTMA SmoothImageUp = in_LImg;
	SmoothImageUp.MainMoveAve(iMainMoveAve);
	SmoothImageUp.SubMoveAve(iSubMoveAve);
	TTMA SmoothImageDown = in_RImg;
	SmoothImageDown.MainMoveAve(iMainMoveAve);
	SmoothImageDown.SubMoveAve(iSubMoveAve);

	//左右の照射野絞りを検出することで感度比のロバスト向上(140823 ttsuji)
	const int iThQLMDif = 100;//14bitログ画像の主微分のQL閾値
	bool bExist0 = false;//照射野境界の検出フラグ
	bool bExistE = false;//照射野境界の検出フラグ
	int iSSearchRadiationField0   = SmoothImageUp.MainPixel - 101;	//	1416 - 101
	int iSSearchRadiationFieldEnd = SmoothImageUp.MainPixel - 1;//画像の下端	1416 - 1
	int iMBorder0 = 0;//照射野境界のｘ座標
	int iMBorderEnd = SmoothImageUp.SubPixel - 1;//照射野境界のｘ座標	//1418 -1

	//左右の照射野境界を検出
	//LPDetectBorderRadiationField()

	//1.ヒストグラムをとって最大QL、最小QLを計算
	 //(左右は画像端から位置ずれ分30pixを除外し、上下は3pix平均とする)
	int iMStart = (iMBorder0 > iMainShiftMax) ? iMBorder0 + 1 : iMainShiftMax;		//30が採用される
	int iMEnd = (iMBorderEnd < in_LImg.SubPixel - iMainShiftMax) ? iMBorderEnd - 1 : in_LImg.SubPixel - 1 - iMainShiftMax;//照射野境界のｘ座標
	in_LImg.Histogram(in_LImg.MainPixel - iUeOffset - 1, in_LImg.MainPixel - iUeOffset + 1, iMStart, iMEnd);
	float fThRate = 0.3;//ヒストの全面積の30%（感度比を計算する画素の判定に使用）
	int iThMaxQL = in_LImg.HistRuisekiDosuuPercent(fThRate, true);

	int iNumOfSample = 30;//上パネルの最小二乗法のサンプル点の個数
	double* dS = new double[iNumOfSample];
	double* dQL = new double[iNumOfSample];
	vector<double> v;

	double dSum = 0.;
	int iCounter = 0;//関心領域内で検出した白帯位置の検出数
	int iSAve(0), iSMax(0);//関心領域内で検出した白帯位置の主方向の統計値
	//関心領域で平均化した副プロファイルをまず計算し、次にそのプロファイルを分析する
	//（白帯は主方向全域に渡る＆ノイズが少ないのでこっちが精度良いと考える）
	TTMA SubProfileU, SubProfileD;
	SubProfileU.NewDt(in_RImg.MainPixel, 1, in_RImg.Bit);//関心領域で平均化した副プロファイル
	SubProfileD.NewDt(in_RImg.MainPixel, 1, in_RImg.Bit);//関心領域で平均化した副プロファイル
	float fSumU = 0.0;
	float fSumD = 0.0;
	int iCounter2 = 0;

	for (int iM = 0; iM < in_RImg.MainPixel; iM++){
		fSumU = fSumD = 0.0;
		iCounter2 = 0;
		for (int iS = iMStart; iS <= iMEnd; iS++){//照射野境界を除外して集計
			if (iThMaxQL <= in_LImg.Dt[in_LImg.MainPixel][iM - iUeOffset]) {//iMが素抜けの場合は排除
				continue;
			}
			//主方向に平均化
			fSumU += SmoothImageUp.Dt[iS][iM];
			fSumD += SmoothImageDown.Dt[iS][iM];
			iCounter2++;

		}
		fSumU /= iCounter2;
		fSumD /= iCounter2;
		SubProfileU.SetVal(iM, 0, fSumU);
		SubProfileD.SetVal(iM, 0, fSumD);
	}

	SubProfileU.SaveTmaTdt("tempImg/SubProfileU.tma");
	SubProfileD.SaveTmaTdt("tempImg/SubProfileD.tma");

#if 0

	//最小二乗法近似で上パネルの傾きを計算
	int EdgeShiftU = m_inParam.overlayY;			// TODO	チック必要
	int iLen_PanelOverlap = 137 + EdgeShiftU;//TopとCenterとで同じ絵が写る重複領域長さ
	for (int iS = 0; iS < iNumOfSample; iS++) {
		dS[iS]  = iS + iLen_PanelOverlap - iNumOfSample;
		dQL[iS] = SubProfileU.Dt[SubProfileU.SubPixel - iNumOfSample + iS][0];
	}

	//直線fitting
	CLeastSquare LSItpl(iNumOfSample, dS, dQL, _LINEAR);
	LSItpl.Calc();
	v = LSItpl.GetSolve();
	//直線fittingの係数Ax+B
	double dA = v[1];
	double dB = v[0];
	dA = round(dA*1000) / 1000.;//小数第3位まで四捨五入で計算

	const int iSDif = 20;//20pix刻みで副微分をとる
	const int iSOffset = 30;//シンチ段差位置から、副微分値を調査するスタート点までのオフセット
	int iSDifHalf = iSDif / 2;

	double dQL10dif;//10pix刻みの副微分値
	int iSSearch = 0;//白帯がシンチ段差下方どこまで続いているか
	const double dGradMarginRate = 1.25;//上凸検出のときの傾きのマージンの％
	const double dAOffset = 0.3;//dAが小さすぎてサーチ範囲の中にdAを下回るdQL10difがない場合はdAをオフセットする

	//白帯起因の上凸カーブであるかをまずチェック
	double dDeltaQL[3];
	int iIndex = 0;
	bool bExistShiroobi = false;
	double dATh = 3.5;//傾きの差分の閾値
	int iYMaxSintiEdge = round(m_SintiEdgeU.ymax / 10);//シンチ段差のｙ座標最下点

	//シンチ段差より下方の副プロファイルの接線の傾きを求める
	for (int iIndex = 0; iIndex < 3; iIndex++) {
		int iS = iYMaxSintiEdge + iSOffset;//iSDifによらずスタート点は固定とする
		dDeltaQL[iIndex] = (double)(SubProfileD.Dt[iS + iIndex*iSDif + iSDifHalf][0] - SubProfileD.Dt[iS + iIndex*iSDif - iSDifHalf][0]) / iSDif;
		//シンチ段差下方の接線の傾きと上パネル下端の接線の傾きの関係から白帯あるか判定
		double dQL10difdif(-1000000.);//二階微分（凸性）
		double dPreQL10dif;//ｙ座標の1個前の傾き（二階微分を計算するため）
		//傾きの定数倍のマージン　or 傾きのオフセット加算マージンの　いずれかを満たすときに白帯ありとする
		const double dGradMarginOffset = 0.05;//1QLの1/iSDif = 1/20で0.05とする
		double dError = 1e-4;
		//上画像の下端の傾きを中画像のシンチ段差直下と比較するように修正
		if (dA < dDeltaQL[0] && 0 < dDeltaQL[0] && dDeltaQL[0] * dGradMarginRate >= dDeltaQL[1] && dDeltaQL[1] * dGradMarginRate >= dDeltaQL[2]) {
			for (int iS = iYMaxSintiEdge + iSOffset; iS < iSSearchEnd; iS += iSDif) {
				dQL10dif = (double)(SubProfileD.Dt[iS + iSDifHalf][0] - SubProfileD.Dt[iS - iSDifHalf][0]) / iSDif;
				if (iS > iYMaxSintiEdge + iSOffset) {
					dQL10difdif = dQL10dif - dPreQL10dif;//二階微分（凸性）
				}
				//目標の傾きdAよりも大きい場合、または二階微分が0以上となる場合は白帯終点とする
				if (dA > dQL10dif) {
					iSSearch = iS;
					bExistShiroobi = true;
					break;
				}
				else if (iS > iYMaxSintiEdge + iSOffset && dQL10difdif >= 0 && dQL10dif - dA < dATh) {
					iSSearch = iS - iSDifHalf;
					bExistShiroobi = true;
					break;
				}
				dPreQL10dif = dQL10dif;//次のｙ座標で二階微分計算用に規定
				dQL10dif = (double)(SubProfileD.Dt[iS + iSDifHalf][0] - SubProfileD.Dt[iS - iSDifHalf][0]) / iSDif;
				if (dA > dQL10dif) {//白帯領域であり、目標の傾きdAよりも大きいときはiSの探索をさらに進める
					iSSearch = iS;
					bExistShiroobi = true;
					break;
				}
			}
			//シンチ段差より下方の白帯の長さ
			if (bExistShiroobi) {
				int iShiroobiLen = iSSearch - iYMaxSintiEdge;
				iSitaOffset = iShiroobiLen;//引数を更新
			}
			else{//dAが小さすぎてサーチ範囲の中にdAを下回るdQL10difがない場合はdAをオフセットする
				dA += dAOffset;
				for (int iS = iYMaxSintiEdge + iSOffset; iS < iSSearchEnd; iS += iSDif) {
					dQL10dif = (double)(SubProfileD.Dt[iS + iSDifHalf][0] - SubProfileD.Dt[iS - iSDifHalf][0]) / iSDif;
					if (dA > dQL10dif) {//白帯領域であり、目標の傾きdAよりも大きいときはiSの探索をさらに進める
						iSSearch = iS;
						bExistShiroobi = true;
						break;
					}
				}

				//シンチ段差より下方の白帯の長さ
				if (bExistShiroobi) {
					int iShiroobiLen = iSSearch - iYMaxSintiEdge;
					iSitaOffset = iShiroobiLen;//引数を更新
				}
			}
		}
	//感度比を計算する際に、Top(or Bottom)のみならずCenterのHistgramも解析する
	in_RImg.Histogram(iYMaxSintiEdge + iSitaOffset - 1, iYMaxSintiEdge + iSitaOffset + 1, iMStart, iMEnd);
	int iThMaxQL_Center = HistRuisekiDosuuPercent(fThRate, true);  L13051
	//感度の比を各ｘごとに計算し、ｘ方向に集計
	for (int iM = 0; iM < MainPixel; iM++){
		if (0 == Dt[edgeY[iM] + iSitaOffset][iM]) {
			ImageRateOfUpToDown.Dt[iM][0] = 0.; //集計時に除外するように値を設定
		}
		else{
			ImageRateOfUpToDown.Dt[iM][0] = ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM] / (float)Dt[edgeY[iM] + iSitaOffset][iM];
		}
	}
	float fSensitivityRate;//主方向に平均した感度比
	float fSum = 0.0;
	const int iCut = iMainShiftMax;//平均値算出から除外する端画素
	const float fLowerTh = fLowerLimitOfPanelSensRatio;
	const float fUpperTh = fUpperLimitOfPanelSensRatio;
	fSumU = 0.0;
	fSumD = 0.0;
	iCounter2 = 0;
	for (int iS = 0; iS < SubPixel; iS++){
		fSumU = fSumD = 0.0;
		iCounter2 = 0;
		for (int iM = iMStart; iM <= iMEnd; iM++){//照射野境界を除外して集計
			//感度比を計算する際に、Top(or Bottom)のみならずCenterの画素値も考慮する
			//主方向に平均化
			fSumU += ImageUp.Dt[iS][iM];
			fSumD += Dt[iS][iM];
			iCounter2++;
		}
		if (iCounter2) {
			fSumU /= iCounter2;
			fSumD /= iCounter2;
		}
		SubProfileU.SetVal(iS, 0, fSumU);
		SubProfileD.SetVal(iS, 0, fSumD);
	}
	//最小二乗法近似で上パネルの傾きを計算
	iLen_PanelOverlap = 137 + round(m_EdgeShiftU.c / 10.);//TopとCenterとで同じ絵が写る重複領域長さ
	for (int iS = 0; iS < iNumOfSample; iS++) {
		dS[iS]  = iS + iLen_PanelOverlap - iNumOfSample;
		dQL[iS] = SubProfileU.Dt[SubProfileU.SubPixel - iNumOfSample + iS][0];
	}
	//冪乗fitting
	CLeastSquare LSPower(iNumOfSample, dS, dQL, _POWER);
	double dPanelSensitivityRate;//パネル間感度補正係数(Center基準)
	double dApower;
	double dBpower;
	double dQLu_GaisohAtShiroobiEnd;
	double dQLd_AtShiroobiEnd;
	if (LSPower.Calc()){
		v = LSPower.GetSolve();
		//冪乗fittingの係数Bx^(A)
		dApower = v[1];
		dBpower = exp(v[0]);
		//感度比を計算する際に、Top下端の副プロファイルをCenterパネルの白帯終点まで外挿することで感度比を計算
		//シンチ段差のｙ座標の最下点を計算する。
		iYMaxSintiEdge = round(m_SintiEdgeU.ymax / 10.);
		dQLu_GaisohAtShiroobiEnd = dBpower * pow(iYMaxSintiEdge + iSitaOffset, dApower);//白帯終点におけるTopパネル下端副プロファイル外挿(冪乗fitting)
		double dQLu_GaisohAtShiroobiEnd = dBpower * pow(edgeY[MainPixel - 1] + iSitaOffset, dApower);//白帯終点におけるTopパネル下端副プロファイル外挿(冪乗fitting)
		dQLd_AtShiroobiEnd = (double)SubProfileD.Dt[iYMaxSintiEdge + iSitaOffset][0]; //150918 ttsuji修正
		dPanelSensitivityRate = dQLd_AtShiroobiEnd / dQLu_GaisohAtShiroobiEnd;

		//異常値の場合は1.0とする
		if (dPanelSensitivityRate <= fLowerTh || dPanelSensitivityRate >= fUpperTh){
			dPanelSensitivityRate = 1.0;
		}
	}
	else{//ゼロ行列のとき
		dPanelSensitivityRate = 1.0;
	}
	//Topパネルに感度補正係数を乗算
	for (int iS = 0; iS < ImageUp.SubPixel; iS++){
		for (int iM = 0; iM < ImageUp.MainPixel; iM++){
			ImageUp.SetVal(iS, iM, ImageUp.Dt[iS][iM] * dPanelSensitivityRate);
		}
	}
	fSensitivityRateDtoU = (float)dPanelSensitivityRate;
	//感度比を計算する際に、Top下端の副プロファイルをCenterパネルの白帯終点まで外挿することで感度比を計算
	double dQLu_GaisohAtShiroobiEnd = dBpower * pow(iYMaxSintiEdge + iSitaOffset, dApower);//白帯終点におけるTopパネル下端副プロファイル外挿(冪乗fitting)
	double dQLd_AtShiroobiEnd = (double)SubProfileD.Dt[iYMaxSintiEdge + iSitaOffset][0];
	double dPanelSensitivityRate;//パネル間感度補正係数(Center基準)
	dPanelSensitivityRate = dQLd_AtShiroobiEnd / dQLu_GaisohAtShiroobiEnd;
	//異常値の場合は1.0とする
	const float fLowerTh = 0.7;
	const float fUpperTh = 1.5;
	if (dPanelSensitivityRate <= fLowerTh || dPanelSensitivityRate >= fUpperTh){
		dPanelSensitivityRate = 1.0;
	}
	//Topパネルに感度補正係数を乗算
	for (int iS = 0; iS < ImageUp.SubPixel; iS++){
		for (int iM = 0; iM < ImageUp.MainPixel; iM++){
			ImageUp.SetVal(iS, iM, ImageUp.Dt[iS][iM] * dPanelSensitivityRate);
		}
	}
	fSensitivityRateDtoU = (float)dPanelSensitivityRate;
	//感度比を計算する際に、Top(or Bottom)のみならずCenterのHistgramも解析する
	Histogram(iYMaxSintiEdge + iSitaOffset - 1, iYMaxSintiEdge + iSitaOffset + 1, iMStart, iMEnd);
	int iThMaxQL_Center = HistRuisekiDosuuPercent(fThRate, true);
	//感度の比を各ｘごとに計算し、ｘ方向に集計
	for (int iM = 0; iM < MainPixel; iM++){
		if (0 == Dt[edgeY[iM] + iSitaOffset][iM]) {
			ImageRateOfUpToDown.Dt[iM][0] = 0.; //集計時に除外するように値を設定
		}
		else{
			ImageRateOfUpToDown.Dt[iM][0] = ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iM] / (float)Dt[edgeY[iM] + iSitaOffset][iM];
		}
	}
	float fSensitivityRate;//主方向に平均した感度比
	float fSum = 0.0;
	const int iCut = iMainShiftMax;//平均値算出から除外する端画素
	const float fLowerTh = fLowerLimitOfPanelSensRatio;
	const float fUpperTh = fUpperLimitOfPanelSensRatio;
	iCounter = 0;
	for (int iMCnt = iMStart; iMCnt <= iMEnd; iMCnt++){//照射野境界を除外して集計
		//感度比を計算する際に、Top(or Bottom)のみならずCenterの画素値も考慮する
		if (0 != ImageRateOfUpToDown.Dt[iMCnt][0] && fLowerTh < ImageRateOfUpToDown.Dt[iMCnt][0] && fUpperTh > ImageRateOfUpToDown.Dt[iMCnt][0] && iThMaxQL > ImageUp.Dt[ImageUp.SubPixel - iUeOffset][iMCnt] && iThMaxQL_Center > Dt[edgeY[iMCnt] + iSitaOffset][iMCnt]) {
			fSum += ImageRateOfUpToDown.Dt[iMCnt][0];
			fSum += ImageRateOfUpToDown.Dt[iMCnt][0];
		}
	}
	for (int iS = 0; iS < SubPixel; iS++){
		for (int iM = 0; iM < MainPixel; iM++){
			if (ImageRateOfUpToDown.Dt[iM][0]) {
				ImageUp.SetVal(iS, iM, ImageUp.Dt[iS][iM]/fSensitivityRate);
			}
			else{
				ImageUp.SetVal(iS, iM, 0);
			}
		}
	}

#endif

	return bRet;

}


