/*
 * CUTTimer.cpp
 *
 *  Created on: 2018/06/26
 *      Author: 10097663
 */

#include <CUTTimer.h>

CUTTimer::CUTTimer(){
	m_start = clock();
}

CUTTimer::~CUTTimer(){

}

double CUTTimer::GetElapsedTime(){

	clock_t end = clock();

	return (double)(end - m_start) / CLOCKS_PER_SEC;

}
