/*
 * CommonErr.h
 *
 *  Created on: 2021/06/22
 *      Author: 10097663
 */

#ifndef INCLUDE_COMMONERR_H_
#define INCLUDE_COMMONERR_H_

enum ImageProcErrType {
	IPET_SUCCEED = 				0,		//成功
	IPET_TRIG_PARAM_ERR = 		1,		//トリガファイルパラメータエラー
	IPET_INPUT_IMG_OPEN_ERR = 	2,		//入力画像ファイルオープンエラー
	IPET_OUTPUT_IMG_OPEN_ERR = 	3,		//出力画像ファイルオープンエラー
	IPET_INPUT_IMG_SIZE_ERR =	4,		//入力画像サイズエラー
	IPET_OVER_XRAY_ERR =		10,		//線量オーバーエラー
	IPET_UNDER_XRAY_ERR =		11,		//線量アンダーエラー
	IPET_OVERLAP_X_ERR =		20,		//左右画像X方向位置エラー
	IPET_OVERLAP_Y_ERR =		21,		//左右画像Y方向位置エラー
	IPET_OVERLAP_ANGLE_ERR =	22,		//左右画像回転方向位置エラー
	IPET_GENERAL_PURPOSE =		999,	//その他
};



#endif /* INCLUDE_COMMONERR_H_ */
