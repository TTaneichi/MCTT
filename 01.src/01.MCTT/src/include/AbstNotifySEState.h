/*
 * AbstNotifySEState.h
 *
 *  Created on: 2021/05/17
 *      Author: 10097663
 */

#ifndef ABSTNOTIFYSESTATE_H_
#define ABSTNOTIFYSESTATE_H_

enum ESENotifyType{
	ESENT_CONNECT = 0,		//接続完了
	ESENT_DISCONNECT,		//接続切れ
	ESENT_PREP_COMP,		//準備完了
	ESENT_SHOTGO_ON,		//ShotGo ON
	ESENT_SHOTGO_OFF,		//ShotGo OFF
	ESENT_DATA_COMP,		//画像データ受信完了
	ESENT_AFTER_COMP,		//画像データ取得後処理完了
};

class AbstNotifySEState {
public:
	AbstNotifySEState();
	virtual ~AbstNotifySEState();

	virtual void UpdateSE(ESENotifyType type, int SENo) = 0;			//通知
};

#endif /* ABSTNOTIFYSESTATE_H_ */
