/*
 * CDataControl.h
 *
 *  Created on: 2021/05/07
 *      Author: 10097663
 */

#ifndef CDATACONTROL_H_
#define CDATACONTROL_H_

#include <string>
using namespace std;

#define SE_NUM1	0	//SE1
#define SE_NUM2 1	//SE2

class CDataControl {
public:

	//インスタンス生成・取得
	static CDataControl* GetInstance();

	virtual ~CDataControl();

	//SEのIPアドレスを設定する
	void SetSE_IPADDR(char* ipAddr, int seNum);

	//RotStageのドライバファイル名を設定する
	void SetRotStageFileName(char* strFileName);

	//RotStageのドライバファイル名を取得する
	string GetRotStageFileName();

	//Windows用ネットワークソケット初期化済みかフラグ
	bool m_bWSAInitFlag;

	//ステージ使用フラグ
	bool m_bUseStage;

	//RasBox使用フラグ
	bool m_bUseRasBox;

private:
	CDataControl();

	string m_SE1_IpAddr;
	string m_SE2_IpAddr;
	string m_strRotStageFileName;


};

#endif /* CDATACONTROL_H_ */
