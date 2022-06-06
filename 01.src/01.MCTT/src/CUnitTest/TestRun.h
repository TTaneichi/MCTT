/*
 * TestRun.h
 *
 *  Created on: 2014/05/27
 *      Author: 10097663
 */
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

#ifndef TESTRUN_H_
#define TESTRUN_H_

//#include "CUnit.h"

/** Simple assertion.
 *  Reports failure with no other action.
 */
#define CU_ASSERT(value) \
	{ CU_assertImplementation((value), 0, "", __LINE__, #value, __FILE__, __func__, 0); }

#define CU_ASSERT2(value, NumOfNG) \
	{ CU_assertImplementation((value), (NumOfNG), "", __LINE__, #value, __FILE__ , __func__, 0); }

#define CU_ASSERT3(value, NumOfNG, UTdescription) \
	{ CU_assertImplementation((value), (NumOfNG),  (UTdescription), __LINE__, #value, __FILE__, __func__, 0); }

#define CU_GETSTRVALUE() \
	{ CU_GetStringValue(__func__); }

/** Types of failures occurring during test runs. */
typedef enum CU_FailureTypes
{
  CUF_SuiteInactive = 1,    /**< Inactive suite was run. */
  CUF_SuiteInitFailed,      /**< Suite initialization function failed. */
  CUF_SuiteCleanupFailed,   /**< Suite cleanup function failed. */
  CUF_TestInactive,         /**< Inactive test was run. */
  CUF_AssertFailed          /**< CUnit assertion failed during test run. */
} CU_FailureType;           /**< Failure type. */

/* CU_FailureRecord type definition. */
/** Data type for holding assertion failure information (linked list). */
typedef struct CU_FailureRecord
{
  CU_FailureType  type;           /**< Failure type. */
  unsigned int    uiLineNumber;   /**< Line number of failure. */
  char*           strFileName;    /**< Name of file where failure occurred. */
  char*           strCondition;   /**< Test condition which failed. */
//  CU_pTest        pTest;          /**< Test containing failure. */
//  CU_pSuite       pSuite;         /**< Suite containing test having failure. */

  struct CU_FailureRecord* pNext; /**< Pointer to next record in linked list. */
  struct CU_FailureRecord* pPrev; /**< Pointer to previous record in linked list. */

} CU_FailureRecord;
typedef CU_FailureRecord* CU_pFailureRecord;  /**< Pointer to CU_FailureRecord. */

/* CU_RunSummary type definition. */
/** Data type for holding statistics and assertion failures for a test run. */
typedef struct CU_RunSummary
{
  char SuitesName[100];		  	  /**< セクション名 */
  char SubSuitesName[100];		  /**< サブセクション名 */
  unsigned int nSuitesRun;        /**< Number of suites completed during run. */
  unsigned int nSuitesFailed;     /**< Number of suites for which initialization failed. */
  unsigned int nSuitesInactive;   /**< Number of suites which were inactive. */
  unsigned int nSubSuitesRun;     /**< Number of subsuites completed during run. */
  unsigned int nSubSuitesFailed;  /**< Number of subsuites for which initialization failed. */
  unsigned int nTestsRun;         /**< Number of tests completed during run. */
  unsigned int nTestsFailed;      /**< Number of tests containing failed assertions. */
  unsigned int nTestsInactive;    /**< Number of tests which were inactive (in active suites). */
  unsigned int nAsserts;          /**< Number of assertions tested during run. */
  unsigned int nAssertsFailed;    /**< Number of failed assertions. */
  unsigned int nFailureRecords;   /**< Number of failure records generated. */
  unsigned int nTotalAsserts;	  /**< トータルのアサート数 */
  unsigned int nTotalAssertsFailed;	/**< トータルのNG数 */
  double       ElapsedTime;       /**< Elapsed time for run in seconds. */
} CU_RunSummary;
typedef CU_RunSummary* CU_pRunSummary;  /**< Pointer to CU_RunSummary. */

/*テスト判定（アサート）.通常、マクロから呼び出されるため、直接は呼ばない。 */
void CU_assertImplementation(int bValue,
		unsigned int NumOfNG,
		const char* strUTdescription,
        unsigned int uiLine,
        const char *strCondition,
        const char *strFile,
        const char *strFunction,
        int bFatal);

/* 新セクションのテストの開始。
       本メソッドを呼ばなくてもテストは実施可能 */
void CU_start_section(const char* strSuiteName);

/* 新サブセクションのテストの開始。
 *　本メソッドを呼ばなくてもテストは実行可能 */
void CU_start_subsection(const char* strSubSuiteName);

/*　結果ファイルの指定 */
void CU_SetResultFilePath(char* strPath);

/* 集計結果ファイルの指定 */
void CU_SetAllResultFilePath(char* strPath);

//テストのサマリ表示
void CU_display_summary();

//関数名の値を取得する
unsigned int CU_GetStringValue(const char* string);

#endif /* TESTRUN_H_ */
