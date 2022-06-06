/*
 * CTIIMG.h
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#ifndef CTIIMG_H_
#define CTIIMG_H_

#include "CTDAT.h"
#include "imageData.h"

class TIIMG
{
private:
	bool DtExist;

public:
	int **Dt;		//画像data（Dt[iM][iS]の順）
//	__int64 **Dt;		//画像data（Dt[iM][iS]の順）
	int MainPixel;		//主画素数
	int SubPixel;		//副画素数

bool bCheckPos(const int iM, const int iS) const;

    void Fill(const int iVal);

    int iFindPos(const int iVal,
            int iDt[],
            int iS,
            int iE);
    void NewDt(const int main, const int sub);
    void DeleteDt();
	double dAve(const int iM1, const int iS1, const int iM2, const int iS2);//ttsuji追加
	double dAve(const TROI_Lag Roi);//ttsuji追加
	double dRMS(int iM0, int iS0, int iM1, int iS1);
	void Format(const int main, const int sub, const int mSt, const int sSt, const int back);
	void CopyFormat(const int iOrgSub, const int iOrgMain,
			TIIMG& CopyImg, const int iCopySubPixel, const int iCopyMainPixel,
							const int iCopyOrgSub, const int iCopyOrgMain);
    void MainSub();
    void MainMoveAve(const int iLength);
	void RotateCW();
	void Rotate(const int angle);
	void RtoL();
    void SetVal(const int iMain, const int iSub, const float fVal);
    void SubMoveAve(const int iLength);
	bool SubExpandMultiAve(const string fileName);//ttsuji 140927 (LongPanelのMCコンペア用として)
	bool SubExpandMultiAve(const string fileName, const int iShiftBit);//ttsuji 150917 (LongPanel粒状抑制処理のLaplacianピラミッド処理のMCコンペア用として)
	void SubExpandFilter(const float fFilter[], const int iLength);//ttsuji 150612 (LongPanelの短冊横傘副作用調査用として)
	void MainMedian(const int iNum);
	void MainMedianAveExpand2(const int iNum);//ttsuji 200903 (DEVO3 ラグ・オフセット分離補正コンペアのMedian橋画素処理用として)
	void MainAveExpand2(const int iNum);//ttsuji 200903 (DEVO3 ラグ・オフセット分離補正コンペアのMedian橋画素処理用として)
	int iMedian(int& iM0, int& iS0, int& iM1, int& iS1);
	void SubMedian(const int iNum);
	void SubMedianAveExpand2(const int iNum);//ttsuji 200903 (DEVO3 ラグ・オフセット分離補正コンペアのMedian橋画素処理用として)
	void SubAveExpand2(const int iNum);//ttsuji 200903 (DEVO3 ラグ・オフセット分離補正コンペアのMedian橋画素処理用として)
	void TrimArea(int& iM0, int& iS0, int& iM1, int& iS1);

    TIIMG(){DtExist = false;}
	TIIMG(const int iM, const int iS);
	explicit TIIMG(const CTDAT &copy);//ttsuji修正 090905 暗黙キャストを避けるため

	~TIIMG();
//    TFIMG(const TFIMG &copy);
	TIIMG& operator =(const TIIMG &copy);
	TIIMG& operator =(const CTDAT &copy);
	TIIMG& operator =(const TSIMG &copy);
//    TFIMG& operator -(const TFIMG &oper);
//    TFIMG& operator -(const TDAT &oper);
//    TFIMG& operator +=(const float fConst);
//    TFIMG& operator +=(const TFIMG& oper2);
	TIIMG& operator +=(const CTDAT& oper2);
	TIIMG& operator +=(const TIIMG& oper2); //ttsuji追加 090904
	TIIMG& operator -=(const CTDAT& oper2);
	TIIMG& operator -=(const TIIMG& oper2);
//    TFIMG& operator -=(const TDAT& oper2);
//    TDAT& operator *=(const TDAT& oper2);
  //  TFIMG& operator *=(const float oper2);
//    TFIMG& operator /=(const int oper2);
//    TFIMG& operator /=(const double oper2);
//    TFIMG& operator /=(const TDAT& oper2);
//    TDAT& operator <<=(const int oper2);
//    TDAT& operator >>=(const int oper2);
//    TDAT& operator /(const float oper2);
	TIIMG& operator +(const int iOffset);//ttsuji追加
//	TIIMG operator +(const int iOffset);//ttsuji追加
	TIIMG& operator =(const int iVal);//ttsuji
	TIIMG& operator /=(const CTDAT dat);//ttsuji
	TIIMG& operator *=(const int iVal);//ttsuji
	TIIMG& operator *=(const float fVal);//ttsuji
	TIIMG& operator /=(const int iVal);//ttsuji
	TIIMG& operator *=(const TIIMG& oper2);//ttsuji
	void SaveTmaTdt(string str);//tssuji
	void SaveTmaTdt(string str, const unsigned short unOff);
	bool bSaveAsText(const string fileName);
	void Clip(int iMin, int iMax);//ttsuji
	bool bReadFromText(const string& fileName, const int iSkipLine,
		const char cMark,       //text　区切り文字
		TIIMG& maxSizeImg);

};

#endif /* CTIIMG_H_ */
