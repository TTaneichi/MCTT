/*
 * CTPENTAGON.cpp
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#include "CTPENTAGON.h"
#include "CVct.h"
#include "imageData.h"
#include <math.h>


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//ttsuji
TPENTAGON::~TPENTAGON()
{
	;
}
//---------------------------------------------------------------------------
//ttsuji
//
//五角形のうち指摘した辺とある点の外積（ある点が五角形の内外になるか判定するため）

int TPENTAGON::GetVectorProduct(int iM, int iS, int flagSide)
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
	case 3://ベクトル3→4
		iSide[0] = GetM4() - GetM3();
		iSide[1] = GetS4() - GetS3();
		iSide[2] = 0.0;
		iSample[0] = iM - GetM3();
		iSample[1] = iS - GetS3();
		iSample[2] = 0.0;
		vSide.set(iSide, 3);
		vSample.set(iSample, 3);
		vVP = vSide.GetVectorProduct(vSample);
		break;
	case 4://ベクトル4→0
		iSide[0] = GetM0() - GetM4();
		iSide[1] = GetS0() - GetS4();
		iSide[2] = 0.0;
		iSample[0] = iM - GetM4();
		iSample[1] = iS - GetS4();
		iSample[2] = 0.0;
		vSide.set(iSide, 3);
		vSample.set(iSample, 3);
		vVP = vSide.GetVectorProduct(vSample);
		break;
	}

	return round(vVP[2]);
}
//---------------------------------------------------------------------------
//五角形をビットシフトでビニング
//　（iBin は２のべき乗）

TPENTAGON TPENTAGON::BinningPentagonWithBitShift(const int iBin)
{

	double result = log((double)iBin) / log(2.0);
	double integral_part = 0.0;
	double fractional_part = std::modf(result, &integral_part);
	if (0.0 != fractional_part){
		return *this;//何もしない
	}
	int iShiftBit = (int)(result + 0.5);// 小数部が0だったら、iShiftBitは2の乗数

	TPENTAGON pent;
	pent = *this;
	pent.iM0 >>= iShiftBit;
	pent.iM1 >>= iShiftBit;
	pent.iM2 >>= iShiftBit;
	pent.iM3 >>= iShiftBit;
	pent.iM4 >>= iShiftBit;
	pent.iS0 >>= iShiftBit;
	pent.iS1 >>= iShiftBit;
	pent.iS2 >>= iShiftBit;
	pent.iS3 >>= iShiftBit;
	pent.iS4 >>= iShiftBit;

	return pent;
}
//---------------------------------------------------------------------------
//ttsuji追加
//
//五角形をクリッピングする

TPENTAGON& TPENTAGON::ClipPentagon(int iMMin, int iMMax, int iSMin, int iSMax)
{
	SetM0(CLIP(GetM0(), iMMin, iMMax));
	SetM1(CLIP(GetM1(), iMMin, iMMax));
	SetM2(CLIP(GetM2(), iMMin, iMMax));
	SetM3(CLIP(GetM3(), iMMin, iMMax));
	SetM4(CLIP(GetM4(), iMMin, iMMax));
	SetS0(CLIP(GetS0(), iSMin, iSMax));
	SetS1(CLIP(GetS1(), iSMin, iSMax));
	SetS2(CLIP(GetS2(), iSMin, iSMax));
	SetS3(CLIP(GetS3(), iSMin, iSMax));
	SetS4(CLIP(GetS4(), iSMin, iSMax));

	return *this;
}
