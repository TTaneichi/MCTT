/*
	line sensor画像 "*.csv" 用class
    	Programmed by T. Kuwabara
*/
//---------------------------------------------------------------------------
#ifndef LineSensorH
#define LineSensorH

#include "imageData.h"
#include "CTDAT.h"
#include <string>

using namespace std;


const int LNSR_BIT_NUM = 16;
//---------------------------------------------------------------------------
class TLNSR : public CTDAT
{
private:

public:
	bool ReadCsv(const string fileName);
};
//---------------------------------------------------------------------------

const int DTA_BIT_NUM = 16;
class TDTA : public CTDAT
{
private:

public:
	bool ReadDta(const string fileName);
};
//---------------------------------------------------------------------------
class TBMP24 : public CTDAT
{
private:

public:
	bool ReadBitMap24(const string fileName);
};
//---------------------------------------------------------------------------
#endif
