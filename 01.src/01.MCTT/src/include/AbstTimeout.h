/*
 * AbstTimeout.h
 *
 *  Created on: 2021/06/09
 *      Author: 10097663
 */

#ifndef ABSTTIMEOUT_H_
#define ABSTTIMEOUT_H_

#include <sys/time.h>

class AbstTimeout {
public:
	AbstTimeout();
	virtual ~AbstTimeout();

	//タイムアウト時実施メソッド
	virtual void UpdateTimeout();

	//タイマー設定
	void SetTimer(long timeoutTime, bool bRepeat);

	//タイマー開始
	void StartTimer();

	//タイマー確認(CTimerManagerから呼ばれる)
	void CheckTimeout(struct timeval currentTime);

	//タイマー停止
	void StopTimer();

	//経過時間リセット(カウント中は、クリアだけで停止しない)
	void ResetTimer();


private:
	struct timeval	m_startTime;		//タイマー開始時刻

	bool	m_bTimerOn;					//測定中フラグ
	bool	m_bRepeat;					//リピートフラグ
	bool	m_bResetOn;					//リセット受付済みフラグ
	long	m_TimeoutTime;				//タイムアウト時間(msec)

};

#endif /* ABSTTIMEOUT_H_ */
