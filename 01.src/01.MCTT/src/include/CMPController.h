/*
 * CMPController.h
 *
 *  Created on: 2021/05/10
 *      Author: 10097663
 */

#ifndef CMPCONTROLLER_H_
#define CMPCONTROLLER_H_

#include "CComSocket.h"
#include "AbstNotifyMPState.h"
#include "AbstTimeout.h"
#include "CMPTranslator.h"

class CMPController : public AbstTimeout {
public:

	//MP状態
	enum EMP_STATE{
		EMPS_DISCONNECT = 0,	//未接続状態
		EMPS_WAIT_CONNECT,		//接続待ち
		EMPS_INIT,				//初期化中
		EMPS_IDLE,				//アイドル
		EMPS_PREP,				//読取準備中
		EMPS_SW1_ON,			//SW1 ON
		EMPS_SW2_ON,			//SW2 ON
		EMPS_SHOT_GO,			//SHOT GO
		EMPS_SW_OFF,			//SW OFF処理中
	};	//ここを修正した場合は、getStateName()も修正すること

	//LifeCheckの監視状態
	enum EMP_LF_CHECK{
		EMPL_NO_CHECK = 0,		//チェック不要
		EMPL_WAIT_SEND,			//送信待ち状態
		EMPL_WAIT_RECV,			//受信待ち状態
	};

	CMPController();
	virtual ~CMPController();

	//応答オブジェクトの登録
	void SetNotifyObject(AbstNotifyMPState *in_obj);

	//タイムアウト時実施メソッド
	virtual void UpdateTimeout();

	//接続処理
	bool Connect();

	//回線接続状態取得
	bool IsConnected();

	//更新処理
	void Update();

	//状態更新
	void SetMPState(EMP_STATE in_state);

	//ShotGo通知
	void SetShotGo(bool bOn);

private:

	AbstNotifyMPState *m_pNotifyObj;	//通知オブジェクト

	string m_IpAddr;				//MPのIPアドレス
	unsigned short m_PortNum;		//MPのポート番号
	EMP_STATE	m_MPState;			//MP状態
	EMP_LF_CHECK	m_MP_LF_Check;	//MP LifeCheck状態
	bool m_bConnect;				//接続状態
	CComSocket	m_Socket;			//ソケット

	const int COM_DATA_LENGTH = 2048;	//データサイズ
	unsigned char *m_pCtrlSendData;		//制御用送信データバッファ
	unsigned char *m_pCtrlRecvData;		//制御用受信データバッファ
	int m_sendDataSize;					//送信中データサイズ
	int m_recvDataSize;					//受信後、未解析データサイズ


	//接続確認
	void checkConnecting();

	//状態変数から状態名を返す
	string getStateName(CMPController::EMP_STATE in_state);

	//制御回線データ送信
	bool sendCtrlData(MPTransData in_cmdData);

	//制御回線データ受信
	bool recvCtrlData(MPTransData &out_cmdData);

	//受信コマンドに応じた処理
	bool procByRecvCmd(MPTransData in_cmdData);

	//LifeCheckタイマー開始
	void startLifeCheck();

	//初期化通知処理
	bool procNtfInit();

	//LifeCheck応答処理
	bool procRspLifeCheck();

	//SW1通知処理
	bool procNtfSw1(bool bOn);

	//SW2通知処理
	bool procNtfSw2(bool bOn);

	//ShotGoOn応答処理
	bool procRspShotGo(bool bOn);

	//デバッグ用データ出力処理
	void LogBinData(unsigned char* pStr, int size, bool send);

};

#endif /* CMPCONTROLLER_H_ */
