/*
 * CTSIMG.h
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#ifndef CTSIMG_H_
#define CTSIMG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

class TSIMG
{
private:

public:
    int Bit;
	short int **Dt;		//画像data（Dt[iM][iS]の順）
    int MainPixel;		//主画素数
    int SubPixel;		//副画素数


	bool ReadDt(string fileName);

	void NewDt(const int main, const int sub, const int iBit);
	void DeleteDt();

	bool bSetBit(const int iBit){if(iBit < 1 || iBit > 15){return(false);} Bit = iBit; return(true);}


	TSIMG() : Bit(0), Dt(NULL), MainPixel(0), SubPixel(0) {}
	TSIMG(const int iM, const int iS, const int iBit);
	~TSIMG();

};

#endif /* CTSIMG_H_ */
