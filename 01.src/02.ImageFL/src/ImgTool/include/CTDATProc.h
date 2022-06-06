/*
 * CTDATProc.h
 *	CTDATの処理の移譲用のクラス
 *	CTDATのソースコード量を5000行以下にしたいために新設。
 *	画像データはポインタだけ保持し、データの確保／解放は行わない。
 *  Created on: 2021/04/08
 *      Author: 10097663
 */

#ifndef CTDATPROC_H_
#define CTDATPROC_H_



class CTDATProc {
private:
	unsigned short int **m_Dt;			//画像data
	unsigned short m_DtMax;			//画像data最大値
	int m_MainPixel;			//主画素数
	int m_SubPixel;			//副画素数
	int m_Bit;				//画像data bit数
public:
	CTDATProc(const int iS, const int iM, const int iBit, unsigned short **in_Dt);
	CTDATProc();
	virtual ~CTDATProc();

	void SetSize(const int iS, const int iM);
	void SetData(unsigned short **in_Dt);

	void SetVal(const int iSub, const int iMain, const float fVal);

	//点(iM, iS)が2点(iXS, iXE), (iXE, iYE)	を結ぶ線分の上にあるか否か判定する
	static bool IsOnLineSegment(int iS, int iM, int iYS, int iXS, int iYE, int iXE);

	//指定領域のminを返す
	unsigned short GetMin(int iS1, int iM1, int iS2, int iM2);

	//座標を指定して、バイリニア補間の値を返す
	unsigned short GetBilinear(const float fSub, const float fMai, unsigned short FillQL = 0);

	//座標と拡大率を指定して画像の拡大縮小を行う
	bool Scaling(float centerX, float centerY, float Xscale, float Yscale, unsigned short FillQL = 0);

	//平行移動を行う(小数での入力可能)
	bool ShiftXY(float deltaX, float deltaY, unsigned short FillQL = 0);

	//左右別の拡大率で画像の拡大縮小を行う
	bool ScalingLR(float Lscale, float Rscale, unsigned short FillQL = 0);

};

#endif /* CTDATPROC_H_ */
