/*
 * CMPTranslator.cpp
 *
 *  Created on: 2021/05/17
 *      Author: 10097663
 */
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
using namespace std;
#include "CMPTranslator.h"

typedef struct _CommandIDSet {
	EMPCommandType commandType;		//コマンド種別
	unsigned short commandCode;		//コマンドコード
	unsigned short source;			//送信元
	unsigned short dist;			//送信先
	unsigned short length;			//データサイズ(1が2byte)
	string	cmdName;				//コマンド名
} CommandIDSet;

#define MC_ID	0x0500
#define	MP_ID	0x0700
#define NO_ID	0x0000

CommandIDSet commandIDtableMP[] =
{
	{ EMPCT_NTF_INIT,			0x1144,	MP_ID,	MC_ID,	120,	"NTF_INIT"},		//初期化通知
	{ EMPCT_REQ_GET_BSTATE,		0x0141, MC_ID,	MP_ID,	2,		"REQ_GET_BSTATE"},	//ボード状態取得要求
	{ EMPCT_RSP_GET_BSTATE,		0x8141,	MP_ID,	MC_ID,	5,		"RSP_GET_BSTATE"},	//ボード状態取得応答
	{ EMPCT_REQ_SET_CLOCK,		0x01A4,	MC_ID,	MP_ID,	14,		"REQ_SET_CLOCK"},	//時刻設定要求
	{ EMPCT_RSP_SET_CLOCK,		0x81A4,	MP_ID,	MC_ID,	3,		"RSP_SET_CLOCK"},	//時刻設定応答
	{ EMPCT_REQ_USERPASS,		0x01A6,	MC_ID,	MP_ID,	0x2E,	"REQ_USERPASS"},	//ユーザー名設定要求?
	{ EMPCT_RSP_USERPASS,		0x81A6,	MP_ID,	MC_ID,	3,		"RSP_USERPASS"},	//ユーザー名設定応答?
	{ EMPCT_REQ_LED_OFF,		0x0152,	MC_ID,	MP_ID,	3,		"REQ_LED_OFF"},		//LED消灯要求
	{ EMPCT_RSP_LED_OFF,		0x8152, MP_ID,	MC_ID,	2,		"RSP_LED_OFF"},		//LED消灯応答
	{ EMPCT_REQ_LIFECHECK,		0x0086,	MC_ID,	MP_ID,	2,		"REQ_LIFECHECK"},	//LifeCheck要求
	{ EMPCT_RSP_LIFECHECK,		0x8086,	MP_ID,	MC_ID,	3,		"RSP_LIFECHECK"},	//LifeCheck応答
	{ EMPCT_NTF_SW1_ON,			0x1131,	MP_ID,	MC_ID,	3,		"NTF_SW1_ON"},		//SW1 On通知
	{ EMPCT_NTF_SW2_ON,			0x1133,	MP_ID,	MC_ID,	3,		"NTF_SW2_ON"},		//SW2 On通知
	{ EMPCT_REQ_SHOTGO_ON,		0x0131,	MC_ID,	MP_ID,	3,		"REQ_SHOTGO_ON"},	//ShotGo On要求
	{ EMPCT_RSP_SHOTGO_ON,		0x8131,	MP_ID,	MC_ID,	3,		"RSP_SHOTGO_ON"},	//ShotGo On応答
	{ EMPCT_REQ_SHOTGO_OFF,		0x0132,	MC_ID,	MP_ID,	3,		"REQ_SHOTGO_OFF"},	//ShotGo Off要求
	{ EMPCT_RSP_SHOTGO_OFF,		0x8132,	MP_ID,	MC_ID,	3,		"RSP_SHOTGO_OFF"},	//ShotGo Off応答
	{ EMPCT_NTF_SW1_OFF,		0x1132,	MP_ID,	MC_ID,	3,		"NTF_SW1_OFF"},		//SW1 Off通知
	{ EMPCT_NTF_SW2_OFF,		0x8134,	MP_ID,	MC_ID,	3,		"NTF_SW2_OFF"},		//SW2 Off通知
	{ EMPCT_NTF_SHOTGO_OFF_B,	0x8132,	NO_ID,	NO_ID,	3,		"NTF_SHOTGO_OFFB"},	//ShotGo Off応答のミスデータ？
	{ EMPCT_NTF_3411,			0x1134,	NO_ID,	NO_ID,	3,		"NTF_3411"},		//謎コマンド
};



CMPTranslator::CMPTranslator() {
}

CMPTranslator::~CMPTranslator() {
}

//送信データ作成
bool CMPTranslator::MakeMessage(unsigned char* out_makeData, int &datalength, MPTransData in_cmdData){

	unsigned char* pData;		//ワーク用
	int commandIndex;
	//該当コマンドの検索
	bool bFound = false;

	for(commandIndex=0;commandIndex < sizeof(commandIDtableMP)/sizeof(CommandIDSet);commandIndex++){
		if (commandIDtableMP[commandIndex].commandType == in_cmdData.eCommandType){
			bFound = true;
			break;
		}
	}

	if (!bFound){
		return false;
	}

	//ここで個別のデータを入力する
	if (!makeEachCmdMessage(out_makeData, in_cmdData)){
		return false;
	}

	datalength = 8 + commandIDtableMP[commandIndex].length * 2;

	return true;
}

//受信データ解析
bool CMPTranslator::TranslateMessage(unsigned char* in_transRawData, MPTransData &out_cmdData, int &useLength){

	int cmdIndex;
	useLength = 0;
	unsigned char *pData = in_transRawData;
	unsigned short *pShortData;

	//該当コマンドの検索
	bool bFound = false;
	pShortData = (unsigned short*)&pData[8];
	unsigned short cmdCode = *pShortData;
	for(cmdIndex=0; cmdIndex < sizeof(commandIDtableMP)/sizeof(CommandIDSet); cmdIndex++){
		if (commandIDtableMP[cmdIndex].commandCode == cmdCode){
			bFound = true;
			break;
		}
	}

	if (!bFound){
		return false;
	}

	out_cmdData.eCommandType = commandIDtableMP[cmdIndex].commandType;
	useLength = 8 + commandIDtableMP[cmdIndex].length*2;

	//データの解析を行う処理を記述する
	if ( !transEachCmdMessage(pData, out_cmdData)){
		return false;
	}


	return true;
}
//コマンド名取得
string	CMPTranslator::GetCmdName(EMPCommandType in_CmdType){

	for(int commandIndex=0;commandIndex < sizeof(commandIDtableMP)/sizeof(CommandIDSet);commandIndex++){
		if (commandIDtableMP[commandIndex].commandType == in_CmdType){
			return commandIDtableMP[commandIndex].cmdName;
		}
	}

	return "";
}

//個別コマンドのデータ部作成
bool CMPTranslator::makeEachCmdMessage(unsigned char* out_makeData, MPTransData in_cmdData){

	unsigned char strNtfInit[] =
	{0x00, 0x07, 0x00, 0x05, 0x10, 0x01, 0x78, 0x00, 0x44, 0x11, 0x00, 0x00, 0x00, 0x00};

	unsigned char strLifeCheck[] =
	{0x00, 0x05, 0x00, 0x07, 0x10, 0x01, 0x02, 0x00, 0x86, 0x00, 0x00, 0x00};

	unsigned char strLifeCheckRsp[] =
	{0x00, 0x07, 0x00, 0x05, 0x10, 0x01, 0x03, 0x00, 0x86, 0x80, 0x00, 0x00, 0x00, 0x00};

	unsigned char strSw1On[] =
	{0x00, 0x07, 0x00, 0x05, 0x10, 0x01, 0x03, 0x00, 0x31, 0x11, 0x00, 0x00, 0x01, 0x00};

	unsigned char strSw2On[] =
	{0x00, 0x07, 0x00, 0x05, 0x10, 0x01, 0x03, 0x00, 0x33, 0x11, 0x00, 0x00, 0x01, 0x00};

	unsigned char strShotGoOn[] =
	{0x00, 0x05, 0x00, 0x07, 0x10, 0x01, 0x03, 0x00, 0x31, 0x01, 0x00, 0x00, 0x01, 0x00};

	unsigned char strShotGoOnRsp[] =
	{0x00, 0x07, 0x00, 0x05, 0x10, 0x01, 0x03, 0x00, 0x31, 0x81, 0x00, 0x00, 0x00, 0x00};

	unsigned char strShotGoOff[] =
	{0x00, 0x05, 0x00, 0x07, 0x10, 0x01, 0x03, 0x00, 0x32, 0x01, 0x00, 0x00, 0x01, 0x00};

	unsigned char strShotGoOffRsp[] =
	{0x00, 0x07, 0x00, 0x05, 0x10, 0x01, 0x03, 0x00, 0x32, 0x81, 0x00, 0x00, 0x00, 0x00};

	unsigned char strSw1Off[] =
	{0x00, 0x07, 0x00, 0x05, 0x10, 0x01, 0x03, 0x00, 0x32, 0x11, 0x00, 0x00, 0x01, 0x00};

	unsigned char strSw2Off[] =
	{0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x03, 0x00, 0x34, 0x81, 0x00, 0x00, 0x00, 0x00};

	unsigned char strShotGoOffRspB[] =
	{0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x03, 0x00, 0x32, 0x81, 0x00, 0x00, 0x00, 0x00};

	switch(in_cmdData.eCommandType){
	case EMPCT_NTF_INIT:		//初期化通知
		memset(out_makeData, 0x00, 248);		//このコマンドだけは一部のデータだけ入れて他は0とする。
		memcpy(out_makeData, strNtfInit, sizeof(strNtfInit));
		break;
	case EMPCT_REQ_GET_BSTATE:	//ボード状態取得要求
		makeReqGetBState(out_makeData, in_cmdData);
		break;
	case EMPCT_RSP_GET_BSTATE:	//ボード状態取得応答
		makeRspGetBState(out_makeData, in_cmdData);
		break;
	case EMPCT_REQ_SET_CLOCK:	//時刻設定要求
		makeReqSetClock(out_makeData, in_cmdData);
		break;
	case EMPCT_RSP_SET_CLOCK:	//時刻設定応答
		makeRspSetClock(out_makeData, in_cmdData);
		break;
	case EMPCT_REQ_USERPASS:	//ユーザー名設定要求?
		makeReqUserPass(out_makeData, in_cmdData);
		break;
	case EMPCT_RSP_USERPASS:	//ユーザー名設定応答?
		makeRspUserPass(out_makeData, in_cmdData);
		break;
	case EMPCT_REQ_LED_OFF:		//LED消灯要求
		makeReqLedOff(out_makeData, in_cmdData);
		break;
	case EMPCT_RSP_LED_OFF:		//LED消灯応答
		makeRspLedOff(out_makeData, in_cmdData);
		break;
	case EMPCT_REQ_LIFECHECK:	//LifeCheck要求
		memcpy(out_makeData, strLifeCheck, sizeof(strLifeCheck));
		break;
	case EMPCT_RSP_LIFECHECK:	//LifeCheck応答
		memcpy(out_makeData, strLifeCheckRsp, sizeof(strLifeCheckRsp));
		break;
	case EMPCT_NTF_SW1_ON:		//SW1 On通知
		memcpy(out_makeData, strSw1On, sizeof(strSw1On));
		break;
	case EMPCT_NTF_SW2_ON:		//SW2 On通知
		memcpy(out_makeData, strSw2On, sizeof(strSw2On));
		break;
	case EMPCT_REQ_SHOTGO_ON:	//ShotGo On要求
		memcpy(out_makeData, strShotGoOn, sizeof(strShotGoOn));
		break;
	case EMPCT_RSP_SHOTGO_ON:	//ShotGo On応答
		memcpy(out_makeData, strShotGoOnRsp, sizeof(strShotGoOnRsp));
		break;
	case EMPCT_REQ_SHOTGO_OFF:	//ShotGo Off要求

		memcpy(out_makeData, strShotGoOff, sizeof(strShotGoOff));
		break;
	case EMPCT_RSP_SHOTGO_OFF:	//ShotGo Off応答
		memcpy(out_makeData, strShotGoOffRsp, sizeof(strShotGoOffRsp));
		break;
	case EMPCT_NTF_SW1_OFF:		//SW1 Off通知
		memcpy(out_makeData, strSw1Off, sizeof(strSw1Off));
		break;
	case EMPCT_NTF_SW2_OFF:		//SW2 Off通知
		memcpy(out_makeData, strSw2Off, sizeof(strSw2Off));
		break;
	case EMPCT_NTF_SHOTGO_OFF_B:	//ShotGo Off応答のミスデータ？
		memcpy(out_makeData, strShotGoOffRspB, sizeof(strShotGoOffRspB));
		break;
	}

	return true;
}

//個別コマンドのデータ解析
bool CMPTranslator::transEachCmdMessage(unsigned char* in_transRawData, MPTransData &out_cmdData){

	// 0x8132だけ、データを正しくチェックする

	if (in_transRawData[8] == 0x32 && in_transRawData[9] == 0x81){
		//ShotGo Off Rspデータ
		if (in_transRawData[1] == 0x07 && in_transRawData[3] == 0x05){
			out_cmdData.eCommandType = EMPCT_RSP_SHOTGO_OFF;
		} else {
			out_cmdData.eCommandType = EMPCT_NTF_SHOTGO_OFF_B;
		}

		// データ長はたまたま同じなので何もしない、、、
	}

	return true;
}

//ボード状態取得要求データ作成
void CMPTranslator::makeReqGetBState(unsigned char* out_makeData, MPTransData in_cmdData){

	unsigned char setData[] =
	{0x00, 0x05, 0x00, 0x07, 0x10, 0x01, 0x02, 0x00, 0x41, 0x01, 0x00, 0x00};

	memcpy(out_makeData, setData, sizeof(setData));

}

//ボード状態取得応答データ作成
void CMPTranslator::makeRspGetBState(unsigned char* out_makeData, MPTransData in_cmdData){

	unsigned char setData[] =
	{0x00, 0x07, 0x00, 0x05, 0x10, 0x01, 0x05, 0x00, 0x41, 0x81, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x01, 0x00};

	memcpy(out_makeData, setData, sizeof(setData));

}

//時刻設定要求データ作成
void CMPTranslator::makeReqSetClock(unsigned char* out_makeData, MPTransData in_cmdData){

	unsigned char setData[] =
	{0x00, 0x05, 0x00, 0x07, 0x10, 0x01, 0x0E, 0x00, 0xA4, 0x01, 0x00, 0x00, 0xE5, 0x07,
	 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00,
	 0x04, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00};

	memcpy(out_makeData, setData, sizeof(setData));

}

//時刻設定応答データ作成
void CMPTranslator::makeRspSetClock(unsigned char* out_makeData, MPTransData in_cmdData){

	unsigned char setData[] =
	{0x00, 0x07, 0x00, 0x05, 0x10, 0x01, 0x03, 0x00, 0xA4, 0x81, 0x00, 0x00, 0x00, 0x00};

	memcpy(out_makeData, setData, sizeof(setData));
}

//ユーザー名設定要求データ作成
void CMPTranslator::makeReqUserPass(unsigned char* out_makeData, MPTransData in_cmdData){

	memset(out_makeData, 0x00, 100);		//このコマンドだけは一部のデータだけ入れて他は0とする。

	unsigned char setData[] =
	{0x00, 0x05, 0x00, 0x07, 0x10, 0x01, 0x2E, 0x00, 0xA6, 0x01};

	memcpy(out_makeData, setData, sizeof(setData));

	memcpy(&out_makeData[36], (char*)"root", sizeof(4));
	memcpy(&out_makeData[68], (char*)"htilonom", sizeof(8));

}

//ユーザー名設定応答データ作成
void CMPTranslator::makeRspUserPass(unsigned char* out_makeData, MPTransData in_cmdData){

	unsigned char setData[] =
	{0x00, 0x07, 0x00, 0x05, 0x10, 0x01, 0x03, 0x00, 0xA6, 0x81, 0x00, 0x00, 0x00, 0x00};

	memcpy(out_makeData, setData, sizeof(setData));

}

//LED消灯要求データ作成
void CMPTranslator::makeReqLedOff(unsigned char* out_makeData, MPTransData in_cmdData){

	unsigned char setData[] =
	{0x00, 0x05, 0x00, 0x07, 0x10, 0x01, 0x03, 0x00, 0x52, 0x01, 0x00, 0x00, 0x04, 0x00};

	memcpy(out_makeData, setData, sizeof(setData));

}

//LED消灯応答データ作成
void CMPTranslator::makeRspLedOff(unsigned char* out_makeData, MPTransData in_cmdData){

	unsigned char setData[] =
	{0x00, 0x07, 0x00, 0x05, 0x10, 0x01, 0x03, 0x00, 0x52, 0x81, 0x00, 0x00, 0x00, 0x00};

	memcpy(out_makeData, setData, sizeof(setData));

}

