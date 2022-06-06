/*
 * CMainLoop.h
 *
 *  Created on: 2021/06/22
 *      Author: 10097663
 */

#ifndef CMAINLOOP_H_
#define CMAINLOOP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

#include "Common3D.h"

class CMainLoop {
public:

	enum CalibType{
		ECLBT_OFFSET = 0,
		ECLBT_GAIN,
		ECLBT_SENS,
		ECLBT_DEFECT,
		ECLBT_POSITION,
		ECLBT_MAX
	};

	enum ImgCorrectType{
		EICT_OFFSET = 0,
		EICT_GAIN,
		EICT_DEFECT,
		EICT_STEP,
		EICT_MAX
	};

	CMainLoop();
	virtual ~CMainLoop();

	//メインループ処理実行
	void Execute();

	//トリガファイル検出処理
	void CheckTrigFile();

private:
	//キャリブレーションデータ生成
	void makeCalibData(FILE* fp);

	//キャリブレーショントリガファイル読み込み
	bool readCalibTrig(FILE* fp, bool *bCalib, char *CalibFolder, char *InputFolder);

	//オフセットデータ生成
	bool makeOffset(string strCalibFolder, string strImgFolder);

	//ゲインデータ生成
	bool makeGainAndSens(string strCalibFolder, string strImgFolder, bool bGain, bool bSens);

	//欠陥データ生成
	bool makeDefect(string strCalibFolder, string strImgFolder);

	//位置データ生成
	bool makePosition(string strCalibFolder, string strImgFolder, float thickness, SVec3D focusPos);

	//画像補正処理
	void correctImage(FILE* fp);

	//補正トリガファイル読み込み
	bool readCorrectTrig(FILE* fp, bool *bCorrect, string &strCalib, string &strInImgR, string &strInImgL, string &strOutImg, string &strOffR, string &strOffL);

	//結果ファイル作成
	void makeResultFile(string filePath, string headerComment, int errCode, string description);

	//デバッグトリガ処理
	void debugImg(FILE* fp);

};

#endif /* CMAINLOOP_H_ */
