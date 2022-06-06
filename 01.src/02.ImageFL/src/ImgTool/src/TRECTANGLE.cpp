/*
 * TRECTANGLE.cpp
 *
 *  Created on: 2021/03/31
 *      Author: 10097663
 */

#include "TRECTANGLE.h"

void TRECTANGLE::Set(const int iMain0, const int iMain1, const int iSub0, const int iSub1){
	iM0 = iMain0;
	iM1 = iMain1;
	iS0 = iSub0;
	iS1 = iSub1;
	iMWidth = iM1 - iM0 + 1;
	iSWidth = iS1 - iS0 + 1;
}

TRECTANGLE::TRECTANGLE(const int iMain0, const int iMain1, const int iSub0, const int iSub1):
	iM0(iMain0), iM1(iMain1), iS0(iSub0), iS1(iSub1){
	iMWidth = iM1 - iM0 + 1;
	iSWidth = iS1 - iS0 + 1;
}

