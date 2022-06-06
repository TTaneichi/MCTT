/*
 * CTimerManager.cpp
 *
 *  Created on: 2021/06/09
 *      Author: 10097663
 */

#include "CTimerManager.h"
#include "CUnitLog.h"
#include <sys/time.h>

static CTimerManager *m_pInstance = NULL;

CTimerManager::CTimerManager() {

	m_timerVector.clear();
}

CTimerManager::~CTimerManager() {
	// TODO Auto-generated destructor stub
}

CTimerManager*	CTimerManager::GetInstance(){

	if ( ! m_pInstance ) {
		m_pInstance = new CTimerManager();
		if ( ! m_pInstance ) {
			SIMLOG_NORM("[TIMER] %s::%s[%d]new error!!!","CTimerManager",__FUNCTION__,__LINE__);
		}
	}
	return m_pInstance;
}

//タイマー監視オブジェクト登録
void CTimerManager::RegisterTimer(AbstTimeout	*timerObj){

	m_timerVector.push_back(timerObj);

}

//タイマーチェック
void CTimerManager::Update(){

	struct timeval	currentTime;

	gettimeofday(&currentTime, NULL);

	for(int i = 0; i< m_timerVector.size(); i++ ){
		m_timerVector[i]->CheckTimeout(currentTime);
	}

}


