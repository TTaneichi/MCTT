/*
	dpiX画像 "*.tif" 用class
    	Programmed by T. Kuwabara
*/
//---------------------------------------------------------------------------
#ifndef dpiXH
#define dpiXH

#include "imageData.h"
#include "CTDAT.h"
#include <string>

using namespace std;

//---------------------------------------------------------------------------
class TTIF : public CTDAT
{
private:
	int SubSize;		//副画素数
	int MainSize;		//主画素数

public:
	bool ReadTif(const string fileName);
//    TTIF& operator =(const TTMA &copy);
};
//---------------------------------------------------------------------------
#endif
