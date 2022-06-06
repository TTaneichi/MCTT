
/*
 *  CUnit - A Unit testing framework library for C.
 *  Copyright (C) 2001       Anil Kumar
 *  Copyright (C) 2004-2006  Anil Kumar, Jerry St.Clair
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

//#include "CUnit.h"
#include "TestRun.h"
#include "CUTTimer.h"
#include "CUTOutputResult.h"

/*=================================================================
 *  Global/Static Definitions
 *=================================================================*/

/** CU_RunSummary to hold results of each test run. */
static CU_RunSummary f_run_summary = {"", "", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//テスト時間計測用
static CUTTimer s_timer;

//テスト結果のファイル出力用
static CUTOutputResult s_outResult;

/*=================================================================
 *  Public Interface functions
 *=================================================================*/
//NG数、テスト使用表示用
void CU_assertImplementation(int bValue,
		unsigned int NumOfNG,
		const char* strUTdescription,
        unsigned int uiLine,
        const char *strCondition,
        const char *strFile,
        const char *strFunction,
        int bFatal)
{
	//チェック追加
	if (strlen(strUTdescription)>300 ||
		strlen(strCondition)>100 ||
		strlen(strFile)>100 ||
		strlen(strFunction)>100 ||
		strlen(f_run_summary.SuitesName)>100 ||
		strlen(f_run_summary.SubSuitesName)>100){
		return;
	}

	char outPutStr[800];
	(void)memset(outPutStr,0x00, sizeof(outPutStr));

	++f_run_summary.nAsserts;
	++f_run_summary.nTotalAsserts;

	char *pResultStr;	//結果の文字列 OK / NG
	if (0 == bValue) {
		pResultStr = (char*)"NG";
		++f_run_summary.nAssertsFailed;
		++f_run_summary.nTotalAssertsFailed;
		//printf("Failed  File: %s L:%d    %s %s\n", strFile, uiLine, strCondition, strUTdescription);
		(void)sprintf(outPutStr, "Failed, %s L:%d,    %s, %s\n", strFile, uiLine, strCondition, strUTdescription);	//W050
		s_outResult.OutputResult(outPutStr);
	} else {
		pResultStr = (char*)"OK";
	}

	//全出力用の文字列を作る
	(void)memset(outPutStr,0x00, sizeof(outPutStr));
	(void)sprintf(outPutStr, "%03d_%03d_%03d, %s, %s, %s, %s, %d\n",
			f_run_summary.nSuitesRun, f_run_summary.nSubSuitesRun, f_run_summary.nAsserts,
			f_run_summary.SuitesName, f_run_summary.SubSuitesName, strUTdescription, pResultStr, NumOfNG);

	s_outResult.OutputAllResult(outPutStr);

	return;
}

void CU_start_section(const char* strSuiteName)
{
	if (strlen(strSuiteName) > 100){
		(void)printf("CU_start_section Failed parameter!!!\n");
		return;
	}

	char outPutStr[500];
	(void)memset(outPutStr,0x00, sizeof(outPutStr));

	(void)memset(f_run_summary.SuitesName,0x00,100);
	(void)strncpy(f_run_summary.SuitesName,strSuiteName,99);
	++f_run_summary.nSuitesRun;
	if (f_run_summary.nSubSuitesFailed > 0 || f_run_summary.nAssertsFailed > 0){
		++f_run_summary.nSuitesFailed;
	}
	f_run_summary.nAsserts = 0;
	f_run_summary.nAssertsFailed = 0;
	f_run_summary.nSubSuitesRun = 0;
	f_run_summary.nSubSuitesFailed = 0;

	//printf("Start Test Suites %d:%s\n",f_run_summary.nSuitesRun,strSuiteName);
	(void)sprintf(outPutStr,"Start Test Suites %d:%s\n",f_run_summary.nSuitesRun,strSuiteName);	//W050
	s_outResult.OutputResult(outPutStr);
}
/* 新サブセクションのテストの開始。
 *　本メソッドを呼ばなくてもテストは実行可能 */
void CU_start_subsection(const char* strSubSuiteName)
{
	if (strlen(strSubSuiteName) > 100){
		(void)printf("CU_start_subsection Failed parameter!!!\n");
		return;
	}

	char outPutStr[500];
	(void)memset(outPutStr,0x00, sizeof(outPutStr));

	(void)memset(f_run_summary.SubSuitesName,0x00,100);
	(void)strncpy(f_run_summary.SubSuitesName,strSubSuiteName,99);
	++f_run_summary.nSubSuitesRun;
	if (f_run_summary.nAssertsFailed > 0){
		++f_run_summary.nSubSuitesRun;
	}
	f_run_summary.nAsserts = 0;
	f_run_summary.nAssertsFailed = 0;

	//printf("Start Test Sub Suites %d:%s\n",f_run_summary.nSuitesRun,strSubSuiteName);
	(void)sprintf(outPutStr, "Start Test Sub Suites %d:%s\n",f_run_summary.nSuitesRun,strSubSuiteName);	//W050
	s_outResult.OutputResult(outPutStr);
}

/*　結果ファイルの指定 */
void CU_SetResultFilePath(char* strPath){
	s_outResult.SetResultFileName(strPath);
}

/* 集計結果ファイルの指定 */
void CU_SetAllResultFilePath(char* strPath){
	s_outResult.SetAllResultFileName(strPath);
}

void CU_display_summary()
{
	//チェック追加
	if (strlen(f_run_summary.SuitesName)>100 ||
		strlen(f_run_summary.SubSuitesName)>100){
		(void)printf("CU_display_summary Failed parameter!!!\n");
		return;
	}

	char outPutStr[10][500];
	(void)memset(outPutStr,0x00, sizeof(outPutStr));

	(void)sprintf(outPutStr[0],"=================================================\n");
	(void)sprintf(outPutStr[1],"Finish Test Suite:%s\n",f_run_summary.SuitesName);
	(void)sprintf(outPutStr[2],"Ran: %d       Passsed: %d            Failed: %d\n",
				f_run_summary.nAsserts,
				f_run_summary.nAsserts - f_run_summary.nAssertsFailed,
				f_run_summary.nAssertsFailed);
	(void)sprintf(outPutStr[3],"Total----\n");
	(void)sprintf(outPutStr[4],"Ran: %d       Passsed: %d            Failed: %d\n",
				f_run_summary.nTotalAsserts,
				f_run_summary.nTotalAsserts - f_run_summary.nTotalAssertsFailed,
				f_run_summary.nTotalAssertsFailed);
	(void)sprintf(outPutStr[5],"elapsed time:%lf sec\n",s_timer.GetElapsedTime());
	(void)sprintf(outPutStr[6],"=================================================\n");

	for(int i=0;i<7;i++){
		s_outResult.OutputResult(outPutStr[i]);
	}

}
//関数名の値を取得する
unsigned int CU_GetStringValue(const char* string)
{
	unsigned int coef = 1;
	int index = 0;
	unsigned int sum = 0;

	while(string[index] != 0x00){
		coef = (index == 0) ? 1 : coef * 128;
		sum += ((unsigned char)string[index] * coef);
		index = (index+1) % 4;
	}

	return sum;
}
