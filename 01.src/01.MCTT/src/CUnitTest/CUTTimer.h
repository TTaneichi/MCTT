/*
 * CUTTimer.h
 * 経過時間を測定するためのクラス
 *  Created on: 2018/06/26
 *      Author: 10097663
 */

#ifndef CUTTIMER_H_
#define CUTTIMER_H_

#include <time.h>

class CUTTimer {
public:
	//コンストラクタで計測開始
	CUTTimer();

	~CUTTimer();

	//経過時間(sec)を返す
	double GetElapsedTime();

private:
	clock_t m_start;	//スタート時間
};

#endif /* CUTTIMER_H_ */
