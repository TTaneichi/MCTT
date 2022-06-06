/*
 * CRotStage.cpp
 *
 *  Created on: 2021/06/18
 *      Author: 10097663
 */

#include "CRotStage.h"
#include "CDataControl.h"
#include "CUnitLog.h"
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "windows.h"
#include "string"
using namespace std;

CRotStage::CRotStage(bool bUse) {

	m_bUse = bUse;
	m_bWaitAngleRecv = false;
	m_bHandleOK = false;
	m_state = EROT_INIT;
	m_pNotifyObj = NULL;
	m_Handle = NULL;
	m_pCtrlRecvData = (unsigned char*)malloc(COM_DATA_LENGTH);
	m_recvDataSize = 0;
	m_Angle = 0.0;
}

CRotStage::~CRotStage() {
	// TODO Auto-generated destructor stub
}

//初期化処理
bool CRotStage::Initialize(){

	if (!m_bUse){			//何もしない
		return false;
	}

	//シリアルポートを開く
	bool bRet = openSerial();
	string portName = CDataControl::GetInstance()->GetRotStageFileName();
	if (bRet){
		SIMLOG_NORM("[RotStage]  Open Serial %s ---- OK.", portName.c_str());
		printf("  Open Serial %s ---- OK.\n", portName.c_str());
		m_bHandleOK = true;
		if (m_pNotifyObj != NULL){
			m_pNotifyObj->UpdateRS(ERSNT_CONNECT);
		}
	} else {
		SIMLOG_NORM("[RotStage]   Open Serial %s ---- NG!!!", portName.c_str());
		printf("  Open Serial %s ---- NG!!!\n", portName.c_str());
		return false;
	}


	return true;
}

//回転処理
void CRotStage::RotateWithAngle(int angleType){

	if (!m_bUse){			//何もしない
		return;
	}

	m_state = EROT_MOVE_NEXT;
	char sendSP[16] = "";

	sprintf(sendSP, "SP%d\r", angleType);
	sendData((char*)sendSP, strlen(sendSP));
	printf("[RotStage] RotStage Send %s\n",sendSP);
	Sleep(200);		//スリープを入れないと、停止したままと誤認識される
}

//定速回転処理
void CRotStage::RotateUntilStop(int speed){

	if (!m_bUse){			//何もしない
		return;
	}

}
//停止指示
void CRotStage::StopRotation(){

	if (!m_bUse){			//何もしない
		return;
	}

}


//更新処理
void CRotStage::Update(){

	if (!m_bUse){			//何もしない
		return;
	}

	if (!m_bHandleOK){		//何もしない
		return;
	}

	bool bRet;

	switch(m_state){
	case EROT_INIT:
		//残りのデータを取得する
		bRet = recvData();
		if (bRet == false){		//受信完了したので、すべてのデータをクリアして初期位置へ移動させる。
			printf("[RotStage]   === RotStage All Initial Data Recv ===\n");
			SIMLOG_NORM("[RotStage]   === RotStage All Initial Data Recv ===");
			memset(m_pCtrlRecvData, 0x00, COM_DATA_LENGTH);
			m_recvDataSize = 0;
			m_state = EROT_INIT_MOVE;
			sendData((char*)"SP0\r", 4);		//初期位置への移動指示
			printf("[RotStage] RotStage Send SP0\n");
			//Sleep(200);
		}
		break;
	case EROT_INIT_MOVE:
		bRet = checkStop();
		if (bRet){
			m_state = EROT_IDLE;
			if (m_pNotifyObj != NULL){
				m_pNotifyObj->UpdateRS(ERSNT_STOPPED);
			}
		}
		break;
	case EROT_IDLE:
		break;
	case EROT_MOVE_NEXT:
		bRet = checkStop();
		if (bRet){
			m_state = EROT_IDLE;
			if (m_pNotifyObj != NULL){
				m_pNotifyObj->UpdateRS(ERSNT_STOPPED);
			}
		}
		break;
	case EROT_MOVING:
		break;
	default:
		break;
	}



}

//状態取得
CRotStage::ROT_STATE CRotStage::GetState(){

	return m_state;

}

//通知オブジェクトの登録
void CRotStage::SetNotifyObj(AbstNotifyRotStage *pObj){
	m_pNotifyObj = pObj;
}
//シリアルポートを開く
bool CRotStage::openSerial(){

	m_Handle=INVALID_HANDLE_VALUE;
	DCB    stDcb;
	COMMTIMEOUTS cto;

	//シリアルポートのハンドルを取得
	string portName = CDataControl::GetInstance()->GetRotStageFileName();
	m_Handle = CreateFile(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, 0, NULL);

	//シリアルポートのハンドルの取得に失敗したら戻る
	if(m_Handle==INVALID_HANDLE_VALUE) return false;

	SetupComm(m_Handle, 1024, 1024);

	//シリアルポートの通信設定の作成
	memset(&stDcb,0,sizeof(DCB));

	stDcb.BaudRate=CBR_9600;
	stDcb.fBinary=TRUE;
	stDcb.ByteSize=8;
	stDcb.Parity=NOPARITY;
	stDcb.StopBits=TWOSTOPBITS;
	stDcb.fOutxCtsFlow = TRUE;
    stDcb.fRtsControl = TRUE;
//    stDcb.fRtsControl = RTS_CONTROL_DISABLE;

    //ハードウェアフロー制御
    stDcb.fOutxDsrFlow = TRUE;       //  DSRハードウェアフロー制御：使用しない場合はFALSEを指定
//    stDcb.fDtrControl = DTR_CONTROL_DISABLE; // DTR有効/無効：　無効なら　DTR_CONTROL_DISABLE;ISABLE
//    stDcb.fRtsControl = RTS_CONTROL_DISABLE;  // RTS制御：　RTS制御をしない場合はRTS_CONTROL_DISABLEを指定
    stDcb.fDtrControl = TRUE; // DTR有効/無効：　無効なら　DTR_CONTROL_DISABLE;ISABLE
    stDcb.fRtsControl = TRUE;  // RTS制御：　RTS制御をしない場合はRTS_CONTROL_DISABLEを指定


	//シリアルポートの通信設定の更新
	if(SetCommState(m_Handle,&stDcb)==FALSE){
		CloseHandle(m_Handle);
		return false;
	}

    GetCommTimeouts( m_Handle, &cto ); // タイムアウトの設定状態を取得
    cto.ReadIntervalTimeout = 0;
    cto.ReadTotalTimeoutMultiplier = 0;
    cto.ReadTotalTimeoutConstant = 0;
    cto.WriteTotalTimeoutMultiplier = 0;
    cto.WriteTotalTimeoutConstant = 0;
    SetCommTimeouts( m_Handle, &cto ); // タイムアウトの状態を設定

	//タイムアウトの設定の作成
//	memset(&cto,0,sizeof(COMMTIMEOUTS));
//	cto.ReadTotalTimeoutConstant    = 1;
//	cto.WriteTotalTimeoutConstant   = 1;

	//タイムアウトの設定の更新
//	if(SetCommTimeouts(m_Handle,&cto)==FALSE) {
//		CloseHandle(m_Handle);
//		return false;
//	}

	return true;
}
//データ受信処理
bool CRotStage::recvData(){

	DWORD dwErrors;
	COMSTAT ComStat;
	DWORD dwCount;

	//何バイト取得できるかを確認する
	ClearCommError(m_Handle, &dwErrors, & ComStat);
	dwCount = ComStat.cbInQue;
	if (dwCount> 0){
		//printf("        dwCount:%d\n", dwCount);
	} else {
		return false;
	}
	dwCount = dwCount > (COM_DATA_LENGTH-m_recvDataSize) ? (COM_DATA_LENGTH-m_recvDataSize) : dwCount;

	unsigned long recvDataSize = 0;
	int ret = ReadFile(m_Handle, (char*)&m_pCtrlRecvData[m_recvDataSize], dwCount,
			&recvDataSize, NULL);
	//printf("[RotStage] CRotStage %d\n",recvDataSize);


	if (recvDataSize > 0){
		m_recvDataSize += recvDataSize;
		//printf("[RotStage] Recv %d:%s\n",recvDataSize, m_pCtrlRecvData);
		return true;
	}

	return false;

}
//データ送信処理
void CRotStage::sendData(char* sendData, int sendSize){

	unsigned long sentSize;
	int ret = WriteFile(m_Handle, sendData, sendSize, &sentSize, NULL);


}
//停止確認処理
//戻り値	true:停止/false:動作中
bool CRotStage::checkStop(){

	bool bRet = false;
	int iAngle = 0;

	if (m_bWaitAngleRecv){
		//受信処理
		recvData();
		//角度情報確認
		if (m_recvDataSize>0){
			char* strAdr;
			strAdr = strstr((char*)m_pCtrlRecvData, (char*)"TD");
			if (NULL != strAdr){
				if (1 == sscanf((char*)strAdr,"TD%d", &iAngle)){
					m_bWaitAngleRecv = false;
					//printf("Angle:%d\n",iAngle);
					float currentAngle = (float)iAngle / 100;
					if (fabsf(m_Angle - currentAngle ) < 0.02 || fabsf(m_Angle - currentAngle) > 359.8){
						bRet = true;
						printf("[RotStage] stopped.\n");
						SIMLOG_NORM("[RotStage]  stopped:%d", iAngle);
					}
					m_Angle = currentAngle;
					memset(m_pCtrlRecvData, 0x00, COM_DATA_LENGTH);
					m_recvDataSize = 0;
					return bRet;
				} else {
					//TDを含むデータを削除する
					int delLen = strAdr - (char*)m_pCtrlRecvData + 2;	//+2はTD分
					memcpy(m_pCtrlRecvData, &m_pCtrlRecvData[delLen], m_recvDataSize - delLen);
					m_recvDataSize -= delLen;
				}
			}
		}
	} else {
		m_bWaitAngleRecv = true;
		sendData((char*)"TD\r",3);		//角度情報取得
		//printf("[RotStage] RotStage Send TD\n");
	}

	return false;

}

