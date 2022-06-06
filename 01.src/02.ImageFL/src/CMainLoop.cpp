/*
 * CMainLoop.cpp
 *
 *  Created on: 2021/06/22
 *      Author: 10097663
 */

#include <math.h>
#include <iostream>
using namespace std;
#include "CMainLoop.h"
#include "CommonErr.h"
#include "imageData.h"
#include "CTDAT.h"
#include "CTTMA.h"
#include "CImgDebug.h"
#include "CCBCTCalib.h"
#include "CCBCTCorrection.h"
#include <windows.h>
#include <direct.h>

#define CALIB_TRIG_FILE_PATH		"Trig/CalibTrigger.txt"			//キャリブレーション用トリガファイルパス
#define IMGCORRECT_TRG_FILE_PATH	"Trig/CorrectTrigger.txt"		//画像補正用トリガファイルパス
#define CALIB_RES_FILE_PATH			"Trig/CalibResult.txt"			//キャリブレーション用結果ファイルパス
#define IMGCORRECT_RES_FILE_PATH	"Trig/CorrectResult.txt"		//画像補正用結果ファイルパス
#define DEBUG_TRIG_FILE_PATH		"Trig/DebugTrigger.txt"			//デバッグ用トリガファイルパス

CMainLoop::CMainLoop() {
}

CMainLoop::~CMainLoop() {

}

//メインループ処理実行
void CMainLoop::Execute(){

	int loopCnt = 0;

	while(1){
		CheckTrigFile();

		Sleep(10);		//msecで指定
		loopCnt++;
		if (loopCnt > 6000){
			loopCnt = 0;
			printf("Checking Trigger File....\n");
		}
	}

}

//トリガファイル検出処理
void CMainLoop::CheckTrigFile(){

	FILE*	fp;

	//キャリブ用トリガファイルの検出処理
	if ((fp = fopen(CALIB_TRIG_FILE_PATH, "r")) != NULL){
		printf("--- detected CalibTrigger.txt ---\n");
		makeCalibData(fp);
		fclose(fp);
		remove(CALIB_TRIG_FILE_PATH);		//トリガファイル削除
		printf("--- Calibration has finished. ----\n");
	}

	//画像補正用トリガファイルの検出処理
	if ((fp = fopen(IMGCORRECT_TRG_FILE_PATH, "r")) != NULL){
		printf("--- detected CorrectTrigger.txt ---\n");
		correctImage(fp);
		fclose(fp);
		remove(IMGCORRECT_TRG_FILE_PATH);		//トリガファイル削除
		printf("--- Image correction has finished. ----\n");
	}

	//デバッグ用トリガファイルの検出処理
	if ((fp = fopen(DEBUG_TRIG_FILE_PATH, "r")) != NULL){
		printf("--- detected DebugTrigger.txt ---\n");
		debugImg(fp);
		fclose(fp);
		remove(DEBUG_TRIG_FILE_PATH);		//トリガファイル削除
		printf("--- Debug Imaging has finished. ----\n");
	}



}

//キャリブレーションデータ生成
void CMainLoop::makeCalibData(FILE* fp){

	bool bRet = true;

	CCBCTCalib cbctCalibObj;
	char CalibFolderName[256];
	char InputImgFolderName[256];
	memset(CalibFolderName,0,sizeof(CalibFolderName));
	memset(InputImgFolderName,0,sizeof(InputImgFolderName));
	bool bCalib[ECLBT_MAX];			//各キャリブレーションを実施するか

	bRet = readCalibTrig(fp, bCalib, CalibFolderName, InputImgFolderName);
	if (!bRet){
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_TRIG_PARAM_ERR, "TriggerFile Parameter Error");
	}

	string strCalibFolder = CalibFolderName;
	string strImgFolder = InputImgFolderName;

	if (bCalib[ECLBT_OFFSET]){	//オフセット
		bRet = makeOffset(strCalibFolder, strImgFolder);
		if (!bRet){
			return;
		}
	}

	if (bCalib[ECLBT_GAIN] || bCalib[ECLBT_SENS]){	//ゲイン&感度
		bRet = makeGainAndSens(strCalibFolder, strImgFolder, bCalib[ECLBT_GAIN], bCalib[ECLBT_SENS]);
		if (!bRet){
			return;
		}
	}

	if (bCalib[ECLBT_DEFECT]){	//欠陥
		bRet = makeDefect(strCalibFolder, strImgFolder);
		if (!bRet){
			return;
		}
	}

	float thickness = 1.0;
	SVec3D	focusPos;
	focusPos.x = 0.0;
	focusPos.y = 0.0;
	focusPos.z = 1162.0;

	if (bCalib[ECLBT_POSITION]){	//段差位置、オーバーラップ量
		bRet = makePosition(strCalibFolder, strImgFolder, thickness, focusPos);
		if (!bRet){
			return;
		}
	}

	//成功時
	makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_SUCCEED, "All calibration was successful.");

}
//キャリブレーショントリガファイル読み込み
bool CMainLoop::readCalibTrig(FILE* fp, bool *bCalib, char *CalibFolder, char *InputFolder){

	char buf[1024];
	int	iTemp;
	int readParamNum;
	unsigned long lineNum = 0;		//読み込み行数
	bool bRet = true;

	while( fgets(buf, 1024, fp) && bRet == true){
		for(int i=0;i<1024;i++){
			if (buf[i]== 0x0a){	//改行コード削除
				buf[i]= 0;
				break;
			}
		}
		if (buf[0] == '#'){		//コメント削除
			memset(buf,0,sizeof(buf));
			continue;
		}
		if (lineNum < ECLBT_MAX){		//各補正を実施するかの確認
			readParamNum = sscanf(buf,"%d", &iTemp);
			if (readParamNum == 0){
				printf(" Failed Reading Calib flag Type:%d!!!\n",lineNum);
				bRet = false;
			} else {
				bCalib[lineNum] = (iTemp == 0) ? false : true;
			}
		}
		switch(lineNum){
		case ECLBT_MAX:		//キャリブデータ出力フォルダ名
			readParamNum = sscanf(buf,"%s", CalibFolder);
			if (readParamNum == 0){
				printf(" Failed Reading Calib Folder Name!!!\n");
				bRet = false;
			}
			break;
		case (ECLBT_MAX+1):	//入力画像フォルダ名
		readParamNum = sscanf(buf,"%s", InputFolder);
			if (readParamNum == 0){
				printf(" Failed Reading Input Image Folder Name!!!\n");
				bRet = false;
			}
			break;
		}
		lineNum++;
	}

	if (lineNum < 7){
		printf(" Not enough parameters!!!\n");
		bRet = false;
	}

	printf("  Calib Trigger File read successfully.\n");
	printf("  %d/%d/%d/%d/%d %s %s\n", bCalib[0],bCalib[1],bCalib[2],bCalib[3],bCalib[4],CalibFolder,InputFolder);

	return bRet;
}
//オフセットデータ生成
bool CMainLoop::makeOffset(string strCalibFolder, string strImgFolder){

	bool bRet;
	CCBCTCalib cbctCalibObj;
	string strInputImg;
	string strOutputImg;

	strInputImg = strImgFolder + "/Offset/Img_L";
	strOutputImg = strCalibFolder + "/Offset_L";
	bRet = cbctCalibObj.MakeOffsetData(strInputImg, strOutputImg);
	if (!bRet){
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_GENERAL_PURPOSE, "L Offset Calibration Error");
		return false;
	}
	strInputImg = strImgFolder + "/Offset/Img_R";
	strOutputImg = strCalibFolder + "/Offset_R";
	bRet = cbctCalibObj.MakeOffsetData(strInputImg, strOutputImg);
	if (!bRet){
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_GENERAL_PURPOSE, "R Offset Calibration Error");
		return false;
	}

	return true;
}

//ゲインデータ生成
bool CMainLoop::makeGainAndSens(string strCalibFolder, string strImgFolder, bool bGain, bool bSens){

	bool bRet;
	CCBCTCalib cbctCalibObj;
	TTMA OffsetImg;

	string strInputImg;
	string strOutputImg;
	float sensL = 1.0;
	float sensR = 1.0;

	//TODO ゲイン保存せず、感度のみ更新する場合の処理を追加する
	strInputImg = strCalibFolder + "/Offset_L_Org.tma";
	bRet = OffsetImg.ReadTma(strInputImg);
	if (!bRet){
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_INPUT_IMG_OPEN_ERR, "L Gain Calibration Error");
		return false;
	}
	strInputImg = strImgFolder + "/Plane/Img_L";
	strOutputImg = strCalibFolder + "/Gain_L";
	bRet = cbctCalibObj.MakeGainData(strInputImg, strOutputImg, OffsetImg, sensL);
	if (!bRet){
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_GENERAL_PURPOSE, "L Gain Calibration Error");
		return false;
	}

	strInputImg = strCalibFolder + "/Offset_R_Org.tma";
	bRet = OffsetImg.ReadTma(strInputImg);
	if (!bRet){
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_INPUT_IMG_OPEN_ERR, "R Gain Calibration Error");
		return false;
	}
	strInputImg = strImgFolder + "/Plane/Img_R";
	strOutputImg = strCalibFolder + "/Gain_R";
	bRet = cbctCalibObj.MakeGainData(strInputImg, strOutputImg, OffsetImg, sensR);
	if (!bRet){
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_GENERAL_PURPOSE, "R Gain Calibration Error");
		return false;
	}

	if (bSens){			//感度更新する場合
		string strOutputSensFile = strCalibFolder + "/Sens.txt";
		bRet = cbctCalibObj.SaveSens(strOutputSensFile, sensL, sensR);
	}

	return true;

}

//欠陥データ生成
bool CMainLoop::makeDefect(string strCalibFolder, string strImgFolder){

	bool bRet;
	CCBCTCalib cbctCalibObj;
	TTMA OffsetImg;
	TTMA GainImg;

	string strInputImg;
	string strOutputImg;

	//L側の欠陥データ生成
	strInputImg = strCalibFolder + "/Offset_L_Org.tma";
	bRet = OffsetImg.ReadTma(strInputImg);
	if (!bRet){
		printf("L Defect Calibration Offset Open Error\n");
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_INPUT_IMG_OPEN_ERR, "L Defect Calibration Offset Open Error");
		return false;
	}
	strInputImg = strCalibFolder + "/Gain_L_Org.tma";
	bRet = GainImg.ReadTma(strInputImg);
	if (!bRet){
		printf("L Defect Calibration Gain Open Error\n");
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_INPUT_IMG_OPEN_ERR, "L Defect Calibration Gain Open Error");
		return false;
	}
	strInputImg = strImgFolder + "/PlaneLow/Img_L";
	strOutputImg = strCalibFolder + "/Defect_L";
	bRet = cbctCalibObj.MakeDefectData(strInputImg, strOutputImg, OffsetImg, GainImg);
	if (!bRet){
		printf("L Defect Calibration Error\n");
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_GENERAL_PURPOSE, "L Defect Calibration Error");
		return false;
	}

	//R側の欠陥データ生成
	strInputImg = strCalibFolder + "/Offset_R_Org.tma";
	bRet = OffsetImg.ReadTma(strInputImg);
	if (!bRet){
		printf("R Defect Calibration Offset Open Error\n");
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_INPUT_IMG_OPEN_ERR, "R Defect Calibration Offset Open Error");
		return false;
	}
	strInputImg = strCalibFolder + "/Gain_R_Org.tma";
	bRet = GainImg.ReadTma(strInputImg);
	if (!bRet){
		printf("R Defect Calibration Gain Open Error\n");
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_INPUT_IMG_OPEN_ERR, "R Defect Calibration Gain Open Error");
		return false;
	}
	strInputImg = strImgFolder + "/PlaneLow/Img_R";
	strOutputImg = strCalibFolder + "/Defect_R";
	bRet = cbctCalibObj.MakeDefectData(strInputImg, strOutputImg, OffsetImg, GainImg);
	if (!bRet){
		printf("R Defect Calibration Error\n");
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_GENERAL_PURPOSE, "R Defect Calibration Error");
		return false;
	}

	return true;
}

//位置データ生成
bool CMainLoop::makePosition(string strCalibFolder, string strImgFolder, float thickness, SVec3D focusPos){

	bool bRet;
	CCBCTCalib cbctCalibObj;
	TTMA OffsetImg;
	TTMA GainImg;
	TTMA OffsetImg2;
	TTMA GainImg2;
	float sensL = 1.0;
	float sensR = 1.0;

	string strInputImg;
	string strOutputImg;

	strInputImg = strCalibFolder + "/Offset_L.tma";
	bRet = OffsetImg.ReadTma(strInputImg);
	if (!bRet){
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_INPUT_IMG_OPEN_ERR, "Position Data L Offset Open Error");
		return false;
	}
	strInputImg = strCalibFolder + "/Gain_L.tma";
	bRet = GainImg.ReadTma(strInputImg);
	if (!bRet){
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_INPUT_IMG_OPEN_ERR, "Position Data L Gain Open Error");
		return false;
	}

	strInputImg = strCalibFolder + "/Offset_R.tma";
	bRet = OffsetImg2.ReadTma(strInputImg);
	if (!bRet){
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_INPUT_IMG_OPEN_ERR, "Position Data R Offset Open Error");
		return false;
	}
	strInputImg = strCalibFolder + "/Gain_R.tma";
	bRet = GainImg2.ReadTma(strInputImg);
	if (!bRet){
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_INPUT_IMG_OPEN_ERR, "Position Data R Gain Open Error");
		return false;
	}

	string strTextFile = strCalibFolder + "/Sens.txt";
	bRet = cbctCalibObj.LoadSensitivity(strTextFile, &sensL, &sensR);
	CCBCTCalib::ST_DIFF_LOCATION locationData;

	strInputImg = strImgFolder + "/Marker/Img_L001.tma";
	string strInputImg2 = strImgFolder + "/Marker/Img_R001.tma";

	bRet = cbctCalibObj.MakePositionData(strInputImg, strInputImg2, OffsetImg, OffsetImg2,
			GainImg, GainImg2, sensL, sensR, thickness, focusPos, locationData);

	if (!bRet){
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_GENERAL_PURPOSE, "Position Data making Error");
		return false;
	}
	strTextFile = strCalibFolder + "/OverlapPos.txt";
	bRet = cbctCalibObj.SaveOverlapPos(strTextFile, locationData);
	if (!bRet){
		makeResultFile(CALIB_RES_FILE_PATH, "Calibration Result", IPET_GENERAL_PURPOSE, "Position Data saving Error");
		return false;
	}

	return true;
}

//	画像補正処理
//	2021/09/16	オフセットデータを指定できるように修正
void CMainLoop::correctImage(FILE* fp){

	bool bCorrect[EICT_MAX];			//各補正を実施するか
	string	strCalib;		//キャリブデータパス
	string	strImgR;		//撮影画像Rパス
	string	strImgL;		//撮影画像Lパス
	string	strOutImg;		//出力画像パス
	string	strOffR;		//オフセット画像Rパス
	string	strOffL;		//オフセット画像Lパス
	bool	bRet;

	bRet = readCorrectTrig(fp, bCorrect, strCalib, strImgR, strImgL, strOutImg, strOffR, strOffL);
	if (!bRet){
		makeResultFile(IMGCORRECT_RES_FILE_PATH, "Image Correcttion Result", IPET_TRIG_PARAM_ERR, "TriggerFile Parameter Error");
	}

	SCBCTCorrectData	CBCTParam;
	CCBCTCalib cbctCalibObj;
	float sensL,sensR;
	CCBCTCalib::ST_DIFF_LOCATION diffPos;

	string strLoadFile = strCalib + "\\Sens.txt";
	printf(">>>> %s\n",strLoadFile.c_str());
	bRet = cbctCalibObj.LoadSensitivity(strLoadFile, &sensL, &sensR);
	if (!bRet){
		makeResultFile(IMGCORRECT_RES_FILE_PATH, "Image Correct Result", IPET_GENERAL_PURPOSE, "No sens.txt");
		return;
	}

	strLoadFile = strCalib + "/OverlapPos.txt";
	bRet = cbctCalibObj.LoadOverlapPos(strLoadFile, &diffPos);
	if (!bRet){
		makeResultFile(IMGCORRECT_RES_FILE_PATH, "Image Correct Result", IPET_GENERAL_PURPOSE, "No OverlapPos.txt");
		return;
	}

	CBCTParam.diffAngle = diffPos.diffAngle;
	CBCTParam.diffStartPos = diffPos.diffStartPos;
	CBCTParam.overlayAngle = diffPos.overlayAngle;
	CBCTParam.overlayX = (int)diffPos.overlay_X;
	CBCTParam.overlayY = (int)diffPos.overlay_Y;
	CBCTParam.lowP_Mag = diffPos.lowP_Mag;
	CBCTParam.LSensitivity = sensL;
	CBCTParam.RSensitivity = sensR;
	CBCTParam.L_InFileName = strImgL;
	CBCTParam.R_InFileName = strImgR;
	if (strOffL.empty()){
		strLoadFile = strCalib + "/Offset_L.tma";
		CBCTParam.L_OffsetFileName = strLoadFile;
		CBCTParam.bOffsetOrgSize = false;
	} else {
		CBCTParam.L_OffsetFileName = strOffL;
		CBCTParam.bOffsetOrgSize = true;
	}
	if (strOffR.empty()){
		strLoadFile = strCalib + "/Offset_R.tma";
		CBCTParam.R_OffsetFileName = strLoadFile;
	} else {
		CBCTParam.R_OffsetFileName = strOffR;
	}
	strLoadFile = strCalib + "/Gain_L.tma";
	CBCTParam.L_GainFileName = strLoadFile;
	strLoadFile = strCalib + "/Gain_R.tma";
	CBCTParam.R_GainFileName = strLoadFile;
	strLoadFile = strCalib + "/Defect_L.tma";
	CBCTParam.L_DefFileName = strLoadFile;
	strLoadFile = strCalib + "/Defect_R.tma";
	CBCTParam.R_DefFileName = strLoadFile;
	CBCTParam.OutFileName = strOutImg;

	CCBCTCorrection CBCTCObj(CBCTParam);
	bRet = CBCTCObj.CorrectImg(bCorrect);
	if (!bRet){
		makeResultFile(IMGCORRECT_RES_FILE_PATH, "Image Correct Result", IPET_GENERAL_PURPOSE, "Correct error");
		return;
	}

	//成功時
	makeResultFile(IMGCORRECT_RES_FILE_PATH, "Image Correct Result", IPET_SUCCEED, "All image correction was successful.");

}
//補正トリガファイル読み込み
bool CMainLoop::readCorrectTrig(FILE* fp, bool *bCorrect, string &strCalib, string &strInImgR, string &strInImgL,
		string &strOutImg, string &strOffR, string &strOffL){

	char buf[1024];
	char path[1024];
	int	iTemp;
	int readParamNum;
	unsigned long lineNum = 0;		//読み込み行数
	bool bRet = true;

	while( fgets(buf, 1024, fp) && bRet == true){
		for(int i=0;i<1024;i++){
			if (buf[i]== 0x0a){	//改行コード削除
				buf[i]= 0;
				break;
			}
		}
		if (buf[0] == '#'){		//コメント削除
			memset(buf,0,sizeof(buf));
			continue;
		}
		if (lineNum < EICT_MAX){		//各補正を実施するかの確認
			readParamNum = sscanf(buf,"%d", &iTemp);
			if (readParamNum == 0){
				printf(" Failed Reading Correct flag Type:%d!!!\n",lineNum);
				bRet = false;
			} else {
				bCorrect[lineNum] = (iTemp == 0) ? false : true;
			}
		}
		switch(lineNum){
		case EICT_MAX:		//キャリブデータフォルダ名
			memset(path,0,sizeof(path));
			readParamNum = sscanf(buf,"%s", path);
			if (readParamNum == 0){
				printf(" Failed Reading Calib Folder Name!!!\n");
				bRet = false;
			} else {
				strCalib = path;
				printf("CalibPath: %s\n",path);
			}
			break;
		case (EICT_MAX+1):	//入力R画像
			memset(path,0,sizeof(path));
			readParamNum = sscanf(buf,"%s", path);
			if (readParamNum == 0){
				printf(" Failed Reading Input R Image path!!!\n");
				bRet = false;
			} else {
				strInImgR = path;
				printf("Input R Image: %s\n",path);
			}
			break;
		case (EICT_MAX+2):	//入力L画像
			memset(path,0,sizeof(path));
			readParamNum = sscanf(buf,"%s", path);
			if (readParamNum == 0){
				printf(" Failed Reading Input L Image path!!!\n");
				bRet = false;
			} else {
				strInImgL = path;
				printf("Input L Image: %s\n",path);
			}
			break;
		case (EICT_MAX+3):	//出力画像
			memset(path,0,sizeof(path));
			readParamNum = sscanf(buf,"%s", path);
			if (readParamNum == 0){
				printf(" Failed Reading Output Image path!!!\n");
				bRet = false;
			} else {
				strOutImg = path;
				printf("Output Image: %s\n",path);
			}
			break;
		case (EICT_MAX+4):	//入力Rオフセット画像(読めなくてもOK)
			memset(path,0,sizeof(path));
			readParamNum = sscanf(buf,"%s", path);
			if (readParamNum == 0){
				strOffR.clear();
			} else {
				strOffR = path;
			}
			break;
		case (EICT_MAX+5):	//入力Lオフセット画像(読めなくてもOK)
			memset(path,0,sizeof(path));
			readParamNum = sscanf(buf,"%s", path);
			if (readParamNum == 0){
				strOffL.clear();
			} else {
				strOffL = path;
			}
			break;
		}
		lineNum++;
	}

	if (lineNum < 8){
		printf(" Not enough parameters!!!\n");
		bRet = false;
	}


	return true;
}
//結果ファイル作成
void CMainLoop::makeResultFile(string filePath, string headerComment, int errCode, string description){

	FILE*	fp;

	//キャリブ用トリガファイルの検出処理
	if ((fp = fopen(filePath.c_str(), "w")) == NULL){
		printf(" %s file open error!!!\n",filePath.c_str());
		return;
	}

	fprintf(fp, "#%s\n", headerComment.c_str());
	fprintf(fp, "%d\n", errCode);
	fprintf(fp, "%s\n", description.c_str());

	fclose(fp);
}
//デバッグトリガ処理
void CMainLoop::debugImg(FILE* fp){

	char buf[1024];
	int	iTemp;
	int readParamNum;
	unsigned long lineNum = 0;		//読み込み行数
	bool bRet = true;

	while( fgets(buf, 1024, fp) && bRet == true){
		for(int i=0;i<1024;i++){
			if (buf[i]== 0x0a){	//改行コード削除
				buf[i]= 0;
				break;
			}
		}
		if (buf[0] == '#'){		//コメント削除
			memset(buf,0,sizeof(buf));
			continue;
		}

		readParamNum = sscanf(buf,"%d", &iTemp);
		if (readParamNum != 1){
			return;
		}

		CImgDebug	imgDebug;

		switch (iTemp){
		case 0:
			imgDebug.CheckScaling();
			break;
		case 1:
			imgDebug.CheckShiftXY();
			break;
		case 2:
			imgDebug.CheckMatrix();
			break;
		case 3:
			imgDebug.CheckCStatUtil();
			break;
		}
	}

}

