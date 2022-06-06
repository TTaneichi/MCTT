/*
 * CMakeImageFile.cpp
 *
 *  Created on: 2021/06/08
 *      Author: 10097663
 */

#include "CMakeImageFile.h"
#include "CUnitLog.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#define	IMG_RECV_BUFF			1000*1000		//画像受信バッファ
#define	IMG_ALL_LINE			2836			//画像ライン数
#define	IMG_ALL_MAIN			2832			//画像主画素数
#define	IMG_UNIT_LINE			100				//画像の1ブロック取得ライン数
#define	IMG_LINE_HEADER_SIZE	8				//画像の1ラインのヘッダーサイズ(byte)
#define	IMG_LINE_FOOTER_SEZE	4				//画像の1ラインのフッターサイズ(byte)

CMakeImageFile::CMakeImageFile() {

	m_pSocket = NULL;
	m_portNum = 0;
	m_bCompSaveImg = true;
	m_AllImageSize = 0;
	m_RecvImageSize = 0;
	m_RecvUnitSize = 0;
	m_RecvLineNum = 0;
	m_ReadingLine = 0;
	m_fp = NULL;
	m_pRecvImgBuf = (char*)malloc(IMG_RECV_BUFF);
}

CMakeImageFile::~CMakeImageFile() {
}

void CMakeImageFile::SetSocket(CComSocket *p_socket, unsigned short portNum){

	m_pSocket = p_socket;
	m_portNum = portNum;

}
//ポート番号の設定
void CMakeImageFile::SetPortNum(unsigned short portNum){

	m_portNum = portNum;
}


//読取開始処理
bool CMakeImageFile::StartReading(string fileName, unsigned long imgSize){

	m_bCompSaveImg = false;
	m_strFileName = fileName;
	m_AllImageSize = imgSize;
	m_RecvImageSize = 0;
	m_RecvUnitSize = 0;
	m_RecvLineNum = 0;

	//TMAファイルの作成
	FILE	*fp = fopen((fileName + ".tma").c_str(), "wb");
	if (fp == NULL){
		printf("%s file open error!!!",fileName.c_str());
		SIMLOG_NORM("[IMG] Image File %s open error!!!",fileName.c_str());
		return false;
	}
	fprintf(fp, "02832028361010          0                                                                        6");
	fprintf(fp, "                                                                                                  ");
	fprintf(fp, "                                                            ");
	fclose(fp);

	//ファイルオープン処理
	m_fp = fopen((fileName + ".tdt").c_str(), "wb");
	if (m_fp == NULL){
		printf("%s file open error!!!",fileName.c_str());
		SIMLOG_NORM("[IMG] Image File %s open error!!!",fileName.c_str());
		return false;
	}

	//ソケットサーバーオープン処理
	int ret = m_pSocket->Setup(m_portNum, "127.0.0.1");
	if (ret != RET_OK){
		SIMLOG_NORM("[IMG] Image Server port open error!!!");
		return false;
	}

	return true;
}

//更新処理
void CMakeImageFile::Update(){

	if (m_pSocket->GetConnectState() == ECS_WAIT_ACCEPT){
		EConnectState retConnectState = m_pSocket->CheckConnecting();
		if (retConnectState == ECS_CONNECTED){
			printf("[IMG] ====  ImageServer Connected  ======\n");
			SIMLOG_NORM("[IMG] ===================================");
			SIMLOG_NORM("[IMG] ====  ImageServer Connected  ======");
			SIMLOG_NORM("[IMG] ===================================");
		}
	} else if (m_pSocket->GetConnectState() == ECS_CONNECTED){
		if (m_bCompSaveImg == false){
			//画像読込処理
			if (m_RecvImageSize == 0 ){		//ブロック最初の読み込み時にどれくらいのサイズを読むかを決める
				m_ReadingLine = ((IMG_ALL_LINE - m_RecvLineNum) > IMG_UNIT_LINE) ? IMG_UNIT_LINE : (IMG_ALL_LINE - m_RecvLineNum);
				m_RecvUnitSize = m_ReadingLine * (IMG_ALL_MAIN * 2 + IMG_LINE_HEADER_SIZE + IMG_LINE_FOOTER_SEZE);
			}

			int recvSize = m_pSocket->RecvData(&m_pRecvImgBuf[m_RecvImageSize], m_RecvUnitSize - m_RecvImageSize);
			if (recvSize > 0){
				m_RecvImageSize += recvSize;
				if (m_RecvUnitSize == m_RecvImageSize){		//1ユニット読取完了
					cutImageAndSave();
					SIMLOG_NORM("[IMG]    *** Image Read Line:%d port:%d ***", m_RecvLineNum, m_portNum);
					printf("[IMG]    *** Image Read Line:%d port:%d ***\n", m_RecvLineNum, m_portNum);
					if (m_RecvLineNum == IMG_ALL_LINE){
						//後処理
						procCompRecvImg();
					}
				}
			}
		}
	}

}

//画像保存処理完了フラグ取得
bool CMakeImageFile::IsCompSaveImg(){

	return m_bCompSaveImg;
}

//全画像受信の後処理
void CMakeImageFile::procCompRecvImg(){

	printf("[IMG] ====  All Image Data Received  ======\n");
	SIMLOG_NORM("[IMG] ===== All Image Data Received =========");

	m_bCompSaveImg = true;

	//ファイルクローズ
	fclose(m_fp);

	//ソケットクローズ
	m_pSocket->Disconnect();

}
//画像の加工と保存
void CMakeImageFile::cutImageAndSave(){

	unsigned long OneLineSize = IMG_ALL_MAIN * 2 + IMG_LINE_HEADER_SIZE + IMG_LINE_FOOTER_SEZE;

	for(unsigned long i = 0; i < m_ReadingLine; i++){
		fwrite(&m_pRecvImgBuf[i*OneLineSize + IMG_LINE_HEADER_SIZE], 1, IMG_ALL_MAIN * 2 , m_fp);
	}
	m_RecvLineNum += m_ReadingLine;

	m_RecvImageSize = 0;		//取得サイズは0に戻す
}
