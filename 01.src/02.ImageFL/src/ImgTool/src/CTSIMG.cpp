/*
 * CTSIMG.cpp
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#include "CTSIMG.h"

/*
	deconstructor
*/
TSIMG::~TSIMG()
{
    DeleteDt();
}
//---------------------------------------------------------------------------
/*
	Dt[][]のdelete
*/
void TSIMG::DeleteDt()
{
    if(Dt != NULL){
    	for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
        	delete[] Dt[iMCnt];
        }
        delete[] Dt;

        Dt = NULL;
    }
}
//---------------------------------------------------------------------------
/*
	Dt[][]の領域確保
    	引数	main ->	主画素数
        		sub ->	副画素数
*/
void TSIMG::NewDt(const int main, const int sub, const int iBit)
{
    if(Dt != NULL){
        if(MainPixel == main && SubPixel == sub){
            return;
        }else{
            DeleteDt();
        }
    }

    Dt = new short int*[main];
    for (int i = 0 ; i < main ; ++i){
    	Dt[i] = new short int[sub];
    }
    Bit = iBit;
	MainPixel = main;
	SubPixel = sub;
}
//---------------------------------------------------------------------------
/*
	Read Data
    	引数	File Name

        戻り値	成功->true
        		失敗->false
*/
bool TSIMG::ReadDt(string fileName)
{

#if 0
	int handle;		//file handler

//Read Data File
    if((handle = open(fileName.c_str(), O_RDONLY|O_BINARY)) == -1){
//    	Application->MessageBox((fileName + " openできませんでした").c_str(), "Error", MB_OK);
    	return(false);
    }
//    int fileLen = filelength(handle);

    short int* iDt = new short int[MainPixel * SubPixel];		//Bufferを確保
    if((read(handle, iDt, MainPixel * SubPixel * sizeof(short int))) == -1){
//    	Application->MessageBox((fileName + " readできませんでした").c_str(), "Error", MB_OK);
        close(handle);
        delete[] iDt;
        return(false);
    }
    close(handle);

    NewDt(MainPixel, SubPixel, Bit);
    int iCt = 0;
    for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
    	for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
        	Dt[iMCnt][iSCnt] = iDt[iCt];
            ++iCt;
        }
    }
    delete[] iDt;

#endif

    return(true);
}
//---------------------------------------------------------------------------
/*
	constructor
    	引数	main ->	主画素数
				sub ->	副画素数
*/
TSIMG::TSIMG(const int iM, const int iS, const int iBit)
    :MainPixel(iM), SubPixel(iS), Bit(iBit)
{
    Dt = new short int*[MainPixel];
    for (int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
    	Dt[iMCnt] = new short int[SubPixel];
    }
}
