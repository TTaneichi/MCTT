/*
 * CCBCTCalib.cpp
 *
 *  Created on: 2021/04/20
 *      Author: 10097663
 */

#include "CCBCTCalib.h"
#include "CTTMA.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
using namespace std;
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define CUT_IMG_WIDTH	60					//短冊の幅
#define STEP_EX_WIDHT	35					//右画像に段差がありうる幅
#define MIN_MARK_QL	500						//マーカーの最低QL (白抜け対策の暫定)本来は0にする
#define MARK_DEL_MEDIAN		67				//マーカー消去用メディアンサイズ
#define DIFF_SEARCH_RANGE	22				//段差検出幅サイズ
#define X_DIFF_MIN	18						//X軸方向の-最大ずれ量
#define X_DIFF_MAX	33						//X軸方向の+最大ずれ量
#define X_DIFF_UNIT	0.25					//X軸方向のずれ探索単位(pixel)
#define Y_DIFF_MIN	-8						//Y軸方向の-最大ずれ量
#define Y_DIFF_MAX	8						//Y軸方向の+最大ずれ量
#define Y_DIFF_UNIT 0.25					//Y軸方向のずれ探索単位(pixel)
#define ANGLE_MIN	-20						//角度探索ループの最小値
#define	ANGLE_MAX	20						//角度探索ループの最大値
#define ANGLE_UNIT	0.01					//角度の最小単位(degree)
#define NORMALIZE_QL	8192				//感度のQL値の基準
#define AVG_MAIN_SIZE	400					//ゲイン用平均値算出主走査サイズ	ビニングのため1/2
#define AVG_SUB_SIZE	400					//ゲイン用平均値算出副走査サイズ	ビニングのため1/2
#define GAIN_MEDIAN_SIZE	33				//ゲイン用メディアンサイズ
#define DEF_MEDIAN_SIZE		33				//副方向メディアンサイズ
#define DEF1_TH			500					//欠陥検出1閾値

CCBCTCalib::CCBCTCalib() {
	// TODO 自動生成されたコンストラクター・スタブ

}

CCBCTCalib::~CCBCTCalib() {
	// TODO Auto-generated destructor stub
}
/* -------------------------------------------------------------------
* @brief	オフセットデータの生成
* @param	inFileName		入力ファイル名　このファイル名に001.tma,002.tmaを付けて読み込む
* @param	outFileName		出力ファイル名　このファイル名に.tmaを付けて書き込む
* @retval   true:成功 /false:失敗
* @date     2021/04/20	FF T.Taneichi
 --------------------------------------------------------------------- */
bool CCBCTCalib::MakeOffsetData(string inFileName, string outFileName){

	TTMA	outImg;		//読込画像データ

	bool bRet = getAverageImage(inFileName, outImg, "MakeOffsetData()");
	if (!bRet){
		return false;
	}

	outImg.SaveTmaTdt(outFileName + "_Org.tma");

	//1/2縮小する
	outImg.Div(2,2);

	outImg.SaveTmaTdt(outFileName + ".tma");

	printf("---- Offset Data is OK. ----\n");


	return true;
}
/* -------------------------------------------------------------------
* @brief	ゲインデータの生成
* @param	inFileName		入力ファイル名　このファイル名に01.tdt,02.tdtを付けて読み込む
* @param	outFileName		出力ファイル名　このファイル名に.tdtを付けて書き込む
* @param	offsetImg		オフセットデータ(ビニング前のデータ)
* @retval   true:成功 /false:失敗
* @date     2021/04/20	FF T.Taneichi
 --------------------------------------------------------------------- */
bool CCBCTCalib::MakeGainData(string inFileName, string outFileName, TTMA &offsetImg, float &sens){

	TTMA	outImg;		//読込画像データ

	bool bRet = getAverageImage(inFileName, outImg, "MakeGainData()");
	if (!bRet){
		return false;
	}

	//オフセットを減算する
	for(int i=0; i < outImg.SubPixel; i++){
		for(int j=0; j < outImg.MainPixel; j++){
			int QL = outImg.Dt[i][j] - offsetImg.Dt[i][j];
			if (QL < 0){
				outImg.Dt[i][j] = 0;
			} else if (QL > 65535){
				outImg.Dt[i][j] = 65535;
			} else {
				outImg.Dt[i][j] = QL;
			}
		}
	}

	//平均値を求める
	int iStartM, iStartS;
	iStartM = (outImg.MainPixel - AVG_MAIN_SIZE) / 2;
	iStartS = (outImg.SubPixel - AVG_SUB_SIZE) / 2;
	TTMA avgImg = outImg;
	avgImg.Format(AVG_SUB_SIZE, AVG_MAIN_SIZE, -1*iStartS, -1*iStartM);
	avgImg.Median(1,GAIN_MEDIAN_SIZE);
	avgImg.Median(GAIN_MEDIAN_SIZE,1);
	float dAve = (float)avgImg.dAve();
	sens = dAve / NORMALIZE_QL;		//感度

	for(int i=0; i < outImg.SubPixel; i++){
		for(int j=0; j < outImg.MainPixel; j++){
			if (outImg.Dt[i][j] != 0){
				int Dt = outImg.Dt[i][j];
				int QL = (dAve * NORMALIZE_QL) / Dt;
				if (QL > 65534){
					outImg.Dt[i][j] = 65535;
				} else {
					outImg.Dt[i][j] = QL;
				}
				if (i == 40 && j == 40){
					printf("QL:%d outImg:%d dAve:%4.3lf\n",QL, Dt, dAve);
				}
			}
		}
	}

	outImg.SaveTmaTdt(outFileName + "_Org.tma");

	//画像を1/2縮小する
	outImg.Div(2,2);

	outImg.SaveTmaTdt(outFileName + ".tma");

	printf("---- Gain Data is OK. ----\n");

	return true;
}
/* -------------------------------------------------------------------
* @brief	感度情報ファイルの生成
* @param	inFileName		出力ファイル名
* @param	sensL			左側パネルの感度情報
* @param	sensR			右側パネルの感度情報
* @retval   true:成功 /false:失敗
* @date     2021/05/10	FF T.Taneichi
 --------------------------------------------------------------------- */
bool CCBCTCalib::SaveSens(string inFileName, float sensL, float sensR){

	FILE *fp;

	if ((fp = fopen(inFileName.c_str(), "w")) == NULL) {
		printf("file open error!! %s\n", inFileName.c_str());
		return false;
	}

	fprintf(fp,"%6.3f %6.3f\n", sensL, sensR);

	fclose(fp);	/* (5)ファイルのクローズ */

	return true;
}
//感度情報ファイルの読み込み
bool CCBCTCalib::LoadSensitivity(string inFileName, float *sensL, float *sensR){

	FILE *fp;


	if ((fp = fopen(inFileName.c_str(), "r")) == NULL) {
		int number = errno;
		printf("file open error!! %s\n", inFileName.c_str());
		printf("errno:%d %s\n", number, strerror(number));
		return false;
	}

	fscanf(fp,"%f %f", sensL, sensR);

	printf("SensL:%3.4lf SEnsR:%3.4lf\n",*sensL, *sensR);

	fclose(fp);	/* (5)ファイルのクローズ */

	return true;


}
/* -------------------------------------------------------------------
* @brief	欠陥データの生成
* @param	inPlaneLowFile	入力ファイル名　このファイル名に01.tdt,02.tdtを付けて読み込む
* @param	outFileName		出力ファイル名　このファイル名に.tdtを付けて書き込む
* @param	offsetImg		オフセットデータ
* @param	gainImg			ゲインデータ
* @retval   true:成功 /false:失敗
* @date     2021/04/27	FF T.Taneichi
 --------------------------------------------------------------------- */
bool CCBCTCalib::MakeDefectData(string inPlaneLowFile, string outFile, TTMA &offsetImg, TTMA &gainImg){

	TTMA	planeLowImg;		//読込画像データ

	bool bRet = getAverageImage(inPlaneLowFile, planeLowImg, "MakeDefectData()");
	if (!bRet){
		return false;
	}

	//オフセットを減算する
	for(int i=0; i < planeLowImg.SubPixel; i++){
		for(int j=0; j < planeLowImg.MainPixel; j++){
			int QL = planeLowImg.Dt[i][j] - offsetImg.Dt[i][j];
			if (QL < 0){
				planeLowImg.Dt[i][j] = 0;
			} else if (QL > 65535){
				planeLowImg.Dt[i][j] = 65535;
			} else {
				planeLowImg.Dt[i][j] = QL;
			}
		}
	}

//	planeLowImg.SaveTmaTdt("LowAvg.tma");		//中間データ保存。のちに削除する。

	TTMA defImg1;		//欠陥マップ1
	TTMA defImg2;		//欠陥マップ2
	TTMA defImg3;		//欠陥マップ3

	if (!makeDefMap1(planeLowImg, offsetImg, defImg1)){
		printf("makeDefMap1() Error!!!\n");
		return false;
	}

	if (!makeDefMap2(planeLowImg, defImg2)){
		printf("makeDefMap2() Error!!!\n");
		return false;
	}

	if (!makeDefMap3(planeLowImg, offsetImg, gainImg, defImg3)){
		printf("makeDefMap3() Error!!!\n");
		return false;
	}


	//1/2縮小した画像に格納する
	TTMA outImg = defImg1;
	outImg.Div(2,2);

	//4画素のうちのどれかが欠陥であれば、欠陥とする。
	for(int i=0;i < outImg.SubPixel;i++){
		for(int j=0;j < outImg.MainPixel;j++){
			if (defImg1.Dt[i*2][j*2] != 0 || defImg2.Dt[i*2][j*2] != 0 || defImg3.Dt[i*2][j*2] != 0 ||
					defImg1.Dt[i*2+1][j*2] != 0 || defImg2.Dt[i*2+1][j*2] != 0 || defImg3.Dt[i*2+1][j*2] != 0 ||
					defImg1.Dt[i*2][j*2+1] != 0 || defImg2.Dt[i*2][j*2+1] != 0 || defImg3.Dt[i*2][j*2+1] != 0 ||
					defImg1.Dt[i*2+1][j*2+1] != 0 || defImg2.Dt[i*2+1][j*2+1] != 0 || defImg3.Dt[i*2+1][j*2+1] != 0){
				outImg.Dt[i][j] = 30000;
			}
		}
	}

	//outImg.SaveTmaTdt("DefOrg.tma");

	if (!makeLineDefPaint(outImg)){
		printf("makeLineDefPaint() Error!!!\n");
		return false;
	}

	outImg.SaveTmaTdt(outFile + ".tma");

	printf("---- Defect Data is OK. ----\n");

	return true;
}
/* -------------------------------------------------------------------
* @brief	位置ずれデータの作成
* @param	inLImgFileName		左側のマーカ画像ファイル名
* @param	inRImgFileName		右側のマーカ画像ファイル名
* @param	LOffset				左側のオフセット画像
* @param	ROffset				右側のオフセット画像
* @param	LGain				左側のゲイン画像
* @param	RGain				右側のゲイン画像
* @param	thick				パネルの重なりの厚み
* @param	focus				パネルの中心に対する焦点座標
* @param	out_Location		位置ずれ検出結果
* @retval   true:成功 /false:失敗
* @date     2021/05/10	FF T.Taneichi
 --------------------------------------------------------------------- */
bool CCBCTCalib::MakePositionData(string inLImgFileName, string inRImgFileName, TTMA &LOffset, TTMA &ROffset,
		  TTMA &LGain, TTMA &RGain, float sensL, float sensR, float thick, SVec3D focus,
		  ST_DIFF_LOCATION &out_Location){

	bool bRet = false;

	TTMA LImg;		//左画像
	TTMA RImg;		//右画像
	LImg.ReadTma(inLImgFileName.c_str());
	RImg.ReadTma(inRImgFileName.c_str());

	//画像を1/2縮小する
	LImg.Div(2,2);
	RImg.Div(2,2);

	TTMA LCrrImg = LImg;	//左補正画像
	TTMA RCrrImg = RImg;	//右補正画像

	//ゲイン&オフセット
	for(int i=0;i<LCrrImg.SubPixel;i++){
		for(int j=0;j<LCrrImg.MainPixel;j++){
			//float QL = 65535 -((LImg.Dt[i][j] - LOffset.Dt[i][j])*LGain.Dt[i][j] / sensL /2048);
			float QL = ((LImg.Dt[i][j] - LOffset.Dt[i][j])*LGain.Dt[i][j] / sensL /2048);
			if (QL < 0){
				LCrrImg.Dt[i][j] = 0;
			} else if (QL > 65535){
				LCrrImg.Dt[i][j] = 65535;
			} else {
				LCrrImg.Dt[i][j] = (unsigned short)QL;
			}
		}
	}
	for(int i=0;i<RCrrImg.SubPixel;i++){
		for(int j=0;j<RCrrImg.MainPixel;j++){
			//float QL = 65535-((RImg.Dt[i][j] - ROffset.Dt[i][j])*RGain.Dt[i][j] / sensR /2048);
			float QL = ((RImg.Dt[i][j] - ROffset.Dt[i][j])*RGain.Dt[i][j] / sensR /2048);
			if (QL < 0){
				RCrrImg.Dt[i][j] = 0;
			} else if (QL > 65535){
				RCrrImg.Dt[i][j] = 65535;
			} else {
				RCrrImg.Dt[i][j] = (unsigned short)QL;
			}
		}
	}

	//R側を180°回転
	RCrrImg.Rotate(180);
	RImg.Rotate(180);

	//画像の切り出し
	TTMA LeftCutImg = LCrrImg;
	TTMA RightCutImg = RCrrImg;

	LeftCutImg.Format(LeftCutImg.SubPixel, CUT_IMG_WIDTH, 0, 0);
	//LeftCutImg.SaveTmaTdt("analysis/LCut.tma");

	RightCutImg.Format(RightCutImg.SubPixel, CUT_IMG_WIDTH, 0, CUT_IMG_WIDTH-RightCutImg.MainPixel);
	RImg.Format(RImg.SubPixel, CUT_IMG_WIDTH, 0, CUT_IMG_WIDTH-RImg.MainPixel);
	RightCutImg.SaveTmaTdt("analysis/RCut.tma");

	//右側を拡大する
	float mag = (focus.z + thick) / focus.z;
	out_Location.lowP_Mag = mag;
	unsigned short avg = RightCutImg.Average();
	RightCutImg.Scaling(RightCutImg.MainPixel/2, RightCutImg.SubPixel/2, mag, mag, avg);
	RightCutImg.SaveTmaTdt("analysis/RCutMag.tma");

	//メディアンをかける
	TTMA LCutMed = LeftCutImg;
	TTMA RCutMed = RightCutImg;
	LCutMed.Median(MARK_DEL_MEDIAN,1);
	RCutMed.Median(MARK_DEL_MEDIAN,1);
	//LCutMed.SaveTmaTdt("analysis/LCutMed.tma");
	//RCutMed.SaveTmaTdt("analysis/RCutMed.tma");

	//メディアン画像から元画像を引く
	TTMA LCutMark = LCutMed;
	TTMA RCutMark = RCutMed;
	LCutMark -= LeftCutImg;
	RCutMark -= RightCutImg;
//	LCutMark.SaveTmaTdt("tempImg/LCutMark.tma");
//	RCutMark.SaveTmaTdt("tempImg/RCutMark.tma");

	//オーバラップ位置の検出
	bRet = CalcImgDiffPosition(LCutMark, RCutMark, out_Location.overlay_X, out_Location.overlay_Y, out_Location.overlayAngle);

	//段差位置の検出
	bRet |= CalcDiffLine(RImg, out_Location.diffAngle, out_Location.diffStartPos);
//	printf("Diff X pos:%f\n",out_Location.diffStartPos);
//	printf("MainPixel:%d CUT_IMG:%d\n",RCrrImg.MainPixel, CUT_IMG_WIDTH);
	out_Location.diffStartPos += RCrrImg.MainPixel - CUT_IMG_WIDTH;

	return bRet;
}
//重なり位置/段差位置情報ファイルの生成
bool CCBCTCalib::SaveOverlapPos(string inFileName, ST_DIFF_LOCATION pos){

	FILE *fp;

	if ((fp = fopen(inFileName.c_str(), "w")) == NULL) {
		printf("file open error!! %s\n", inFileName.c_str());
		return false;
	}

	fprintf(fp,"%5.2lf %5.2lf %5.3f\n", pos.overlay_X, pos.overlay_Y, pos.overlayAngle);
	fprintf(fp,"%5.3f %5.3f\n", pos.diffStartPos, pos.diffAngle);
	fprintf(fp,"%8.5lf\n",pos.lowP_Mag);

	fclose(fp);	/* (5)ファイルのクローズ */

	return true;
}
//重なり位置/段差位置情報ファイルの読み込み
bool CCBCTCalib::LoadOverlapPos(string inFileName, ST_DIFF_LOCATION *pos){

	FILE *fp;

	if ((fp = fopen(inFileName.c_str(), "r")) == NULL) {
		printf("file open error!! %s\n", inFileName.c_str());
		return false;
	}

	fscanf(fp,"%f %f %f", &pos->overlay_X, &pos->overlay_Y, &pos->overlayAngle);
	fscanf(fp,"%f %f", &pos->diffStartPos, &pos->diffAngle);
	fscanf(fp,"%f", &pos->lowP_Mag);
//	printf("x:%f y:%f angle:%f\n",pos->overlay_X, pos->overlay_Y, pos->overlayAngle);
//	printf("spos:%f angle:%f\n",pos->diffStartPos, pos->diffAngle);
//	printf("lowP_mag:%f\n",pos->lowP_Mag);

	fclose(fp);

	return true;
}
/* -------------------------------------------------------------------
* @brief	ファイルを読み込み、平均化した画像を戻す
* @param	inFileName		入力ファイル名　このファイル名に001.tma,002.tmaを付けて読み込む
* @param	outImg			出力画像
* @retval   true:成功 /false:失敗
* @date     2021/04/27	FF T.Taneichi
 --------------------------------------------------------------------- */
bool CCBCTCalib::getAverageImage(string inFileName, TTMA &outImg, string WarningString){

	TTMA	inputImg;		//読込画像データ

	string readFileName = inFileName + "001.tma";
	bool bRet = inputImg.ReadTma(readFileName, false);
	if (!bRet){
		printf("%s input File Error!!! %s\n",WarningString.c_str(), readFileName.c_str());
		return false;
	}
	printf("%s read file\n", readFileName.c_str());

	outImg = inputImg;

	float *array = new float[inputImg.SubPixel * inputImg.MainPixel];
	for(int i=0; i < inputImg.SubPixel; i++){
		for(int j=0; j < inputImg.MainPixel; j++){
			array[inputImg.MainPixel*i + j] = inputImg.Dt[i][j];
		}
	}

	int addedNum = 1;

	char fileName[256];
	for(int k=2;k<999;k++){
		std::stringstream ss;
		ss << std::setw(3) << std::setfill('0') << k << ".tma";
		bRet = inputImg.ReadTma(inFileName + ss.str(), false);
		if (!bRet){
			break;		//ファイルが存在しなくなったら、抜ける
		}
		printf("%s read file\n", (inFileName + ss.str()).c_str());
		for(int i=0; i < inputImg.SubPixel; i++){
			for(int j=0; j < inputImg.MainPixel; j++){
				array[inputImg.MainPixel*i + j] += inputImg.Dt[i][j];
			}
		}
		addedNum++;
	}

	//平均化
	for(int i=0; i < outImg.SubPixel; i++){
		for(int j=0; j < outImg.MainPixel; j++){
			array[outImg.MainPixel*i + j] /= addedNum;
			if (array[outImg.MainPixel*i + j] < 0.0){
				outImg.Dt[i][j] = 0;
			} else if (array[outImg.MainPixel*i + j] > 65534){
				outImg.Dt[i][j] = 65535;
			} else {
				outImg.Dt[i][j] = array[outImg.MainPixel*i + j];
			}
		}
	}

	delete[] array;

	return true;

}
/* -------------------------------------------------------------------
* @brief	画素ずれ量の算出
* @param	LCutImg			左側の画像
* @param	RCutImg			右側の画像
* @param	x				オーバラップ量
* @param	y				副走査方向ずれ量(左に対する右のずれ量)
* @param	angle
* @retval   true:成功 /false:失敗
* @date     2021/04/21	FF T.Taneichi
* @date		2021/07/13	FF T.Taneichi	ずれ量をint->floatに変更
 --------------------------------------------------------------------- */
bool CCBCTCalib::CalcImgDiffPosition(TTMA &LCutImg, TTMA &RCutImg, float &x, float &y, float &angle){

	TTMA RotRImg = RCutImg;

	time_t startTime = time(NULL);

	FILE *fp;
	if ((fp = fopen("tempImg/diffPosRes.txt", "w")) == NULL) {
		printf("file open error!! %s\n", "tempImg/diffPosRes.txt");
	}

//	printf("LCutImg:%d,%d\n",LCutImg.SubPixel,LCutImg.MainPixel);
//	printf("RCutImg:%d,%d\n",RCutImg.SubPixel,RCutImg.MainPixel);
//	printf("Y_DIFF:%d-%d\n",Y_DIFF_MIN,Y_DIFF_MAX);
//	printf("X_DIFF:%d-%d\n",X_DIFF_MIN,X_DIFF_MAX);

	double minSSD;
	int base_x;			//1pixel単位の荒い探索x
	int base_y;			//1pixel単位の荒い探索y

	for (int rot = ANGLE_MIN; rot < ANGLE_MAX; rot++){
		//回転画像の作成
		RotRImg = RCutImg;
		RotateImage(RotRImg, (double)rot*ANGLE_UNIT);

		for(int k = Y_DIFF_MIN; k<Y_DIFF_MAX; k++){		//Y軸方向の探索
			for(int l = X_DIFF_MIN; l < X_DIFF_MAX; l++){
				double SSD = 0.0;
				for(int i=0;i<RotRImg.SubPixel;i++){
					for(int j=0;j<RotRImg.MainPixel;j++){
						double a = LCutImg.Dt[i][j];
						if ( (i+k) >= 0 && (i+k) < RotRImg.SubPixel &&
								(j+l) >= 0 && (j+l) < RotRImg.MainPixel){
							double b = RotRImg.Dt[i+k][j+l];
							SSD += (a-b)*(a-b);
							//SSD += abs(a-b);
						}
					}
				}
				if (k == Y_DIFF_MIN && l == X_DIFF_MIN && rot == ANGLE_MIN){
					minSSD = SSD;
					base_x = l;
					base_y = k;
					angle = (float)rot*ANGLE_UNIT;
				} else if (SSD < minSSD){
					minSSD = SSD;
					base_x = l;
					base_y = k;
					angle = (float)rot*ANGLE_UNIT;
					//RotRImg.SaveTmaTdt("RotR.tma");
					if (fp != NULL){
						fprintf(fp, "angle:%5.3f %d,%d SSD:%f **\n",angle, l,k,SSD/1000000000000);
					}
					//printf("angle:%5.3lf %d,%d SSD:%lf\n",angle, l,k,SSD/1000000000000);
				} else {
					if (fp != NULL){
						fprintf(fp, "angle:%5.3f %d,%d SSD:%f\n",angle, l,k,SSD/1000000000000);
					}
				}
			}
		}
	}

	//細かくシフトしてチェック
	RotRImg = RCutImg;
	RotateImage(RotRImg, (double)angle);
	TTMA ShiftImg = RotRImg;
	float delta_x, delta_y;
	if (X_DIFF_UNIT < 1.0 && Y_DIFF_UNIT < 1.0){
		int x_loop = (1.0 / X_DIFF_UNIT) * 2;
		int y_loop = (1.0 / Y_DIFF_UNIT) * 2;
		printf("xloop:%d yloop:%d\n",x_loop, y_loop);
		float shift_x, shift_y;
		for(int k = 0; k < x_loop; k++){
			for(int l = 0; l < y_loop; l++){
				shift_x = -1 + (float)X_DIFF_UNIT*k;
				shift_y = -1 + (float)Y_DIFF_UNIT*l;
				ShiftImg.ShiftXY(shift_x,shift_y,0);
				double SSD = 0.0;
				for(int i=0;i<ShiftImg.SubPixel;i++){
					for(int j=0;j<ShiftImg.MainPixel;j++){
						double a = LCutImg.Dt[i][j];
						if ( (i+k) >= 0 && (i+k) < ShiftImg.SubPixel &&
								(j+l) >= 0 && (j+l) < ShiftImg.MainPixel){
							double b = ShiftImg.Dt[i+k][j+l];
							SSD += (a-b)*(a-b);
							//SSD += abs(a-b);
						}
					}
				}
				if ((k == 0 && l == 0) || SSD < minSSD){
					minSSD = SSD;
					delta_x = shift_x;
					delta_y = shift_y;
				}
			}
		}
	}

	x = CUT_IMG_WIDTH - base_x + delta_x;
	y = base_y + delta_y;
	printf("Overlap:%5.2lf Y:%5.2lf angle:%f\n",x,y,angle);
	printf("Elapsed time:%.4f\n",difftime(time(NULL), startTime));

	if (fp != NULL){
		fprintf(fp, "Overlap:%5.2lf Y:%5.2lf angle:%f\n",x,y,angle);
		fprintf(fp, "Elapsed time:%.4f\n",difftime(time(NULL), startTime));
		fclose(fp);
	}

	return true;
}

/* -------------------------------------------------------------------
* @brief	欠陥マップ作成1(オフセットからの欠陥検出)
* 			点欠陥が抽出される。線欠陥は検出されない。
* @param	planeLowImg		オフセット補正後平均化画像
* @param	offsetImg		オフセット画像
* @param	outDefMap		欠陥データ
* @retval   true:成功 /false:失敗
* @date     2021/04/27	FF T.Taneichi
 --------------------------------------------------------------------- */
bool CCBCTCalib::makeDefMap1(TTMA &planeLowImg, TTMA &offsetImg, TTMA &outDefMap){

	outDefMap = planeLowImg;

	TTMA procImg = planeLowImg;

	//procImg = procImg + offsetImg;

	//オフセットデータの加算
	for(int i=0;i < procImg.SubPixel;i++){
		for(int j=0;j < procImg.MainPixel;j++){
			int QL = planeLowImg.Dt[i][j] + offsetImg.Dt[i][j];
			if (QL > 65535){
				QL = 65535;
			} else if (QL < 0){
				QL = 0;
			}
			procImg.Dt[i][j] = QL;
		}
	}

	//procImg.SaveTmaTdt("DefMap1_1.tma");

	//副メディアン
	TTMA medImg = procImg;
	medImg.Median(DEF_MEDIAN_SIZE,1);

	//medImg.SaveTmaTdt("DefMap1_2.tma");

	for(int i=0;i < procImg.SubPixel;i++){
		for(int j=0;j < procImg.MainPixel;j++){
			int QL = procImg.Dt[i][j] - medImg.Dt[i][j] +30000;
			if (QL > 65535){
				QL = 65535;
			} else if (QL < 0){
				QL = 0;
			}
			procImg.Dt[i][j] = QL;
		}
	}

	//procImg.SaveTmaTdt("DefMap1_3.tma");

	medImg = procImg;
	medImg.Median(1, DEF_MEDIAN_SIZE);

	//medImg.SaveTmaTdt("DefMap1_4.tma");

	for(int i=0;i < procImg.SubPixel;i++){
		for(int j=0;j < procImg.MainPixel;j++){
			int QL = procImg.Dt[i][j] - medImg.Dt[i][j] +30000;
			if (QL > 65535){
				QL = 65535;
			} else if (QL < 0){
				QL = 0;
			}
			procImg.Dt[i][j] = QL;
		}
	}

	//procImg.SaveTmaTdt("DefMap1_5.tma");

	for(int i=0;i < procImg.SubPixel;i++){
		for(int j=0;j < procImg.MainPixel;j++){
			int QL = procImg.Dt[i][j];
			if (QL > 30000 + DEF1_TH || QL < 30000 - DEF1_TH){
				outDefMap.Dt[i][j] = 30000;
			} else {
				outDefMap.Dt[i][j] = 0;
			}
		}
	}

	//procImg.SaveTmaTdt("DefOrg.tma");
	//outDefMap.SaveTmaTdt("DefMap1.tma");	//デバック用

	return true;
}
/* -------------------------------------------------------------------
* @brief	欠陥マップ作成2
*			オフセットデータで点欠陥を検出する。線欠陥は検出されない。
* @param	offsetImg		オフセット画像
* @param	outDefMap		欠陥データ
* @retval   true:成功 /false:失敗
* @date     2021/05/06	FF T.Taneichi
 --------------------------------------------------------------------- */
bool CCBCTCalib::makeDefMap2(TTMA &offsetImg, TTMA &outDefMap){

	outDefMap = offsetImg;	//サイズの反映用

	TTMA leakSepImg = offsetImg;

	if (!makePDLeakSeparate(offsetImg, leakSepImg)){
		printf("makePDLeakSeparate() ERROR!!!\n");
		return false;
	}

	//副メディアン
	TTMA medImg = leakSepImg;
	medImg.Median(DEF_MEDIAN_SIZE,1);

	//medImg.SaveTmaTdt("DefMap1_2.tma");

	for(int i=0;i < leakSepImg.SubPixel;i++){
		for(int j=0;j < leakSepImg.MainPixel;j++){
			int QL = leakSepImg.Dt[i][j] - medImg.Dt[i][j] +30000;
			if (QL > 65535){
				QL = 65535;
			} else if (QL < 0){
				QL = 0;
			}
			leakSepImg.Dt[i][j] = QL;
		}
	}

	//procImg.SaveTmaTdt("DefMap1_3.tma");

	medImg = leakSepImg;
	medImg.Median(1, DEF_MEDIAN_SIZE);

	//medImg.SaveTmaTdt("DefMap1_4.tma");

	for(int i=0;i < leakSepImg.SubPixel;i++){
		for(int j=0;j < leakSepImg.MainPixel;j++){
			int QL = leakSepImg.Dt[i][j] - medImg.Dt[i][j] +30000;
			if (QL > 65535){
				QL = 65535;
			} else if (QL < 0){
				QL = 0;
			}
			leakSepImg.Dt[i][j] = QL;
		}
	}

	//procImg.SaveTmaTdt("DefMap1_5.tma");

	for(int i=0;i < leakSepImg.SubPixel;i++){
		for(int j=0;j < leakSepImg.MainPixel;j++){
			int QL = leakSepImg.Dt[i][j];
			if (QL > 30000 + DEF1_TH || QL < 30000 - DEF1_TH){
				outDefMap.Dt[i][j] = 30000;
			} else {
				outDefMap.Dt[i][j] = 0;
			}
		}
	}

	//outDefMap.SaveTmaTdt("DefMap2.tma");	//デバック用

	return true;
}
/* -------------------------------------------------------------------
* @brief	欠陥マップ作成3
*			線欠陥のマップを作成する
* @param	planeLowImg		オフセット補正後平均化画像
* @param	offsetImg		オフセット画像
* @param	gainImg			ゲイン画像
* @param	outDefMap		欠陥データ
* @retval   true:成功 /false:失敗
* @date     2021/05/06	FF T.Taneichi
 --------------------------------------------------------------------- */
bool CCBCTCalib::makeDefMap3(TTMA &planeLowImg, TTMA &offsetImg, TTMA &gainImg, TTMA &outDefMap){

	outDefMap = planeLowImg;

	TTMA afterGainImg = planeLowImg;

	for(int i=0;i < afterGainImg.SubPixel;i++){
		for(int j=0;j < afterGainImg.MainPixel;j++){
			float QL = planeLowImg.Dt[i][j] * gainImg.Dt[i][j] / 32768;
			if (QL > 65535){
				QL = 65534;
			} else if (QL < 0){
				QL = 0;
			}
			afterGainImg.Dt[i][j] = (unsigned short)QL;
		}
	}

	//afterGainImg.SaveTmaTdt("AfterGain.tma");

	TTMA medImg = afterGainImg;
	medImg.Median(DEF_MEDIAN_SIZE,1);
	medImg.Median(1,DEF_MEDIAN_SIZE);

	for(int i=0;i < afterGainImg.SubPixel;i++){
		for(int j=0;j < afterGainImg.MainPixel;j++){
			int QL = afterGainImg.Dt[i][j] - medImg.Dt[i][j];
			if (QL > DEF1_TH || QL < -1* DEF1_TH){
				outDefMap.Dt[i][j] = 30000;
			} else {
				outDefMap.Dt[i][j] = 0;
			}
		}
	}

	//outDefMap.SaveTmaTdt("DefMap3.tma");

	return true;
}

//PDリーク分離処理
bool CCBCTCalib::makePDLeakSeparate(TTMA &offsetImg, TTMA &outLeakSepImg){

	//副メディアン
	TTMA medImg = offsetImg;
	medImg.Median(DEF_MEDIAN_SIZE,1);

	//主メディアン
	medImg.Median(1,DEF_MEDIAN_SIZE);

	float centerAve;	//中央の平均値

	//平均値を求める
	int iStartM, iStartS;
	iStartM = (medImg.MainPixel - AVG_MAIN_SIZE) / 2;
	iStartS = (medImg.SubPixel - AVG_SUB_SIZE) / 2;
	TTMA aveImg = medImg;
	TTMA centerAveImg = medImg;
	centerAveImg.Format(AVG_SUB_SIZE, AVG_MAIN_SIZE, -1*iStartS, -1*iStartM);
	float dAve = (float)centerAveImg.dAve();

	for(int i=0;i<aveImg.SubPixel;i++){
		for(int j=0;j<aveImg.MainPixel;j++){
			if ((medImg.Dt[i][j] - dAve) > 0.0){
				aveImg.Dt[i][j] = (medImg.Dt[i][j] - dAve);
			} else {
				aveImg.Dt[i][j] = 0;
			}
		}
	}

	float A_Value = 1.0;
	unsigned short B_Value = 0;

	//閾値付き減算処理
	TTMA diffImg = offsetImg;

	for(int i=0;i<diffImg.SubPixel;i++){
		for(int j=0;j<diffImg.MainPixel;j++){
			if (abs(offsetImg.Dt[i][j] - medImg.Dt[i][j]) < (A_Value * aveImg.Dt[i][j] + B_Value)){
				diffImg.Dt[i][j] = offsetImg.Dt[i][j];
			} else {
				diffImg.Dt[i][j] = medImg.Dt[i][j];
			}
		}
	}

	outLeakSepImg = offsetImg;

	for(int i=0;i<outLeakSepImg.SubPixel;i++){
		for(int j=0;j<outLeakSepImg.MainPixel;j++){
			int QL = offsetImg.Dt[i][j] - diffImg.Dt[i][j] + 30000;
			if (QL > 65535){
				QL = 65535;
			} else if (QL < 0){
				QL = 0;
			}
			outLeakSepImg.Dt[i][j] = QL;
		}
	}

	//outLeakSepImg.SaveTmaTdt("leakSepImg.tma");

	return true;
}
/* -------------------------------------------------------------------
* @brief	線欠陥塗りつぶし処理
*			同一Line/Pixel上に欠陥画素が50%以上ある場合には、線欠陥とみなしてすべて欠陥とする
* @param	defMap		欠陥データ
* @retval   true:成功 /false:失敗
* @date     2021/05/06	FF T.Taneichi
 --------------------------------------------------------------------- */
bool CCBCTCalib::makeLineDefPaint(TTMA &defMap){

	//線欠陥検出
	//副方向
	std::vector<int> subLineDef;
	for(int i=0;i < defMap.SubPixel;i++){
		int defNum = 0;
		for(int j=0;j < defMap.MainPixel;j++){
			if (defMap.Dt[i][j] != 0){
				defNum++;
			}
		}
		float rate = (float)defNum * 100 / defMap.MainPixel;
		if (rate > 50){	//50%以上が欠陥の場合、線欠陥と判断
			subLineDef.push_back(i);
		}
	}

	//主方向
	std::vector<int> mainLineDef;
	for(int j=0;j < defMap.MainPixel;j++){
		int defNum = 0;
		for(int i=0; i < defMap.SubPixel;i++){
			if (defMap.Dt[i][j] != 0){
				defNum++;
			}
		}
		float rate = (float)defNum * 100 / defMap.SubPixel;
		if (rate > 50){	//50%以上が欠陥の場合、線欠陥と判断
			mainLineDef.push_back(j);
		}
	}

	//副方向データ更新
	for(int i=0;i<subLineDef.size();i++){
		int subPos = subLineDef[i];
		for(int j=0;j<defMap.MainPixel;j++){
			defMap.Dt[subPos][j] = 30000;
		}
	}

	//主方向データ更新
	for(int j=0;j<mainLineDef.size();j++){
		int mainPos = mainLineDef[j];
		for(int i=0;i<defMap.SubPixel;i++){
			defMap.Dt[i][mainPos] = 30000;
		}
	}

//	defMap.SaveTmaTdt("DefLine.tma");

	return true;
}


//回転画像の作成
void CCBCTCalib::RotateImage(TTMA &OutputImg, double rotAngle){

	OutputImg.RotateAnyAngle(rotAngle, OutputImg.MainPixel-1, 0);
}
/* -------------------------------------------------------------------
* @brief	段差の線分の算出
* @param	RImg	段差検出する画像
* @param	angle	上下垂線に対してCW方向にどれだけ回転しているか(degree)
* @param	pix		上側の面で、右側から何pixel目に段差があるか
* @retval   true:成功 /false:失敗
* @date     2021/03/10	FF T.Taneichi
 --------------------------------------------------------------------- */
bool CCBCTCalib::CalcDiffLine(TTMA &RImg, float &angle, float &pix){

	unsigned short* rightData;

	//RImg.SaveTmaTdt("RImgCut.tma");

	//副走査方向にメディアンフィルタをかけ、マーカーを消す
	RImg.Median(51,1);
	//RImg.SaveTmaTdt("RImgCut2.tma");

	vector<int>	dQLMax(RImg.SubPixel);		//差分の絶対値の最大値
	vector<int> dQLMaxX(RImg.SubPixel);		//差分の絶対値が最大のX座標
	vector<int> dQLMaxX10(RImg.SubPixel);	//差分の絶対値が最大のX座標（10倍精度)

	int absdQLMax, diffQL, uodQLMax;

	//各Yについて段差のX座標を求める
	for(int i=0;i< RImg.SubPixel;i++){
		absdQLMax = 0;

		for(int j=0;j < RImg.MainPixel-1; j++){
			//diffQL = cutImg[i*DIFF_SEARCH_RANGE+j+1]-cutImg[i*DIFF_SEARCH_RANGE+j];
			diffQL = RImg.Dt[i][j+1] - RImg.Dt[i][j];
			if (abs(diffQL) > absdQLMax){
				absdQLMax = abs(diffQL);
				uodQLMax = diffQL;
				dQLMaxX[i] = j;
			}
		}
		dQLMax[i] = uodQLMax;
		if (absdQLMax < 500){	//段差削除
			dQLMaxX[i] = 0;
		}
	}

	//X座標の精度を10倍にする
	for(int i=0;i< RImg.SubPixel;i++){
		if (dQLMaxX[i] - 3 > 0 && dQLMaxX[i] + 4 < RImg.MainPixel -1){	//範囲内の場合のみ
			float wQL = 0;
			for(int j = 0; j< 9; j++){
				//wQL += cutImg[i*RImg.MainPixel+dQLMaxX[i]-4+j];
				wQL += RImg.Dt[i][dQLMaxX[i]-4+j];
			}
			wQL /= 9;
			//dQLMaxX10[i] = (int)((dQLMaxX[i] + (wQL - cutImg[i*DIFF_SEARCH_RANGE+dQLMaxX[i]])/dQLMax[i])*10);
			dQLMaxX10[i] = (int)((dQLMaxX[i] + (wQL - RImg.Dt[i][dQLMaxX[i]])/dQLMax[i])*10);
		} else {	//計算対象から除外
			dQLMaxX[i] = 0;
		}
	}

	//最小二乗法により、y=ax+bのaとbを求める
	float a,b;
	float avgX = 0;
	float avgY = 0;
	int addNum = 0;
	for(int i=0;i< RImg.SubPixel;i++){
		//printf("%d %d\n",i, dQLMaxX10[i]);
		if (dQLMaxX[i] != 0){
			avgX += i;
			avgY += dQLMaxX10[i] / 10;
			addNum++;
		}
	}
	avgX /= addNum;
	avgY /= addNum;

	float SumDenom = 0;	//分母
	float SumNumera = 0; //分子
	for(int i=0;i< RImg.SubPixel;i++){
		if (dQLMaxX[i] != 0){
			SumDenom += (i  - avgX)*(i - avgX);
			SumNumera += (i - avgX)*(dQLMaxX10[i]/10 - avgY);
		}
	}

	a = SumNumera / SumDenom;
	b = avgY - a * avgX;
	angle = (double)atanf(a) / M_PI * 180;
	pix = (RImg.MainPixel - CUT_IMG_WIDTH + (double)(b));

	printf("angle:%lf pix:%lf\n",angle, pix);

	return true;
}
