/*
 * CRotStage.h
 *
 *  Created on: 2021/06/18
 *      Author: 10097663
 */

#ifndef CROTSTAGE_H_
#define CROTSTAGE_H_

#include "AbstNotifyRotStage.h"
#include "windows.h"

class CRotStage {
public:

	//RotateStage状態
	enum ROT_STATE{
		EROT_INIT = 0,			//初期化状態
		EROT_INIT_MOVE,			//初期化位置移動中
		EROT_IDLE,				//アイドル中
		EROT_MOVE_NEXT,			//次の角度へ移動中
		EROT_MOVING,			//移動中
	};

	CRotStage(bool bUse);
	virtual ~CRotStage();

	//初期化処理
	bool Initialize();

	//角度指定回転処理
	void RotateWithAngle(int angleType);

	//定速回転処理
	void RotateUntilStop(int speed);

	//停止指示
	void StopRotation();

	//更新処理
	void Update();

	//状態取得
	ROT_STATE GetState();

	//通知オブジェクトの登録
	void SetNotifyObj(AbstNotifyRotStage *pObj);

private:
	bool				m_bUse;				//RotateStageを使用するか
	bool				m_bWaitAngleRecv;	//角度情報受信待ち
	bool				m_bHandleOK;		//ファイルオープン成功
	ROT_STATE			m_state;			//状態
	AbstNotifyRotStage	*m_pNotifyObj;		//通知先

	HANDLE				m_Handle;
	const int COM_DATA_LENGTH = 2048;	//データサイズ
	unsigned char *m_pCtrlRecvData;		//制御用受信データバッファ
	int m_recvDataSize;					//受信後、未解析データサイズ
	float m_Angle;						//ステージ角度

	//シリアルポートを開く
	bool openSerial();

	//データ受信処理
	bool recvData();

	//データ送信処理
	void sendData(char* sendData, int sendSize);

	//停止確認処理
	bool checkStop();

};

#endif /* CROTSTAGE_H_ */
