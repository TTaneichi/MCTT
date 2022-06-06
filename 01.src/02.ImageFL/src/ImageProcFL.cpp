//============================================================================
// Name        : ImageToolBase.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <math.h>
#include <iostream>
using namespace std;
#include <time.h>
#include <unistd.h>
#include "imageData.h"
#include "CTDAT.h"
#include "CTTMA.h"
#include "CCBCTCorrection.h"
#include "CCBCTCalib.h"
#include "CMainLoop.h"


int main() {

	CMainLoop mainLoopObj;

	mainLoopObj.Execute();

	return 0;
}

