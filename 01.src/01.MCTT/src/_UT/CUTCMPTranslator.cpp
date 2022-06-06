/*
 * CUTCMPTranslator.cpp
 *
 *  Created on: 2021/05/17
 *      Author: 10097663
 */

#include <CUTCMPTranslator.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TestRun.h"
#include "CMPTranslator.h"

CUTCMPTranslator::CUTCMPTranslator() {
	// TODO 自動生成されたコンストラクター・スタブ

}

CUTCMPTranslator::~CUTCMPTranslator() {
	// TODO Auto-generated destructor stub
}

void CUTCMPTranslator::Execute(){

	CU_start_section("CMPTranslatorクラス");

	MakeMessageTest();

	TranslateMessageTest();

	CU_display_summary();

}


void CUTCMPTranslator::MakeMessageTest(){

	CU_start_subsection("CMPTranslator::MakeMessageTest()");
	CMPTranslator transObj;

	unsigned char message[2048] ="";
	int datalength;
	bool bRet;

	MPTransData transData;

	memset(message,0x00, sizeof(message));
	transData.eCommandType = EMPCT_NTF_INIT;
	bRet = transObj.MakeMessage(message, datalength, transData);
	CU_ASSERT3(bRet == true, 0, "NTF_INIT");
	CU_ASSERT3(datalength == 248, 0, "NTF_INIT");
	//Display(message, datalength);

	memset(message,0x00, sizeof(message));
	transData.eCommandType = EMPCT_REQ_LIFECHECK;
	bRet = transObj.MakeMessage(message, datalength, transData);
	CU_ASSERT3(bRet == true, 0, "EMPCT_REQ_LIFECHECK");
	CU_ASSERT3(datalength == 12, 0, "EMPCT_REQ_LIFECHECK");
	unsigned char strReqLifeCheck[] =
	{0x00,0x05,0x00,0x07,0x00,0x00,0x02,0x00,0x86,0x00,0x00,0x00};
	CU_ASSERT3(memcmp(message,strReqLifeCheck,datalength)== 0, 0, "EMPCT_REQ_LIFECHECK");

	memset(message,0x00, sizeof(message));
	transData.eCommandType = EMPCT_RSP_LIFECHECK;
	bRet = transObj.MakeMessage(message, datalength, transData);
	CU_ASSERT3(bRet == true, 0, "EMPCT_RSP_LIFECHECK");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_RSP_LIFECHECK");
	unsigned char strRspLifeCheck[] =
	{0x00,0x07,0x00,0x05,0x10,0x01,0x03,0x00,0x86,0x80,0x00,0x00,0x00,0x00};
	CU_ASSERT3(memcmp(message,strRspLifeCheck,datalength)== 0, 0, "EMPCT_RSP_LIFECHECK");

	memset(message,0x00, sizeof(message));
	transData.eCommandType = EMPCT_NTF_SW1_ON;
	bRet = transObj.MakeMessage(message, datalength, transData);
	CU_ASSERT3(bRet == true, 0, "EMPCT_NTF_SW1_ON");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_NTF_SW1_ON");
	unsigned char strSW1On[] =
	{0x00,0x07,0x00,0x05,0x10,0x01,0x03,0x00,0x31,0x11,0x00,0x00,0x01,0x00};
	CU_ASSERT3(memcmp(message,strSW1On,datalength)== 0, 0, "EMPCT_NTF_SW1_ON");

	memset(message,0x00, sizeof(message));
	transData.eCommandType = EMPCT_NTF_SW2_ON;
	bRet = transObj.MakeMessage(message, datalength, transData);
	CU_ASSERT3(bRet == true, 0, "EMPCT_NTF_SW2_ON");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_NTF_SW2_ON");
	unsigned char strSW2On[] =
	{0x00,0x07,0x00,0x05,0x10,0x01,0x03,0x00,0x33,0x11,0x00,0x00,0x01,0x00};
	CU_ASSERT3(memcmp(message,strSW2On,datalength)== 0, 0, "EMPCT_NTF_SW2_ON");

	memset(message,0x00, sizeof(message));
	transData.eCommandType = EMPCT_REQ_SHOTGO_ON;
	bRet = transObj.MakeMessage(message, datalength, transData);
	CU_ASSERT3(bRet == true, 0, "EMPCT_RSP_LIFECHECK");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_RSP_LIFECHECK");
	unsigned char strReqShotGoOn[] =
	{0x00,0x05,0x00,0x07,0x00,0x00,0x03,0x00,0x31,0x01,0x00,0x00,0x01,0x00};
	CU_ASSERT3(memcmp(message,strReqShotGoOn,datalength)== 0, 0, "EMPCT_REQ_SHOTGO_ON");

	memset(message,0x00, sizeof(message));
	transData.eCommandType = EMPCT_RSP_SHOTGO_ON;
	bRet = transObj.MakeMessage(message, datalength, transData);
	CU_ASSERT3(bRet == true, 0, "EMPCT_RSP_SHOTGO_ON");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_RSP_SHOTGO_ON");
	unsigned char strRspShotGoOn[] =
	{0x00,0x07,0x00,0x05,0x10,0x01,0x03,0x00,0x31,0x81,0x00,0x00,0x00,0x00};
	CU_ASSERT3(memcmp(message,strRspShotGoOn,datalength)== 0, 0, "EMPCT_RSP_SHOTGO_ON");

	memset(message,0x00, sizeof(message));
	transData.eCommandType = EMPCT_REQ_SHOTGO_OFF;
	bRet = transObj.MakeMessage(message, datalength, transData);
	CU_ASSERT3(bRet == true, 0, "EMPCT_REQ_SHOTGO_OFF");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_REQ_SHOTGO_OFF");
	unsigned char strReqShotGoOff[] =
	{0x00,0x05,0x00,0x07,0x00,0x00,0x03,0x00,0x32,0x01,0x00,0x00,0x01,0x00};
	CU_ASSERT3(memcmp(message,strReqShotGoOff,datalength)== 0, 0, "EMPCT_REQ_SHOTGO_OFF");

	memset(message,0x00, sizeof(message));
	transData.eCommandType = EMPCT_RSP_SHOTGO_OFF;
	bRet = transObj.MakeMessage(message, datalength, transData);
	CU_ASSERT3(bRet == true, 0, "EMPCT_RSP_SHOTGO_OFF");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_RSP_SHOTGO_OFF");
	unsigned char strRspShotGoOff[] =
	{0x00,0x07,0x00,0x05,0x10,0x01,0x03,0x00,0x32,0x81,0x00,0x00,0x00,0x00};
	CU_ASSERT3(memcmp(message,strRspShotGoOff,datalength)== 0, 0, "EMPCT_RSP_SHOTGO_OFF");

	memset(message,0x00, sizeof(message));
	transData.eCommandType = EMPCT_NTF_SW1_OFF;
	bRet = transObj.MakeMessage(message, datalength, transData);
	CU_ASSERT3(bRet == true, 0, "EMPCT_NTF_SW1_OFF");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_NTF_SW1_OFF");
	unsigned char strSw1Off[] =
	{0x00,0x07,0x00,0x05,0x10,0x01,0x03,0x00,0x32,0x11,0x00,0x00,0x01,0x00};
	CU_ASSERT3(memcmp(message,strSw1Off,datalength)== 0, 0, "EMPCT_NTF_SW1_OFF");

	memset(message,0x00, sizeof(message));
	transData.eCommandType = EMPCT_NTF_SW2_OFF;
	bRet = transObj.MakeMessage(message, datalength, transData);
	CU_ASSERT3(bRet == true, 0, "EMPCT_NTF_SW2_OFF");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_NTF_SW2_OFF");
	unsigned char strSw2Off[] =
	{0x00,0x00,0x00,0x00,0x10,0x01,0x03,0x00,0x34,0x81,0x00,0x00,0x00,0x00};
	CU_ASSERT3(memcmp(message,strSw2Off,datalength)== 0, 0, "EMPCT_NTF_SW2_OFF");

	memset(message,0x00, sizeof(message));
	transData.eCommandType = EMPCT_NTF_SHOTGO_OFF_B;
	bRet = transObj.MakeMessage(message, datalength, transData);
	CU_ASSERT3(bRet == true, 0, "EMPCT_NTF_SHOTGO_OFF_B");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_NTF_SHOTGO_OFF_B");
	unsigned char strShotGoOffRspB[] =
	{0x00,0x00,0x00,0x00,0x10,0x01,0x03,0x00,0x32,0x81,0x00,0x00,0x00,0x00};
	CU_ASSERT3(memcmp(message,strShotGoOffRspB,datalength)== 0, 0, "EMPCT_NTF_SHOTGO_OFF_B");

}

void CUTCMPTranslator::TranslateMessageTest(){

	CU_start_subsection("CMPTranslator::TranslateMessageTest()");

	CMPTranslator transObj;

	int datalength;
	bool bRet;

	MPTransData transData;

	unsigned char strNtfInit[250];
	unsigned char strNtfInitPart[] = {0x00,0x07,0x00,0x05,0x10,0x01,0x78,0x00,0x44,0x11,0x00,0x00};
	memcpy(strNtfInit,strNtfInitPart,sizeof(strNtfInitPart));
	bRet = transObj.TranslateMessage(strNtfInit,transData , datalength);
	CU_ASSERT3(bRet == true, 0, "NTF_INIT");
	CU_ASSERT3(datalength == 248, 0, "NTF_INIT");
	CU_ASSERT3(transData.eCommandType == EMPCT_NTF_INIT,0,"NTF_INIT");

	unsigned char strReqLifeCheck[] =
	{0x00,0x05,0x00,0x07,0x00,0x00,0x02,0x00,0x86,0x00,0x00,0x00};
	bRet = transObj.TranslateMessage(strReqLifeCheck,transData , datalength);
	CU_ASSERT3(bRet == true, 0, "EMPCT_REQ_LIFECHECK");
	CU_ASSERT3(datalength == 12, 0, "EMPCT_REQ_LIFECHECK");
	CU_ASSERT3(transData.eCommandType == EMPCT_REQ_LIFECHECK, 0, "EMPCT_REQ_LIFECHECK");

	unsigned char strRspLifeCheck[] =
	{0x00,0x07,0x00,0x05,0x10,0x01,0x03,0x00,0x86,0x80,0x00,0x00,0x00,0x00};
	bRet = transObj.TranslateMessage(strRspLifeCheck,transData , datalength);
	CU_ASSERT3(bRet == true, 0, "EMPCT_RSP_LIFECHECK");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_RSP_LIFECHECK");
	CU_ASSERT3(transData.eCommandType == EMPCT_RSP_LIFECHECK, 0, "EMPCT_RSP_LIFECHECK");

	unsigned char strSW1On[] =
	{0x00,0x07,0x00,0x05,0x10,0x01,0x03,0x00,0x31,0x11,0x00,0x00,0x01,0x00};
	bRet = transObj.TranslateMessage(strSW1On, transData , datalength);
	CU_ASSERT3(bRet == true, 0, "EMPCT_NTF_SW1_ON");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_NTF_SW1_ON");
	CU_ASSERT3(transData.eCommandType == EMPCT_NTF_SW1_ON, 0, "EMPCT_NTF_SW1_ON");

	unsigned char strSW2On[] =
	{0x00,0x07,0x00,0x05,0x10,0x01,0x03,0x00,0x33,0x11,0x00,0x00,0x01,0x00};
	bRet = transObj.TranslateMessage(strSW2On, transData , datalength);
	CU_ASSERT3(bRet == true, 0, "EMPCT_NTF_SW2_ON");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_NTF_SW2_ON");
	CU_ASSERT3(transData.eCommandType == EMPCT_NTF_SW2_ON, 0, "EMPCT_NTF_SW2_ON");

	unsigned char strReqShotGoOn[] =
	{0x00,0x05,0x00,0x07,0x00,0x00,0x03,0x00,0x31,0x01,0x00,0x00,0x01,0x00};
	bRet = transObj.TranslateMessage(strReqShotGoOn, transData , datalength);
	CU_ASSERT3(bRet == true, 0, "EMPCT_RSP_LIFECHECK");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_RSP_LIFECHECK");
	CU_ASSERT3(transData.eCommandType == EMPCT_REQ_SHOTGO_ON, 0, "EMPCT_RSP_LIFECHECK");

	unsigned char strRspShotGoOn[] =
	{0x00,0x07,0x00,0x05,0x10,0x01,0x03,0x00,0x31,0x81,0x00,0x00,0x00,0x00};
	bRet = transObj.TranslateMessage(strRspShotGoOn, transData , datalength);
	CU_ASSERT3(bRet == true, 0, "EMPCT_RSP_SHOTGO_ON");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_RSP_SHOTGO_ON");
	CU_ASSERT3(transData.eCommandType == EMPCT_RSP_SHOTGO_ON, 0, "EMPCT_RSP_SHOTGO_ON");

	unsigned char strReqShotGoOff[] =
	{0x00,0x05,0x00,0x07,0x00,0x00,0x03,0x00,0x32,0x01,0x00,0x00,0x01,0x00};
	bRet = transObj.TranslateMessage(strReqShotGoOff, transData , datalength);
	CU_ASSERT3(bRet == true, 0, "EMPCT_REQ_SHOTGO_OFF");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_REQ_SHOTGO_OFF");
	CU_ASSERT3(transData.eCommandType == EMPCT_REQ_SHOTGO_OFF, 0, "EMPCT_REQ_SHOTGO_OFF");

	unsigned char strRspShotGoOff[] =
	{0x00,0x07,0x00,0x05,0x10,0x01,0x03,0x00,0x32,0x81,0x00,0x00,0x00,0x00};
	bRet = transObj.TranslateMessage(strRspShotGoOff, transData , datalength);
	CU_ASSERT3(bRet == true, 0, "EMPCT_RSP_SHOTGO_OFF");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_RSP_SHOTGO_OFF");
	CU_ASSERT3(transData.eCommandType = EMPCT_RSP_SHOTGO_OFF, 0, "EMPCT_RSP_SHOTGO_OFF");


	unsigned char strSw1Off[] =
	{0x00,0x07,0x00,0x05,0x10,0x01,0x03,0x00,0x32,0x11,0x00,0x00,0x01,0x00};
	bRet = transObj.TranslateMessage(strSw1Off, transData , datalength);
	CU_ASSERT3(bRet == true, 0, "EMPCT_NTF_SW1_OFF");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_NTF_SW1_OFF");
	CU_ASSERT3(transData.eCommandType == EMPCT_NTF_SW1_OFF, 0, "EMPCT_NTF_SW1_OFF");


	unsigned char strSw2Off[] =
	{0x00,0x00,0x00,0x00,0x10,0x01,0x03,0x00,0x34,0x81,0x00,0x00,0x00,0x00};
	bRet = transObj.TranslateMessage(strSw2Off, transData , datalength);
	CU_ASSERT3(bRet == true, 0, "EMPCT_NTF_SW2_OFF");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_NTF_SW2_OFF");
	CU_ASSERT3(transData.eCommandType == EMPCT_NTF_SW2_OFF, 0, "EMPCT_NTF_SW2_OFF");


	unsigned char strShotGoOffRspB[] =
	{0x00,0x00,0x00,0x00,0x10,0x01,0x03,0x00,0x32,0x81,0x00,0x00,0x00,0x00};
	bRet = transObj.TranslateMessage(strShotGoOffRspB, transData , datalength);
	CU_ASSERT3(bRet == true, 0, "EMPCT_NTF_SHOTGO_OFF_B");
	CU_ASSERT3(datalength == 14, 0, "EMPCT_NTF_SHOTGO_OFF_B");
	CU_ASSERT3(transData.eCommandType == EMPCT_NTF_SHOTGO_OFF_B, 0, "EMPCT_NTF_SHOTGO_OFF_B");

}

//データの表示
void CUTCMPTranslator::Display(unsigned char* message, int length){

	printf("length:%d\n",length);

	for(int i=0;i<length;i++){
		printf("0x%02x ", message[i]);
	}
	printf("\n");

}
