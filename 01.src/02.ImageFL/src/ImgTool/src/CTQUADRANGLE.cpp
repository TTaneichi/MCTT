/*
 * CTQUADRANGLE.cpp
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#include "imageData.h"
#include "CTQUADRANGLE.h"
#include "CVct.h"
#include <math.h>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//ttsuji
TQUADRANGLE::~TQUADRANGLE()
{
	;
}
//---------------------------------------------------------------------------
//ttsuji
//
//四角形のうち指摘した辺とある点の外積（ある点が四角形の内外になるか判定するため）
//メモリリークあり。原因不明。Vct型の解放ができていない？

int TQUADRANGLE::GetVectorProduct(int iM, int iS, int flagSide)
{
	Vct vSide, vSample, vVP;
	int  iSide[3], iSample[3];

	switch (flagSide) {
	case 0://ベクトル0→1
		iSide[0] = GetM1() - GetM0();
		iSide[1] = GetS1() - GetS0();
		iSide[2] = 0.0;
		iSample[0] = iM - GetM0();
		iSample[1] = iS - GetS0();
		iSample[2] = 0.0;
		vSide.set(iSide, 3);
		vSample.set(iSample, 3);
		vVP = vSide.GetVectorProduct(vSample);
		break;
	case 1://ベクトル1→2
		iSide[0] = GetM2() - GetM1();
		iSide[1] = GetS2() - GetS1();
		iSide[2] = 0.0;
		iSample[0] = iM - GetM1();
		iSample[1] = iS - GetS1();
		iSample[2] = 0.0;
		vSide.set(iSide, 3);
		vSample.set(iSample, 3);
		vVP = vSide.GetVectorProduct(vSample);
		break;
	case 2://ベクトル2→3
		iSide[0] = GetM3() - GetM2();
		iSide[1] = GetS3() - GetS2();
		iSide[2] = 0.0;
		iSample[0] = iM - GetM2();
		iSample[1] = iS - GetS2();
		iSample[2] = 0.0;
		vSide.set(iSide, 3);
		vSample.set(iSample, 3);
		vVP = vSide.GetVectorProduct(vSample);
		break;
	case 3://ベクトル3→0
		iSide[0] = GetM0() - GetM3();
		iSide[1] = GetS0() - GetS3();
		iSide[2] = 0.0;
		iSample[0] = iM - GetM3();
		iSample[1] = iS - GetS3();
		iSample[2] = 0.0;
		vSide.set(iSide, 3);
		vSample.set(iSample, 3);
		vVP = vSide.GetVectorProduct(vSample);
		break;
	}

#if 0
	return round(vVP[2]);
#else //メモリリーク解消されず。
	int iRet = round(vVP[2]);

	vSide.clear();
	vSample.clear();
	vVP.clear();

	return iRet;
#endif
}
//---------------------------------------------------------------------------
//ttsuji
//
//四角形をビットシフトでビニング

TQUADRANGLE TQUADRANGLE::BinningQuadrangleWithBitShift(const int iBin)
{
	double result = log((double)iBin) / log(2.0);
	double integral_part = 0.0;
	double fractional_part = std::modf(result, &integral_part);
	if (0.0 != fractional_part){
		return *this;//何もしない
	}
	int iShiftBit = (int)(result + 0.5);// 小数部が0だったら、iShiftBitは2の乗数

	TQUADRANGLE quad;
	quad = *this;
	quad.iM0 >>= iShiftBit;
	quad.iM1 >>= iShiftBit;
	quad.iM2 >>= iShiftBit;
	quad.iM3 >>= iShiftBit;
	quad.iS0 >>= iShiftBit;
	quad.iS1 >>= iShiftBit;
	quad.iS2 >>= iShiftBit;
	quad.iS3 >>= iShiftBit;

	return quad;
}
//---------------------------------------------------------------------------
//ttsuji追加
//
//四角形をクリッピングする

TQUADRANGLE& TQUADRANGLE::ClipQuadrangle(int iMMin, int iMMax, int iSMin, int iSMax)
{
	SetM0(CLIP(GetM0(), iMMin, iMMax));
	SetM1(CLIP(GetM1(), iMMin, iMMax));
	SetM2(CLIP(GetM2(), iMMin, iMMax));
	SetM3(CLIP(GetM3(), iMMin, iMMax));
	SetS0(CLIP(GetS0(), iSMin, iSMax));
	SetS1(CLIP(GetS1(), iSMin, iSMax));
	SetS2(CLIP(GetS2(), iSMin, iSMax));
	SetS3(CLIP(GetS3(), iSMin, iSMax));

	return *this;
}
//---------------------------------------------------------------------------
//ttsuji
//
//四角形領域のうち、最小および最大のX座標を返す

void TQUADRANGLE::CalcXMinMax(int& iXMin, int& iXMax)
{
	const int iNumSort = 4;
	int iXsort[4];
	int iX[4] = {GetM0(), GetM1(), GetM2(), GetM3()};
	quick_sort(iX , iXsort , iNumSort);//MinはiXsort[0]

	iXMin = iXsort[0];
	iXMax = iXsort[3];
}
//---------------------------------------------------------------------------
//ttsuji
//
//四角形領域のうち、最小および最大のY座標を返す

void TQUADRANGLE::CalcYMinMax(int& iYMin, int& iYMax)
{
	const int iNumSort = 4;
	int iYsort[4];
	int iY[4] = {GetS0(), GetS1(), GetS2(), GetS3()};
	quick_sort(iY , iYsort , iNumSort);//MinはiYsort[0]

	iYMin = iYsort[0];
	iYMax = iYsort[3];
}
