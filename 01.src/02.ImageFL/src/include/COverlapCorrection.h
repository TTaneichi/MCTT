/*
 * COverlapCorrection.h
 * 2画像の重ね合わせ処理
 *  Created on: 2021/09/22
 *      Author: 10097663
 */

#ifndef COVERLAPCORRECTION_H_
#define COVERLAPCORRECTION_H_

#include <string>
#include "CTTMA.h"

using namespace std;

//重ね合わせ補正用パラメータ
typedef struct _SOverlapCorrectParam {
	float diffAngle;			//段差位置の角度（画素のならびに対してCW方向がプラス,degree)
	float diffStartPos;			//段差開始位置(1ライン目のピクセル位置)
	float overlayAngle;			//重なり位置の角度(左画像に対する右画像がCW方向がプラス,degree)
	int overlayX;				//重なり位置のX座標のずれ(
	int overlayY;				//重なり位置のY座標のずれ(
	float lowP_Mag;				//下側パネルの上側パネルに対する画像拡大率
} SOverlapCorrectParam;

//位置ずれ検出パラメータ
typedef struct _SOverlapDetailParam {
	float overlayX;			//位置ずれ量X pix
	float overlayY;			//位置ずれ量Y pix
	float lowP_Mag;
} SOverlapDetailParam;

class COverlapCorrection {
public:
	COverlapCorrection(SOverlapCorrectParam inParam);
	virtual ~COverlapCorrection();

	//補正用パラメータの設定処理
	void SetCorrectParameter(SOverlapCorrectParam inParam);

	//重ね合わせ実施
	bool ExecuteOverlap(TTMA in_LImg, TTMA in_RImg, TTMA &out_CombImg);

private:
	SOverlapCorrectParam m_inParam;		//補正用パラメータ

	//LP用オーバーラップ処理
	bool changeSensitivityLP(TTMA in_LImg, TTMA in_RImg, TTMA &out_RImg);

	//オーバラップ部の感度調整
	bool changeSensitivity(TTMA in_LImg, TTMA in_RImg, TTMA &out_RImg);

	//オーバラップ部の感度調整2
	bool changeSensitivity2(TTMA in_LImg, TTMA in_RImg, TTMA &out_RImg);

	//オーバラップ部の感度調整3
	bool changeSensitivity3(TTMA in_LImg, TTMA in_RImg, TTMA &out_RImg);

	//段差補正処理
	bool correctStepImage(TTMA in_RImg, TTMA &out_RImg);

	//LR画像の重ね合わせ処理
	bool overlapLR(TTMA in_LImg, TTMA in_RImg, TTMA &out_CombImg);

	//ななめエッジ検出時の感度補正値計算
	bool calcSensitivityInDiagonalEdge(bool *bEdge, TTMA in_RImg, TTMA &inout_Sens, int overlapSize);

	//オーバーラップ部の詳細の位置ずれ検出
	bool detectPosDiff(TTMA in_LImg, TTMA in_RImg, SOverlapDetailParam &out_detailParam);


	/////////////////////////////////////////////
	//        LP用の処理
	/////////////////////////////////////////////
	//左画像と右画像の感度違いを検出して左画像の感度を修正する
	bool LPAdjustEndPanelSensitivity2(TTMA in_LImg, TTMA in_RImg, TTMA &out_RImg);



};

#endif /* COVERLAPCORRECTION_H_ */
