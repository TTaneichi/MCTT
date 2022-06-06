/*
 * CTDATProc.cpp
 *
 *  Created on: 2021/04/08
 *      Author: 10097663
 */

#include "CTDATProc.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

CTDATProc::CTDATProc(const int iS, const int iM, const int iBit, unsigned short **in_Dt){

	m_Dt = in_Dt;
	m_MainPixel = iM;
	m_SubPixel = iS;
	m_Bit = iBit;
	m_DtMax =(unsigned short int)((1 << iBit) - 1);		//画像data最大値

//	printf("CTDATProc:%x\n",m_Dt);
}

CTDATProc::CTDATProc() {

	m_Dt = NULL;
	m_Bit = 16;
	m_DtMax =(unsigned short int)((1 << m_Bit) - 1);		//画像data最大値
	m_MainPixel = 0;
	m_SubPixel = 0;

}

CTDATProc::~CTDATProc() {

}

void CTDATProc::SetSize(const int iS, const int iM){

	m_MainPixel = iM;
	m_SubPixel = iS;

}
void CTDATProc::SetData(unsigned short **in_Dt){

	m_Dt = in_Dt;
}
void CTDATProc::SetVal(const int iSub, const int iMain, const float fVal){

    if(fVal <= 0.0){
        m_Dt[iSub][iMain] = 0;
    }else if(fVal >= float(m_DtMax)){
        m_Dt[iSub][iMain] = m_DtMax;
    }else{
        m_Dt[iSub][iMain] = (unsigned short int)(fVal + 0.5f);
    }

}

//点(iM, iS)が2点(iXS, iXE), (iXE, iYE)	を結ぶ線分の上にあるか否か判定する
bool CTDATProc::IsOnLineSegment(int iS, int iM, int iYS, int iXS, int iYE, int iXE){

	//Y = AX + B 上にあるかどうか判定
	float fA, fB, fX, fY;
	if (iXS == iXE) {//2点間を結ぶ直線はX=iXS
		int iYLarge, iYSmall;
		if (iYS >= iYE) {
			iYLarge = iYS;
			iYSmall = iYE;
		}
		else{
			iYLarge = iYE;
			iYSmall = iYS;
		}

		if (iM == iXS && (iYSmall <= iS && iS <= iYLarge)) {
			return true;
		}
		else{
			return false;
		}
	}
	else{//2点間を結ぶ直線はY=AX+B
		if (fabs(iYE - iYS) <= fabs(iXE - iXS)) {//傾きの絶対値が1以下の場合はxをインクリメント
			int iXLarge, iXSmall;
			if (iXS >= iXE) {
				iXLarge = iXS;
				iXSmall = iXE;
			}
			else{
				iXLarge = iXE;
				iXSmall = iXS;
			}

			fA = (float)(iYE - iYS) / (iXE - iXS);
			fB = (float)(iXE*iYS - iXS*iYE) / (iXE - iXS);
			fY = fA * iM + fB;

			if (iS == round(fY) && (iXSmall <= iM && iM <= iXLarge)) {
				return true;
			}
			else{
				return false;
			}
		}
		else{//傾きの絶対値が1より大きい場合はyをインクリメント
			int iYLarge, iYSmall;
			if (iYS >= iYE) {
				iYLarge = iYS;
				iYSmall = iYE;
			}
			else{
				iYLarge = iYE;
				iYSmall = iYS;
			}

			fA = (float)(iXE - iXS) / (iYE - iYS);
			fB = (float)(iYE*iXS - iYS*iXE) / (iYE - iYS);
			fY = fA * iS + fB;

			if (iM == round(fY) && (iYSmall <= iS && iS <= iYLarge)) {
				return true;
			}
			else{
				return false;
			}
        }
	}

}
//指定領域のminを返す
unsigned short CTDATProc::GetMin(int iS1, int iM1, int iS2, int iM2){

    if(iS1 < 0){
        iS1 = 0;
    }
    if(m_SubPixel <= iS2){
        iS2 = m_SubPixel - 1;
    }
    if(iM1 < 0){
        iM1 = 0;
    }
    if(m_MainPixel <= iM2){
        iM2 = m_MainPixel - 1;
    }

    unsigned short nMin = m_Dt[iS1][iM1];
    for(int iSCnt = iS1; iSCnt <= iS2; ++iSCnt){
        for(int iMCnt = iM1; iMCnt <= iM2; ++iMCnt){
            if(m_Dt[iSCnt][iMCnt] < nMin){
                nMin = m_Dt[iSCnt][iMCnt];
            }
        }
    }
    return(nMin);
}

//座標を指定して、バイリニア補間の値を返す
unsigned short CTDATProc::GetBilinear(const float fSub, const float fMai, unsigned short FillQL){

	//範囲外は0を返す
	if (fSub < 0 || fSub > (m_SubPixel-1) || fMai < 0 || fMai > (m_MainPixel-1)){
		return FillQL;
	}

	int 	isub0, isub1, imain0, imain1;	//　取得元の4点の座標
	float 	dsub0, dsub1, dmain0, dmain1;

	isub0 = (int)fSub;
	isub1 = isub0 + 1;

	dsub0 = fSub - isub0;
	dsub1 = 1 - dsub0;

	imain0 = (int)fMai;
	imain1 = imain0 + 1;

	dmain0 = fMai - imain0;
	dmain1 = 1 - dmain0;

	//端がはみ出していた場合には、端と同じ座標とする
	if (isub0 < 0){	isub0 = 0;}
	if (isub1 > m_SubPixel -1){ isub1 = m_SubPixel -1;}
	if (imain0 < 0){ imain0 = 0;}
	if (imain1 > m_MainPixel -1){ imain1 = m_MainPixel -1; }

	float retTemp =  dsub1 * dmain1 * m_Dt[isub0][imain0] +
					 dsub0 * dmain1 * m_Dt[isub1][imain0] +
					 dsub1 * dmain0 * m_Dt[isub0][imain1] +
					 dsub0 * dmain0 * m_Dt[isub1][imain1];

	return (unsigned short)retTemp;

}

//座標と拡大率を指定して画像の拡大縮小を行う
//画像のサイズ自体は変えず、縮小（拡大でも）でデータが存在しない場合は指定のQL値で埋める
//2倍以上の拡大／縮小では間に挟まれた画像のデータが利用されないので、注意!!!
//@param	centerX		中心座標X(画像範囲外を指定してもよい)
//@param	centerY		中心座標Y(画像範囲外を指定してもよい)
//@param	Xscale		X拡大率(1.00で拡縮なし)
//@param	Yscale		Y拡大率(1.00で拡縮なし)
//@param	FillQL		縮小したときに隙間を埋めるQL値
//@return	true:成功／false:失敗
//@date		2021/07/08	T.Taneichi
bool CTDATProc::Scaling(float centerX, float centerY, float Xscale, float Yscale, unsigned short FillQL){

	unsigned short *pData = new unsigned short[m_MainPixel*m_SubPixel];

	for(int iSCnt = 0; iSCnt < m_SubPixel; iSCnt++){
		for(int iMCnt = 0; iMCnt < m_MainPixel; iMCnt++){
			//元画像の座標位置を求める
			float x, y;
			x = ((float)iMCnt - centerX) / Xscale + centerX;		//一旦,中心を(0,0)の座標に平行移動させて、拡大して再び中心座標へ戻す
			y = ((float)iSCnt - centerY) / Yscale + centerY;

			//バイリニアで値を求める
			pData[iSCnt*m_MainPixel + iMCnt] = GetBilinear(y, x, FillQL);

		}
	}

	//データ入れ替え
	for(int iSCnt = 0; iSCnt < m_SubPixel; iSCnt++){
		for(int iMCnt = 0; iMCnt < m_MainPixel; iMCnt++){
			m_Dt[iSCnt][iMCnt] = pData[iSCnt*m_MainPixel + iMCnt];
		}
	}

	delete[] pData;

	return true;
}
//平行移動を行う(小数での入力可能)
bool CTDATProc::ShiftXY(float deltaX, float deltaY, unsigned short FillQL){

	unsigned short *pData = new unsigned short[m_MainPixel*m_SubPixel];

	for(int iSCnt = 0; iSCnt < m_SubPixel; iSCnt++){
		for(int iMCnt = 0; iMCnt < m_MainPixel; iMCnt++){
			//元画像の座標位置を求める
			float x, y;
			x = (float)iMCnt - deltaX;
			y = (float)iSCnt - deltaY;

			//バイリニアで値を求める
			pData[iSCnt*m_MainPixel + iMCnt] = GetBilinear(y, x, FillQL);
		}
	}

	//データ入れ替え
	for(int iSCnt = 0; iSCnt < m_SubPixel; iSCnt++){
		for(int iMCnt = 0; iMCnt < m_MainPixel; iMCnt++){
			m_Dt[iSCnt][iMCnt] = pData[iSCnt*m_MainPixel + iMCnt];
		}
	}


	delete[] pData;

	return true;
}
//左右別の拡大率で画像の拡大縮小を行う
//画像のサイズ自体は変えず、縮小（拡大でも）でデータが存在しない場合は指定のQL値で埋める
//2倍以上の拡大／縮小では間に挟まれた画像のデータが利用されないので、注意!!!
//@param	Lscale		左端の拡大率(1.00で拡縮なし)
//@param	Rscale		右端の拡大率(1.00で拡縮なし)
//@param	FillQL		縮小したときに隙間を埋めるQL値
//@return	true:成功／false:失敗
//@date		2021/07/14	T.Taneichi
bool CTDATProc::ScalingLR(float Lscale, float Rscale, unsigned short FillQL){

	unsigned short *pData = new unsigned short[m_MainPixel*m_SubPixel];

	float deltaMag = (Lscale - Rscale) / m_MainPixel;
	for(int iMCnt = 0; iMCnt < m_MainPixel; iMCnt++){
		float mag = Rscale + deltaMag * iMCnt;
		for(int iSCnt = 0; iSCnt < m_SubPixel; iSCnt++){
			//元画像の座標位置を求める
			float x, y;
			x = (float)iMCnt;
			y = (((float)iSCnt - m_SubPixel / 2 ) / mag) + (float)m_SubPixel /2;

			//バイリニアで値を求める
			pData[iSCnt*m_MainPixel + iMCnt] = GetBilinear(y, x, FillQL);

		}
	}

	//データ入れ替え
	for(int iSCnt = 0; iSCnt < m_SubPixel; iSCnt++){
		for(int iMCnt = 0; iMCnt < m_MainPixel; iMCnt++){
			m_Dt[iSCnt][iMCnt] = pData[iSCnt*m_MainPixel + iMCnt];
		}
	}

	delete[] pData;


	return true;
}
