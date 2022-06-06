/*
 * CMPController.cpp
 *
 *  Created on: 2021/05/10
 *      Author: 10097663
 */

#include "CMPController.h"
#include "CUnitLog.h"
#include "CMPTranslator.h"
#include "CTimerManager.h"

CMPController::CMPController() {

	m_pNotifyObj = NULL;
	m_bConnect = false;
	m_IpAddr = "192.168.0.20";
	m_PortNum = 61000;
	m_Socket.SetServerClient(CONNECT_TYPE_CLIENT);
	m_MPState = EMPS_DISCONNECT;
	m_MP_LF_Check = EMPL_NO_CHECK;
	m_pCtrlSendData = (unsigned char*)malloc(COM_DATA_LENGTH);
	m_pCtrlRecvData = (unsigned char*)malloc(COM_DATA_LENGTH);
	m_sendDataSize = 0;
	m_recvDataSize = 0;

	//タイマー登録
	CTimerManager* pTimerManager = CTimerManager::GetInstance();
	pTimerManager->RegisterTimer(this);
}

CMPController::~CMPController() {
}

//応答オブジェクトの登録
void CMPController::SetNotifyObject(AbstNotifyMPState *in_obj){

	if (in_obj != NULL){
		m_pNotifyObj = in_obj;
		SIMLOG_NORM("[MP] CMPController::SetNotifyObject()");
	} else {
		SIMLOG_NORM("[MP] CMPController::SetNotifyObject() Obj is NULL!!!");
	}
}
//タイムアウト時実施メソッド
void CMPController::UpdateTimeout(){

	SIMLOG_NORM("[MP] CMPController UpdateTimeout");

	switch(m_MP_LF_Check){
	case EMPL_NO_CHECK:
		//何もしない
		break;
	case EMPL_WAIT_SEND:
		//LifeCheckを出して、タイマーを設定する
		MPTransData	transData;
		transData.eCommandType = EMPCT_REQ_LIFECHECK;
		sendCtrlData(transData);

		m_MP_LF_Check = EMPL_WAIT_RECV;
		this->StopTimer();
		this->SetTimer(2000, false);
//		this->SetTimer(10000, false);
		this->StartTimer();

		break;
	case EMPL_WAIT_RECV:
		//応答がないので、接続切れ
		SIMLOG_NORM("[MP] ====        MP Disconnected!!!!     ====");
		SetMPState(EMPS_DISCONNECT);
		m_bConnect = false;
		//TODO

		break;
	}


}
//接続処理
bool CMPController::Connect(){
	if (m_Socket.CheckConnecting() == ECS_CONNECTED){
		return true;		//なにもしない
	}

	SIMLOG_NORM("[MP] CMPController::Connect())");
	printf("MP Connect()\n");

	int ret = m_Socket.Setup(m_PortNum, m_IpAddr);
	SetMPState(EMPS_WAIT_CONNECT);

	return true;
}

//回線接続状態取得
bool CMPController::IsConnected(){

	return m_bConnect;
}

//更新処理
void CMPController::Update(){

	switch (m_Socket.GetConnectState()){
	case ECS_CONNECTED:
		//データ受信&解析
		MPTransData	recvCmdData;
		if (recvCtrlData(recvCmdData) == true){
			//受信コマンドに応じた処理
			bool bRet = procByRecvCmd(recvCmdData);
		}

		break;
	default:
		checkConnecting();
		break;
	}


}
//状態更新
void CMPController::SetMPState(EMP_STATE in_state){

	if (m_MPState == in_state){
		return ;
	}

	SIMLOG_NORM("[MP_STATE] MP STATE   %s -> %s  ",getStateName(m_MPState).c_str(), getStateName(in_state).c_str());
	m_MPState = in_state;

}
//ShotGo通知
void CMPController::SetShotGo(bool bOn){

	SIMLOG_NORM("[MP] -----CMPController::SetShotGo() State:%d------", bOn);

	if (bOn){
		SetMPState(EMPS_SHOT_GO);
		MPTransData	transData;
		transData.eCommandType = EMPCT_REQ_SHOTGO_ON;
		sendCtrlData(transData);
	} else {
		SetMPState(EMPS_SW_OFF);
		MPTransData	transData;
		transData.eCommandType = EMPCT_REQ_SHOTGO_OFF;
		sendCtrlData(transData);
	}

}

//接続確認
void CMPController::checkConnecting(){

	EConnectState retConnectState = m_Socket.CheckConnecting();
	if (retConnectState == ECS_CONNECTED && EMPS_WAIT_CONNECT == EMPS_WAIT_CONNECT){
		SetMPState(EMPS_IDLE);
		m_bConnect = true;
		SIMLOG_NORM("[MP] ===================================");
		SIMLOG_NORM("[MP] ====        MP Connected       ====");
		SIMLOG_NORM("[MP] ===================================");
		m_pNotifyObj->UpdateMP(EMPNT_CONNECT);
	}


}
/* -------------------------------------------------------------------
* @brief	状態変数から状態名を返す
* @param	in_state		状態変数
* @retval   状態名
* @date     2021/05/10	MS T.Taneichi
 --------------------------------------------------------------------- */
string CMPController::getStateName(CMPController::EMP_STATE in_state){

	std::vector<std::string> strList;

	//挿入順序に気をつける. Enum変更でこちらも変更すること
	strList.push_back("EMPS_DISCONNECT");
	strList.push_back("EMPS_WAIT_CONNECT");
	strList.push_back("EMPS_INIT");
	strList.push_back("EMPS_IDLE");
	strList.push_back("EMPS_PREP");
	strList.push_back("EMPS_SW1_ON");
	strList.push_back("EMPS_SW2_ON");
	strList.push_back("EMPS_SHOT_GO");
	strList.push_back("EMPS_SW_OFF");

	return strList[in_state];
}
/* -------------------------------------------------------------------
* @brief	制御回線データ送信
* @param	in_cmdData		送信データ
* @retval   true:成功／false:失敗
* @date     2021/05/17	FF MS T.Taneichi
 --------------------------------------------------------------------- */
bool CMPController::sendCtrlData(MPTransData in_cmdData){

	bool bRet = false;

	CMPTranslator translator;
	unsigned char sendData[512];
	int dataLength;

	bRet = translator.MakeMessage(sendData, dataLength, in_cmdData);

	memcpy(&m_pCtrlSendData[m_sendDataSize],sendData,dataLength);
	m_sendDataSize += dataLength;

	SIMLOG_NORM("[MP] sendCtrlData cmd:%s", translator.GetCmdName(in_cmdData.eCommandType).c_str());
	printf("[MP] sendCtrlData cmd:%s\n", translator.GetCmdName(in_cmdData.eCommandType).c_str());
	LogBinData(m_pCtrlSendData, m_sendDataSize, true);

	int retSendSize = m_Socket.SendData((char*)m_pCtrlSendData, m_sendDataSize);

	if (retSendSize < 0){	//送信失敗
		return false;
	}

	if (retSendSize <= m_sendDataSize && retSendSize > 0){	//送信成功
		//送信できた分だけデータをずらす
		for(int i= 0; i<(m_sendDataSize-retSendSize); i++){
			m_pCtrlSendData[i] = m_pCtrlSendData[i + retSendSize];
		}
		m_sendDataSize -= retSendSize;
	}

	return true;
}
/* -------------------------------------------------------------------
* @brief	データ受信
* @param	out_cmdData		受信データ
* @retval   true:解析済みデータ有／false:解析済みデータなし
* @date     2021/05/17	FF MS T.Taneichi
 --------------------------------------------------------------------- */
bool CMPController::recvCtrlData(MPTransData &out_cmdData){

	bool bRet = false;

	CMPTranslator translator;

	//データ受信
	int recvDataSize = m_Socket.RecvData((char*)&m_pCtrlRecvData[m_recvDataSize], COM_DATA_LENGTH-m_recvDataSize);
	if (recvDataSize > 0){
		m_recvDataSize += recvDataSize;
		//SIMLOG_NORM("MP recvData vvvvvvv %d", recvDataSize);
		LogBinData(m_pCtrlRecvData, m_recvDataSize, false);
		//LogBinData(m_pCtrlRecvData, recvDataSize);			//TODO 後で消す
	}

	//解析処理
	if (m_recvDataSize > 0){
		int useLength = 0;
		bool  bTransRet = translator.TranslateMessage(m_pCtrlRecvData, out_cmdData, useLength);
		if (bTransRet){
//			SIMLOG_NORM("[MP] CmdID:%d useLength:%d", out_cmdData.eCommandType, useLength);
			SIMLOG_NORM("[MP] Cmd:%s useLength:%d AllSize:%d", translator.GetCmdName(out_cmdData.eCommandType).c_str(), useLength, m_recvDataSize);
//			printf("MP CmdID:%d useLength:%d\n", out_cmdData.eCommandType, useLength);
			printf("[MP] Cmd:%s useLength:%d AllSize:%d\n", translator.GetCmdName(out_cmdData.eCommandType).c_str(), useLength, m_recvDataSize);
			//データバッファの移動
			for(int i=0; i <(m_recvDataSize-useLength); i++){
				m_pCtrlRecvData[i] = m_pCtrlRecvData[i + useLength];
			}
			m_recvDataSize -= useLength;
			//printf("MP m_recvDataSize:%d\n",m_recvDataSize);
			//未使用領域は0クリア
			memset(&m_pCtrlRecvData[m_recvDataSize], 0x00, COM_DATA_LENGTH - m_recvDataSize);
			return true;
		} else {
			return false;
		}
	}

	return false;
}

//受信コマンドに応じた処理
bool CMPController::procByRecvCmd(MPTransData in_cmdData){

	SIMLOG_NORM("[MP] procByRecvCmd() cmdData:%d",in_cmdData.eCommandType);

	bool bRet = true;

	switch(in_cmdData.eCommandType){	//来る可能性のあるコマンドのみ実装する
	case EMPCT_NTF_INIT:
		bRet = procNtfInit();
		break;
	case EMPCT_REQ_LIFECHECK:
		//来ることない
		break;
	case EMPCT_RSP_LIFECHECK:
		bRet = procRspLifeCheck();
		break;
	case EMPCT_NTF_SW1_ON:
		bRet = procNtfSw1(true);
		break;
	case EMPCT_NTF_SW2_ON:
		bRet = procNtfSw2(true);
		break;
	case EMPCT_REQ_SHOTGO_ON:
		//来ることない
		break;
	case EMPCT_RSP_SHOTGO_ON:
		bRet = procRspShotGo(true);
		break;
	case EMPCT_REQ_SHOTGO_OFF:
		//来ることない
		break;
	case EMPCT_RSP_SHOTGO_OFF:
		bRet = procRspShotGo(false);
		break;
	case EMPCT_NTF_SW1_OFF:
		bRet = procNtfSw1(false);
		break;
	case EMPCT_NTF_SW2_OFF:
		bRet = procNtfSw2(false);
		break;
	case EMPCT_NTF_SHOTGO_OFF_B:
		//処理記述
		break;
	default:
		break;
	}


	return bRet;
}
//LifeCheckタイマー開始
void CMPController::startLifeCheck(){

	m_MP_LF_Check = EMPL_WAIT_SEND;

	//LifeCheck用のタイマー設定
	this->StopTimer();
	this->SetTimer(2000, false);
//	this->SetTimer(10000, false);
	this->StartTimer();

}
//初期化通知処理
bool CMPController::procNtfInit(){

	startLifeCheck();

	return true;
}

//LifeCheck応答処理
bool CMPController::procRspLifeCheck(){

	startLifeCheck();

	return true;
}

//SW1通知処理
bool CMPController::procNtfSw1(bool bOn){


	if (m_pNotifyObj == NULL){
		SIMLOG_NORM("[MP] m_pNotifyObj is NULL!!!");
		return false;
	}

	//通知処理
	if (bOn){
		SIMLOG_NORM("[MP] ===== MP  SW1 ON ======");
		SetMPState(EMPS_SW1_ON);
		m_pNotifyObj->UpdateMP(EMPNT_SW1_ON);
		this->StopTimer();		//タイマーを停止する
		m_MP_LF_Check = EMPL_NO_CHECK;
	} else {
		SIMLOG_NORM("[MP] ===== MP  SW1 OFF =====");
		m_pNotifyObj->UpdateMP(EMPNT_SW1_OFF);
	}

	return true;
}

//SW2通知処理
bool CMPController::procNtfSw2(bool bOn){

	if (m_pNotifyObj == NULL){
		SIMLOG_NORM("[MP] m_pNotifyObj is NULL!!!");
		return false;
	}

	//通知処理
	if (bOn){
		SIMLOG_NORM("[MP] ===== MP  SW2 ON ======");
		SetMPState(EMPS_SW2_ON);
		m_pNotifyObj->UpdateMP(EMPNT_SW2_ON);
	} else {
		SIMLOG_NORM("[MP] ===== MP  SW2 OFF =====");
		SetMPState(EMPS_IDLE);
		m_pNotifyObj->UpdateMP(EMPNT_SW2_OFF);
		//タイマーを再開する
		startLifeCheck();
	}
	return true;
}

//ShotGoOn応答処理
bool CMPController::procRspShotGo(bool bOn){

	if (bOn){
		SIMLOG_NORM("[MP] ===== MP SHOT GO ON Rsp =====");
	} else {
		SIMLOG_NORM("[MP] ===== MP SHOT GO OFF Rsp =====");
	}
	//特に何もしない

	return true;
}
//デバック用データ出力処理
void CMPController::LogBinData(unsigned char* pStr, int size, bool send){

	int maxData = (size > 16) ? 16 : size;

	char log[110] = "";
	for(int i=0; i<maxData; i++){
		sprintf(&log[i*3], "%02x ", pStr[i]);
	}

	string outLog;

	if (send){
		outLog = "[MP_Raw] S ";
	}else {
		outLog = "[MP_Raw] R ";
	}

	outLog += log;


	SIMLOG_NORM("%s",outLog.c_str());
	//printf("%s\n",outLog.c_str());

}


