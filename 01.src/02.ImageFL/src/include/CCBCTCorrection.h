/*
 * CCBCTCorrection.h
 *
 *  Created on: 2021/04/12
 *      Author: 10097663
 */

#ifndef CCBCTCORRECTION_H_
#define CCBCTCORRECTION_H_

#include <string>
#include "CTTMA.h"

using namespace std;

typedef struct _SCorrectEnable {
	bool bOffset;		//オフセット補正
	bool bGain;			//ゲイン補正
	bool bDefect;		//欠陥補正
	bool bRot;			//右側回転処理
	bool bCombiSens;	//つなぎ部感度補正
	bool bOverlap;		//重ね合わせ処理
	bool bTrimming;		//画像トリミング処理
} SCorrectEnable;

typedef struct _SCBCTCorrectData{
	float diffAngle;			//段差位置の角度（画素のならびに対してCW方向がプラス,degree)
	float diffStartPos;			//段差開始位置(1ライン目のピクセル位置)
	float overlayAngle;			//重なり位置の角度(左画像に対する右画像がCW方向がプラス,degree)
	int overlayX;				//重なり位置のX座標のずれ(
	int overlayY;				//重なり位置のY座標のずれ(
	float lowP_Mag;				//下側パネルの上側パネルに対する画像拡大率
	float LSensitivity;			//左画像の感度
	float RSensitivity;			//右画像の感度
	bool bOffsetOrgSize;		//オフセット画像サイズがビニング前のサイズか
	string L_InFileName;		//左側入力画像ファイル名
	string R_InFileName;		//右側入力画像ファイル名
	string L_OffsetFileName;	//左側オフセット画像ファイル名
	string R_OffsetFileName;	//右側オフセット画像ファイル名
	string L_GainFileName;		//左側ゲイン画像ファイル名
	string R_GainFileName;		//右側ゲイン画像ファイル名
	string L_DefFileName;		//左側欠陥画像ファイル名
	string R_DefFileName;		//右側欠陥画像ファイル名
	string OutFileName;			//出力ファイル名
} SCBCTCorrectData;

class CCBCTCorrection {
private:
	SCBCTCorrectData m_SCBCTCorrectData;
	TTMA	m_LCorrectImg;		//左補正処理画像
	TTMA	m_RCorrectImg;		//右補正処理画像
	TTMA	m_CombinationImg;	//結合画像

public:

	CCBCTCorrection();

	CCBCTCorrection(SCBCTCorrectData in_SCBCTCorrectData);

	virtual ~CCBCTCorrection();

	void SetCorrectData(SCBCTCorrectData in_SCBCTCorrectData);

	//欠陥補正データ生成
	void MakeDefectMap(string inBetaFile, string outDefectMapFile);

	//全補正処理実施
	bool CorrectImg(bool *bCorrect);

private:
	//処理対象画像の読み込み
	bool loadImage();

	//右側画像回転処理
	bool RotateImage();

	//オフセット&ゲイン補正
	bool exeOffsetAndGain(bool bOffset = true, bool bGain = true);

	//欠陥補正処理
	bool correctDefectImage(bool defect = true);

	//右側画像左端拡大処理
	bool scaleRImage();

	//段差補正処理
	bool correctStepImage();

	//段差補正処理2
	bool correctStepImage2();

	//段差補正処理3
	bool correctStepImage3();

	//シンチ段差補正
	bool correctSintiEdge();

	//つなぎ部感度調整
	bool adjustOverlapSensitivity();


	bool adjustOverlapSensitivity2();

	//重ね合わせ処理
	bool overlapImage();

	//トリミング処理
	bool trimmingImage();

	//段差中心Pixel座標を求める
	float getDiffCenterPos(float angleD, float topPix, int iSub);
};

#endif /* CCBCTCORRECTION_H_ */
