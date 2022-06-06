/*
 * CCBCTCalib.h
 *	CBCTのキャリブレーションデータ作成用クラス
 *  Created on: 2021/04/20
 *      Author: 10097663
 */

#ifndef CCBCTCALIB_H_
#define CCBCTCALIB_H_

#include "CTTMA.h"
#include "Common3D.h"
#include <string>
using namespace std;

class CCBCTCalib {
public:
	//位置ずれ情報
	typedef struct _ST_DIFF_LOCATION {
		float 	overlayAngle;	//画像の回転角度(degree)	左画像に対して右側の画像がCW方向でプラス
		float	overlay_X;		//画像のX重なり量 pixel
		float	overlay_Y;		//画像のデフォルト位置からのYずれ量 pixel
		float	lowP_Mag;		//下側パネルの上側パネルに対する拡大率
		float	diffAngle;		//段差の角度(degree)	画像に対してCW方向がプラス
		float	diffStartPos;	//段差の開始ピクセル位置(上側基準)
	} ST_DIFF_LOCATION;


	CCBCTCalib();
	virtual ~CCBCTCalib();

	//オフセットデータの生成
	bool MakeOffsetData(string inFileName, string outFileName);

	//ゲインデータの生成
	bool MakeGainData(string inFileName, string outFileName, TTMA &offsetImg, float &sens);

	//感度情報ファイルの生成
	bool SaveSens(string inFileName, float sensL, float sensR);

	//感度情報ファイルの読み込み
	bool LoadSensitivity(string inFileName, float *sensL, float *sensR);

	//欠陥データの生成
	bool MakeDefectData(string inPlaneLowFile, string outFile, TTMA &offsetImg, TTMA &gainImg);

	//位置ずれデータの作成
	bool MakePositionData(string inLImgFileName, string inRImgFileName, TTMA &LOffset, TTMA &ROffset,
						  TTMA &LGain, TTMA &RGain, float sensL, float sensR, float thick, SVec3D focus, ST_DIFF_LOCATION &out_Location);

	//重なり位置/段差位置情報ファイルの生成
	bool SaveOverlapPos(string inFileName, ST_DIFF_LOCATION pos);

	//重なり位置/段差位置情報ファイルの読み込み
	bool LoadOverlapPos(string inFileName, ST_DIFF_LOCATION *pos);

private:

	//ファイルを読み込み、平均化した画像を戻す
	bool getAverageImage(string inFileName, TTMA &outImg, string WarningString = "");

	//欠陥マップ作成1
	bool makeDefMap1(TTMA &planeLowImg, TTMA &offsetImg, TTMA &outDefMap);

	//欠陥マップ作成2
	bool makeDefMap2(TTMA &offsetImg, TTMA &outDefMap);

	//欠陥マップ作成3
	bool makeDefMap3(TTMA &planeLowImg, TTMA &offsetImg, TTMA &gainImg, TTMA &outDefMap);

	//PDリーク分離処理
	bool makePDLeakSeparate(TTMA &offsetImg, TTMA &outLeakSepImg);

	//線欠陥塗りつぶし処理
	bool makeLineDefPaint(TTMA &defMap);

	//線分の傾き角度(deg)の算出
	bool CalcDiffLine(TTMA &RImg, float &angle, float &pix);

	//画素ずれ量の算出
	bool CalcImgDiffPosition(TTMA &LCutImg, TTMA &RCutImg, float &x, float &y, float &angle);



	//回転画像の作成
	void RotateImage(TTMA &OutputImg, double rotAngle);

	//座標(x,y)のQL値を返す
	double GetQLInCoodinate(double x, double y, unsigned short* pInputImg);
};

#endif /* CCBCTCALIB_H_ */
