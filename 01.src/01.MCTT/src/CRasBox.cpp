/*
 * CRasBox.cpp
 *
 *  Created on: 2021/06/18
 *      Author: 10097663
 */

#include "CRasBox.h"
#include "CUnitLog.h"
#include "CRBTranslator.h"
#include "string.h"

CRasBox::CRasBox(bool bUse) {

	m_bUse = bUse;
	m_State = ERBST_DISCONNECT;
	m_pNotifyObj = NULL;
	m_pCtrlRecvData = (unsigned char*)malloc(COM_DATA_LENGTH);
	m_recvDataSize = 0;

}

CRasBox::~CRasBox() {
}

//初期化処理
bool CRasBox::Initialize(){

	if (!m_bUse){			//何もしない
		return false;
	}

	SIMLOG_NORM("[RASBOX] RasBox::Initialize()");
	printf("[RASBOX] RasBox::Initialize()\n");

	//接続処理
	m_Socket.SetServerClient(CONNECT_TYPE_CLIENT);
	int ret = m_Socket.Setup(40009, "192.168.0.120");

	if (ret == RET_OK){
		SIMLOG_NORM("[RASBOX] Connected.");
		printf("[RASBOX] Connected.\n");
	} else {
		SIMLOG_NORM("[RASBOX] Connection failed!!!");
		printf("[RASBOX] Connection failed!!!\n");
	}

	return true;
}

//照射開始
void CRasBox::ReqExposureStart(bool bSW1, bool bSW2, bool bExpRes, int swInterval){

	if (!m_bUse){
		if (NULL != m_pNotifyObj){
			m_pNotifyObj->UpdateRB(ERBNT_RES_COMPEX);		//これを送る？
		}
		return;
	}

	if (m_State == ERBST_IDLE){
		//データを送信する
		CRBTranslator	translator;
		RBTransData		transData;

		transData.eCommandType = ERBCT_REQ_START_EXP;
		transData.seqId = 0;
		transData.data.reqStartExp.bSW1 = bSW1;
		transData.data.reqStartExp.bSW2 = bSW2;
		transData.data.reqStartExp.bExpRes = bExpRes;
		transData.data.reqStartExp.SWInterval = (unsigned char)swInterval;
		unsigned char	sendData[8];
		int length;
		translator.MakeMessage(sendData, length, transData);
		m_Socket.SendData((char*)sendData, 8);
		printf("RasBox send Data\n");

	}

}

//更新処理
void CRasBox::Update(){

	if (!m_bUse){			//何もしない
		return;
	}

	switch(m_Socket.GetConnectState()){
	case ECS_CONNECTED:
		//データ受信&解析
		RecvAndProc();
		break;
	default:
		checkConnecting();
		break;
	}

}
//受信処理
void CRasBox::RecvAndProc(){

	//データ受信
	int recvDataSize = m_Socket.RecvData((char*)&m_pCtrlRecvData[m_recvDataSize], COM_DATA_LENGTH-m_recvDataSize);

	if (recvDataSize > 0){
		m_recvDataSize += recvDataSize;
	} else {
		return;
	}

	bool bRet = false;
	CRBTranslator translator;
	RBTransData		transData;

	bool  bTransRet = translator.TranslateMessage(m_pCtrlRecvData, transData);
	if (bTransRet){
		SIMLOG_NORM("[RASBOX] RB CmdID:%d", transData.eCommandType);
		printf("[RASBOX] RB CmdID:%d\n", transData.eCommandType);
		//データバッファの移動
		for(int i=0; i <(m_recvDataSize-8); i++){
			m_pCtrlRecvData[i] = m_pCtrlRecvData[i + 8];
		}
		m_recvDataSize -= 8;
		//printf("MP m_recvDataSize:%d\n",m_recvDataSize);
		//未使用領域は0クリア
		memset(&m_pCtrlRecvData[m_recvDataSize], 0x00, COM_DATA_LENGTH - m_recvDataSize);

		//コマンドに応じた処理(現状は1つのみのため、関数化しない)
		switch(transData.eCommandType){
		case ERBCT_RES_COMP_EXP:
			if (NULL != m_pNotifyObj){
				m_pNotifyObj->UpdateRB(ERBNT_RES_COMPEX);
			}
			break;
		default:
			break;
		}
	}

	return;

}
//通知オブジェクトの登録
void CRasBox::SetNotifyObj(AbstNotifyRasBox *pObj){

	m_pNotifyObj = pObj;

}
//接続確認
void CRasBox::checkConnecting(){

	EConnectState retConnectState = m_Socket.CheckConnecting();
	if (retConnectState == ECS_CONNECTED && m_State == ERBST_DISCONNECT){
		m_State = ERBST_IDLE;
		SIMLOG_NORM("[RASBOX] ===================================");
		SIMLOG_NORM("[RASBOX] ====      RasBox Connected     ====");
		SIMLOG_NORM("[RASBOX] ===================================");
		printf("[RASBOX] ===================================\n");
		printf("[RASBOX] ====      RasBox Connected     ====\n");
		printf("[RASBOX] ===================================\n");
		if (NULL != m_pNotifyObj){
			m_pNotifyObj->UpdateRB(ERBNT_CONNECT);
		}
	}

}

