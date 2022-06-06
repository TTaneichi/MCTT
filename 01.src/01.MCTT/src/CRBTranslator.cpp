/*********************************************************************
 * @file   CRBTranslator.cpp
 * @brief  RasBoxの通信データ解析クラス
 * @note
 *
 * @author T.Taneichi
 * @date  2021/07/02 T.Taneichi
 * @license Copyright (C) 2021 FUJIFILM Corporation. All rights reserved.
 *********************************************************************/

#include "CRBTranslator.h"
#include "string.h"

CRBTranslator::CRBTranslator() {

}

CRBTranslator::~CRBTranslator() {
}

//送信データ作成
bool CRBTranslator::MakeMessage(unsigned char* out_makeData, int &dataLength, RBTransData in_cmdData){

	memset(out_makeData, 0, 8);
	dataLength = 0;

	out_makeData[0] = in_cmdData.seqId;
	unsigned char condValue = 0;
	unsigned short *pUShort;

	switch (in_cmdData.eCommandType){
	case ERBCT_REQ_START_EXP:
		out_makeData[1] = 0x04;
		if (in_cmdData.data.reqStartExp.bSW1) {	condValue += 1; }
		if (in_cmdData.data.reqStartExp.bSW2) {	condValue += 2; }
		if (in_cmdData.data.reqStartExp.bExpRes) {	condValue += 4; }
		out_makeData[2] = condValue;
		out_makeData[3] = in_cmdData.data.reqStartExp.SWInterval;
		break;
	case ERBCT_RES_COMP_EXP:
		out_makeData[1] = 0x84;
		out_makeData[2] = in_cmdData.data.resCompEx.result;
		out_makeData[3] = in_cmdData.data.resCompEx.kV;
		pUShort = (unsigned short*)&out_makeData[4];
		*pUShort = in_cmdData.data.resCompEx.mA;
		pUShort = (unsigned short*)&out_makeData[6];
		*pUShort = in_cmdData.data.resCompEx.mSec;
		break;
	}

	dataLength = 8;

	return true;
}

//受信データ解析
bool CRBTranslator::TranslateMessage(unsigned char* in_transRawData, RBTransData &out_cmdData){

	unsigned short *pUShort;

	switch(in_transRawData[1]){
	case 0x04:
		out_cmdData.eCommandType = ERBCT_REQ_START_EXP;
		out_cmdData.data.reqStartExp.bSW1 = in_transRawData[2] & 0x01;
		out_cmdData.data.reqStartExp.bSW2 = in_transRawData[2] & 0x02;
		out_cmdData.data.reqStartExp.bExpRes = in_transRawData[2] & 0x04;
		out_cmdData.data.reqStartExp.SWInterval = in_transRawData[3];
		break;
	case 0x84:
		out_cmdData.eCommandType = ERBCT_RES_COMP_EXP;
		out_cmdData.data.resCompEx.result = in_transRawData[2];
		out_cmdData.data.resCompEx.kV = in_transRawData[3];
		pUShort = (unsigned short*)&in_transRawData[4];
		out_cmdData.data.resCompEx.mA = *pUShort;
		pUShort = (unsigned short*)&in_transRawData[6];
		out_cmdData.data.resCompEx.mSec = *pUShort;
		break;
	default:
		return false;
	}

	out_cmdData.seqId = in_transRawData[0];

	return true;

}




