/***********************************************************
 * @file    CTFIMG.h
 * @brief	float型の画素値を持つ画像クラス
 * @author	T.Taneichi
 * @date    2021/04/01 imageData.hから移植
 * Copyright(c) 2021 FUJIFILM Co.,Ltd.
 ***********************************************************/
#ifndef CTFIMG_H_
#define CTFIMG_H_

#include <string>
using namespace std;

class TSINGLE_DATA;
class CTDAT;
class TIIMG;
class TTMA;

class TFIMG
{
private:
	bool DtExist;

public:
	float **Dt;		//画像data（Dt[iM][iS]の順）
    int MainPixel;		//主画素数
	int SubPixel;		//副画素数

//    bool ReadDt(const string fileName);
//    bool ReadDtD(const string fileName);
    void Abs();
    double AverageD(int iMainSrt, int iMainEnd, int iSubSrt, int iSubEnd);
    double SummationD(int iMainSrt, int iMainEnd, int iSubSrt, int iSubEnd);
	void Bin(float thres, float xmin, float xmax);
	bool bReadRaw(string fileName, int iMainPixel, int iSubPixel, int iPixelByte, int iHeaderByte);//ttsuji追加(180402)
	bool bReadTxt(string fileName);
    bool bWriteTxt(const string fileName, string="");
	bool bSaveAsText(const string fileName);
	bool bSaveAsText(const string fileName, bool bLineFeed);//ttsuji121116
    bool bSaveAsTextWithIndex(const string fileName);
    bool bSubLine(TSINGLE_DATA& line,
                    const int iMainStart, const int iMainEnd);
    bool bCheckPos(const int iS, const int iM) const;
    void CopyFormat(const int iOrgMain, const int iOrgSub,
            CTDAT& CopyImg, const int iCopySubPixel, const int iCopyMainPixel,
                            const int iCopyOrgSub, const int iCopyOrgMain);
	void CopyFormat(const int iOrgMain, const int iOrgSub,
			TFIMG& CopyImg, const int iCopyMainPixel, const int iCopySubPixel,
							 const int iCopyOrgMain, const int iCopyOrgSub);
    double  dAve(int iMain1, int iSub1, int iMain2, int iSub2);
    void DifS1_0();
    void Div(const int iMainDiv,
             const int iSubDiv);
	void DivWithRest(const int iMainDiv, const int iSubDiv);
	bool DivMabikiForOddSizeImage(const int iDiv);//縦横が奇数画素数の間引き
	void DivTrancation(const int iMainDiv, const int iSubDiv);//平均縮小は切り捨て(ttsuji追加)
	float fAbsMax(const int iMain1, const int iSub1,
            const int iMain2, const int iSub2);
    float fAve(const int iMain1, const int iSub1,
            const int iMain2, const int iSub2);
    void    FFT2D(const int iFFTNum);        //2D-FFT
//2D-FFT 指定領域を計算幅/2ずらしながら計算したpower平均値を求める
    void    FFT2DRepeat(const int iFFTNum,      //計算幅2^n
                        const int iMain1,        //計算領域主開始座標
                        const int iSub1,        //計算領域副開始座標
                        const int iMain2,        //計算領域主終了座標
                        const int iSub2,        //計算領域副終了座標
                        TFIMG&   result);        //2D-FFTpower平均結果
//2D-FFT 指定領域を計算幅/2ずらしながら平均値を引いてから計算したpower平均値を求める
    void    FFT2DRepeatRemoveAve(const int iFFTNum,      //計算幅2^n
                        const int iMain1,        //計算領域主開始座標
                        const int iSub1,        //計算領域副開始座標
                        const int iMain2,        //計算領域主終了座標
                        const int iSub2,        //計算領域副終了座標
                        TFIMG&   result);        //2D-FFTpower平均結果
    void Fill(const float fVal);
    float fMedian(int iM0, int iS0, int iM1, int iS1, const float fTh = 0.0f);
    void Format(int main, int sub, const int mSt, const int sSt, const float back);
    float fRMS( const int iMain1,        //計算領域主開始座標
                const int iSub1,        //計算領域副開始座標
                const int iMain2,        //計算領域主終了座標
                const int iSub2);        //計算領域副終了座標
    float fYKMedian(int iM0, int iS0, int iM1, int iS1, const TFIMG& img, const float fTh);

    int iFindPos(const float fVal,
            float fDt[],
            int iS,
            int iE);
    void LinearExpand(const int iMainExpand,
             const int iSubExpand);
	void LMS(double& dA, double& dB);
    void NewDt(const int main,const int sub);
    void DeleteDt();
//    short int Average();
//    double RMS();
//    double Variance();
//   double VarianceD();
//    void Format(const int sub, const int main, const int sSt, const int mSt, const unsigned short int back);
//    void Div(const int sDiv, const int mDiv);
//    void Mul(const int sMul, const int mMul);
//    void MainSub();
//    void Shift(const int num, const unsigned short int background);
//    void RtoL();
//    bool RemoveMainShading(const int start, const int last);
//    bool Suji(const int start, const int last, const int addConst);
//    bool LineFilter(const int start, const int last, const int ave);
//    void Averaging(int dt[], const int s, int e, const int aveNum);
//    void BadCor();
//    bool BadFind(bool **bad, const float threshold);
//    void ExtractOddLine();
//    void ExtractEvenLine();
    void MainAve(const int iStart, const int iEnd);
    void MainMoveAve(const int iLength);
    void MainExpandFilter(const float fFilter[], const int iLength);
    void MainExpandMoveAve(const int aveNum);
//    float Maxmin(int x1, int y1, int x2, int y2, bool swit=true);
	float Maxmin(int x1, int x2, int y1, int y2, bool bReturnMax = true);
	void SubMoveAve(const int iLength);
    void SubExpandMoveAve(const int aveNum);
    void Filter(const float fFilter[], const int iLength, const bool isMain);
    void MainFilter(const float fFilter[], const int iLength);
    void MainLowFilter(const float fFilter[], const int iLength);
    bool MainExpandMultiAve(const string fileName);
	bool MainExpandMultiAveD3(const string fileName);//Fukuyo追加
	bool MainExpandMultiAveForEachDetectedLine(const string fileName, CTDAT& DetectedLineImg);
    bool MainMultiAve(const string fileName);
	bool ReadDt(const string fileName, int mainp, int subp);
    bool Save(const string fileName);
	bool SaveDt(const string fileName);
    bool SaveDoubleDt(const string fileName);
    bool ReadDoubleDt(const string fileName, int mainp, int subp);
	void SaveTmaTdt(string str,
			const double dFct = 1.0f, const unsigned short unOff = 0);
    void SubAve(const int iStart, const int iEnd);
    void SubMoveAveAveExpand(const int aveNum);
    void SubExpandFilter(const float fFilter[], const int iLength);
    bool SubExpandMultiAve(const string fileName);
	bool SubExpandMultiAveD3(const string fileName);//Fukuyo追加
//    bool bSubLine(TSINGLE_DATA& line, const int iMainStart, const int iMainEnd);
    bool SubMultiAve(const string fileName);
    void SubFilter(const float fFilter[], const int iLength);
//    void Filter(const float filter[], const int f_num, const bool main);
    void MainMedian(const int iNum);
    void SubMedian(const int iNum);
    void    TrimArea(int& iM0, int& iS0, int& iM1, int& iS1);
//    void Extract(const int iSub, const int iMain);
	void MainSub();//ttsuji追加 140523
    bool DifSub(int iPos, int iPosRef);//ttsuji追加 170912
	void WriteTma(string str, const double dFct, const unsigned short unOff, bool b);//kuwabara追加
	void LogToLinear2(const int iBit, const double dCenter, const int iKeta);//ttsuji追加 180617

    TFIMG(){DtExist = false;}
    ~TFIMG();
    TFIMG(const CTDAT& tdat);
    TFIMG(const TFIMG &copy);
    TFIMG(const int iM, const int iS);
	TFIMG& operator =(const TFIMG &copy);
	TFIMG& operator =(const CTDAT &copy);
	TFIMG& operator =(const TIIMG &copy);//ttsuji追加
	TFIMG& operator -(const TFIMG &oper);
    TFIMG& operator -(const CTDAT &oper);
    TFIMG& operator +=(const float fConst);
	TFIMG& operator +=(const TFIMG& oper2);
    TFIMG& operator +=(const CTDAT& oper2);
	TFIMG& operator -=(const TFIMG& oper2);
    TFIMG& operator -=(const CTDAT& oper2);
//    TDAT& operator *=(const TDAT& oper2);
	TFIMG& operator *=(const float oper2);
    TFIMG& operator /=(const int oper2);
	TFIMG& operator /=(const double oper2);
    TFIMG& operator /=(const CTDAT& oper2);
//    TDAT& operator <<=(const int oper2);
//    TDAT& operator >>=(const int oper2);
//    TDAT& operator /(const float oper2);
	TFIMG operator +(const int iOffset);//ttsuji追加
	TFIMG operator -(const int iOffset);//ttsuji追加
	TFIMG operator *(const float iOffset);
	TFIMG operator /(const TFIMG Fimg);//ttsuji追加

};

#endif /* CTFIMG_H_ */
