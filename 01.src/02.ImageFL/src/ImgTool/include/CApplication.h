/*
 * CApplication.h
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#ifndef CAPPLICATION_H_
#define CAPPLICATION_H_

#include <string>

using namespace std;

typedef enum {
	MB_OK = 0,				//[OK]
	MB_OKCANCE,				//[OK][キャンセル]
	MB_ABORTRETRYIGNORE,	//[中止][再試行][無視]
	MB_YESNOCANCEL,			//[はい][いいえ][キャンセル]
	MB_YESNO,				//YES/NO選択肢
	MB_RETRYCANCEL,			//[再試行][キャンセル]
	MB_ICONSTOP,			//×
} MsgBoxChoice;

typedef enum {
	IDOK = 1,
	IDCANCEL,
	IDABORT,
	IDRETRY,
	IDIGNORE,
	IDYES,
	IDNO,
} MsgBoxReturn;


class CApplication {
private:
	CApplication();
	virtual ~CApplication();
	static CApplication* m_pInstance;

public:

	static CApplication* GetInstance();

	//メッセージボックス表示
	MsgBoxReturn MessageBox(const char* msg, string caption, MsgBoxChoice choice);


};

#endif /* CAPPLICATION_H_ */
