/***********************************************************
 * @file    CApplication.cpp
 * @brief   メッセージダイアログ用のクラス
 * 			現状は呼ばれても何もしない。GUIを追加する場合にはこの先に処理を記述する
 * @author  FF T.Taneichi
 * @date    2021/04/02 新規作成
 * Copyright(c) 2021 FUJIFILM Corporation.
 ***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "CApplication.h"


CApplication* CApplication::m_pInstance = NULL;
/* -------------------------------------------------------------------
* @brief	コンストラクタ
* @param	なし
* @retval   なし
* @date     2018/09/07	FFS T.Taneichi
 --------------------------------------------------------------------- */
CApplication::CApplication() {

}
/* -------------------------------------------------------------------
* @brief	デストラクタ
* @param	なし
* @retval   なし
* @date     2018/09/07	FFS T.Taneichi
 --------------------------------------------------------------------- */
CApplication::~CApplication() {
}
/* -------------------------------------------------------------------
* @brief	インスタンス取得
* @param	なし
* @retval   CSysCtrlIf*
* @date     2018/09/07	FFS T.Taneichi
 --------------------------------------------------------------------- */
CApplication* CApplication::GetInstance(){

    if (m_pInstance == NULL){
    	m_pInstance = new CApplication;
    }

    return m_pInstance;
}
//メッセージボックス表示
MsgBoxReturn CApplication::MessageBox(const char* msg, string caption, MsgBoxChoice choice){

	printf("=======================\n");
	printf("[   %s   ]\n",caption.c_str());
	printf("%s\n",msg);
	printf("=======================\n");


	return IDOK;
}

