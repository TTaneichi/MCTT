/*
 * CMainLoop.h
 *
 *  Created on: 2021/05/10
 *      Author: 10097663
 */

#ifndef CMAINLOOP_H_
#define CMAINLOOP_H_

#include <string>
#include "AbstNotifySEState.h"
#include "AbstNotifyMPState.h"
#include "AbstNotifyRotStage.h"
#include "AbstNotifyRasBox.h"
#include "AbstTimeout.h"
#include "CSEController.h"
#include "CMPController.h"
#include "CRasBox.h"
#include "CRotStage.h"

using namespace std;

class CMainLoop : public AbstNotifySEState, public AbstNotifyMPState, public AbstNotifyRotStage,
public AbstNotifyRasBox, public AbstTimeout {
public:

	//Main状態
	enum EML_STATE{
		EMLS_DISCONNECT = 0,	//未接続状態
		EMLS_IDLE,				//アイドル状態(トリガ待ち状態)
		EMLS_OFFSET_IMG,
		EMLS_EXP_IMG,
	};

	//画像取得中状態
	enum EMI_STATE{
		EMIS_PREP,			//画像取得のSE準備状態
		EMIS_SW1_ON,		//画像取得のSW1送信状態
		EMIS_SW2_ON,		//画像取得のSW2送信状態
		EMIS_SHOTGO,		//画像取得のSHOTGO受信状態
		EMIS_DATA_RECV,		//画像取得のデータ待ち
		EMIS_SW1_OFF,		//画像取得のSW1Oｆｆ送信状態
	};

	CMainLoop();
	virtual ~CMainLoop();

	//メインループ処理実行
	void Execute();

	//SE状態通知
	virtual void UpdateSE(ESENotifyType type, int SENo);

	//MP状態通知
	virtual void UpdateMP(EMPNotifyType type);

	//RasBox状態通知
	virtual void UpdateRB(ERasBoxNotifyType type);

	//ステージ状態通知
	virtual void UpdateRS(ERotStageNotifyType type);

	//タイムアウト時実施メソッド
	virtual void UpdateTimeout();

	//自身の更新
	void UpdateMyself();

	//状態設定
	void SetMainState(EML_STATE in_state);

	//画像取得状態設定
	void SetEMIState(EMI_STATE in_state);

private:
	//トリガファイル処理
	void CheckTrigFile();

	//トリガファイルロード処理
	bool loadTriggerFile();

	//ログ保存トリガファイル処理
	void CheckLogSaveTrigFile();

	//画像取得開始
	void startImageGet();

	//SE読取準備完了受付処理
	void execUpdateSE_PrepComp(int SENo);

	//SE ShotGoOn受付処理
	void execUpdateSE_ShotGoOn();

	//SE ShotGoOff受付処理
	void execUpdateSE_ShotGoOff();

	//SE データ受信完了処理
	void execUpdateSE_DataComp(int SENo);

	//SE 画像データ取得後処理完了処理
	void execUpdateSE_AfterComp(int SENo);

	//MP SW1_ON/OFF受付処理
	void execUpdateMP_SW1(bool bOn);

	//MP SW2_ON/OFF受付処理
	void execUpdateMP_SW2(bool bOn);

	//MP 接続受付処理
	void execUpdateMP_Connect();

	//MP 接続断受付処理
	void execUpdateMP_Disconnect();

	//SE両方とも準備OKか
	bool isBothSE_OK();

	//状態変数から状態名を返す
	string getStateName(EML_STATE in_state);

	//状態変数から状態名を返す
	string getEMIStateName(EMI_STATE in_state);

	//結果ファイル作成
	void makeResultFile(int errCode, string description);

	EML_STATE	m_State;			//Main状態
	EMI_STATE	m_ImgState;			//画像取得中状態
	CSEController	*m_pSE_Ctrl1;
	CSEController	*m_pSE_Ctrl2;
	CSEController	*m_pSE_Main;	//メイン制御側のSE
	CSEController	*m_pSE_Sub;		//2枚目のSE
	CMPController	*m_pMP_Ctrl;
	CRasBox			*m_pRasBox;
	CRotStage		*m_pRotStage;
	bool m_bSE1Connect;
	bool m_bSE2Connect;
	bool m_bMPConnect;
	bool m_bRasBoxConnect;
	bool m_bSEBothWait[2];
	bool m_bRasBoxUse;				//RasBoxを使用するか
	bool m_bRotUse;					//回転ステージを使用するか

	SImgGetParam	m_ImgGetParam;	//画像取得パラメータ
	int				m_GetImageNum;	//画像取得枚数
	bool m_BothOffsetGet;			//両方取得モードでオフセットを取得したかフラグ

};

#endif /* CMAINLOOP_H_ */
