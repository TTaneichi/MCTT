/*
 * CSEController.h
 *
 *  Created on: 2021/05/07
 *      Author: 10097663
 */

#ifndef CSECONTROLLER_H_
#define CSECONTROLLER_H_

#include <string>
#include <vector>
using namespace std;
#include "Common.h"
#include "CComSocket.h"
#include "CSETranslator.h"
#include "AbstNotifySEState.h"
#include "CMakeImageFile.h"

#define	SE_SOCK_TYPE_MAIN	0
#define	SE_SOCK_TYPE_IMG	1
#define SE_SW1				0
#define SE_SW2				1

class CSEController {
public:

	//SE状態
	enum ESE_STATE{
		ESES_DISCONNECT = 0,	//未接続状態
		ESES_WAIT_CONNECT,		//接続待ち
		ESES_INIT,				//初期化中
		ESES_IDLE,				//アイドル
		ESES_PREP,				//読取準備中
		ESES_PREP_COMP,			//読取準備完了
		ESES_SW1_ON,			//SW1 ON
		ESES_SW2_ON,			//SW2 ON
		ESES_SHOT_GO,			//SHOT GO
		ESES_IMG_TRANS,			//画像転送
		ESES_AFTER_IMG_GET,		//画像転送完了後処理
	};	//ここを修正した場合は、getStateName()も修正すること

	//SE初期状態
	enum ESE_INIT_STATE{
		ESEI_INI_START = 0,		//初期化開始状態
		ESEI_READY1,			//起動時Ready中1
		ESEI_NOT_READY,			//起動時ReadyOff中
		ESEI_SET_CLOCK,			//時刻設定中
		ESEI_READY2,			//起動時READY中2
	};

	//SE読取準備状態
	enum ESE_PREP_STATE{
		ESEM_DATA_DELETING = 0,		//データ削除中
		ESEM_READY,					//Ready中
		ESEM_REG_READ,				//レジスタリード中
		ESEM_REG_WRITE,				//レジスタライト中
	};

	//SE読取後処理状態
	enum ESE_AFTER_READ_STATE{
		ESEA_DATAOUT_END = 0,		//データ出力終了要求中
		ESEA_REG_READ,				//レジスタ読み込み中
		ESEA_REG_WRITE1,			//レジスタ書き込み中1
		ESEA_NOT_READY,				//NotReady中
		ESEA_RMV_WLAN_RESET,		//RMV WLANリセット中
		ESEA_READY1,				//READY中1
		ESEA_RMV_SEQ_RESET,			//RMVシーケンスリセット
		ESEA_REG_WRITE2,			//レジスタ書き込み中2
		ESEA_READY2,				//READY中2
	};

	CSEController(int debNo);
	virtual ~CSEController();

	//画像ポート番号設定
	void SetImgPortNum(unsigned short portNo);

	//応答オブジェクトの登録
	void SetNotifyObject(AbstNotifySEState *in_obj);

	//ソケットの登録
	void SetSocketObject(CComSocket *pSocket, int type);

	//接続処理
	bool Connect();

	//メイン回線接続状態取得
	EConnectState GetConnectState();

	//装置状態取得
	ESE_STATE GetSEState();

	//SE Noの取得
	int GetSENo();

	//装置状態設定(外部から呼ぶことある？)
	void  SetSEState(ESE_STATE in_state);

	//更新処理
	void Update();

	//レジスタ書き込み
	void WriteRegister(SRegSet regData);

	//レジスタFIFOクリア
	void ClearRegFIFO();

	//読取準備指示(0:オフセット/1:曝射画像)
	void ReadPrepare(int imgType, string fileName);

	//ShotSW通知(0:SW1/1:SW2)
	void SetShotSW(int type, bool bOn);

	//画像読取指示
	void ReadImg();

private:
	//接続確認
	void checkConnecting();


	//初期化時データ削除処理
	void deleteDataInInit();

	//初期化時レジスタ読み込み処理
	void getInitRegister();

	//撮影準備時レジスタ書き込み処理
	void setPrepWriteRegister();

	//Ready設定
	void setReadyFlag(bool bReady);

	//状態変数から状態名を返す
	string getStateName(ESE_STATE in_state);

	//制御回線データ送信
	bool sendCtrlData(SETransData in_cmdData);

	//制御回線データ受信
	bool recvCtrlData(SETransData &out_cmdData);

	//受信コマンドに応じた処理
	bool procByRecvCmd(SETransData in_cmdData);

	//レジスタ書き込み応答受信処理
	void recvRspRegWrite(SETransData in_cmdData);

	//レジスタ読み込み応答受信処理
	void recvRspRegRead(SETransData in_cmdData);

	//画像データ削除応答受信処理
	void recvRspDeleteImg(SETransData in_cmdData);

	//Ready応答受信処理
	void recvRspReady(SETransData in_cmdData);

	//NotReady応答受信処理
	void recvRspNotReady(SETransData in_cmdData);

	//画像読取設定応答受信処理
	void recvRspImgConf(SETransData in_cmdData);

	//データ転送接続応答受信処理
	void recvConnect(SETransData in_cmdData);

	//データ出力終了応答受信処理
	void recvEndOut(SETransData in_cmdData);

	//時刻設定応答受信処理
	void recvSetTime(SETransData in_cmdData);

	//RMV-WLANリセット応答処理
	void recvWlanReset(SETransData in_cmdData);

	//ShotGo受信処理
	void recvShotGo(SETransData in_cmdData);

	//Accum通知受信処理
	void recvAccum(SETransData in_cmdData);

	//起動通知受信処理
	void recvWakeUp(SETransData in_cmdData);

	//WLANErr受信処理
	void recvWLanErr(SETransData in_cmdData);

	//RMVErr受信処理
	void recvRMVErr(SETransData in_cmdData);

	//RMVReset受信処理
	void recvRMVReset(SETransData in_cmdData);

	//SE状態通知受信処理
	void recvSEState(SETransData in_cmdData);

	//RMV ACK通知受信処理
	void recvRmvAck(SETransData in_cmdData);

	//データ出力指示判定
	void checkAndSendOutData();

	AbstNotifySEState *m_pNotifyObj;

	int m_SECDebNo;						//デバッグ用の番号
	int	m_SENo;							//SE番号 1:192.168.0.110/2:192:168.0.111
	ESE_STATE	m_SE_State;				//SE状態
	ESE_INIT_STATE	m_SE_InitState;		//SE初期化中詳細状態
	ESE_PREP_STATE m_SE_PrepState;		//SE撮影準備中状態
	ESE_AFTER_READ_STATE m_SE_AfterReadState;		//SE読取後状態

	vector<SRegSet>	regFIFO;			//レジスタ設定用FIFO

	string m_IpAddr;					//SEのIPアドレス
	unsigned short m_MainPortNum;		//制御回線のポート番号(MCTT側)
	unsigned short m_ImgPortNum;		//画像回線のポート番号(MCTT側)

	CComSocket *m_pMainSocket;			//制御回線ソケット
	CComSocket *m_pImgSocket;			//画像回線ソケット
	int 		m_MainSockNo;			//制御回線のソケット番号
	int			m_ImgSockNo;			//画像回線のソケット番号

	const int COM_DATA_LENGTH = 2048;	//データサイズ
	unsigned char *m_pCtrlSendData;		//制御用送信データバッファ
	unsigned char *m_pCtrlRecvData;		//制御用受信データバッファ
	unsigned char *m_pImgRecvData;		//画像用受信データバッファ
	int m_sendDataSize;					//送信中データサイズ
	int m_recvDataSize;					//受信後、未解析データサイズ

	bool m_bConnectRsp;					//データ転送接続応答受信済みか
	bool m_bNtfOutData;					//データ出力指示済みか

	CMakeImageFile	m_MakeImageFile;	//画像ファイル化オブジェクト
	string			m_fileName;			//画像ファイル名

	//デバック用データ出力処理
	void LogBinData(unsigned char* pStr, int size, bool send = true);

};

#endif /* CSECONTROLLER_H_ */
