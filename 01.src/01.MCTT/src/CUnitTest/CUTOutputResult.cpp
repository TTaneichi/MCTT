/*
 * CUTOutputResult.cpp
 *
 *  Created on: 2018/06/28
 *      Author: 10097663
 */

#include <stdio.h>
#include <CUTOutputResult.h>

CUTOutputResult::CUTOutputResult(){
	m_pStrResultFileName = NULL;
	m_pStrAllResultFileName = NULL;
	m_bMadeResultFile = false;
	m_bMadeAllResultFile = false;
}

CUTOutputResult::~CUTOutputResult(){

}

//結果ファイル名を指定する
void CUTOutputResult::SetResultFileName(char* strFileName){
	m_pStrResultFileName = strFileName;
	m_bMadeResultFile = false;
}

//集計用の結果ファイル名を指定する
void CUTOutputResult::SetAllResultFileName(char* strFileName){
	m_pStrAllResultFileName = strFileName;
	m_bMadeAllResultFile = false;
}

//結果ファイルへ出力する
void CUTOutputResult::OutputResult(const char* output){

	FILE *fp;

	if (m_bMadeResultFile == false){
		m_bMadeResultFile = true;
		fp = fopen(m_pStrResultFileName, "w");
	} else {
		fp = fopen(m_pStrResultFileName, "a");
	}

	if (fp == NULL){
		(void)printf("%s",output);
		return;
	}

	(void)fprintf(fp,"%s",output);

	(void)fclose(fp);

}

//エラーファイルへ出力する
void CUTOutputResult::OutputAllResult(const char* output){

	FILE *fp;

	if (m_bMadeAllResultFile == false){
		m_bMadeAllResultFile = true;
		fp = fopen(m_pStrAllResultFileName, "w");
	} else {
		fp = fopen(m_pStrAllResultFileName, "a");
	}

	if (fp == NULL){
		(void)printf("%s",output);
		return;
	}

	(void)fprintf(fp,"%s",output);

	(void)fclose(fp);
}

