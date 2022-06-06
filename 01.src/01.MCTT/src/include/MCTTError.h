/*
 * MCTTError.h
 *
 *  Created on: 2021/06/28
 *      Author: 10097663
 */

#ifndef INCLUDE_MCTTERROR_H_
#define INCLUDE_MCTTERROR_H_

enum MCTTErrType {
	MCET_SUCCEED = 				0,		//成功
	MCET_TRIG_PARAM_ERR = 		1,		//トリガファイルパラメータエラー
	MCET_SE_CONNECT_ERR =		2,		//SE接続エラー
	MCET_MP_CONNECT_ERR =		3,		//MP接続エラー
	MCET_GENERAL_PURPOSE =		999,	//その他
};



#endif /* INCLUDE_MCTTERROR_H_ */
