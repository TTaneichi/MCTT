/*
 * CUTCSETranslator.cpp
 *
 *  Created on: 2021/05/13
 *      Author: 10097663
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUTCSETranslator.h>
#include "CSETranslator.h"
#include "TestRun.h"

CUT_CSETranslator::CUT_CSETranslator() {
	// TODO 自動生成されたコンストラクター・スタブ

}

CUT_CSETranslator::~CUT_CSETranslator() {
	// TODO Auto-generated destructor stub
}

void CUT_CSETranslator::Execute(){

	CU_SetAllResultFilePath((char*)"UT_Result.txt");

	CU_start_section("CSETranslatorクラス");

	MakeMessageTest();

	TranslateMessageTest();

	CU_display_summary();
}
void CUT_CSETranslator::MakeMessageTest(){

	CU_start_subsection("CSETranslator::MakeMessageTest()");
	CSETranslator transObj;

	unsigned char message[2048] ="";
	int datalength;

	SETransData	transData;

	memset(message,0x00, sizeof(message));
	transData.eCommandType = ESECT_REQ_READY;
	transObj.MakeMessage(message, datalength, transData);
//	Diplay(message, datalength);

	transData.eCommandType = ESECT_RSP_READY;
	transObj.MakeMessage(message, datalength, transData);
//	Diplay(message, datalength);

	transData.eCommandType = ESECT_REQ_NOT_READY;
	transObj.MakeMessage(message, datalength, transData);
//	Diplay(message, datalength);

	transData.eCommandType = ESECT_RSP_NOT_READY;
	transObj.MakeMessage(message, datalength, transData);
//	Diplay(message, datalength);

	transData.eCommandType = ESECT_REQ_SET_TIME;
	transObj.MakeMessage(message, datalength, transData);
//	Diplay(message, datalength);

	transData.eCommandType = ESECT_RSP_SET_TIME;
	transObj.MakeMessage(message, datalength, transData);
//	Diplay(message, datalength);

	transData.eCommandType = ESECT_NTF_WAKEUP;
	transObj.MakeMessage(message, datalength, transData);
//	Diplay(message, datalength);

	transData.eCommandType = ESECT_NTF_WLANERR;
	transObj.MakeMessage(message, datalength, transData);
//	Diplay(message, datalength);

	transData.eCommandType = ESECT_NTF_RMVERR;
	transObj.MakeMessage(message, datalength, transData);
//	Diplay(message, datalength);

	transData.eCommandType = ESECT_NTF_SE_STATE;
	transObj.MakeMessage(message, datalength, transData);
//	Diplay(message, datalength);

}

void CUT_CSETranslator::TranslateMessageTest(){

	CU_start_subsection("CSETranslator::TranslateMessageTest()");

	CSETranslator transObj;
	int datalength;
	SETransData	transData;
	ESECommandTransValidation ret;

	unsigned char readyMes[] = {0x00,0x05, 0x00, 0x06, 0x10, 0x01, 0x03, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00};
	ret = transObj.TranslateMessage(readyMes, transData, datalength);
	CU_ASSERT3(ret == VALID_COMMAND_RES, 0, "Ready要求の解析");
	CU_ASSERT3(datalength == 14, 0, "Ready要求の解析");
	CU_ASSERT3(transData.eCommandType == ESECT_REQ_READY, 0, "Ready要求の解析");

	unsigned char readyRspMes[] = {0x00, 0x06, 0x00, 0x05, 0x10, 0x01, 0x03, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00};
	ret = transObj.TranslateMessage(readyRspMes, transData, datalength);
	CU_ASSERT3(ret == VALID_COMMAND_RES, 0, "Ready応答の解析");
	CU_ASSERT3(datalength == 14, 0, "Ready応答の解析");
	CU_ASSERT3(transData.eCommandType == ESECT_RSP_READY, 0, "Ready応答の解析");

	unsigned char NotreadyReqMes[] = {0x00, 0x05, 0x00, 0x06, 0x10, 0x01, 0x03, 0x00, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00};
	ret = transObj.TranslateMessage(NotreadyReqMes, transData, datalength);
	CU_ASSERT3(ret == VALID_COMMAND_RES, 0, "NotReady要求の解析");
	CU_ASSERT3(datalength == 14, 0, "NotReady要求の解析");
	CU_ASSERT3(transData.eCommandType == ESECT_REQ_NOT_READY, 0, "NotReady要求の解析");

	unsigned char NotreadyRspMes[] = {0x00, 0x06, 0x00, 0x05, 0x10, 0x01, 0x03, 0x00, 0x81, 0x80, 0x00, 0x00, 0x00, 0x00};
	ret = transObj.TranslateMessage(NotreadyRspMes, transData, datalength);
	CU_ASSERT3(ret == VALID_COMMAND_RES, 0, "NotReady応答の解析");
	CU_ASSERT3(datalength == 14, 0, "NotReady応答の解析");
	CU_ASSERT3(transData.eCommandType == ESECT_RSP_NOT_READY, 0, "NotReady応答の解析");

	unsigned char timeSetReq[] = {0x00, 0x05, 0x00, 0x06, 0x10, 0x01, 0x06, 0x00, 0xa8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	ret = transObj.TranslateMessage(timeSetReq, transData, datalength);
	CU_ASSERT3(ret == VALID_COMMAND_RES, 0, "時刻設定要求の解析");
	CU_ASSERT3(datalength == 20, 0, "時刻設定要求の解析");
	CU_ASSERT3(transData.eCommandType == ESECT_REQ_SET_TIME, 0, "時刻設定要求の解析");

	unsigned char timeSetRsp[] = {0x00, 0x06, 0x00, 0x05, 0x10, 0x01, 0x03, 0x00, 0xa8, 0x80, 0x00, 0x00, 0x00, 0x00};
	ret = transObj.TranslateMessage(timeSetRsp, transData, datalength);
	CU_ASSERT3(ret == VALID_COMMAND_RES, 0, "時刻設定応答の解析");
	CU_ASSERT3(datalength == 14, 0, "時刻設定応答の解析");
	CU_ASSERT3(transData.eCommandType == ESECT_RSP_SET_TIME, 0, "時刻設定応答の解析");

	unsigned char wakeupNtf[] = {0x00, 0x06, 0x00, 0x05, 0x10, 0x01, 0x03, 0x00, 0x80, 0x10, 0x00, 0x00, 0x00, 0x00};
	ret = transObj.TranslateMessage(wakeupNtf, transData, datalength);
	CU_ASSERT3(ret == VALID_COMMAND_RES, 0, "起動通知の解析");
	CU_ASSERT3(datalength == 14, 0, "起動通知の解析");
	CU_ASSERT3(transData.eCommandType == ESECT_NTF_WAKEUP, 0, "起動通知の解析");

	unsigned char wlanerr[] = {0x00, 0x06, 0x00, 0x05, 0x10, 0x01, 0x03, 0x00, 0x87, 0x10, 0x00, 0x00, 0x00, 0x00};
	ret = transObj.TranslateMessage(wlanerr, transData, datalength);
	CU_ASSERT3(ret == VALID_COMMAND_RES, 0, "WLANエラー通知の解析");
	CU_ASSERT3(datalength == 14, 0, "WLANエラー通知の解析");
	CU_ASSERT3(transData.eCommandType == ESECT_NTF_WLANERR, 0, "WLANエラー通知の解析");

	unsigned char rmvErr[] = {0x00, 0x06, 0x00, 0x05, 0x10, 0x01, 0x03, 0x00, 0x88, 0x10, 0x00, 0x00, 0x00, 0x00};
	ret = transObj.TranslateMessage(rmvErr, transData, datalength);
	CU_ASSERT3(ret == VALID_COMMAND_RES, 0, "RMVエラー通知の解析");
	CU_ASSERT3(datalength == 14, 0, "RMVエラー通知の解析");
	CU_ASSERT3(transData.eCommandType == ESECT_NTF_RMVERR, 0, "RMVエラー通知の解析");

	unsigned char seState[1024];
	unsigned char seStateBase[] = {0x00, 0x06, 0x00, 0x05, 0x10, 0x01, 0x02, 0x01, 0x90, 0x10};
	memset(seState, 0x00, sizeof(seState));
	memcpy(seState, seStateBase, sizeof(seStateBase));
	ret = transObj.TranslateMessage(seState, transData, datalength);
	CU_ASSERT3(ret == VALID_COMMAND_RES, 0, "SE状態通知の解析");
	CU_ASSERT3(datalength == 524, 0, "SE状態通知の解析");
	CU_ASSERT3(transData.eCommandType == ESECT_NTF_SE_STATE, 0, "SE状態通知の解析");

}
//データの表示
void CUT_CSETranslator::Diplay(unsigned char* message, int length){

	for(int i=0;i<length;i++){
		printf("0x%02x ", message[i]);
	}
	printf("\n");

}


