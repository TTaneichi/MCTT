/*
 * CTDAT.h
 *
 *  Created on: 2021/03/30
 *      Author: 10097663
 */

#ifndef CTDAT_H_
#define CTDAT_H_

#include "imageData.h"
#include "CTFIMG.h"

class CTDAT {
private:


public:
	bool DtExist;
	bool HistExist;
	unsigned short int **Dt;		//画像data（Dt[iS][iM]の順）
	unsigned short int DtMax;		//画像data最大値
	int MainPixel;		//主画素数
    int SubPixel;		//副画素数
	int Bit;			//画像data bit数
	int *Hist;			//Histogram
	long HistRuisekiSum;//Histの累積度数の和　(ttsuji 140518)

	CTDAT();

	CTDAT(const int iS, const int iM, const int iBit);

	CTDAT(const CTDAT &copy);

	~CTDAT();

	//Dt[][]メモリ確保		副・主の順序
	void NewDt(const int sub, const int main, const int bit);

	//Dt[][]メモリ開放
	void DeleteDt();

	//Bitの設定(DtMaxの設定も行う)
	void SetBit(const int in_bit);

	CTDAT& operator =(const CTDAT &copy);
//	CTDAT& operator =(const TFIMG &copy);
//	CTDAT& operator =(const TIIMG& copy);//ttsuji

	CTDAT  operator +(const int oper);//ttsuji追加

	CTDAT  operator -(const int oper);//ttsuji追加

	CTDAT  operator *(const float fVal);//ttsuji追加
//	TFIMG operator -(const TFIMG& oper);

	CTDAT& operator +=(const int oper2);

	CTDAT& operator -=(const int oper);

	CTDAT& operator -=(const double dVal);


	CTDAT& operator *=(const float oper2);


	CTDAT& operator *=(const int oper2);


	CTDAT& operator /=(const int oper2);


	CTDAT& operator /=(const float fOper2);
//    CTDAT& operator /=(const TFIMG& oper2);


	CTDAT& operator <<=(const int oper2);


	CTDAT& operator >>=(const int oper2);


	CTDAT& operator /(const float oper2);


	CTDAT  operator +(const CTDAT& copy);//ttsuji追加

	CTDAT& operator +=(const CTDAT& oper2);
//    CTDAT& operator +=(const TFIMG& oper2);


	CTDAT& operator -=(const CTDAT& oper2);
//    CTDAT& operator -=(const TFIMG& oper);


	CTDAT& operator *=(const CTDAT& oper2);


	 //HistExistのフラグを取得
	bool GetFlagHistExist()	{return HistExist;}

	//副方向段差検出？
	//副差分絶対値が指定値以上でかつ主差分絶対値が指定値以下の場合には該当画素を最大値にし、そうでない場合は、0にする
	void Grad(const int iSDifOffset, const int iMDifOffset, CTDAT& result);

	//副方向段差検出？
	void Grad(const int iDifOffset, CTDAT& result);

	//全画素の平均値を求める
	double dAve();

	//指定領域の平均値を求める
	double dAve(const int iS1, const int iM1, const int iS2, const int iM2);

	//画像加算(コメントアウトされている)
	void Add(CTDAT& addImg);

	//Bit変換 dataも変換
	void BitChange(const int iNewBit);

	//Bad Pixel補間   周囲8画素の正常画素の平均値とする
	void BadCor();

	//Bad Pixel補間
	void BadCor(bool **bad, int iCorNum = 100);

	//Bad Pixel補間
	void BadCor2(bool **bad);

	//Bad Pixel補間 欠陥周辺画素の距離で重みづけ
	bool bBadCorAround(CTDAT& bad);

	//引数領域が画像範囲内かの確認
    bool bCheckArea(const int iS1, const int iM1, const int iS2, const int iM2);

    //引数領域が画像範囲内かの確認
    bool bCheckArea(TRECTANGLE area);

    //引数領域が画像範囲内かの確認
    bool bCheckPos(const int iS, const int iM) const;

	//画像データをdestinationにコピーする
	void Copy(CTDAT& destination);

	void Copy(TFIMG& fimg, const int iBit);

	//floatの画像を画像サイズと基準値を指定してコピーする
	void Copy(float* fDt, unsigned short int iNormalize, int iMainPixel, int iSubPixel, int iBit);

	void Copy(TFIMG& destination);

	//0でないbadの連結領域をcopyへnValでcopyする
	void Copy(CTDAT& bad, const int iM0, const int iS0, CTDAT& copy, const unsigned short int nVal);


	//領域を指定してデータをコピーする
	void CopyFormat(const int iOrgSub, const int iOrgMain,
            CTDAT& CopyImg, const int iCopySubPixel, const int iCopyMainPixel,
                            const int iCopyOrgSub, const int iCopyOrgMain);

	//欠陥補正
	bool bCorBadForNoise(CTDAT &DefectImage);

	//欠陥検出　欠陥画像として上書き
	void DetectDefect(const int iMedianLength, double dDetectTh, double dHemTh);

	//主方向差分のデータとする
	void DifMainDown();

	//主方向差分のデータとする
	void DifMainUp();

	//副方向差分のデータとする
	void DifSubDown();

	//副方向差分のデータとする
	void DifSubUp();

	//座標指定して、値を取得
	unsigned short int nGetVal(int iS, int iM);


//	void GPR(bool subswit=true);
//
//
//	void GPR(const float fMainFilter[],
//            const int iMainFilterNum,
//            const float fSubFilter[],
//            const int iSubFilterNum);


	//void HorUmb();
//	void HorUmb(const AnsiString NoiseFilter,
//                const AnsiString MainFilter,
//                const AnsiString SubFilter,
//                const double dClip);


	//void HorUmbMainRed();


	//void HorUmbMainRedAmp();
//    void HorUmbMainRedAmp(const String SubHighCutFilter,
//            const String MainLowPassFilter,
//            const String SubLowPassFilter,
//            const float fClip);
//    void HorUmbMainRedAmp2(const String SubHighCutFilter,
//            const String MainLowPassFilter,
//            const String SubLowPassFilter,
//            const float fClip);
//    void HorUmbMainRedAmp3(const String SubLowCutFilter,
//            const String MainLowPassFilter,
//			const String SubLowPassFilter,
//            const float fClip);
//    void HorUmbMainRedAmp4(const String SubHighCutFilter,
//            const String MainLowPassFilter,
//			const String SubLowPassFilter,
//            const float fClip);
//    void HorUmbMainRedAmp5(const String SubHighCutFilter,
//            const String MainLowPassFilter,
//            const String SubLowPassFilter,
//            const float fClip);
//    void HorUmbMainRedAmp6(const String SubHighCutFilter,
//            const String MainLowPassFilter,
//            const String SubLowPassFilter,
//            const float fClip);
//    int iBadCorReplaceJudge(TIIMG& bad, const int iM0, const int iS0, const int iTh, const int iMOffset, const int iSOffset, CTDAT& def);

	//降順にソートされている配列を渡したときに、該当画素値のデータが配列の何番目に入っているかを返す
	int iFindPos(const unsigned short int nVal,
            unsigned short int nDt[],
            int iS,
            int iE);


	bool IsDtExist(){return(DtExist);}


	void LittleBig();

	//Log変換
	void Log(const double dMid, const double dL);

	//閾値処理付きLog変換
    void LogThreshold(const double dMid, const double dL, const int iTh);//ttsuji追加

    //LOG変換後2ビットダウン
    void Log2BitDown(const double dMid, const double dL);//ttsuji追加


    void Lin16ToLog14_LowDoseLinear(const double dMid, const double dL);//ttsuji追加


    void LogToLinear(const double dC, const double dK, const double dOffset,
			const int iNewBit);


    void LogToLinear2(const int iBit, const double dCenter, const int iKeta);//ttsuji追加


    void Log14ToLin16_LowDoseLinear(const int iBit, const double dCenter, const int iKeta);    //ttsuji追加

    //主方向線欠陥検出
	void MainBinary(const int iM0, const int iM1 ,const int iWTh, CTDAT& result);
//	void MainShading(const TSINGLE_DATA& SHADING);


	void Median(const int iSNum, const int iMNum);


	void RemoveAddDt( unsigned short *Buff, const int num, const unsigned short iRem, const unsigned short iAdd);


	unsigned short int nMedian(int& iS0, int& iM0, int& iS1, int& iM1);

    bool Read1Byte(const string fileName);
	bool ReadDt(const string fileName);
	bool ReadDtSkipHead(const string fileName);//ttsuji追加
	bool ReadFramesSkipHead(const string fileName, const int iFrame);//ttsuji追加
	bool ReadDtSkipFoot(const string fileName);//ttsuji追加
	bool ReadDtD(const string fileName);

	//指定領域のminを返す
	unsigned short int nMin(int iS1, int iM1, int iS2, int iM2);

	//位置指定しての画素値設定(double)
	void SetVal(const int iSub, const int iMain, const double dVal);

	//位置指定しての画素値設定(float)
	void SetVal(const int iSub, const int iMain, const float fVal);

	//位置指定しての画素値設定(int)
	void SetVal(const int iSub, const int iMain, const int iVal);

	//位置指定しての画素値設定(unsigned short)
	void SetVal(const int iSub, const int iMain, const unsigned int iVal);

	//位置指定しての画素値設定(unsigned long)
	void SetVal(const int iSub, const int iMain, const unsigned long iVal);

	//位置指定しての画素値設定(double、切り捨て)
	void SetValWithTruncation(const int iSub, const int iMain, const double dVal);


	void SetValWithAreaCheck(int iSub, int iMain, const double dVal);


	void SetValWithAreaCheck(int iSub, int iMain, const int iVal);


	void SetValWithAreaCheck(int iSub, int iMain, const unsigned short int nVal);


	unsigned short int Average();


	unsigned short int Average(int iSubSrt, int iSubEnd, int iMainSrt, int iMainEnd);


	double AverageD(int iSubSrt, int iSubEnd, int iMainSrt, int iMainEnd);//ttsuji追加

	//ヒストグラムデータの更新(矩形領域指定)
	void Histogram(const int s1, const int s2, const int m1, const int m2);

	//ヒストグラムの最頻QL値を返す
	int HistMaxQL();

	//画像の最大QLを返す。(ヒストグラムのデータから返す)
	int HistExistMaxQL();//ttsuji追加

	//度数累積和の指定％相当のQLを返す
	int HistRuisekiDosuuPercent(float fPercentage, bool bFromMax);

	//度数累積和の指定％で指定された範囲のピクセル数のQL値の平均値を返す
	double HistRuisekiDosuuPercentRange(float fPercentMin, float fPercentMax);//ttsuji追加（191029）

	//全画像から指定した順位のQL値を返す
	bool bCalcQLOfGivenOrderPixel(int iOrder,  int& iRetQL);//ttsuji追加（191107）
//	int HistExistMaxQL(TRECTANGLE& rect, int iThQLUpper, AnsiString str, bool bSaveTxt);//ttsuji追加(201216)

	//画像のroot mean squareの値を返す
	double RMS();
//    double dRMS(const int iS1, const iM1, const int iS2, const int iM2);

	//画像の分散値を返す
	double Variance();

	//濃度変換後の分散値を返す
	double VarianceD();


	void Hist0Cut();


	double HistVariance();


	double HistVarRatio();

	//白黒反転処理　反転はDtMax-> 0 / 0 ->DtMax　となるように行う
	void Reverse();

	//画像サイズの変更（切り出し含む）
	void Format(int sub, int main, const int sSt, const int mSt, const unsigned short int back = 0);

	//画像縮小(1/整数でしか不可)
	void Div(const int sDiv, const int mDiv);
//	void Div(const int sDiv, const int mDiv, bool b45=true, bool b2D=false, bool bByLinear=false);

	void Div(const int sDiv, const int mDiv, bool b45, bool b2D, bool bByLinear = false);


	void DivAdd(const int sDiv, const int mDiv); //ttsuji追加


	void DivNearestNeighbor(const int sDiv, const int mDiv);//ttsuji追加


	void DivAll0To0(const int sDiv, const int mDiv);//ttsuji追加


	void DivSPC2x2();//ttsuji追加


	void DivTrancation(const int sDiv, const int mDiv);//平均縮小は切り捨て(ttsuji追加)


	void DivByImg(const CTDAT& img, const double dReg);


	void DivWithRest(const int iSubDiv, const int iMainDiv);


	void DownConnect(const CTDAT& down, int background = - 1);


	void Mul(const int sMul, const int mMul);


	void Mul0Insert(const int sMul, const int mMul);//ttsuji追加


	void MulQLInsert(const int sMul, const int mMul, const int iQL);//ttsuji追加

	//主 <--> 副　入れ替え
	void MainSub();


	void Shift(const int num, const unsigned short int background);

	//左右反転
	void RtoL();


	bool RemoveMainShading(const int start, const int last);
//    bool Suji(const int start, const int last, const int addConst);


	bool Suji(const int start, const int last, const int addConst, const int iMMedian = 1, const int iSMedian = 1);


	void LinearExpand(const int iSExp, const int iMExp);


	bool LineFilter(const int start, const int last, const int ave);


	void Averaging(int dt[], const int s, int e, const int aveNum);



	bool BadFind(bool **bad, const float threshold);


	void ExtractOddLine();


	void ExtractEvenLine();


	void ExtractMainOddLine();//ttsuji追加


	void ExtractMainEvenLine();//ttsuji追加


	void ExtractSubOddLine();//ttsuji追加


	void ExtractSubEvenLine();//ttsuji追加


	void MainMoveAve(const int iLength);


	void MainMoveAveAveExpand(const int aveNum);


	void MoveAveSPC(const int iMLength, const int iSLength, int bunbo = 65536);//ttsuji（SPC画像期待値ソフトの端点処理を反映）


	void SubMedian(const int iNum);


	void SubMedianAveExpand( const int iNum );


	void SubMedianBorderPixelExpand( const int iNum );//ttsuji追加


	void SubAveExpand( const int iNum );


	void SubBorderPixelExpand( const int iNum );//ttsuji追加


	void SubPixelExpand( const int iNum );


	void SubMedianSPC(const int iNum);//ttsuji追加（SPC（FPGAエミュレータ）の処理にあわせたもの）


	void SubMoveAve(const int iLength);


	void SubMoveAveAveExpand(const int aveNum);


	void Filter(const float fFilter[], const int iLength, const bool isMain);


	void Filter_long(const int iFilter[], const int iLength, const bool isMain);//ttsuji（CUフィルター演算と同じ2冪の整数フィルター）
//    void Filter(const float filter[], const int f_num, const bool main);
//    void MainMedian(const int ilength);


	void MainMedian(const int iNum);


	void MainMedianAveExpand( const int iNum );


	void MainAveExpand( const int iNum );


	void MainMedianAveExpand2( const int iNum );//ttsuji追加160720（透視TFT段差補正のMedian端処理用として）


	void MainAveExpand2( const int iNum );//ttsuji追加160720（透視TFT段差補正のMedian端処理用として）


	void MainPixelExpand( const int iNum );


	void MainMedianSPC(const int iNum);//ttsuji追加（SPC（FPGAエミュレータ）の処理にあわせたもの）


	void Shading(const CTDAT& shading, const float fReg);

	//横へ画像連結
	void SideConnect(const CTDAT& side, int background = - 1);


	void Extract(const int iSub, const int iSubOffset,
			const int iMain, const int iMainOffset);
//	void Fill(const TRECTANGLE& area, const unsigned short int nVal);


	void FillValue(const unsigned short int nVal);
//	bool    bSubLine(TSINGLE_DATA& line, const int iMainStart, const int iMainEnd);
//    bool    bSubLineDelMinMax2(TSINGLE_DATA& line,
//            const int iMainStart, const int iMainEnd);

	//上下入れ換え
	void UpDown();
//	bool bWriteDt(const AnsiString fileName);


	void Rotate(const int angle);


	void RotateCW();


	void RotateAnyAngle(double theta, int centerX, int centerY);


	void RotateAnyAngle(double theta, int centerX, int centerY, bool bExtendVirtualQL, int iInternalWidth);


	void LineSymmetricalCopy(int iXBorder, int iYBorder, CTDAT& ImgExtAreaFlag);//ttsuji追加210302


	void TrimArea(int& iS0, int& iM0, int& iS1, int& iM1);
	void Clip(int iMin, int iMax);

	CTDAT& FillRectLineWithQLShift(TRECTANGLE rect, const int iQLShift);

	CTDAT& FillRectAreaWithQLShift(TRECTANGLE rect, const int iQLShift);
//	CTDAT& FillQuadLineWithQLShift(TQUADRANGLE quad, const int iQLShift);//ttsuji追加
//	CTDAT& FillQuadAreaWithQLShift(TQUADRANGLE quad, const int iQLShift);//ttsuji追加
//	CTDAT& FillPentagonalLineWithQLShift(TPENTAGON pent, const int iQLShift);//ttsuji追加
//	CTDAT& FillPentagonalAreaWithQLShift(TPENTAGON pent, const int iQLShift);//ttsuji追加


	CTDAT& FillCrossPointWithQLShift(int iS, int iM, const int iQLShift, const int iSize);//ttsuji追加


	CTDAT& FillCrossPointWithQLShift(int iS, int iM, const int iQLShift, const int iSSize, const int iMSize);//ttsuji追加


	static bool IsOnLineSegment(int iS, int iM, int iS0, int iM0, int iS1, int iM1);//ttsuji追加
//	bool IsOnLineSegment(int iS, int iM, TQUADRANGLE quad, int fLine);//ttsuji追加
//	bool IsOnLineSegment(int iS, int iM, TPENTAGON pent, int fLine);//ttsuji追加
//	bool IsInsideQuadrangle(int iS, int iM, TQUADRANGLE quad);//ttsuji追加
//	bool IsInsidePentagon(int iS, int iM, TPENTAGON pent);//ttsuji追加
//	double dAve(TQUADRANGLE quad);//ttsuji追加
//	double dAve(TPENTAGON pent);//ttsuji追加


	CTDAT& DupePixelHVLine(int iS, int iM, int iSDupe, int iMDupe);//ttsuji追加
//   	TFIMG GetProfile(bool bMain, int iStart, int iEnd);//ttsuji追加
//    CTDAT& AddLineQL(bool bHorizontal, int iCoord, int iQL);//ttsuji追加


	void AddLineQL(bool bHorizontal, int iCoord, int iQL);//ttsuji追加

//	explicit CTDAT(const TIIMG &copy);//ttsuji 200905 （explicitをつけることで暗黙キャストによるクリップ処理を防ぐようにした）

//	CTDAT(const TFIMG &copy);

	//座標を指定して、バイリニア補間の値を返す
	unsigned short GetBilinear(const float fSub, const float fMai);

	//座標と拡大率を指定して画像の拡大縮小を行う
	bool Scaling(float centerX, float centerY, float Xscale, float Yscale, unsigned short FillQL = 0);

	//平行移動を行う(小数での入力可能)
	bool ShiftXY(float deltaX, float deltaY, unsigned short FillQL = 0);

	//左右別の拡大率で画像の拡大縮小を行う
	bool ScalingLR(float Lscale, float Rscale, unsigned short FillQL = 0);

    friend class TTMA;
//	friend class TMARS;
//	friend class TSIMARS;


};

#endif /* CTDAT_H_ */
