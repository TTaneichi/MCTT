/*
 * CTimerManager.h
 *
 *  Created on: 2021/06/09
 *      Author: 10097663
 */

#ifndef CTIMERMANAGER_H_
#define CTIMERMANAGER_H_

#include <vector>
#include "AbstTimeout.h"


using namespace std;


class CTimerManager {
public:
	static CTimerManager*	GetInstance();

	virtual ~CTimerManager();

	//タイマー監視オブジェクト登録
	void RegisterTimer(AbstTimeout	*timerObj);

	//タイマーチェック
	void Update();

private:
	vector<AbstTimeout*>	m_timerVector;

	CTimerManager();


};

#endif /* CTIMERMANAGER_H_ */
