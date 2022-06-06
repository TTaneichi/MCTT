/*
 * AbstTimeout.cpp
 *
 *  Created on: 2021/06/09
 *      Author: 10097663
 */

#include "AbstTimeout.h"
#include "stdio.h"

AbstTimeout::AbstTimeout() {

	m_startTime.tv_sec = 0;
	m_startTime.tv_usec = 0;
	m_bTimerOn = false;
	m_bRepeat = false;
	m_bResetOn = false;
	m_TimeoutTime = 0;

}

AbstTimeout::~AbstTimeout() {
	// TODO Auto-generated destructor stub
}

//タイムアウト時実施メソッド
void AbstTimeout::UpdateTimeout(){

	printf("No implement!!!\n");
}

//タイマー設定
void AbstTimeout::SetTimer(long timeoutTime, bool bRepeat){

	m_TimeoutTime = timeoutTime;
	m_bRepeat = bRepeat;

}

//タイマー開始
//一時停止に対する再開(resume)は対応しない
void AbstTimeout::StartTimer(){

	if (m_bTimerOn){
		//なにもしない
		return;
	} else {
		m_bTimerOn = true;
		m_bResetOn = true;
	}


}

//タイマー確認
void AbstTimeout::CheckTimeout(struct timeval currentTime){

	if (m_bResetOn && m_bTimerOn){

		m_bResetOn = false;
		m_startTime = currentTime;

	}

	if (m_bTimerOn && m_TimeoutTime > 0){
		long elapsed_msec = (currentTime.tv_sec - m_startTime.tv_sec) * 1000 +
							(currentTime.tv_usec - m_startTime.tv_usec) / 1000;

		if (elapsed_msec >= m_TimeoutTime ){		//タイムアウト検出
			UpdateTimeout();
			if (m_bRepeat){
				m_startTime = currentTime;
			} else {
				m_bTimerOn = false;
				m_startTime.tv_sec = 0;
				m_startTime.tv_usec = 0;

			}
		}
	}

}

//タイマー停止
void AbstTimeout::StopTimer(){

	if (m_bTimerOn){
		m_bTimerOn = false;
	} else {
		//何もしない
	}

}

//経過時間リセット(カウント中は、クリアだけで停止しない)
void AbstTimeout::ResetTimer(){

	m_bResetOn = true;
}



