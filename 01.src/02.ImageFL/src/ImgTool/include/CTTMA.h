/*
 * CTTMA.h
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#ifndef CTTMA_H_
#define CTTMA_H_

#include "imageData.h"
#include "CTDAT.h"
#include "CTSIMG.h"
#include "LineSensor.h"
#include "dpiX.h"

const int HEADDER_SIZE = 256 ;
const int X_FILE_SIZE = 5;		//Headderの主画素数byte
const int Y_FILE_SIZE = 5;		//Headderの副画素数byte
const int MAIN_GASO_MITSUDO = 2;		//主走査方向画素密度byte
const int SUB_GASO_MITSUDO = 2;		//副走査方向画素密度byte
const int COMP_LOC = 24;		//Headder内の画像圧縮方法位置
const int BIT = 97;		//画像data bit数 位置

const   int iEDR_OFF    =   0;
const   int iEDR_FIX    =   1;
const   int iEDR_SEMIAUTO = 2;
const int iEDR_POS = 154;
const int iEDR_BYTE = 1;

const int iSk_POS = 155;
const int iSk_BYTE = 3;

const int iL_POS = 165;
const int iL_BYTE = 3;
//---------------------------------------------------------------------------
class TSIMGH : public TSIMG
{
private:


public:
    char Head[HEADDER_SIZE];		//Headder
	float m_fMainPixelSize, m_fSubPixelSize;
	int iEDR;
	int iSk;    //*100
    int iL;     //*100
    char mainGasoMitsudo[MAIN_GASO_MITSUDO + 1];		//主走査方向画素密度
    char subGasoMitsudo[SUB_GASO_MITSUDO + 1];		//副走査方向画素密度

    float MitsudoToPixelSize(char* psMitsudo);
    void NewDt(const int sub, const int main, const int bit,
			const float fPixelSize = 0.1f,
            const int iEDRVal = iEDR_OFF,
            const int iSkVal = 200,
            const int iLVal = 400);
	bool ReadTma(const string fileName);
    void SetMainPixelSize(const float fPixelSize){m_fMainPixelSize = fPixelSize;}
    void SetSubPixelSize(const float fPixelSize){m_fSubPixelSize = fPixelSize;}

//    TSIMGH();
};
//---------------------------------------------------------------------------
class TTMA : public CTDAT
{
private:

public:

    char Head[HEADDER_SIZE];		//Headder
    float m_fMainPixelSize, m_fSubPixelSize;
	int iEDR;
    int iSk;    //*100
	int iL;     //*100
    string Comment;   // フイルムに出力されるコメント

    char mainGasoMitsudo[MAIN_GASO_MITSUDO + 1];		//主走査方向画素密度
    char subGasoMitsudo[SUB_GASO_MITSUDO + 1];		//副走査方向画素密度
#if 0
    void AntiEDR();
    void AntiEDR(const int iS, const float fL, const int iNewBit = 12);
    void BetaEDR(const float fMag = 4.0f);
    bool bReadFromText(const string& fileName, const int iSkipLine, const char cMark, TTMA& maxSizeImg);
    bool bUSM(const int iRn, const float fRe);
    void EDR();
    void EDR(const int iS, const float fL, const int iNewBit = 10);
    int iGetEDR(){return(iEDR);};
    int iGetL(){return(iL);};
    int iGetSk(){return(iSk);};
    float fGetL(){return(iL / 100.0f);};
    int iGetS();
#endif
    int iReadHeadder(string fileName);
#if 0
    float fGetSk(){return(iSk / 100.0f);};
#endif
    float MitsudoToPixelSize(char* psMitsudo);

    void NewDt(const int sub, const int main, const int bit,
            const float fPixelSize = 0.1f,
            const int iEDRVal = iEDR_OFF,
            const int iSkVal = 200,
            const int iLVal = 400,/*);*/
            const string cComment = "");

//	bool ReadTma(const AnsiString fileName);
	bool ReadTma(const string fileName, const bool bShowError = true);
#if 0
	bool ReadSCN(const string fileName);
	bool ReadFramesSkipHead(const string fileName, const int iFrame);//ttsuji追加
#endif
	bool SaveTma(const string fileName, const bool bVerify = true);
	bool SaveTmaTdt(const string fileName);//ttsuji追加（WriteTma関数のラッパー）
#if 0
	void SetEDR(const int iVal);
#endif
	bool SetGasoMitsudo(const float fMitsudo);
	bool SetGasoMitsudo(const int iMitsudo);
	void SetGasoMitsudo();
#if 0
	void SetL(const float fL){iL = int(fL * 100.0f + 0.5);};
#endif
	void SetMainPixelSize(const float fPixelSize){m_fMainPixelSize = fPixelSize;}
	void SetSubPixelSize(const float fPixelSize){m_fSubPixelSize = fPixelSize;}
	void SetSkWithS(const int iSVal);
	bool WriteTma(const string fileName, const bool bVerify = true);
#if 0
	bool WriteSCN(const string fileName);
#endif
	float fMainPixelSize() const {return(m_fMainPixelSize);}
    float fSubPixelSize() const {return(m_fSubPixelSize);}

    TTMA();
	TTMA(const TFIMG &copy);
	TTMA(const string fileName){ReadTma(fileName);}

    TTMA(const int iS, const int iM, const int iBit,
            const float fPixelSize = 0.1f,
            const int iEDRVal = iEDR_OFF,
            const int iSkVal = 200,
            const int iLVal = 400){NewDt(iS, iM, iBit, fPixelSize, iEDRVal, iSkVal, iLVal);}

	TTMA(const TTMA &copy);
    TTMA& operator =(const CTDAT &copy);
    TTMA& operator =(const TTIF &copy);
    TTMA& operator =(TTMA &copy);
#if 0
    TTMA& operator =(const TLNSR &copy);
    TTMA& operator =(const TDTA &copy);
    TTMA& operator =(const TBMP24 &copy);
    TTMA& operator =(const TFIMG &copy);

	friend class TMARS;
#endif
//	friend class TSIMARS;
};
#endif /* CTTMA_H_ */
