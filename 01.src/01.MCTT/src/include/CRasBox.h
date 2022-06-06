/*
 * CRasBox.h
 *
 *  Created on: 2021/06/18
 *      Author: 10097663
 */

#ifndef CRASBOX_H_
#define CRASBOX_H_

#include "CComSocket.h"
#include "AbstNotifyRasBox.h"

enum ERasBoxState{
	ERBST_DISCONNECT = 0,	//未接続中
	ERBST_IDLE,				//アイドル(接続済み)
	ERBST_EXPSEQ,			//撮影シーケンス中
};

class CRasBox {
public:
	CRasBox(bool bUse);
	virtual ~CRasBox();

	//初期化処理
	bool Initialize();

	//照射開始
	void ReqExposureStart(bool bSW1, bool bSW2, bool bExpRes, int swInterval);

	//更新処理
	void Update();

	//受信処理
	void RecvAndProc();

	//通知オブジェクトの登録
	void SetNotifyObj(AbstNotifyRasBox *pObj);

private:
	bool				m_bUse;		//RasBoxを使用するか
	ERasBoxState		m_State;	//RasBoxの状態
	CComSocket			m_Socket;	//ソケットクラス
	AbstNotifyRasBox	*m_pNotifyObj;

	const int COM_DATA_LENGTH = 2048;	//データサイズ
	unsigned char *m_pCtrlRecvData;		//制御用受信データバッファ
	int m_recvDataSize;					//受信後、未解析データサイズ

	//接続確認
	void checkConnecting();

};

#endif /* CRASBOX_H_ */
