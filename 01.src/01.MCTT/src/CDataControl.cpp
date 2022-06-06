/*
 * CDataControl.cpp
 *
 *  Created on: 2021/05/07
 *      Author: 10097663
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CDataControl.h"
#include "CUnitLog.h"

static CDataControl *m_pInstance = NULL;

CDataControl::CDataControl() {

	m_bWSAInitFlag = false;
	m_bUseStage = false;
	m_bUseRasBox = false;

}

CDataControl::~CDataControl() {
}

CDataControl* CDataControl::GetInstance(){

	if ( ! m_pInstance ) {
		m_pInstance = new CDataControl();
		if ( ! m_pInstance ) {
			SIMLOG_NORM("%s::%s[%d]new error!!!","CDataControl",__FUNCTION__,__LINE__);
		}
	}
	return m_pInstance;

}

//SEのIPアドレスを設定する
void CDataControl::SetSE_IPADDR(char* ipAddr, int seNum){

	if (SE_NUM1 == seNum){
		m_SE1_IpAddr = ipAddr;
	} else if (SE_NUM2 == seNum){
		m_SE2_IpAddr = ipAddr;
	}

}

//RotStageのドライバファイル名を設定する
void CDataControl::SetRotStageFileName(char* strFileName){

	m_strRotStageFileName = strFileName;

}
//RotStageのドライバファイル名を取得する
string CDataControl::GetRotStageFileName(){

	return m_strRotStageFileName;

}

