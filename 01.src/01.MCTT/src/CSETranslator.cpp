/*
 * CSETranslator.cpp
 *
 *  Created on: 2021/05/13
 *      Author: 10097663
 */

#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
using namespace std;
#include "CSETranslator.h"

typedef struct _CommandIDSet {
	ESECommandType commandType;		//コマンド種別
	unsigned short commandCode;		//コマンドコード
	unsigned short source;			//送信元
	unsigned short dist;			//送信先
	unsigned short length;			//データサイズ(1が2byte)
	string	cmdName;				//コマンド名
} CommandIDSet;

#define MC_ID	0x0500
#define	SH4_ID	0x0600
#define RMV_ID	0x0610


CommandIDSet commandIDtable[] =
{
	{ ESECT_REQ_REGWRITE, 	0x0010, MC_ID,	RMV_ID,	5,	"REQ_REGWRITE"},		//v
	{ ESECT_RSP_REGWRITE, 	0x8010, RMV_ID,	MC_ID,	3,	"RSP_REGWRITE"},		//v
	{ ESECT_REQ_REGREAD, 	0x0011, MC_ID,	RMV_ID,	3,	"REQ_REGREAD"},		//v
	{ ESECT_RSP_REGREAD, 	0x8011, RMV_ID,	MC_ID,	5,	"RSP_REGREAD"},		//v
	{ ESECT_NTF_OUT_DATA, 	0x1015, MC_ID,	RMV_ID,	4,	"NTF_OUT_DATA"},
	{ ESECT_REQ_DELETEIMG, 	0x0016, MC_ID,	RMV_ID,	4,	"REQ_DEL_IMG"},		//v
	{ ESECT_RSP_DELETEIMG, 	0x8016, RMV_ID,	MC_ID,	3,	"RSP_DEL_IMG"},
	{ ESECT_REQ_READY, 		0x0080, MC_ID,	SH4_ID,	3,	"REQ_READY"},
	{ ESECT_RSP_READY, 		0x8080, SH4_ID,	MC_ID,	3,	"RSP_READY"},
	{ ESECT_REQ_NOT_READY, 	0x0081, MC_ID,	SH4_ID,	3,	"REQ_NOT_READY"},
	{ ESECT_RSP_NOT_READY, 	0x8081, SH4_ID,	MC_ID,	3,	"RSP_NOT_READY"},

	{ ESECT_REQ_IMGCONF, 	0x0088, MC_ID,	SH4_ID,	4,	"REQ_IMGCONF"},
	{ ESECT_RSP_IMGCONF, 	0x8088, SH4_ID,	MC_ID,	3,	"RSP_IMGCONF"},
	{ ESECT_REQ_CONNECT, 	0x008A, MC_ID,	SH4_ID,	4,	"REQ_CONNECT"},
	{ ESECT_RSP_CONNECT, 	0x808A, SH4_ID,	MC_ID,	4,	"RSP_CONNECT"},		//サイズがバージョンにより異なる!!!
	{ ESECT_REQ_END_OUT, 	0x008B, MC_ID,	SH4_ID,	3,	"REQ_END_OUT"},
	{ ESECT_RSP_END_OUT, 	0x808B, SH4_ID,	MC_ID,	4,	"RSP_END_OUT"},

	{ ESECT_REQ_SET_TIME, 	0x00A8, MC_ID,	SH4_ID,	6,	"REQ_SET_TIME"},
	{ ESECT_RSP_SET_TIME, 	0x80A8, SH4_ID,	MC_ID,	3,	"RSP_SET_TIME"},
	{ ESECT_REQ_WLAN_RESET, 0x00A9, MC_ID,	SH4_ID,	2,	"REQ_WLAN_RESET"},
	{ ESECT_RSP_WLAN_RESET, 0x80A9, SH4_ID,	MC_ID,	3,	"RSP_WLAN_RESET"},
	{ ESECT_NTF_SHOTSW1, 	0x1081, MC_ID,	SH4_ID,	3,	"NTF_SHOTSW1"},		//v
	{ ESECT_NTF_SHOTSW2, 	0x1082, MC_ID,	SH4_ID,	3,	"NTF_SHOTSW2"},		//v
	{ ESECT_NTF_SHOTGO, 	0x1084, SH4_ID,	MC_ID,	3,	"NTF_SHOTGO"},		//v
	{ ESECT_NTF_ACCUM, 		0x1085, SH4_ID,	MC_ID,	2,	"NTF_ACCUM"},
	{ ESECT_NTF_WAKEUP, 	0x1080, SH4_ID,	MC_ID,	3,	"NTF_WAKEUP"},
	{ ESECT_NTF_WLANERR, 	0x1087, SH4_ID,	MC_ID,	3,	"NTF_WLANERR"},
	{ ESECT_NTF_RMVERR, 	0x1088, SH4_ID,	MC_ID,	3,	"NTF_RMVERR"},
	{ ESECT_NTF_RMVRESET, 	0x108D, MC_ID,	SH4_ID,	4,	"NTF_RMVRESET"},
	{ ESECT_NTF_SE_STATE, 	0x1090, SH4_ID,	MC_ID,	258,"NTF_SE_STATE"},
	{ ESECT_NTF_RMVACK,		0x1086, SH4_ID, MC_ID,	3,	"NTF_RMVACK"},
};


CSETranslator::CSETranslator() {


}

CSETranslator::~CSETranslator() {

}

//送信データ作成
bool CSETranslator::MakeMessage(unsigned char* out_makeData, int &datalength, SETransData in_cmdData){

	unsigned char* pData;		//ワーク用
	int commandIndex;
	//該当コマンドの検索
	bool bFound = false;
	for(commandIndex=0;commandIndex < sizeof(commandIDtable)/sizeof(CommandIDSet);commandIndex++){
		if (commandIDtable[commandIndex].commandType == in_cmdData.eCommandType){
			bFound = true;
			break;
		}
	}

	if (!bFound){
		return false;
	}

	unsigned short* pShortData;
	pData = out_makeData;
	pShortData = (unsigned short*)&pData[0];
	*pShortData = commandIDtable[commandIndex].source;	//送信元
	pShortData = (unsigned short*)&pData[2];
	*pShortData = commandIDtable[commandIndex].dist;	//送信先
	pShortData = (unsigned short*)&pData[4];
	if (commandIDtable[commandIndex].source == SH4_ID || commandIDtable[commandIndex].dist == SH4_ID){
		*pShortData = 0x110;		//CommandID
	} else {
		*pShortData = 0x100;		//CommandID
	}
	pShortData = (unsigned short*)&pData[6];
	*pShortData = commandIDtable[commandIndex].length;

	memset(&pData[8], 0x00, commandIDtable[commandIndex].length*2);		//データ領域を一旦0x00でクリア

	pShortData = (unsigned short*)&pData[8];
	*pShortData = commandIDtable[commandIndex].commandCode;	//CommandID

	//ここで個別のデータを入力する
	if (!makeEachCmdMessage(&pData[12], in_cmdData)){	//データ領域は10byte目からだが、予約2byteは飛ばすため、12
		return false;
	}

	datalength = 8 + commandIDtable[commandIndex].length * 2;

	return true;
}

//受信データ解析
ESECommandTransValidation CSETranslator::TranslateMessage(unsigned char* in_transRawData, SETransData &out_cmdData, int &useLength){

	int cmdIndex;
	useLength = 0;
	unsigned char *pData = in_transRawData;
	unsigned short *pShortData;

	//該当コマンドの検索
	bool bFound = false;
	pShortData = (unsigned short*)&pData[8];
	unsigned short cmdCode = *pShortData;
	for(cmdIndex=0; cmdIndex < sizeof(commandIDtable)/sizeof(CommandIDSet); cmdIndex++){
		if (commandIDtable[cmdIndex].commandCode == cmdCode){
			bFound = true;
			break;
		}
	}

	if (!bFound){
		return INVALID_COMMAND_RES;
	}

	out_cmdData.eCommandType = commandIDtable[cmdIndex].commandType;

	//データの解析を行う処理を記述する
	if (VALID_COMMAND_RES != transEachCmdMessage(&pData[12], out_cmdData)){
		return INVALID_PARAMETER_RES;
	}

	//useLengthを受信データから取得する用に修正(バージョンによりサイズが変わるコマンドがあったため)
	//useLength = 8 + commandIDtable[cmdIndex].length*2;
	pShortData = (unsigned short*)&pData[6];
	useLength = (int)(*pShortData * 2 + 8);

	return VALID_COMMAND_RES;
}
//コマンド名取得
string	CSETranslator::GetCmdName(ESECommandType in_CmdType){

	for(int commandIndex=0;commandIndex < sizeof(commandIDtable)/sizeof(CommandIDSet);commandIndex++){
		if (commandIDtable[commandIndex].commandType == in_CmdType){
			return commandIDtable[commandIndex].cmdName;
		}
	}

	return "";
}

//個別コマンドのデータ部作成
//0バイト目から書き込んでOK!
bool CSETranslator::makeEachCmdMessage(unsigned char* out_makeData, SETransData in_cmdData){

	bool bRet = true;

	switch(in_cmdData.eCommandType){
	case ESECT_REQ_REGWRITE:
		bRet = makeRegData(out_makeData, in_cmdData);
		break;
	case ESECT_RSP_REGWRITE:
		bRet = makeCommonData(out_makeData, in_cmdData);
		break;
	case ESECT_REQ_REGREAD:
		bRet = makeCommonData(out_makeData, in_cmdData);
		break;
	case ESECT_RSP_REGREAD:
		bRet = makeRegData(out_makeData, in_cmdData);
		break;
	case ESECT_NTF_OUT_DATA:
		bRet = makeCommonData2(out_makeData, in_cmdData);
		break;
	case ESECT_REQ_DELETEIMG:
		bRet = makeReqDeleteImg(out_makeData, in_cmdData);
		break;
	case ESECT_RSP_DELETEIMG:
		bRet = makeCommonData(out_makeData, in_cmdData);
		break;
	case ESECT_REQ_IMGCONF:
		bRet = makeImgConfData(out_makeData, in_cmdData);
		break;
	case ESECT_RSP_IMGCONF:
		bRet = makeCommonData(out_makeData, in_cmdData);
		break;
	case ESECT_REQ_CONNECT:
		bRet = makeConnectData(out_makeData, in_cmdData);
		break;
	case ESECT_RSP_CONNECT:
		bRet = makeCommonData(out_makeData, in_cmdData);
		break;
	case ESECT_REQ_END_OUT:
		//bRet = makeCommonData(out_makeData, in_cmdData);
		break;
	case ESECT_RSP_END_OUT:
		bRet = makeDataOutEndData(out_makeData, in_cmdData);
		break;
	case ESECT_REQ_SET_TIME:
		bRet = makeSetTimeData(out_makeData, in_cmdData);
		break;
	case ESECT_RSP_SET_TIME:
		//do nothing
		break;
	case ESECT_NTF_SHOTSW1:
		bRet = makeShotSWData(out_makeData, in_cmdData);
		break;
	case ESECT_NTF_SHOTSW2:
		bRet = makeShotSWData(out_makeData, in_cmdData);
		break;
	case ESECT_NTF_SHOTGO:
		bRet = makeShotSWData(out_makeData, in_cmdData);
		break;
	case ESECT_NTF_RMVRESET:
		bRet = makeCommonData(out_makeData, in_cmdData);
		break;
	default:
		//あとは作成不要の認識
		break;
	}

	return bRet;
}

//個別コマンドのデータ解析
ESECommandTransValidation CSETranslator::transEachCmdMessage(unsigned char* in_transRawData, SETransData &out_cmdData){

	ESECommandTransValidation	ERet = VALID_COMMAND_RES;

	switch(out_cmdData.eCommandType){
	case ESECT_REQ_REGWRITE:
		ERet = transRegData(in_transRawData, out_cmdData);
		break;
	case ESECT_RSP_REGWRITE:
		ERet = transCommonData(in_transRawData, out_cmdData);
		break;
	case ESECT_REQ_REGREAD:
		ERet = transCommonData(in_transRawData, out_cmdData);
		break;
	case ESECT_RSP_REGREAD:
		ERet = transRegData(in_transRawData, out_cmdData);
		break;
	case ESECT_NTF_OUT_DATA:
		ERet = transCommonLongData(in_transRawData, out_cmdData);
		break;
	case ESECT_REQ_DELETEIMG:
		ERet = transReqDeleteImg(in_transRawData, out_cmdData);
		break;
	case ESECT_RSP_DELETEIMG:
		ERet = transCommonData(in_transRawData, out_cmdData);
		break;
	case ESECT_REQ_IMGCONF:
		ERet = transImgConfData(in_transRawData, out_cmdData);
		break;
	case ESECT_RSP_IMGCONF:
		ERet = transCommonData(in_transRawData, out_cmdData);
		break;
	case ESECT_REQ_CONNECT:
		ERet = transConnectData(in_transRawData, out_cmdData);
		break;
	case ESECT_RSP_CONNECT:
		ERet = transCommonData(in_transRawData, out_cmdData);
		break;
	case ESECT_REQ_END_OUT:
		ERet = transCommonData(in_transRawData, out_cmdData);
		break;
	case ESECT_RSP_END_OUT:
		ERet = transDataOutEndData(in_transRawData, out_cmdData);
		break;
	case ESECT_REQ_SET_TIME:
		//do nothing
		break;
	case ESECT_RSP_SET_TIME:
		//do nothing
		break;
	case ESECT_NTF_SHOTSW1:
		ERet = transShotSWData(in_transRawData, out_cmdData);
		break;
	case ESECT_NTF_SHOTSW2:
		ERet = transShotSWData(in_transRawData, out_cmdData);
		break;
	case ESECT_NTF_SHOTGO:
		ERet = transShotSWData(in_transRawData, out_cmdData);
		break;
	case ESECT_NTF_RMVRESET:
		ERet = transCommonData(in_transRawData, out_cmdData);
		break;
	default:
		//あとは作成不要の認識
		break;
	}

	return ERet;
}
//レジスタデータ作成
bool CSETranslator::makeRegData(unsigned char* out_makeData, SETransData in_cmdData){

	unsigned short* pWriteData = (unsigned short*)out_makeData;

	//アドレス
	*pWriteData = in_cmdData.data.RegSet.regAddr;

	//データ値
	pWriteData = (unsigned short*)&out_makeData[2];
	*pWriteData = in_cmdData.data.RegSet.data_H;
	pWriteData = (unsigned short*)&out_makeData[4];
	*pWriteData = in_cmdData.data.RegSet.data_L;

	return true;
}
//データ削除要求データ作成
bool CSETranslator::makeReqDeleteImg(unsigned char* out_makeData, SETransData in_cmdData){

	unsigned short* pWriteData;

	out_makeData[0] = in_cmdData.data.delParam.delNum;
	out_makeData[1] = in_cmdData.data.delParam.exp_mode;
	pWriteData = (unsigned short*)&out_makeData[2];
	*pWriteData = in_cmdData.data.delParam.dataMen;

	return true;
}

//画像読取設定データ作成
bool CSETranslator::makeImgConfData(unsigned char* out_makeData, SETransData in_cmdData){

	unsigned short* pWriteData = (unsigned short*)out_makeData;

	*pWriteData = in_cmdData.data.imagConf.subPixel;

	pWriteData = (unsigned short*)&out_makeData[2];
	*pWriteData = in_cmdData.data.imagConf.transLine;

	return true;
}

//データ転送接続要求データ作成
bool CSETranslator::makeConnectData(unsigned char* out_makeData, SETransData in_cmdData){

	unsigned short* pWriteData = (unsigned short*)out_makeData;

	*pWriteData = in_cmdData.data.connectPort.dataConnectID;

	pWriteData = (unsigned short*)&out_makeData[2];
	*pWriteData = in_cmdData.data.connectPort.portNum;

	return true;
}

//データ出力終了応答データ作成
bool CSETranslator::makeDataOutEndData(unsigned char* out_makeData, SETransData in_cmdData){

	unsigned short* pWriteData = (unsigned short*)out_makeData;

	*pWriteData = in_cmdData.data.endOutputData.state;

	pWriteData = (unsigned short*)&out_makeData[2];
	*pWriteData = in_cmdData.data.endOutputData.dataConnectID;

	return true;
}
//時刻情報設定要求データ作成
bool CSETranslator::makeSetTimeData(unsigned char* out_makeData, SETransData in_cmdData){

	unsigned short* pWriteData = (unsigned short*)out_makeData;

	pWriteData[0] = in_cmdData.data.timeData.year;
	out_makeData[2] = in_cmdData.data.timeData.month;
	out_makeData[3] = in_cmdData.data.timeData.day;
	out_makeData[4] = in_cmdData.data.timeData.hour;
	out_makeData[5] = in_cmdData.data.timeData.min;
	out_makeData[6] = in_cmdData.data.timeData.sec;

	return true;
}

//共通データ作成(UShort)
bool CSETranslator::makeCommonData(unsigned char* out_makeData, SETransData in_cmdData){

	unsigned short* pWriteData = (unsigned short*)out_makeData;
	*pWriteData = in_cmdData.data.commonData;

	return true;
}
//共通データ作成(ULong)
bool CSETranslator::makeCommonLongData(unsigned char* out_makeData, SETransData in_cmdData){

	unsigned long* pWriteData = (unsigned long*)out_makeData;
	*pWriteData = in_cmdData.data.commonData;

	return true;
}
//共通データ2作成(UShort*2)
bool CSETranslator::makeCommonData2(unsigned char* out_makeData, SETransData in_cmdData){

	unsigned short* pWriteData = (unsigned short*)out_makeData;
	*pWriteData = in_cmdData.data.common2Data[0];

	pWriteData = (unsigned short*)&out_makeData[2];
	*pWriteData = in_cmdData.data.common2Data[1];

	return true;
}

//ショットSwデータ作成
bool CSETranslator::makeShotSWData(unsigned char* out_makeData, SETransData in_cmdData){

	unsigned short* pWriteData = (unsigned short*)out_makeData;

	if (in_cmdData.data.bShotSW){
		*pWriteData = 1;
	} else {
		*pWriteData = 0;
	}

	return true;
}
//レジスタデータ解析
ESECommandTransValidation CSETranslator::transRegData(unsigned char* in_transRawData, SETransData &out_cmdData){


	unsigned short* pReadData = (unsigned short*)in_transRawData;

	//アドレス
	out_cmdData.data.RegSet.regAddr = *pReadData;

	//データ値
	pReadData = (unsigned short*)&in_transRawData[2];
	out_cmdData.data.RegSet.data_H = *pReadData;
	pReadData = (unsigned short*)&in_transRawData[4];
	out_cmdData.data.RegSet.data_L = *pReadData;


	return VALID_COMMAND_RES;
}

//データ削除要求データ解析
ESECommandTransValidation CSETranslator::transReqDeleteImg(unsigned char* in_transRawData, SETransData &out_cmdData){

	unsigned short* pReadData;

	out_cmdData.data.delParam.delNum = in_transRawData[0];
	out_cmdData.data.delParam.exp_mode = in_transRawData[1];
	pReadData = (unsigned short*)&in_transRawData[2];
	out_cmdData.data.delParam.dataMen = *pReadData;

	return VALID_COMMAND_RES;
}

//画像読取設定データ解析
ESECommandTransValidation CSETranslator::transImgConfData(unsigned char* in_transRawData, SETransData &out_cmdData){

	unsigned short* pReadData = (unsigned short*)in_transRawData;

	out_cmdData.data.imagConf.subPixel = *pReadData;

	pReadData = (unsigned short*)&in_transRawData[2];
	out_cmdData.data.imagConf.transLine = *pReadData;

	return VALID_COMMAND_RES;
}

//データ転送接続要求データ解析
ESECommandTransValidation CSETranslator::transConnectData(unsigned char* in_transRawData, SETransData &out_cmdData){

	unsigned short* pReadData = (unsigned short*)in_transRawData;

	out_cmdData.data.connectPort.dataConnectID = *pReadData;

	pReadData = (unsigned short*)&in_transRawData[2];
	out_cmdData.data.connectPort.portNum = *pReadData;

	return VALID_COMMAND_RES;
}

//データ出力終了応答データ解析
ESECommandTransValidation CSETranslator::transDataOutEndData(unsigned char* in_transRawData, SETransData &out_cmdData){

	unsigned short* pReadData = (unsigned short*)in_transRawData;

	out_cmdData.data.endOutputData.state = *pReadData;

	pReadData = (unsigned short*)&in_transRawData[2];
	out_cmdData.data.endOutputData.dataConnectID = *pReadData;

	return VALID_COMMAND_RES;
}

//共通データ解析(UShort)
ESECommandTransValidation CSETranslator::transCommonData(unsigned char* in_transRawData, SETransData &out_cmdData){

	unsigned short* pReadData = (unsigned short*)in_transRawData;
	out_cmdData.data.commonData = *pReadData;

	return VALID_COMMAND_RES;
}

//共通データ解析(ULong)
ESECommandTransValidation CSETranslator::transCommonLongData(unsigned char* in_transRawData, SETransData &out_cmdData){

	unsigned long* pReadData = (unsigned long*)in_transRawData;
	out_cmdData.data.commonLongData = *pReadData;

	return VALID_COMMAND_RES;
}

//ショットSwデータ解析
ESECommandTransValidation CSETranslator::transShotSWData(unsigned char* in_transRawData, SETransData &out_cmdData){

	unsigned short* pReadData = (unsigned short*)in_transRawData;

	if (0x01 & *pReadData){
		out_cmdData.data.bShotSW = true;
	} else {
		out_cmdData.data.bShotSW = false;
	}

	return VALID_COMMAND_RES;
}


