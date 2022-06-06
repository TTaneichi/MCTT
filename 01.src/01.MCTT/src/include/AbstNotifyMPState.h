/*
 * AbstNotifyMPState.h
 *
 *  Created on: 2021/05/17
 *      Author: 10097663
 */

#ifndef ABSTNOTIFYMPSTATE_H_
#define ABSTNOTIFYMPSTATE_H_

enum EMPNotifyType{
	EMPNT_SW1_ON = 0,	//SW1 On
	EMPNT_SW1_OFF,		//SW1 Off
	EMPNT_SW2_ON,		//SW2 On
	EMPNT_SW2_OFF,		//SW2 Off
	EMPNT_CONNECT,		//接続完了
	EMPNT_DISCONNECT,	//接続切れ
};

class AbstNotifyMPState {
public:
	AbstNotifyMPState();
	virtual ~AbstNotifyMPState();

	virtual void UpdateMP(EMPNotifyType type) = 0;			//通知
};

#endif /* ABSTNOTIFYMPSTATE_H_ */
