/*
 * CUnitLog.cpp
 *
 *  Created on: 2016/10/08
 *      Author: 10097663
 */
#include "CUnitLog.h"
//#include "CGuiIf.h"		//2020/1/16
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include "time.h"
#include <winsock2.h>
#include <windows.h>

#define LOG_MAX 2*1024*1024			//ログサイズ
#define LOG_FILEPATH "MCTT.log"  //ログファイルパス(変更する)
#define LOG_LAST_STRING "@@@@"		//ログの最後の文字列

CUnitLog* CUnitLog::m_pInstance = 0;

/* --------------------------------------------------
 * @brief	シングルトン取得用
 *
 * -------------------------------------------------- */
CUnitLog* CUnitLog::GetInstance()
{
    if ( ! m_pInstance ) {
    	m_pInstance = new CUnitLog();
        if ( ! m_pInstance ) {
        	return 0;
        }
    }

    return m_pInstance;

}
/* --------------------------------------------------
 * @brief コンストラクタ
 *
 * -------------------------------------------------- */
CUnitLog::CUnitLog(){

	//内部変数初期化処理

	m_LogMax = LOG_MAX;
	m_LogTbl = (char*)malloc(LOG_MAX+128);

	//前回ファイルの読み込み処理
	FILE *fp = fopen(LOG_FILEPATH, "r");
	if (fp == NULL){			//ログファイルがない場合
		m_logSize = 0;
		m_LogIdx = 0;
	} else {					//ログがある場合
		struct stat filestat;	//ファイル情報
		int ret;
		unsigned long readSize;

		ret = stat(LOG_FILEPATH, &filestat);
		if (ret == 0){
			if (filestat.st_size > LOG_MAX){
				readSize = LOG_MAX;
			} else {
				readSize = filestat.st_size;
			}
			m_LogIdx = readSize;	//とりあえず最後にしておく
			fread(m_LogTbl, 1, readSize,fp);	//ファイル読み込み
			m_logSize = readSize;
		}
		fclose(fp);
		//最終更新ポイントを探す
		char* pLastPoint = strstr(m_LogTbl,LOG_LAST_STRING);
		if (pLastPoint != NULL){
			m_LogIdx = (int)(pLastPoint - m_LogTbl);
		}
	}

    //Tickのスタートタイムの設定
    DWORD start =GetTickCount();
    m_start_up_time = start/(double)1000;
}
/* --------------------------------------------------
 * @brief ログの出力
 * @brief ファイルへの書き出しも行う
 * -------------------------------------------------- */
void CUnitLog::putLog(const char* format,  ...){

	static bool bWrite = false;

	int i=0;
	va_list ap;

	long arg[9];
	char LogBuf[128];
	char Log2Buf[128];
	char FormatBuf[64];
	char TimeBuf[64];
	memset(arg,0x00,sizeof(arg));
	memset(LogBuf,0x00,sizeof(LogBuf));
	memset(Log2Buf,0x00,sizeof(Log2Buf));
	memset(FormatBuf,0x00,sizeof(FormatBuf));
	memset(TimeBuf,0x00,sizeof(TimeBuf));

	if (bWrite == false){
		bWrite = true;
		char strLine[] = "------------------------------------------------------------------------------------------------------\n";

	 	for(i=0;i<strlen(strLine);i++){
			*(m_LogTbl + m_LogIdx) = strLine[i];
			m_LogIdx++;
			m_LogIdx &= LOG_MAX-1;
		}
	}


	time_t now = time(NULL);
	struct tm *pnow = localtime(&now);

    //tickカウント取得
    DWORD current = GetTickCount();
    double accum = current/(double)1000;
    unsigned int tickTime = ((unsigned int)((accum - m_start_up_time)*100.0));
    sprintf(TimeBuf,"[%04d/%02d/%02d %02d:%02d:%02d] %06X ", (1900+pnow->tm_year), (1+pnow->tm_mon), pnow->tm_mday, pnow->tm_hour, pnow->tm_min, pnow->tm_sec, tickTime);

	va_start(ap, format);
	for(i=0;i<6;i++){
		arg[i]=va_arg(ap, long);
	}
	va_end(ap);

	strcpy(FormatBuf,TimeBuf);
	strcat(FormatBuf, format);
	strcat(FormatBuf, "\n");

	snprintf(LogBuf, sizeof(LogBuf), FormatBuf, arg[0],arg[1],arg[2],arg[3],arg[4],arg[5]);

	int MsgLen = (int)strlen(LogBuf);

 	for(i=0;i<MsgLen;i++){
		*(m_LogTbl + m_LogIdx) = *(LogBuf+i);
		m_LogIdx++;
		m_LogIdx &= LOG_MAX-1;
	}

	//終端に終端文字列を追加
	int overLogIdx = m_LogIdx;
	for(i=0;i<strlen(LOG_LAST_STRING);i++){
		*(m_LogTbl + overLogIdx) = *(LOG_LAST_STRING+i);
		overLogIdx++;
		overLogIdx &= LOG_MAX-1;
	}

	m_logSize += (int)(MsgLen + strlen(LOG_LAST_STRING));
	if (m_logSize >= LOG_MAX){
		m_logSize = LOG_MAX;
	}


}

void CUnitLog::putGuiLog(const char* format,  ...){
    int i=0;
    va_list ap;

    long arg[9];
    char LogBuf[128];
    char Log2Buf[128];
    char FormatBuf[64];
    char TimeBuf[64];
    memset(arg,0x00,sizeof(arg));
    memset(LogBuf,0x00,sizeof(LogBuf));
    memset(Log2Buf,0x00,sizeof(Log2Buf));
    memset(FormatBuf,0x00,sizeof(FormatBuf));
   memset(TimeBuf,0x00,sizeof(TimeBuf));

//    time_t now = time(NULL);
//    struct tm *pnow = localtime(&now);
//    sprintf(TimeBuf,"[%04d/%02d/%02d %02d:%02d:%02d]", (1900+pnow->tm_year), (1+pnow->tm_mon), pnow->tm_mday, pnow->tm_hour, pnow->tm_min, pnow->tm_sec);

   //tickカウント取得
   DWORD current = GetTickCount();
   double accum = current/(double)1000;
   unsigned int tickTime = ((unsigned int)((accum - m_start_up_time)*100.0));

    va_start(ap, format);
    for(i=0;i<6;i++){
        arg[i]=va_arg(ap, long);
    }
    va_end(ap);

//    strcpy(FormatBuf,TimeBuf);
    strcat(FormatBuf, format);
//    strcat(FormatBuf, "\n");

    snprintf(LogBuf, sizeof(LogBuf), FormatBuf, arg[0],arg[1],arg[2],arg[3],arg[4],arg[5]);
    sprintf(Log2Buf, "%06X %s", tickTime, LogBuf);  // tickを入れてログ出力

//    int MsgLen = strlen(LogBuf);

//    for(i=0;i<MsgLen;i++){
//        *(m_LogTbl + m_LogIdx) = *(LogBuf+i);
//        m_LogIdx++;
//        m_LogIdx &= LOG_MAX-1;
//    }

//    //終端に終端文字列を追加
//    int overLogIdx = m_LogIdx;
//    for(i=0;i<strlen(LOG_LAST_STRING);i++){
//        *(m_LogTbl + overLogIdx) = *(LOG_LAST_STRING+i);
//        overLogIdx++;
//        overLogIdx &= LOG_MAX-1;
//    }

//    CGuiIf::GetInstance()->PutLog(Log2Buf);		//2020/1/16
}
void CUnitLog::Update(){

	//ファイルへの出力
	FILE *fp = fopen(LOG_FILEPATH, "w");
	if (NULL != fp){
		fwrite(m_LogTbl,1,m_logSize,fp);
		fclose(fp);
	}

}
