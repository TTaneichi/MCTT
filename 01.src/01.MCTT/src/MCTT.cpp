//============================================================================
// Name        : MCTT.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;
#include "CInitFileLoader.h"
#include "CMainLoop.h"
#include "CUnitLog.h"
#include "CUTCSETranslator.h"
#include "CUTCMPTranslator.h"

bool Initialize();
void UnitTest();

int main() {

	bool bRet = false;
	bool bUnitTest = false;

	if (bUnitTest){		//ユニットテスト時

		UnitTest();

	} else {			//通常時
		SIMLOG_NORM("[ALL] ==========================");
		SIMLOG_NORM("[ALL]      Start MCTT           ");
		SIMLOG_NORM("[ALL] ==========================");

		bRet = Initialize();

		CMainLoop mainLoop;

		mainLoop.Execute();
	}

	cout << "Program End!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
void UnitTest(){

	CUT_CSETranslator utObj;
	utObj.Execute();

	CUTCMPTranslator MP_UTObj;
	MP_UTObj.Execute();
}

bool Initialize(){

	SIMLOG_NORM("[ALL] ====   Initialize Start ===");

	bool bRet;
	CInitFileLoader	fileLoader;

	bRet = fileLoader.LoadInitFile();

	return true;
}
