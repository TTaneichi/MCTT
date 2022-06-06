/*********************************************************************
 * @file   CMPTranslator.h
 * @brief  MP通信の通信データ解析クラス
 * @note   ここに簡単な説明を記入する
 *
 * @author T.Taneichi
 * @date  2021/05/17 T.Taneichi
 * @license Copyright (C) 2021 FUJIFILM Corporation. All rights reserved.
 *********************************************************************/
#ifndef CMPTRANSLATOR_H_
#define CMPTRANSLATOR_H_

#include <stdio.h>
#include <string>
using namespace std;
#include "Common.h"

//MPコマンド定義
enum EMPCommandType {
	EMPCT_NTF_INIT = 0,		//初期化通知
	EMPCT_REQ_GET_BSTATE,	//ボード状態取得要求
	EMPCT_RSP_GET_BSTATE,	//ボード状態取得応答
	EMPCT_REQ_SET_CLOCK,	//時刻設定要求
	EMPCT_RSP_SET_CLOCK,	//時刻設定応答
	EMPCT_REQ_USERPASS,		//ユーザー名設定要求?
	EMPCT_RSP_USERPASS,		//ユーザー名設定応答?
	EMPCT_REQ_LED_OFF,		//LED消灯要求
	EMPCT_RSP_LED_OFF,		//LED消灯応答
	EMPCT_REQ_LIFECHECK,	//LifeCheck要求
	EMPCT_RSP_LIFECHECK,	//LifeCheck応答
	EMPCT_NTF_SW1_ON,		//SW1 On通知
	EMPCT_NTF_SW2_ON,		//SW2 On通知
	EMPCT_REQ_SHOTGO_ON,	//ShotGo On要求
	EMPCT_RSP_SHOTGO_ON,	//ShotGo On応答
	EMPCT_REQ_SHOTGO_OFF,	//ShotGo Off要求
	EMPCT_RSP_SHOTGO_OFF,	//ShotGo Off応答
	EMPCT_NTF_SW1_OFF,		//SW1 Off通知
	EMPCT_NTF_SW2_OFF,		//SW2 Off通知
	EMPCT_NTF_SHOTGO_OFF_B,	//ShotGo Off応答のミスデータ？
	EMPCT_NTF_3411,			//謎データ
};


//SEコマンド構造体
typedef struct _SIMTransData {
	EMPCommandType eCommandType;			//コマンドタイプ
} MPTransData;

class CMPTranslator {
public:
	CMPTranslator();
	virtual ~CMPTranslator();

	//送信データ作成
	bool MakeMessage(unsigned char* out_makeData, int &datalength, MPTransData in_cmdData);

	//受信データ解析
	bool TranslateMessage(unsigned char* in_transRawData, MPTransData &out_cmdData, int &useLength);

	//コマンド名取得
	string	GetCmdName(EMPCommandType in_CmdType);

private:
	//個別コマンドのデータ部作成
	bool makeEachCmdMessage(unsigned char* out_makeData, MPTransData in_cmdData);

	//個別コマンドのデータ解析
	bool transEachCmdMessage(unsigned char* in_transRawData, MPTransData &out_cmdData);

	//ボード状態取得要求データ作成
	void makeReqGetBState(unsigned char* out_makeData, MPTransData in_cmdData);

	//ボード状態取得応答データ作成
	void makeRspGetBState(unsigned char* out_makeData, MPTransData in_cmdData);

	//時刻設定要求データ作成
	void makeReqSetClock(unsigned char* out_makeData, MPTransData in_cmdData);

	//時刻設定応答データ作成
	void makeRspSetClock(unsigned char* out_makeData, MPTransData in_cmdData);

	//ユーザー名設定要求データ作成
	void makeReqUserPass(unsigned char* out_makeData, MPTransData in_cmdData);

	//ユーザー名設定応答データ作成
	void makeRspUserPass(unsigned char* out_makeData, MPTransData in_cmdData);

	//LED消灯要求データ作成
	void makeReqLedOff(unsigned char* out_makeData, MPTransData in_cmdData);

	//LED消灯応答データ作成
	void makeRspLedOff(unsigned char* out_makeData, MPTransData in_cmdData);


};

#endif /* CMPTRANSLATOR_H_ */
