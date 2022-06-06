/*
 * AbstNotifyRotStage.h
 *
 *  Created on: 2021/06/21
 *      Author: 10097663
 */

#ifndef ABSTNOTIFYROTSTAGE_H_
#define ABSTNOTIFYROTSTAGE_H_

enum ERotStageNotifyType{
	ERSNT_CONNECT = 0,		//接続完了
	ERSNT_DISCONNECT,		//接続切れ
	ERSNT_STOPPED,			//停止
};

class AbstNotifyRotStage {
public:
	AbstNotifyRotStage();
	virtual ~AbstNotifyRotStage();

	virtual void UpdateRS(ERotStageNotifyType type) = 0;			//通知
};

#endif /* ABSTNOTIFYROTSTAGE_H_ */
