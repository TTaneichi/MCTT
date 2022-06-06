/*
 * CTSINGLEDATA.h
 *
 *  Created on: 2021/04/01
 *      Author: 10097663
 */

#ifndef CTSINGLEDATA_H_
#define CTSINGLEDATA_H_

#include <string>
using namespace std;

class TSINGLE_DATA
{
private:
	bool bDtExist;

public:
	double *dDt;		//data
	int iDtNum;		//data数

    void    ClearDt();
    void    Bin(const int iBin);
	void BinInvRest(const int iBin);
	void BinInvTopSideRest(const int iBin);
	bool	bReadFromText(const string fileName);
	void    CopyTo(TSINGLE_DATA& destination);
	int     iAbsMaxPos();
	int     iFindAbsMaxPos(const int iStart, const int iEnd);
    int     iAbsSabunMax(const int iStart, const int iEnd);
	double  dSum();
	double  dAve(const int iStart, const int iEnd);
	double  dMedian(const int iStart, const int iEnd);
	double  dFindNanameEdge();
	double  dFindNanameEdgeDD();
	double  dFindPos(const double dValue);
	double  dFindPos(const double dValue, const int iS);
	double  dFindHalfPos(const double dRatio);
	void    Fill(const double dVAL);
	void    FormatCopy(TSINGLE_DATA& destination, const int iDestHead,
						const int iSourceHead, const int iSourceNum);
	void FWHM(const int iPeak, int& iSt, int& iEnd);
    void FindPeakNextPos(const int iSt, int& iPrev, int& iAft);
	void DeleteDt();
	void LinExpand(const int iExp);
	void LinExpandCnvRest(const int iExp, const int iRestNum);
	void LinExpandCnvTopSideRest(const int iExp, const int iRestNum);
    void Median(const int iNum);
	void MoveAve(const int iNum);
	void MoveAve2(const int iNum);
	void NewDt(const int iNum);
    void Sabun();
	void SabunMinMaxPos(int& iSabunMinPos, int& iSabunMaxPos);
	int  SabunToAndFro(int iStPos, int iEndPos);
	bool SaveWithIndex(const string fileName);
    int  iValPosSearch(const double dVal);
	int  dFindHalfPos(double& dB0, double& dB1, int iSEnd);
	void Format(const int iOffset, double dBack = 0.0);

    TSINGLE_DATA(){bDtExist = false;}
	~TSINGLE_DATA();
    TSINGLE_DATA(const TSINGLE_DATA &copy);
    TSINGLE_DATA(const int iNum) : bDtExist(true), iDtNum(iNum) {dDt = new double[iNum];}
	TSINGLE_DATA& operator =(const TSINGLE_DATA &copy);
	TSINGLE_DATA& operator +=(const TSINGLE_DATA& oper2);
    TSINGLE_DATA& operator -=(const TSINGLE_DATA& oper2);
    TSINGLE_DATA& operator *=(const float fVal);
    TSINGLE_DATA& operator /=(const int oper2);
	TSINGLE_DATA& operator +=(const float fVal);//ttsuji追加
	TSINGLE_DATA& operator -=(const float fVal);//ttsuji追加
};

#endif /* CTSINGLEDATA_H_ */
