/*********************************************************************
 * @file   CRBTranslator.h
 * @brief  RasBoxの通信データ解析クラス
 * @note
 *
 * @author T.Taneichi
 * @date  2021/07/02 T.Taneichi
 * @license Copyright (C) 2021 FUJIFILM Corporation. All rights reserved.
 *********************************************************************/
#ifndef CRBTRANSLATOR_H_
#define CRBTRANSLATOR_H_

#include <stdio.h>
#include "Common.h"

//RasBoxコマンド定義
enum ERBCommandType {
	ERBCT_REQ_START_EXP	= 0,		//照射開始要求
	ERBCT_RES_COMP_EXP,				//照射完了応答
};

//照射開始要求パラメータ
typedef struct _SRBReqStartExp {
	bool bSW1;
	bool bSW2;
	bool bExpRes;
	unsigned char	SWInterval;			//照射時間(1単位で100msecなので、100msec-25600msec)
} SRBReqStartExp;

//撮影完了応答
typedef struct _SRBResCompExp {
	unsigned char	result;
	unsigned char	kV;
	unsigned short	mA;
	unsigned short	mSec;
} SRBResCompExp;

//RasBox通信データ
typedef	union {
	SRBReqStartExp	reqStartExp;
	SRBResCompExp	resCompEx;
} RBCmdData;

//RasBoxコマンド構造体
typedef struct _RBTransData {
	ERBCommandType	eCommandType;		//コマンドタイプ
	RBCmdData		data;				//データ構造体
	unsigned char	seqId;				//SequenceID
} RBTransData;


class CRBTranslator {
public:
	CRBTranslator();
	virtual ~CRBTranslator();

	//送信データ作成
	bool MakeMessage(unsigned char* out_makeData, int &dataLength, RBTransData in_cmdData);

	//受信データ解析
	bool TranslateMessage(unsigned char* in_transRawData, RBTransData &out_cmdData);

};

#endif /* CRBTRANSLATOR_H_ */
