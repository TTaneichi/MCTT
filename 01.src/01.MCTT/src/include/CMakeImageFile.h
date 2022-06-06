/*
 * CMakeImageFile.h
 *
 *  Created on: 2021/06/08
 *      Author: 10097663
 */

#ifndef CMAKEIMAGEFILE_H_
#define CMAKEIMAGEFILE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

#include "CComSocket.h"

class CMakeImageFile {
public:
	CMakeImageFile();
	virtual ~CMakeImageFile();

	//ソケットオブジェクトの設定
	void SetSocket(CComSocket *p_socket, unsigned short portNum);

	//ポート番号の設定
	void SetPortNum(unsigned short portNum);

	//読取開始処理
	bool StartReading(string fileName, unsigned long imgSize);

	//更新処理
	void Update();

	//画像保存処理完了
	bool IsCompSaveImg();

private:

	//全画像受信の後処理
	void procCompRecvImg();

	//画像の加工と保存
	void cutImageAndSave();

	CComSocket	*m_pSocket;				//ソケットオブジェクト
	unsigned short m_portNum;			//ポート番号
	bool		m_bCompSaveImg;			//画像保存完了フラグ
	unsigned long	m_AllImageSize;		//全画像サイズ
	unsigned long	m_RecvImageSize;	//受信済み画像サイズ
	unsigned long	m_RecvUnitSize;		//1ユニット取得サイズ
	unsigned long	m_RecvLineNum;		//受信済みライン数
	unsigned long 	m_ReadingLine;		//読み込むべきライン数
	string		m_strFileName;			//保存ファイル名
	FILE*		m_fp;					//保存ファイルポインタ
	char*		m_pRecvImgBuf;			//画像受信バッファ

};

#endif /* CMAKEIMAGEFILE_H_ */
