/*
 * CSEController.cpp
 *
 *  Created on: 2021/05/07
 *      Author: 10097663
 */
#include <vector>
using namespace std;
#include "CSEController.h"
#include "CSETranslator.h"
#include "CUnitLog.h"

CSEController::CSEController(int debNo) {

	m_SECDebNo = debNo;
	m_pNotifyObj = NULL;
	m_SE_State = ESES_DISCONNECT;
	m_SE_InitState = ESEI_INI_START;
	m_SE_PrepState = ESEM_REG_WRITE;
	m_SE_AfterReadState = ESEA_DATAOUT_END;
	m_MainPortNum = 61000;
	m_ImgPortNum =  61001;
	m_pMainSocket = NULL;
	m_pImgSocket = NULL;
	m_MainSockNo = -1;
	m_ImgSockNo = -1;
//	m_MainSocket.SetServerClient(CONNECT_TYPE_SERV);
//	m_ImgSocket.SetServerClient(CONNECT_TYPE_SERV);

	m_pCtrlSendData = (unsigned char*)malloc(COM_DATA_LENGTH);
	m_pCtrlRecvData = (unsigned char*)malloc(COM_DATA_LENGTH);
	m_pImgRecvData = (unsigned char*)malloc(COM_DATA_LENGTH);
	m_sendDataSize = 0;
	m_recvDataSize = 0;
	m_SENo = 0;
	m_bConnectRsp = false;
	m_bNtfOutData = false;

}

CSEController::~CSEController() {

	free(m_pCtrlSendData);
	free(m_pCtrlRecvData);
	free(m_pImgRecvData);

}
//画像ポート番号設定
void CSEController::SetImgPortNum(unsigned short portNo){

	SIMLOG_NORM("[SE] CSEController::SetImgPortNum() protNo:%d DebNo:%d",portNo, m_SECDebNo);

	if (portNo < 65535){
		m_ImgPortNum = portNo;
	}
	m_MakeImageFile.SetPortNum(portNo);

}
//応答オブジェクトの登録
void CSEController::SetNotifyObject(AbstNotifySEState *in_obj){

	printf("SetNotifyObject\n");

	if (in_obj != NULL){
		m_pNotifyObj = in_obj;
		SIMLOG_NORM("[SE] CSEController::SetNotifyObject() DebNo:%d",m_SECDebNo);
	} else {
		SIMLOG_NORM("[SE] CSEController::SetNotifyObject() Obj is NULL!!!");
	}

}
//ソケットの登録
//type:	0:Main/1:画像
void CSEController::SetSocketObject(CComSocket *pSocket, int type){

	SIMLOG_NORM("[SE] CSEController::SetSocketObject() type:%d DebNo:%d",type, m_SECDebNo);

	if (type == SE_SOCK_TYPE_MAIN){
		m_pMainSocket = pSocket;
	} else if (type == SE_SOCK_TYPE_IMG){
		m_pImgSocket = pSocket;
		m_MakeImageFile.SetSocket(m_pImgSocket, m_ImgPortNum );
	}

}

//接続処理
bool CSEController::Connect(){

	if (m_pMainSocket->CheckConnecting(m_MainSockNo) == ECS_CONNECTED){
		return true;		//なにもしない
	}

	SIMLOG_NORM("[SE] CSEController::Connect() SENo:%d debNo:%d",m_SENo,m_SECDebNo);
	printf("[SE] Connect() SENo:%d debNo:%d\n",m_SENo, m_SECDebNo);

	//接続処理を記述
	SetSEState(ESES_DISCONNECT);

	int ret = m_pMainSocket->Setup(m_MainPortNum, "127.0.0.1");

	printf("[SE] Connect() socketNum:%d\n",ret);
	SetSEState(ESES_WAIT_CONNECT);
	m_MainSockNo = ret;

	return true;
}
//メイン回線接続状態取得
EConnectState CSEController::GetConnectState(){

	return m_pMainSocket->GetConnectState();

}
//装置状態取得
CSEController::ESE_STATE CSEController::GetSEState(){

	return m_SE_State;
}

//装置状態設定(外部から呼ぶことある？)
void  CSEController::SetSEState(CSEController::ESE_STATE in_state){

	if (m_SE_State == in_state){	//設定する状態が今と同じでは処理しない
		return;
	}

	SIMLOG_NORM("[SE_STATE] SE STATE    %s -> %s    No:%d",getStateName(m_SE_State).c_str(), getStateName(in_state).c_str(), m_SECDebNo);
	m_SE_State = in_state;

}
//SE Noの取得
int CSEController::GetSENo(){

	return m_SENo;
}
//更新処理
void CSEController::Update(){

	//SIMLOG_NORM("CSE::Update()");

	if (m_pMainSocket->GetConnectState(m_MainSockNo) ==  ECS_WAIT_ACCEPT){		//接続待ち時

		checkConnecting();

	} else if (m_pMainSocket->CheckConnecting(m_MainSockNo) == ECS_CONNECTED){	//接続済み
		//データ受信＆解析
		SETransData recvCmdData;
		if (recvCtrlData(recvCmdData) == true){
			//受信コマンドに応じた処理
			bool bRet = procByRecvCmd(recvCmdData);
		}

		if (m_SE_State == ESES_IMG_TRANS){
			m_MakeImageFile.Update();
			if (m_bNtfOutData == false){
				checkAndSendOutData();
			}

			if (m_MakeImageFile.IsCompSaveImg()){	//画像取得完了
				//画像取得通知
				m_pNotifyObj->UpdateSE(ESENT_DATA_COMP, m_SENo);
				//SEの後処理を実施
				SetSEState(ESES_AFTER_IMG_GET);
				m_SE_AfterReadState = ESEA_DATAOUT_END;
				SETransData sendData;
				sendData.eCommandType = ESECT_REQ_END_OUT;
				sendCtrlData(sendData);
			}
		}

	} else {
		//接続切れ状態

		//TODO
	}

}
//レジスタ書き込み
void CSEController::WriteRegister(SRegSet regData){

	SIMLOG_NORM("[SE%d] WriteRegister %x %x %x FIFO:%d", m_SENo, regData.regAddr,
			regData.data_H, regData.data_L, regFIFO.size());

	bool bSend = false;		//SEに今送信するかの判定用

	if (regFIFO.empty()){	//空ならばすぐ送る
		bSend = true;
	}
	regFIFO.push_back(regData);

	if (bSend){
		//SEへのレジスタ書き込み処理
		SETransData sendData;
		sendData.eCommandType = ESECT_REQ_REGWRITE;
		sendData.data.RegSet.regAddr = regData.regAddr;
		sendData.data.RegSet.data_H = regData.data_H;
		sendData.data.RegSet.data_L = regData.data_L;
		sendCtrlData(sendData);
	}

}

//レジスタFIFOクリア
void CSEController::ClearRegFIFO(){

	SIMLOG_NORM("[SE%d] ClearRegFIFO()", m_SENo);

	regFIFO.clear();
}
//読取準備指示(0:オフセット/1:曝射画像)
//別クラスから呼ばれる
void CSEController::ReadPrepare(int imgType, string fileName){

//	SIMLOG_NORM("SE::ReadPrepare(%d) 0:offset/1:exposure SENo:%d %s", imgType, m_SENo, fileName.c_str());
	m_fileName = fileName;
	m_bNtfOutData = false;

	//現状ではオフセットと曝射画像は区別しない
	if (m_SE_State == ESES_IDLE){
		SetSEState(ESES_PREP);
		m_SE_PrepState = ESEM_DATA_DELETING;
		//データ削除要求を出す
		SETransData sendData;
		sendData.eCommandType = ESECT_REQ_DELETEIMG;
		sendData.data.delParam.delNum = 0;
		sendData.data.delParam.exp_mode = 0;
		sendData.data.delParam.dataMen = 2;
		sendCtrlData(sendData);
		SIMLOG_NORM("[SE] ------ReqDataDelete Send-----");
	} else {
		SIMLOG_NORM("[SE] ReadPrepare called at unexpected timing!!!");
	}

}

//ShotSW通知(0:SW1/1:SW2)
void CSEController::SetShotSW(int type, bool bOn){

	SIMLOG_NORM("[SE] SetShotSW() SW%d State:%d", type+1, bOn);

	if (ESES_PREP_COMP == m_SE_State){
		if (type == SE_SW1 && bOn){	//SW1 ON 通知
			SetSEState(ESES_SW1_ON);
		} else {
			SIMLOG_NORM("[SE] SetShotSW() unexpected timing!!! type:%d On:%d",type, bOn);
		}
	} else if (ESES_SW1_ON == m_SE_State ) {
		if (type == SE_SW2 && bOn){	//SW2 ON 通知
			SetSEState(ESES_SW2_ON);
		}
	}


	SETransData sendData;
	sendData.data.bShotSW = bOn;

	if (type == 0){
		sendData.eCommandType = ESECT_NTF_SHOTSW1;
		sendCtrlData(sendData);
	} else if (type == 1){
		sendData.eCommandType = ESECT_NTF_SHOTSW2;
		sendCtrlData(sendData);
	}

}

//画像読取指示
void CSEController::ReadImg(){

	SetSEState(ESES_IMG_TRANS);			//SUBのために、状態を変えておく

	//画像読取設定送信
	SETransData sendData;
	sendData.eCommandType = ESECT_REQ_IMGCONF;
	sendData.data.imagConf.subPixel = 2836;
	sendData.data.imagConf.transLine = 2560;
	sendCtrlData(sendData);
	//画像取得依頼
	m_MakeImageFile.StartReading(m_fileName, 2836*2832*2);	//TODO	画像ファイル名を変える

}
//接続確認
void CSEController::checkConnecting(){

	//printf("checkConnecting() %d\n", m_SECDebNo);

	EConnectState retConnectState = m_pMainSocket->CheckConnecting(m_MainSockNo);
	if (retConnectState == ECS_CONNECTED){
		SIMLOG_NORM("[SE] ===================================");
		SIMLOG_NORM("[SE] ====  CSEController Connected  ====");
		SIMLOG_NORM("[SE] ====  %X",m_pMainSocket->GetDstIpAddr(m_MainSockNo));
		SIMLOG_NORM("[SE] ===================================");
		if (m_pMainSocket->GetDstIpAddr(m_MainSockNo) == 0x6F00A8C0){		//192.168.0.111
			m_SENo = 2;
			SIMLOG_NORM("[SE]    VVVVV  m_SENo 2  VVVVV");
		} else {
			m_SENo = 1;
			SIMLOG_NORM("[SE]    VVVVV  m_SENo 1  VVVVV");
		}
		SetSEState(ESES_INIT);
		//m_pNotifyObj->UpdateSE(ESENT_CONNECT, 1);		//TODO 後で消す
	}

}

//初期化時データ削除処理
void CSEController::deleteDataInInit(){

}

//初期化時レジスタ読み込み処理
void CSEController::getInitRegister(){

}

//撮影準備時レジスタ書き込み処理
void CSEController::setPrepWriteRegister(){

	SRegSet	regSetList[] = {
			{0x1950, 0x0000, 0x3122},
			{0x00B4, 0x0000, 0xFFEF},
			{0x1900, 0x0000, 0x0000},
			{0x1904, 0x0000, 0x0001},
			{0x0060, 0x0000, 0x0514},
//			{0x0058, 0x0000, 0x0000},
//			{0x1918, 0x0000, 0x0000},
//			{0x191C, 0x0000, 0x0000},
//			{0x004C, 0x0000, 0x0000},
//			{0x00D0, 0x0000, 0x0000},
			{0x0044, 0x0000, 0xFFFF},
			{0x008C, 0x1000, 0x0010},
			{0x00B4, 0x0000, 0xFFFF}
	};

	if (m_SENo == 2){
		regSetList[0].data_L = 0x3121;
	}

	for (int i=0;i<sizeof(regSetList)/sizeof(SRegSet);i++){
		WriteRegister(regSetList[i]);
	}


}

//Ready設定
void CSEController::setReadyFlag(bool bReady){


}
/* -------------------------------------------------------------------
* @brief	状態変数から状態名を返す
* @param	in_state		状態変数
* @retval   状態名
* @date     2021/05/10	MS T.Taneichi
 --------------------------------------------------------------------- */
string CSEController::getStateName(CSEController::ESE_STATE in_state){

	std::vector<std::string> strList;

	//挿入順序に気をつける. Enum変更でこちらも変更すること
	strList.push_back("ESES_DISCONNECT");
	strList.push_back("ESES_WAIT_CONNECT");
	strList.push_back("ESES_INIT");
	strList.push_back("ESES_IDLE");
	strList.push_back("ESES_PREP");
	strList.push_back("ESES_PREP_COMP");
	strList.push_back("ESES_SW1_ON");
	strList.push_back("ESES_SW2_ON");
	strList.push_back("ESES_SHOT_GO");
	strList.push_back("ESES_IMG_TRANS");
	strList.push_back("ESES_AFTER_IMG_GET");

	return strList[in_state];
}
/* -------------------------------------------------------------------
* @brief	制御回線データ送信
* @param	in_cmdData		送信データ
* @retval   true:成功／false:失敗
* @date     2021/05/17	FF MS T.Taneichi
 --------------------------------------------------------------------- */
bool CSEController::sendCtrlData(SETransData in_cmdData){

	bool bRet = false;

	CSETranslator translator;
	unsigned char sendData[512];
	int dataLength;

	bRet = translator.MakeMessage(sendData, dataLength, in_cmdData);

	memcpy(&m_pCtrlSendData[m_sendDataSize],sendData,dataLength);
	m_sendDataSize += dataLength;

	SIMLOG_NORM("[SE] sendCtrlData cmd:%s", translator.GetCmdName(in_cmdData.eCommandType).c_str());
	//LogBinData(m_pCtrlSendData, m_sendDataSize, true);

	int retSendSize = m_pMainSocket->SendData((char*)m_pCtrlSendData, m_sendDataSize, m_MainSockNo);

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
* @brief	制御回線データ受信
* @param	out_cmdData		受信データ
* @retval   true:解析済みデータ有／false:解析済みデータなし
* @date     2021/05/17	FF MS T.Taneichi
 --------------------------------------------------------------------- */
bool CSEController::recvCtrlData(SETransData &out_cmdData){

	bool bRet = false;

	CSETranslator translator;

	//データ受信
	int recvDataSize = m_pMainSocket->RecvData((char*)&m_pCtrlRecvData[m_recvDataSize], COM_DATA_LENGTH-m_recvDataSize, m_MainSockNo);
	if (recvDataSize > 0){
		//printf("-----recvCtrlData\n");
		m_recvDataSize += recvDataSize;
		//SIMLOG_NORM("SE recvData vvvvvvv Recv:%d Total:%d", recvDataSize, m_recvDataSize);
		//LogBinData(m_pCtrlRecvData, recvDataSize, false);			//TODO 後で消す
	} else if (recvDataSize < 0){
		//SIMLOG_NORM("   <<<<<<<<<<<<<<<<< SE recvData Size is %d total:%d %x",recvDataSize, m_recvDataSize, m_pCtrlRecvData);
	}

	//解析処理
	if (m_recvDataSize > 0){
		int useLength = 0;

		ESECommandTransValidation  resValid = translator.TranslateMessage(m_pCtrlRecvData, out_cmdData, useLength);
		if (resValid == VALID_COMMAND_RES){
			SIMLOG_NORM("[SE] Cmd:%s useLength:%d", translator.GetCmdName(out_cmdData.eCommandType).c_str(), useLength);
			printf("[SE] Cmd:%s useLength:%d\n", translator.GetCmdName(out_cmdData.eCommandType).c_str(), useLength);
			//データバッファの移動
			for(int i=0; i <(m_recvDataSize-useLength); i++){
				m_pCtrlRecvData[i] = m_pCtrlRecvData[i + useLength];
			}
			m_recvDataSize -= useLength;
			//printf("m_recvDataSize:%d\n",m_recvDataSize);
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
bool CSEController::procByRecvCmd(SETransData in_cmdData){

	if (in_cmdData.eCommandType != ESECT_NTF_RMVERR &&
			in_cmdData.eCommandType != ESECT_NTF_SE_STATE){			//定期的に送られるデータは表示しない
		//SIMLOG_NORM("[SE] procByRecvCmd() cmdData:%d",in_cmdData.eCommandType);
	}

	switch(in_cmdData.eCommandType){	//来る可能性のあるコマンドのみ実装する
	case ESECT_RSP_REGWRITE:			//レジスタ書き込み応答
		recvRspRegWrite(in_cmdData);
		break;
	case ESECT_RSP_REGREAD:				//レジスタ読み込み応答
		recvRspRegRead(in_cmdData);
		break;
	case ESECT_RSP_DELETEIMG:			//画像削除応答
		recvRspDeleteImg(in_cmdData);
		break;
	case ESECT_RSP_READY:				//Ready応答
		recvRspReady(in_cmdData);
		break;
	case ESECT_RSP_NOT_READY:			//NoｔReady応答
		recvRspNotReady(in_cmdData);
		break;
	case ESECT_RSP_IMGCONF:				//画像読取設定応答
		recvRspImgConf(in_cmdData);
		break;
	case ESECT_RSP_CONNECT:				//データ転送接続応答
		recvConnect(in_cmdData);
		break;
	case ESECT_RSP_END_OUT:				//データ出力終了応答
		recvEndOut(in_cmdData);
		break;
	case ESECT_RSP_SET_TIME:			//時刻設定応答
		recvSetTime(in_cmdData);
		break;
	case ESECT_RSP_WLAN_RESET:			//WLANリセット応答
		recvWlanReset(in_cmdData);
		break;
	case ESECT_NTF_SHOTGO:				//ShotGo通知
		recvShotGo(in_cmdData);
		break;
	case ESECT_NTF_ACCUM:				//ACCUM通知
		recvAccum(in_cmdData);
		break;
	case ESECT_NTF_WAKEUP:				//起動通知
		recvWakeUp(in_cmdData);
		break;
	case ESECT_NTF_WLANERR:				//WLANエラー通知
		recvWLanErr(in_cmdData);
		break;
	case ESECT_NTF_RMVERR:				//RMVエラー通知
		recvRMVErr(in_cmdData);
		break;
	case ESECT_NTF_RMVRESET:			//RMVシーケンスリセット通知
		recvRMVReset(in_cmdData);
		break;
	case ESECT_NTF_SE_STATE:			//SE状態通知
		recvSEState(in_cmdData);
		break;
	case ESECT_NTF_RMVACK:				//RMV ACK通知
		recvRmvAck(in_cmdData);
		break;
	}

	return true;

}

//レジスタ書き込み応答受信処理
void CSEController::recvRspRegWrite(SETransData in_cmdData){

	if (!regFIFO.empty()){	//空でなければ、先頭の要素を削除する
		regFIFO.erase(regFIFO.begin());
	}

	if (!regFIFO.empty()){	//空でなければ、先頭の要素を送信する
		//SEへのレジスタ書き込み処理
		SRegSet regData = regFIFO[0];
		SETransData sendData;
		sendData.eCommandType = ESECT_REQ_REGWRITE;
		sendData.data.RegSet.regAddr = regData.regAddr;
		sendData.data.RegSet.data_H = regData.data_H;
		sendData.data.RegSet.data_L = regData.data_L;
		sendCtrlData(sendData);
	} else {	//書き込み完了
		if (m_SE_PrepState == ESEM_REG_WRITE && m_SE_State == ESES_PREP){
			m_SE_PrepState = ESEM_DATA_DELETING;		//念のため初期値へ戻す
			SetSEState(ESES_PREP_COMP);
			//上位に通知する
			m_pNotifyObj->UpdateSE(ESENT_PREP_COMP, m_SENo);
		} else if (m_SE_AfterReadState == ESEA_REG_WRITE1 && m_SE_State == ESES_AFTER_IMG_GET){
			m_SE_AfterReadState = ESEA_NOT_READY;
			//NotREADY要求を出す
			SETransData sendData;
			sendData.eCommandType = ESECT_REQ_NOT_READY;
			sendCtrlData(sendData);
			SIMLOG_NORM("[SE] ------ReqNotReady Send-----");
		} else if (	m_SE_AfterReadState == ESEA_REG_WRITE2 && m_SE_State == ESES_AFTER_IMG_GET){
			m_SE_AfterReadState = ESEA_READY2;
			//READY要求を出す
			SETransData sendData;
			sendData.eCommandType = ESECT_REQ_READY;
			sendCtrlData(sendData);
		}
	}

}

//レジスタ読み込み応答受信処理
void CSEController::recvRspRegRead(SETransData in_cmdData){

	if (m_SE_AfterReadState == ESEA_REG_READ && m_SE_State == ESES_AFTER_IMG_GET){
		m_SE_AfterReadState = ESEA_REG_WRITE1;
		SRegSet	regSetList[] = {
				{0x00A0, 0x0000, 0x0000},
				{0x1950, 0x0000, 0x0000}};
		WriteRegister(regSetList[0]);
		WriteRegister(regSetList[1]);
	}

}

//画像データ削除応答受信処理
void CSEController::recvRspDeleteImg(SETransData in_cmdData){

	if (m_SE_PrepState == ESEM_DATA_DELETING){
		m_SE_PrepState =ESEM_READY;
		//READY要求を出す
		SETransData sendData;
		sendData.eCommandType = ESECT_REQ_READY;
		sendCtrlData(sendData);
		SIMLOG_NORM("[SE] ------ReqReady Send in pre -----");
	}
}

//Ready応答受信処理
void CSEController::recvRspReady(SETransData in_cmdData){

	if (m_SE_State == ESES_INIT && m_SE_InitState == ESEI_READY1){
		m_SE_InitState = ESEI_NOT_READY;
		//NotREADY要求を出す
		SETransData sendData;
		sendData.eCommandType = ESECT_REQ_NOT_READY;
		sendCtrlData(sendData);
		SIMLOG_NORM("[SE] ------ReqNotReady Send-----");
	} else if (m_SE_State == ESES_INIT && m_SE_InitState == ESEI_READY2){
		SIMLOG_NORM("[SE] ===================================");
		SIMLOG_NORM("[SE] ====  CSEController IDLE %d    ====",m_SENo);
		SIMLOG_NORM("[SE] ===================================");
		printf("[SE] === SE:%d Initialize Comp! =========\n", m_SENo);
		m_SE_InitState = ESEI_INI_START;
		SetSEState(ESES_IDLE);
		m_pNotifyObj->UpdateSE(ESENT_CONNECT, m_SENo);	//TODO
	} else if ( m_SE_State == ESES_PREP && m_SE_PrepState == ESEM_READY){
		m_SE_PrepState = ESEM_REG_WRITE;
		setPrepWriteRegister();
	} else if (	m_SE_AfterReadState == ESEA_READY1 && m_SE_State == ESES_AFTER_IMG_GET){
		m_SE_AfterReadState = ESEA_REG_WRITE2;
		SETransData sendData;
		sendData.eCommandType = ESECT_NTF_RMVRESET;
		sendCtrlData(sendData);
		SRegSet	regSet = {0x1950, 0x0000, 0x000};
		WriteRegister(regSet);
	} else if (	m_SE_AfterReadState == ESEA_READY2 && m_SE_State == ESES_AFTER_IMG_GET){
		SetSEState(ESES_IDLE);
		m_SE_AfterReadState = ESEA_DATAOUT_END;
		m_pNotifyObj->UpdateSE(ESENT_AFTER_COMP, m_SENo);
	}

}

//NotReady応答受信処理
void CSEController::recvRspNotReady(SETransData in_cmdData){

	if (m_SE_InitState == ESEI_NOT_READY){
		m_SE_InitState = ESEI_SET_CLOCK;
		//時刻設定要求を出す
		SETransData sendData;
		sendData.eCommandType = ESECT_REQ_SET_TIME;
		//TODO 本来は時刻を正しく入れる
		sendData.data.timeData.year = 2021;
		sendData.data.timeData.month = 5;
		sendData.data.timeData.day = 20;
		sendData.data.timeData.hour = 10;
		sendData.data.timeData.min = 10;
		sendData.data.timeData.sec = 0;
		sendCtrlData(sendData);
		SIMLOG_NORM("[SE] ------ReqSetTime Send-----");
	} else if (m_SE_AfterReadState == ESEA_NOT_READY && m_SE_State == ESES_AFTER_IMG_GET){
		m_SE_AfterReadState = ESEA_RMV_WLAN_RESET;
		//RMV-WLANリセット要求を出す
		SETransData sendData;
		sendData.eCommandType = ESECT_REQ_WLAN_RESET;
		sendCtrlData(sendData);
	}

}

//画像読取設定応答受信処理
void CSEController::recvRspImgConf(SETransData in_cmdData){

	m_bConnectRsp = false;

	//データ転送接続を送る
	SETransData sendData;
	sendData.eCommandType = ESECT_REQ_CONNECT;
	sendData.data.connectPort.dataConnectID = 0;
	sendData.data.connectPort.portNum = m_ImgPortNum;
	sendCtrlData(sendData);

}

//データ転送接続応答受信処理
void CSEController::recvConnect(SETransData in_cmdData){

	SIMLOG_NORM("[SE] ------recvConnect-----");
	m_bConnectRsp = true;
	checkAndSendOutData();

}

//データ出力終了応答受信処理
void CSEController::recvEndOut(SETransData in_cmdData){

	if (m_SE_AfterReadState == ESEA_DATAOUT_END && m_SE_State == ESES_AFTER_IMG_GET){
		m_SE_AfterReadState = ESEA_REG_READ;
		//レジスタ読込要求を出す
		SETransData sendData;
		sendData.eCommandType = ESECT_REQ_REGREAD;
		sendData.data.RegSet.regAddr = 0x0004;
		sendCtrlData(sendData);
		SIMLOG_NORM("[SE] ------ReqRegRead After Reading-----");
	}


}

//時刻設定応答受信処理
void CSEController::recvSetTime(SETransData in_cmdData){

	if (m_SE_InitState == ESEI_SET_CLOCK){
		m_SE_InitState = ESEI_READY2;
		//READY要求を出す
		SETransData sendData;
		sendData.eCommandType = ESECT_REQ_READY;
		sendCtrlData(sendData);
		SIMLOG_NORM("[SE] ------ReqReady 2 Send-----");
	}
}
//RMV-WLANリセット応答処理
void CSEController::recvWlanReset(SETransData in_cmdData){

	if (m_SE_AfterReadState == ESEA_RMV_WLAN_RESET && m_SE_State == ESES_AFTER_IMG_GET){
		m_SE_AfterReadState = ESEA_READY1;
		//READY要求を出す
		SETransData sendData;
		sendData.eCommandType = ESECT_REQ_READY;
		sendCtrlData(sendData);
		SIMLOG_NORM("[SE] ------ReqReady 1 AfterImg-----");
	}

}
//ShotGo受信処理
void CSEController::recvShotGo(SETransData in_cmdData){

	SIMLOG_NORM("[SE] ======= SHOT GO %d===========", in_cmdData.data.bShotSW);

	if (m_pNotifyObj != NULL){
		if (in_cmdData.data.bShotSW == true){
			SetSEState(ESES_SHOT_GO);
			m_pNotifyObj->UpdateSE(ESENT_SHOTGO_ON, m_SENo);		//この先でMPへShotGoを送る
		} else {
			SetSEState(ESES_IMG_TRANS);
			m_pNotifyObj->UpdateSE(ESENT_SHOTGO_OFF, m_SENo);		//この先でMPへShotGoを送る

		}
	}

}

//Accum通知受信処理
void CSEController::recvAccum(SETransData in_cmdData){

}

//起動通知受信処理
void CSEController::recvWakeUp(SETransData in_cmdData){

	if (m_SE_State == ESES_INIT){
		m_SE_InitState = ESEI_READY1;

		//READY要求を出す
		SETransData sendData;
		sendData.eCommandType = ESECT_REQ_READY;
		sendCtrlData(sendData);
		SIMLOG_NORM("[SE] ------ReqReady Send-----");
	}

}

//WLANErr受信処理
void CSEController::recvWLanErr(SETransData in_cmdData){

	//特に何もしない
}

//RMVErr受信処理
void CSEController::recvRMVErr(SETransData in_cmdData){

	//特に何もしない
}

//RMVReset受信処理
void CSEController::recvRMVReset(SETransData in_cmdData){

}

//SE状態通知受信処理
void CSEController::recvSEState(SETransData in_cmdData){

}
//RMV ACK通知受信処理
void CSEController::recvRmvAck(SETransData in_cmdData){

}
//データ出力指示判定
void CSEController::checkAndSendOutData(){

//	SIMLOG_NORM("------ CSE::checkAndSendOutData() ConnectRsp:%d Socket:%d ------", m_bConnectRsp, m_pImgSocket->CheckConnecting(0));
	if (m_bConnectRsp && m_pImgSocket->CheckConnecting(0) == ECS_CONNECTED && m_bNtfOutData == false){
		m_bNtfOutData = true;
		//データ出力指示を送る
		SETransData sendData;
		sendData.eCommandType = ESECT_NTF_OUT_DATA;
		sendData.data.common2Data[0] = 0;
		sendData.data.common2Data[1] = 2;
		sendCtrlData(sendData);
	}

}
//デバック用データ出力処理
void CSEController::LogBinData(unsigned char* pStr, int size, bool send){

	int maxData = (size > 18) ? 18 : size;

	char log[130] = "";
	for(int i=0; i<maxData; i++){
		sprintf(&log[i*3], "%02x ", pStr[i]);
	}

	string outLog;

	if (send){
		outLog = "S ";
	}else {
		outLog = "R ";
	}

	if (m_SENo == 1) {
		outLog = "[SE1_Raw] " + outLog;
	} else {
		outLog = "[SE2_Raw] " + outLog;
	}

	outLog += log;


	SIMLOG_NORM("%s",outLog.c_str());
	//printf("%s\n",outLog.c_str());

}
