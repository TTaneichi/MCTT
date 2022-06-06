/*
 * CSIMARSF.cpp
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#include "CSIMARSF.h"
#include "CTSINGLEDATA.h"
#include "CApplication.h"
#include "CTFIMG.h"
#include "CTIIMG.h"

#include <vector>
#include <stack>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <iomanip>
#include <string>

using namespace std;

#include "CSIMARSF.h"
//#include "Unit1.h"
//#include "LeastSquare.h"
//#include "ItplCubeFloat.h"
//#include "GridLineDefect.h"//takeshi_kuwabara追加
#include "utility.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
const float fPIX_SIZE = 0.15f;

//段差主プロファイルのスムージング方法
#define _MEDIAN_ONLY     (0)
#define _MOVEAVE_ONLY    (1)
#define _MED_AND_MOVE   (2)

//中央→上下端方向の補正加重テーブル
#define _TABLE_R01      (0)
#define _TABLE_FLAT     (1)

//段差量定義
#define _1PIX		(0)
#define _2PIX_AVE	(1)
#define _3PIX_AVE	(2)
#define _MEDIAN3	(3)
#define _MEDIAN5	(4)
#define _MEDIAN7	(5)

//LongPanel Debug
#define _LP_DEBUG_OUTPUT

//円周率
//#define PI 3.141592
#define PI 3.141592653589793238462643383279//30桁(コンペア不一致対策として 141205 ttsuji)

//短冊横傘の中間画像出力on/off
#define _LOG_TANZAKUYOKOKASA

#define _DLL_INPUT_SIZE_352//CTFチャートパターン検出のDLLの入出力画像サイズ352
#define _DEBUG_OUTPUT

CSimars::CSimars(){
	m_pTTMA = NULL;
}

CSimars::CSimars(TTMA *pTTMA){
	m_pTTMA = pTTMA;
}
CSimars::~CSimars(){

}

void CSimars::SetTTMA(TTMA *pTTMA){

	m_pTTMA = pTTMA;

}

//---------------------------------------------------------------------------
//SPC実装のゲイン補正（Bmapを用いた乗算処理（SELENE,ROOTS用））
	//
	//
	//               U16.0           U20.12->U20.0  ------------    U16.0
	//     in_data[] ------------------(x)----------| limiter  |--- out_data[]
	//                                  |           ------------
	//                                  | U4.12
	//                                  |
	//           U3.13   U4.28->U4.12   |
	//     hosei_data[]-----(x)---------|
	//                       |
	//                       | U1.15 kando
	//
bool CSimars::bGainCorrectionSPC(TTMA& ShadingImg, int iCoeSense, int iShosuKirisute){
	//補正画像と被補正画像のサイズ判定
	if (ShadingImg.MainPixel != m_pTTMA->MainPixel || ShadingImg.SubPixel != m_pTTMA->SubPixel) {
		CApplication::GetInstance()->MessageBox("ゲインキャリブ画像と被補正画像のサイズが合っていません。", "Error", MB_OK);
		return false;
	}

#if 0
	for (int i = 0 ; i < SubPixel ; i++){
		for (int j = 0 ; j < MainPixel ; j++){
			float fDiv = 1 << 15;//2^15 = 32768
			float fDt = (float)m_pTTMA->Dt[i][j] * ShadingImg.Dt[i][j] / fDiv;
			m_pTTMA->SetVal(i, j, fDt);
		}
	}
#else
	//感度補正も同時に実行（SPC対応）
	for (int i = 0 ; i < ShadingImg.SubPixel ; i++){
		for (int j = 0 ; j < ShadingImg.MainPixel ; j++){
			__int64 i64 = ShadingImg.Dt[i][j];//U1.15(U3.13) 　()内はゲイン1倍が8192のとき
			i64 *= iCoeSense;//U2.30(U4.28) 　()内はゲイン1倍が8192のとき
			i64 /= 65536;//U2.14(U4.12) 　()内はゲイン1倍が8192のとき
			ShadingImg.Dt[i][j] = (int)i64;
		}
	}
//	ShadingImg.SaveTmaTdt(ExtractFilePath(in_str) + "Kando_Inverse_o_GainBeta.tma");


	TIIMG img_tmp = (TIIMG)*m_pTTMA;
	for (int i = 0 ; i < ShadingImg.SubPixel; i++){
		for (int j = 0 ; j < ShadingImg.MainPixel; j++){
			__int64 i64 = m_pTTMA->Dt[i][j];//U16.0
			i64 *= ShadingImg.Dt[i][j];//U18.14(U20.12) 　()内はゲイン1倍が8192のとき
//			i64 /= 16384;//U18.0
			i64 /= iShosuKirisute;//U18.0(U20.0) 　()内はゲイン1倍が8192のとき
			img_tmp.Dt[j][i] = (int)i64;
		}
	}

	for (int i = 0 ; i < m_pTTMA->SubPixel; i++){
		for (int j = 0 ; j < m_pTTMA->MainPixel; j++){
			m_pTTMA->Dt[i][j] = CLIP(img_tmp.Dt[j][i], 0, m_pTTMA->DtMax);//U16.0
		}
	}
#endif

	return true;
}
//---------------------------------------------------------------------------
//SEのFPGA実装のゲイン補正（Bmapを用いた乗算処理（AXIS,47C,77G用）。Bmapは8192QLが1倍で固定）
	//
	//
	//               U16.0           U20.16->U20.0  ------------    U16.0
	//     in_data[] ------------------(x)----------| limiter  |--- out_data[]
	//                                  |           ------------
	//                                  | U4.16
	//                                  |
	//           U3.13   U4.28->U4.16   |
	//     hosei_data[]-----(x)---------|
	//                       |
	//                       | U1.15 kando
	//
bool CSimars::bGainCorrectionAXIS(TTMA& ShadingImg, int iCoeSense, int iPanel_Kind){

	//補正画像と被補正画像のサイズ判定
	if (ShadingImg.MainPixel != m_pTTMA->MainPixel || ShadingImg.SubPixel != m_pTTMA->SubPixel) {
		CApplication::GetInstance()->MessageBox("ゲインキャリブ画像と被補正画像のサイズが合っていません。", "Error", MB_OK);
		return false;
	}

	//Mayoスジ対策
	unsigned int limit;
	if (iPanel_Kind == _PANEL_47C || iPanel_Kind == _PANEL_77C){
		limit = iCoeSense * 55000;//47C
	}
	else if (iPanel_Kind == _PANEL_AXIS || iPanel_Kind == _PANEL_77G) {
		limit = iCoeSense * 65535;//AXIS/77G
	}
	else{
		CApplication::GetInstance()->MessageBox("指定したパネル種が不正です。", "Error", MB_OK);
		return false;
	}

	limit >>= 15;
	if (limit > 65535){
		limit = 65535;
	}


	//感度補正も同時に実行
	for (int i = 0 ; i < ShadingImg.SubPixel ; i++){
		for (int j = 0 ; j < ShadingImg.MainPixel ; j++){
			__int64 i64 = ShadingImg.Dt[i][j];//U3.13 　ゲイン1倍は8192固定
			i64 *= iCoeSense;//U4.28

			i64 >>= 12;//U4.16
			i64 *= m_pTTMA->Dt[i][j];//U20.16
			i64 >>= 16;//U20.0
/*
			i64 >>= 16;//U4.12
			i64 *= Dt[i][j];//U20.12
			i64 >>= 12;//U20.0
*/
			if (i64 > limit){
				i64 = limit;
			}
			m_pTTMA->Dt[i][j] = CLIP(i64, 0, limit);//U16.0
		}
	}

	return true;
}
//---------------------------------------------------------------------------
//SEのFPGA実装のゲイン補正（Bmapを用いた乗算処理（2430,HM,DEVO2用）。Bmapは8192QLが1倍で固定）
	//
	//
	//               U16.0           U20.16->U20.0  ------------    U16.0
	//     in_data[] ------------------(x)----------| limiter  |--- out_data[]
	//                                  |           ------------
	//                                  | U4.16
	//                                  |
	//           U3.13   U4.28->U4.16   |
	//     hosei_data[]-----(x)---------|
	//                       |
	//                       | U1.15 kando
	//

//#define _LP_FORWARD_GAIN_HOSEI
//#define _RANDOM_OUTPUT
bool CSimars::bGainCorrection2430(TTMA& ShadingImg, int iCoeSense, int iPanel_Kind, int iClipQL){

	//補正画像と被補正画像のサイズ判定
	if (ShadingImg.MainPixel != m_pTTMA->MainPixel || ShadingImg.SubPixel != m_pTTMA->SubPixel) {
		CApplication::GetInstance()->MessageBox("ゲインキャリブ画像と被補正画像のサイズが合っていません。", "Error", MB_OK);
		return false;
	}

	//Mayoスジ対策
	unsigned int limit;
	if (iPanel_Kind == _PANEL_2430){
		limit = iCoeSense * iClipQL;//2430 49700
	}
	else if (iPanel_Kind == _PANEL_HM){
		limit = iCoeSense * iClipQL;//HM 51900
	}
	else if (iPanel_Kind == _PANEL_DEVO2_1417_GOS){
		limit = iCoeSense * iClipQL;//52000
	}
	else if (iPanel_Kind == _PANEL_DEVO2_1417_CSI){
		limit = iCoeSense * iClipQL;//52000
	}
	else if (iPanel_Kind == _PANEL_DEVO2_1717_GOS){
		limit = iCoeSense * iClipQL;//52000
	}
	else if (iPanel_Kind == _PANEL_DEVO2_1717_CSI){
		limit = iCoeSense * iClipQL;//52000
	}
	else{
		CApplication::GetInstance()->MessageBox("指定したパネル種が不正です。", "Error", MB_OK);
		return false;
	}

	limit >>= 15;
	if (limit > 65535){
		limit = 65535;
	}

#ifdef _RANDOM_OUTPUT
	TTMA RandImg, RandImg6Bit;
	RandImg.NewDt(m_pTTMA->SubPixel, m_pTTMA->MainPixel, m_pTTMA->Bit);
	RandImg.FillValue(0);
	RandImg6Bit.NewDt(m_pTTMA->SubPixel, m_pTTMA->MainPixel, m_pTTMA->Bit);
	RandImg6Bit.FillValue(0);
#endif

	unsigned short random = 0x1234;//1 0010 0011 0100(2進数)
	unsigned short random_tmp;

	//感度補正も同時に実行
	for (int i = 0 ; i < ShadingImg.SubPixel ; i++){
		for (int j = 0 ; j < ShadingImg.MainPixel ; j++){
			__int64 i64 = ShadingImg.Dt[i][j];//U3.13 　ゲイン1倍は8192固定
			i64 *= iCoeSense;//U4.28
			i64 >>= 12;//U4.16

			__int64 i64tmp = m_pTTMA->Dt[i][j];//U16.0
			i64tmp <<= 4;//U16.4
#ifdef _LP_FORWARD_GAIN_HOSEI
			i64tmp += ( random & 0x003F );//U16.4  小数部に乱数加算(0x000F -> 111111(2進数)であり、下6bitだけを抜き出して乱数としている)
#else
			i64tmp += ( random & 0x000F );//U16.4  小数部に乱数加算(0x000F -> 1111(2進数)であり、下4bitだけを抜き出して乱数としている)
#endif
			i64 *= i64tmp;//U20.20
			i64 >>= 20;//U20


			if (i64 > limit){
				i64 = limit;
			}
			m_pTTMA->Dt[i][j] = CLIP(i64, 0, limit);//U16.0


#ifdef _RANDOM_OUTPUT
            RandImg.SetVal(i, j, random);
            RandImg6Bit.SetVal(i, j, random & 0x003F);
#endif

			//乱数を更新
			random_tmp =  (( random & 0x0001 ) >> 0 ) << 15;
			random_tmp += (( random & 0x8000 ) >> 15 ) << 14;
			random_tmp += (((random & 0x4000 ) >> 14 ) ^ (( random & 0x0001 ) >> 0 ) ) << 13;
			random_tmp += (((random & 0x2000 ) >> 13 ) ^ (( random & 0x0001 ) >> 0 ) ) << 12;
			random_tmp += (( random & 0x1000 ) >> 12 ) << 11;
			random_tmp += (((random & 0x0800 ) >> 11 ) ^ (( random & 0x0001 ) >> 0 ) ) << 10;
			random_tmp += (( random & 0x0400 ) >> 10 ) << 9;
			random_tmp += (( random & 0x0200 ) >> 9 ) << 8;
			random_tmp += (( random & 0x0100 ) >> 8 ) << 7;
			random_tmp += (( random & 0x0080 ) >> 7 ) << 6;
			random_tmp += (( random & 0x0040 ) >> 6 ) << 5;
			random_tmp += (( random & 0x0020 ) >> 5 ) << 4;
			random_tmp += (( random & 0x0010 ) >> 4 ) << 3;
			random_tmp += (( random & 0x0008 ) >> 3 ) << 2;
			random_tmp += (( random & 0x0004 ) >> 2 ) << 1;
			random_tmp += (( random & 0x0002 ) >> 1 ) << 0;
			random = random_tmp;
		}
	}

#ifdef _RANDOM_OUTPUT
    RandImg.SaveTmaTdt("C:\\RandomImage.tma");
    RandImg6Bit.SaveTmaTdt("C:\\RandomImage6Bit.tma");
#endif

	return true;

}


bool CSimars::bForwardGainCorrectionLP(TTMA& ShadingImg, int iCoeSense, int iClipQL, bool bExeForwardSensCor, double dKando32768, int iNormBitSense){

	//補正画像と被補正画像のサイズ判定
	if (ShadingImg.MainPixel != m_pTTMA->MainPixel || ShadingImg.SubPixel != m_pTTMA->SubPixel) {
		CApplication::GetInstance()->MessageBox("ゲインキャリブ画像と被補正画像のサイズが合っていません。", "Error", MB_OK);
		return false;
	}

	//Mayoスジ対策
	unsigned int limit = iCoeSense * iClipQL;//52000　（DEVO2　141021情報）

	limit >>= 15;
	if (limit > 65535){
		limit = 65535;
	}

#ifdef _RANDOM_OUTPUT
	TTMA RandImg, RandImg6Bit;
	RandImg.NewDt(m_pTTMA->SubPixel, m_pTTMA->MainPixel, m_pTTMA->Bit);
	RandImg.FillValue(0);
	RandImg6Bit.NewDt(m_pTTMA->SubPixel, m_pTTMA->MainPixel, m_pTTMA->Bit);
	RandImg6Bit.FillValue(0);
#endif

	unsigned short random = 0x1234;//1 0010 0011 0100(2進数)
	unsigned short random_tmp;

	//感度補正も同時に実行
	for (int i = 0 ; i < ShadingImg.SubPixel ; i++){
		for (int j = 0 ; j < ShadingImg.MainPixel ; j++){
			__int64 i64 = ShadingImg.Dt[i][j];//U3.13 　ゲイン1倍は8192固定
			i64 *= iCoeSense;//U4.28
			i64 >>= 12;//U4.16

			__int64 i64tmp = m_pTTMA->Dt[i][j];//U16.0
			i64tmp <<= 4;//U16.4
			i64tmp += ( random & 0x003F );//U16.4  小数部に乱数加算(0x003F -> 111111(2進数)であり、下6bitだけを抜き出して乱数としている)

			switch (bExeForwardSensCor) {
			case false://順感度補正OFF
				i64 *= i64tmp;//U20.20
				i64 >>= 20;//U20
				break;
			case true://順感度補正ON
				i64 *= i64tmp;//U20.20
				i64 *= round(dKando32768);// U21.35  四捨五入して規格値32768の整数に戻す
				i64 >>= (20 + iNormBitSense);//U20
				break;
			}


			if (i64 > limit){
				i64 = limit;
			}
			m_pTTMA->Dt[i][j] = CLIP(i64, 0, limit);//U16.0


#ifdef _RANDOM_OUTPUT
            RandImg.SetVal(i, j, random);
            RandImg6Bit.SetVal(i, j, random & 0x003F);
#endif

			//乱数を更新
			random_tmp =  (( random & 0x0001 ) >> 0 ) << 15;
			random_tmp += (( random & 0x8000 ) >> 15 ) << 14;
			random_tmp += (((random & 0x4000 ) >> 14 ) ^ (( random & 0x0001 ) >> 0 ) ) << 13;
			random_tmp += (((random & 0x2000 ) >> 13 ) ^ (( random & 0x0001 ) >> 0 ) ) << 12;
			random_tmp += (( random & 0x1000 ) >> 12 ) << 11;
			random_tmp += (((random & 0x0800 ) >> 11 ) ^ (( random & 0x0001 ) >> 0 ) ) << 10;
			random_tmp += (( random & 0x0400 ) >> 10 ) << 9;
			random_tmp += (( random & 0x0200 ) >> 9 ) << 8;
			random_tmp += (( random & 0x0100 ) >> 8 ) << 7;
			random_tmp += (( random & 0x0080 ) >> 7 ) << 6;
			random_tmp += (( random & 0x0040 ) >> 6 ) << 5;
			random_tmp += (( random & 0x0020 ) >> 5 ) << 4;
			random_tmp += (( random & 0x0010 ) >> 4 ) << 3;
			random_tmp += (( random & 0x0008 ) >> 3 ) << 2;
			random_tmp += (( random & 0x0004 ) >> 2 ) << 1;
			random_tmp += (( random & 0x0002 ) >> 1 ) << 0;
			random = random_tmp;
		}
	}

#ifdef _RANDOM_OUTPUT
    RandImg.SaveTmaTdt("C:\\RandomImage.tma");
	RandImg6Bit.SaveTmaTdt("C:\\RandomImage6Bit.tma");
#endif

	return true;
}

//---------------------------------------------------------------------------
/*
	欠陥補正６
		欠陥補正4,2,5の後の線欠陥補正(桑原さんorg)   //120807 ttsuji
*/

bool CSimars::bDefectCorrection6(string str_img, string str_coord, bool bAutoDetect, int& iGridDirection, float& fHonsuu, double& dA1, double& dA2, double& dA3, double& dA4, double dTh)
{
	TTMA Image, outImage;
	string FileName;
	const int iHashi = 3;   // これ以上遠くからは補間しない。

	/*        設定読み込み             */
	fstream fcoord;                                 // 入力座標データ
	fcoord.open( str_coord.c_str(), ios::in );
	if( fcoord.fail() ) {
		CApplication::GetInstance()->MessageBox((str_coord + " がopenできません").c_str(), "Error", MB_OK);
		return false;
	}

	// 設定読み込み
	int iSet; fcoord >> iSet;				// 行数
	int *iOn; iOn = new int[iSet];			// 縦・横・無視
	int *iLine; iLine = new int[iSet];		// 座標
	int *iWid; iWid = new int[iSet];		// 座標

	for( int i=0; i<iSet; i++){
		fcoord >> iOn[i] >> iLine[i] >> iWid[i];
	}
	fcoord.close();

	string Prefix = "GL";
	/**********   Prefix Section  **********/
	int iMs, iMw;  // 何本横から平均初めて何画素までか
	double a, b1, b2, b3, b4;
	double iThres;
	Prefix += "Lin";
	iMs=1;
	iMw=4;
	b1=dA1;
	b2=dA2;
	b3=dA3;
	b4=dA4;
	iThres=dTh;

	Prefix += "R0";

	/**************   補間    **************/
	int iRandom0[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int iRandom1[15] = {1,0,-1,0,1,-1,1,0,0,1,-1,0,-1,1,-1};
	int iRandom2[15] = {0,1,0,-2,2,1,0,-2,-1,2,-1,1,-2,-1,2};
	int *iRandom;
	int iR =0;
	int iSHashi=0;

	iRandom = iRandom0;

	bool Yoko = false;
	int  iGridDirectionByFFT = 1;//有無・方向（0:無、1:H方向、2:V方向、-1:エラー）
	if( bAutoDetect == true ){

		float hon;

#if 0	//TODO とりあえずコメントアウト
		GridLineDefectF->DetectGridPeak( str_img, hon, iGridDirectionByFFT );
#endif

		if( hon>=30.05 && hon <36.65){  // A1.0
			b1=0;  b2=1;  b3=0;  b4=0;
		} else if( hon>=36.65 && hon <37.95){  //A1.25
			b1=0;  b2=1.25;  b3=-0.25;  b4=0;
		} else if( hon>=37.95 && hon <38.55){  //A1.5
			b1=0;  b2=1.5;  b3=-0.5;  b4=0;
		} else if( hon>=38.55 && hon <38.85){  //A1.75
			b1=0;  b2=1.75;  b3=-0.75;  b4=0;
		} else if( hon>=38.85 && hon <39.15){  //A2.0
			b1=0;  b2=2;  b3=-1;  b4=0;
		} else if( hon>=39.15 && hon <40.75){  //B
			b1=0;  b2=1;  b3=0.7;  b4=-0.7;
		} else if( hon>=40.75 && hon <41.25){  //C1.6
			b1=0.5;  b2=-0.5;  b3=1.6;  b4=-0.6;
		} else if( hon>=41.25 && hon <41.75){  //C1.45
			b1=0.5;  b2=-0.5;  b3=1.45;  b4=-0.45;
		} else if( hon>=41.75 && hon <42.55){  //C1.3
			b1=0.5;  b2=-0.5;  b3=1.3;  b4=-0.3;
		} else if( hon>=42.55 && hon <43.75){  //C1.15
			b1=0.5;  b2=-0.5;  b3=1.15;  b4=-0.15;
		} else if( hon>=43.75 && hon <47.75){  //C1.0
			b1=0.5;  b2=-0.5;  b3=1;  b4=0;
		} else if( hon>=47.75 && hon <50.65){  //C0.5
			b1=0.5;  b2=-0.5;  b3=0.5;  b4=0.5;
		} else if( hon>=50.65 && hon <52.95){  //D1.8
			b1=1.8;  b2=-0.8;  b3=0;  b4=0;
		} else if( hon>=52.95 && hon <57.05){  //D1.5
			b1=1.5;  b2=-0.5;  b3=0;  b4=0;
		} else if( hon>=57.05 && hon <66.75){  //D1.4
			b1=1.4;  b2=-0.4;  b3=0;  b4=0;
		} else {  //else
			delete[] iOn;
			delete[] iLine;
			delete[] iWid;
			return false;
		}

		if( iGridDirectionByFFT==2 ){
			Yoko = false;
		} else if( iGridDirectionByFFT==1 ){
			Yoko = true;
		} else {//グリッド検出せず

			//グリッド検出できない場合は隣接画素で補間する
			b1 = 1.0;
			b2 = b3 = b4 = 0.0;

		}
		fHonsuu = hon;
	}
	else{//グリッド本数手動指定のときにはグリッド向きも合わせて指定（120807 ttsuji修正）
		Yoko = (bool)iGridDirection;
	}

	Image = *m_pTTMA;
	outImage = *m_pTTMA;

	for( int iL=0; iL<iSet; iL++){
	//すべて縦線欠陥として扱うために、横線欠陥の場合は主副反転させておく（ttsuji）

		if(iOn[iL] == 2) {//線欠陥が横であれば主副反転する

			Image.MainSub();
			outImage.MainSub();
		}

		//120807 ttsuji修正
		double b1tmp, b2tmp, b3tmp, b4tmp;
		if( (iOn[iL] == 2) ^ (Yoko==true) ) {//線欠陥が横、もしくはグリッドが横の排他的論理和（線欠陥とグリッド向きが直交する場合）
			b1tmp = b1;
			b2tmp = b2;
			b3tmp = b3;
			b4tmp = b4;
			//グリッド向きと直交する線欠陥の場合は隣接画素で補間する
			b1 = 1.0;
			b2 = b3 = b4 = 0.0;
		}


		//画像端の線欠陥は処理しない（ttsuji）
		if( iLine[iL] < iHashi || iLine[iL]+iWid[iL]-1>=Image.MainPixel-iHashi ) {

			if(iOn[iL] == 2) {//線欠陥が横であれば主副反転する

				Image.MainSub();
				outImage.MainSub();
			}
			//120807 ttsuji修正
			if( (iOn[iL] == 2) ^ (Yoko==true) ) {//線欠陥が横、もしくはグリッドが横の排他的論理和（線欠陥とグリッド向きが直交する場合）
				//補正係数を元に戻しておく
				b1 = b1tmp;
				b2 = b2tmp;
				b3 = b3tmp;
				b4 = b4tmp;
			}
			continue;//主副反転を元に戻してから次の欠陥に移る（ttsuji）
		}

		if( iWid[iL] == 1 ){
			int iM = iLine[iL];
			double fLeft=0, fRight=0;
			int iLeft0=0, iRight0=0;

			//すべて縦線欠陥として扱う（横線欠陥の場合は主副反転させておく）（ttsuji）
			for( int iS=iSHashi; iS<Image.SubPixel-iSHashi; iS++){
				fLeft =   b1/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs]
						+ b2/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-1]
						+ b3/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-2]
						+ b4/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-3];
				fRight =  b1/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs]
						+ b2/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+1]
						+ b3/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+2]
						+ b4/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+3];
				iLeft0 = Image.Dt[iS+iRandom[iR++%15]][iM-iMs-2];
				iRight0 = Image.Dt[iS+iRandom[iR++%15]][iM+iMs+2];

				if( iLeft0*iRight0==0 || double(iLeft0)/iRight0>iThres || double(iRight0)/iLeft0>iThres ){
					fLeft = Image.Dt[iS+iRandom[iR++%15]][iM-iMs]/2;
					fRight = Image.Dt[iS+iRandom[iR++%15]][iM+iMs]/2;
				}
				outImage.SetVal(iS, iM , int(fLeft + fRight) );
				Image.SetVal(iS, iM, outImage.Dt[iS][iM]);//線欠陥の交差点の補正残差対策として、補正結果を次の補正に用いるために導入(ttsuji120814)
			}
		}
		else if( iWid[iL] == 2 ){
			int iM = iLine[iL];
			int iLeft=0, iRight=0, iLeft0=0, iRight0=0;
			for( int iS=iSHashi; iS<Image.SubPixel-iSHashi; iS++){
				iLeft =
					  b1/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs]
					+ b2/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-1]
					+ b3/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-2]
					+ b4/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-3];
				iRight =
					+ b1/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs]
					+ b2/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+1]
					+ b3/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+2]
					+ b4/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+3];
				iLeft0 = Image.Dt[iS+iRandom[iR++%15]][iM-iMs-2];
				iRight0 = Image.Dt[iS+iRandom[iR++%15]][iM+iMs+2];

				if( iLeft0*iRight0==0 || double(iLeft0)/iRight0>iThres || double(iRight0)/iLeft0>iThres ){
					iLeft = Image.Dt[iS+iRandom[iR++%15]][iM-iMs]/2;
					iRight = Image.Dt[iS+iRandom[iR++%15]][iM+iMs]/2;
				}
				int iM1 = iLeft + iRight;

				iLeft =
					  b1/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs+1]
					+ b2/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs]
					+ b3/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-1]
					+ b4/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-2];
				iRight =
					+ b1/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+1]
					+ b2/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+2]
					+ b3/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+3]
					+ b4/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+4];
				iLeft0 = Image.Dt[iS+iRandom[iR++%15]][iM-iMs-1];
				iRight0 = Image.Dt[iS+iRandom[iR++%15]][iM+iMs+3];

				if( iLeft0*iRight0==0 || double(iLeft0)/iRight0>iThres || double(iRight0)/iLeft0>iThres ){
					iLeft = Image.Dt[iS+iRandom[iR++%15]][iM-iMs+1]/2;
					iRight = Image.Dt[iS+iRandom[iR++%15]][iM+iMs+1]/2;
				}
				int iM2 = iLeft + iRight;

				outImage.SetVal(iS, iM , iM1 );
				outImage.SetVal(iS,iM+1, iM2 );
				Image.SetVal(iS, iM, outImage.Dt[iS][iM]);//線欠陥の交差点の補正残差対策として、補正結果を次の補正に用いるために導入(ttsuji120814)
				Image.SetVal(iS, iM+1, outImage.Dt[iS][iM+1]);//線欠陥の交差点の補正残差対策として、補正結果を次の補正に用いるために導入(ttsuji120814)

			}
		}

		if(iOn[iL] == 2) {//線欠陥が横であれば主副反転する

			Image.MainSub();
			outImage.MainSub();
		}

		//120807 ttsuji修正
		if( (iOn[iL] == 2) ^ (Yoko==true) ) {//線欠陥が横、もしくはグリッドが横の排他的論理和（線欠陥とグリッド向きが直交する場合）
			//補正係数を元に戻しておく
			b1 = b1tmp;
			b2 = b2tmp;
			b3 = b3tmp;
			b4 = b4tmp;
		}


	}

	delete[] iOn;
	delete[] iLine;
	delete[] iWid;

	dA1 = b1;
	dA2 = b2;
	dA3 = b3;
	dA4 = b4;
	iGridDirection = (int)Yoko;
	Image = outImage;
	//*this = (TSIMARS)outImage;
	*m_pTTMA = outImage;

#if 0
	// 横線欠陥は略
	Image.WriteTma( AddPrefix( str_img, Prefix ));
#endif

	return true;
}
/* -------------------------------------------------------------------
* @brief	欠陥補正６(線欠陥)
* 			欠陥補正4,2,5の後の線欠陥補正(桑原さんorg)
* @param	lineDefect					[in]線欠陥データ
* @param	bAutoDetect					[in]グリッド有無と向きの自動検出フラグ
* @param	iGridDirection				[out]グリッドの有無と向き
* @param	fHonsuu						[out]グリッド本数
* @param	dA1							[out]第1隣接画素の補正ウェイト
* @param	dA2							[out]第2隣接画素の補正ウェイト
* @param	dA3							[out]第3隣接画素の補正ウェイト
* @param	dA4							[out]第4隣接画素の補正ウェイト
* @param	dTh							[out]とげ判定の閾値
* @param	bPreCor2Line				[out]仮埋めフラグ
* @retval   true:成功 /false:失敗
* @date     2012/08/07	ttsuji
 --------------------------------------------------------------------- */
bool CSimars::bDefectCorrection6a(vector<SLineDefect> lineDefect, bool bAutoDetect, int& iGridDirection, float& fHonsuu, double& dA1, double& dA2, double& dA3, double& dA4, double dTh, bool bPreCor2Line)
{
#define _LineDefectVer	(1)
#define _LineDefectHor	(2)
#define _GridNon	(0)
#define _GridHor	(1)
#define _GridVer	(2)

	TTMA Image, outImage;
	string FileName;
	const int iHashi = 3;   // これ以上遠くからは補間しない。

	// 設定読み込み
	int iDefLineNum;							// 線欠陥数
	iDefLineNum = lineDefect.size();
	int *iDefLineType;							// 縦(1)・横(2)・無視(0)
	iDefLineType = new int[iDefLineNum];
	int *iLine; 								// 座標
	iLine = new int[iDefLineNum];
	int *iWid; 							// 太さ
	iWid = new int[iDefLineNum];

	for( int i=0; i<iDefLineNum; i++){
		iDefLineType[i] = lineDefect[i].type;
		iLine[i] = lineDefect[i].lineNum;
		iWid[i] = lineDefect[i].width;
		//printf("type:%d lineNum:%d width:%d\n",iDefLineType[i], iLine[i], iWid[i]);
	}

	string Prefix = "GL";
	/**********   Prefix Section  **********/
	int iMs, iMw;  // 何本横から平均初めて何画素までか
	double a, b1, b2, b3, b4;
	double iThres;
	Prefix += "Lin";
	iMs=1;
	iMw=4;
	b1=dA1;
	b2=dA2;
	b3=dA3;
	b4=dA4;
	iThres=dTh;

	Prefix += "R0";

	/**************   補間    **************/
	int iRandom0[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int iRandom1[15] = {1,0,-1,0,1,-1,1,0,0,1,-1,0,-1,1,-1};
	int iRandom2[15] = {0,1,0,-2,2,1,0,-2,-1,2,-1,1,-2,-1,2};
	int *iRandom;
	int iR =0;
	int iSHashi=0;
	iRandom = iRandom0;

	bool Yoko = false;
	int  iGridDirectionByFFT = _GridHor;//有無・方向（0:無、1:H方向、2:V方向、-1:エラー）
	if( bAutoDetect == true ){

		float hon;

#if 0			//TODO GridLineDefectFコンパイルエラーのためとりあえずコメントアウト
		GridLineDefectF->DetectGridPeak( str_img, hon, iGridDirectionByFFT );
#endif
		if( hon>=30.05 && hon <36.65){  // A1.0
			b1=0;  b2=1;  b3=0;  b4=0;
		} else if( hon>=36.65 && hon <37.95){  //A1.25
			b1=0;  b2=1.25;  b3=-0.25;  b4=0;
		} else if( hon>=37.95 && hon <38.55){  //A1.5
			b1=0;  b2=1.5;  b3=-0.5;  b4=0;
		} else if( hon>=38.55 && hon <38.85){  //A1.75
			b1=0;  b2=1.75;  b3=-0.75;  b4=0;
		} else if( hon>=38.85 && hon <39.15){  //A2.0
			b1=0;  b2=2;  b3=-1;  b4=0;
		} else if( hon>=39.15 && hon <40.75){  //B
			b1=0;  b2=1;  b3=0.7;  b4=-0.7;
		} else if( hon>=40.75 && hon <41.25){  //C1.6
			b1=0.5;  b2=-0.5;  b3=1.6;  b4=-0.6;
		} else if( hon>=41.25 && hon <41.75){  //C1.45
			b1=0.5;  b2=-0.5;  b3=1.45;  b4=-0.45;
		} else if( hon>=41.75 && hon <42.55){  //C1.3
			b1=0.5;  b2=-0.5;  b3=1.3;  b4=-0.3;
		} else if( hon>=42.55 && hon <43.75){  //C1.15
			b1=0.5;  b2=-0.5;  b3=1.15;  b4=-0.15;
		} else if( hon>=43.75 && hon <47.75){  //C1.0
			b1=0.5;  b2=-0.5;  b3=1;  b4=0;
		} else if( hon>=47.75 && hon <50.65){  //C0.5
			b1=0.5;  b2=-0.5;  b3=0.5;  b4=0.5;
		} else if( hon>=50.65 && hon <52.95){  //D1.8
			b1=1.8;  b2=-0.8;  b3=0;  b4=0;
		} else if( hon>=52.95 && hon <57.05){  //D1.5
			b1=1.5;  b2=-0.5;  b3=0;  b4=0;
		} else if( hon>=57.05 && hon <66.75){  //D1.4
			b1=1.4;  b2=-0.4;  b3=0;  b4=0;
		} else {  //else
			delete[] iDefLineType;
			delete[] iLine;
			delete[] iWid;
			return false;
		}

		if(_GridVer == iGridDirectionByFFT){//縦グリッド
			Yoko = false;
		} else if (_GridHor == iGridDirectionByFFT){//横グリッド
			Yoko = true;
		} else if (_GridNon == iGridDirectionByFFT){//グリッド検出せず
			//グリッド検出できない場合は隣接画素で補間する
			b1 = 1.0;
			b2 = b3 = b4 = 0.0;
		} else {//エラー
			delete[] iDefLineType;
			delete[] iLine;
			delete[] iWid;
			return false;
		}
		fHonsuu = hon;


		switch (iGridDirectionByFFT) {
		case _GridVer:
			iGridDirection = 0;
			break;
		case _GridHor:
			iGridDirection = 1;
			break;
		case _GridNon:
			iGridDirection = 2;
			break;
		}
	}
	else{//グリッド本数手動指定のときにはグリッド向きも合わせて指定（120807 ttsuji修正）
		Yoko = (bool)iGridDirection;

		switch (iGridDirection){ //ImageToolのGUI(0:V方向、1:H方向、2:無)
		case 0:
			iGridDirectionByFFT = _GridVer;//有無・方向（0:無、1:H方向、2:V方向、-1:エラー）
			break;
		case 1:
			iGridDirectionByFFT = _GridHor;//有無・方向（0:無、1:H方向、2:V方向、-1:エラー）
			break;
		case 2:
			iGridDirectionByFFT = _GridNon;//有無・方向（0:無、1:H方向、2:V方向、-1:エラー）
			break;
		}
	}

	Image = *m_pTTMA;
	outImage = *m_pTTMA;

	const int iToge = 3;

	for( int iL=0; iL<iDefLineNum; iL++){
		//すべて縦線欠陥として扱うために、横線欠陥の場合は主副反転させておく（ttsuji）
		if(_LineDefectHor == iDefLineType[iL]) {//線欠陥が横であれば主副反転する
			Image.MainSub();
			outImage.MainSub();
		}

		//120807 ttsuji修正
		double b1tmp(100.), b2tmp(100.), b3tmp(100.), b4tmp(100.);
		double dLeft=0, dRight=0;
		int iM = iLine[iL];			//線欠陥の主方向位置
		int iLeft=0, iRight=0, iLeft0=0, iRight0=0;

		if ( iWid[iL] == 1 ){		//太さ1

//			if ((_LineDefectVer == iDefLineType[iL]) & (_GridVer == iGridDirectionByFFT) ||
//				(_LineDefectHor == iDefLineType[iL]) & (_GridHor == iGridDirectionByFFT)) {//グリッドと線欠陥が平行なときはそのまま(横線欠陥が横グリッドと平行なときが抜けていたのを修正)170619tsuji
//
//				;
//			}
//			else{//線欠陥とグリッドが直交する場合、もしくはグリッドが検出されなかった場合
				b1tmp = b1;
				b2tmp = b2;
				b3tmp = b3;
				b4tmp = b4;
				//両隣の画素で補間
				b1 = 1.0;
				b2 = b3 = b4 = 0.0;
//			}

			//すべて縦線欠陥として扱う（横線欠陥の場合は主副反転させておく）（ttsuji）
			for( int iS=iSHashi; iS<Image.SubPixel-iSHashi; iS++){
				//画像端に線欠陥がある場合
				if (0 == iM) {
					outImage.SetVal(iS, iM , Image.Dt[iS][iM+1]);
					Image.SetVal(iS, iM, Image.Dt[iS][iM+1]);//隣接する片側の画素値をコピーする
				}
				else if (Image.MainPixel - 1 == iM) {
					outImage.SetVal(iS, iM , Image.Dt[iS][iM-1]);
					Image.SetVal(iS, iM, Image.Dt[iS][iM-1]);//隣接する片側の画素値をコピーする
				}
				//画像端以外に線欠陥がある場合
				else{
					dLeft =   b1/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs]
							+ b2/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-1]
							+ b3/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-2]
							+ b4/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-3];
					dRight =  b1/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs]
							+ b2/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+1]
							+ b3/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+2]
							+ b4/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+3];
					iLeft0 = Image.Dt[iS+iRandom[iR++%15]][iM-iToge];
					iRight0 = Image.Dt[iS+iRandom[iR++%15]][iM+iToge];

					//トゲ判定
					if (iLeft0*iRight0 == 0 || double(iLeft0)/iRight0 > iThres || double(iRight0)/iLeft0 > iThres){
						dLeft = Image.Dt[iS+iRandom[iR++%15]][iM-iMs]/2;
						dRight = Image.Dt[iS+iRandom[iR++%15]][iM+iMs]/2;
					}
					outImage.SetVal(iS, iM , int(dLeft + dRight) );
					Image.SetVal(iS, iM, outImage.Dt[iS][iM]);//線欠陥の交差点の補正残差対策として、補正結果を次の補正に用いるために導入(ttsuji120814)
				}
			}
		}
		else if ( iWid[iL] == 2 ){		//太さ2

			for( int iS=iSHashi; iS<Image.SubPixel-iSHashi; iS++){
				double dQL_M0, dQL_M1;//補間対象のQL値

				//画像端に線欠陥がある場合
				if (0 == iM) {
					outImage.SetVal(iS, iM , Image.Dt[iS][iM+2]);
					outImage.SetVal(iS, iM+1 , Image.Dt[iS][iM+2]);
					Image.SetVal(iS, iM, Image.Dt[iS][iM+2]);//隣接する片側の画素値をコピーする
					Image.SetVal(iS, iM+1, Image.Dt[iS][iM+2]);//隣接する片側の画素値をコピーする
				}
				else if (Image.MainPixel - 2 == iM) {
					outImage.SetVal(iS, iM , Image.Dt[iS][iM-1]);
					outImage.SetVal(iS, iM+1 , Image.Dt[iS][iM-1]);
					Image.SetVal(iS, iM, Image.Dt[iS][iM-1]);//隣接する片側の画素値をコピーする
					Image.SetVal(iS, iM+1, Image.Dt[iS][iM-1]);//隣接する片側の画素値をコピーする
				}
				//画像端から1～3画素離れたところから線欠陥の場合
				else if (1 == iM || 2 == iM || 3 == iM || Image.MainPixel - 3 == iM || Image.MainPixel - 4 == iM || Image.MainPixel - 5 == iM) {//線形補間とする
					dQL_M0 =  2/3.*Image.Dt[iS][iM-1] + 1/3.*Image.Dt[iS][iM+2];//2ライン連続線欠陥に隣接する正常画素で直線補間
					dQL_M1 =  1/3.*Image.Dt[iS][iM-1] + 2/3.*Image.Dt[iS][iM+2];//2ライン連続線欠陥に隣接する正常画素で直線補間
					outImage.SetVal(iS, iM,   dQL_M0);//仮埋め
					outImage.SetVal(iS, iM+1, dQL_M1);//仮埋め
					Image.SetVal(iS, iM,   dQL_M0);//仮埋め
					Image.SetVal(iS, iM+1, dQL_M1);//仮埋め
				}
				//画像端以外に線欠陥がある場合
				else{
					if (bPreCor2Line & ((_LineDefectVer == iDefLineType[iL]) & (_GridVer == iGridDirectionByFFT)) ||
						bPreCor2Line & ((_LineDefectHor == iDefLineType[iL]) & (_GridHor == iGridDirectionByFFT))) {//グリッドと線欠陥が平行なときはそのまま(横線欠陥が横グリッドと平行なときが抜けていたのを修正)170619tsuji
  #if 1
						//ここに、欠陥補正7相当アルゴで仮埋めするコードを書く。（欠陥補正4の仮埋めを使わない場合はこれが必須）
						dQL_M0 =  2/3.*Image.Dt[iS][iM-1] + 1/3.*Image.Dt[iS][iM+2];//2ライン連続線欠陥に隣接する正常画素で直線補間
						dQL_M1 =  1/3.*Image.Dt[iS][iM-1] + 2/3.*Image.Dt[iS][iM+2];//2ライン連続線欠陥に隣接する正常画素で直線補間
						Image.SetVal(iS, iM,   dQL_M0);//仮埋め
						Image.SetVal(iS, iM+1, dQL_M1);//仮埋め
#endif
					}

					//iMについて(dQL_M0)
					//まずトゲ判定
					iLeft0  = Image.Dt[iS][iM - iToge];//線欠陥から３画素離れた画素で判定
					iRight0 = Image.Dt[iS][iM + iToge];//線欠陥から３画素離れた画素で判定
					if( iLeft0*iRight0==0 || double(iLeft0)/iRight0>iThres || double(iRight0)/iLeft0>iThres ){//トゲ
						//2ライン連続線欠陥に隣接する正常画素で直線補間
						dQL_M0 =  2/3.*Image.Dt[iS][iM-1] + 1/3.*Image.Dt[iS][iM+2];
					}
					else{//非トゲ
//						if ((_LineDefectVer == iOn[iL]) ^ (_GridHor == iGridDirectionByFFT)) {//線欠陥が縦とグリッドが横の排他的論理和（グリッドと線欠陥が平行の場合）（←bug! （横グリッドの偽は縦のみならずグリッドなしも含まれるため））
//						if ((_LineDefectVer == iOn[iL]) & (_GridVer == iGridDirectionByFFT)) {//グリッドと線欠陥が平行なときだけグリッド本数に基づいて補正する
						if ((_LineDefectVer == iDefLineType[iL]) & (_GridVer == iGridDirectionByFFT) ||
							(_LineDefectHor == iDefLineType[iL]) & (_GridHor == iGridDirectionByFFT)) {//グリッドと線欠陥が平行なときはそのまま(横線欠陥が横グリッドと平行なときが抜けていたのを修正)170619tsuji
							dLeft =
								  b1/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs]
								+ b2/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-1]
								+ b3/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-2]
								+ b4/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-3];
							dRight =
								+ b1/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs]
								+ b2/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+1]
								+ b3/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+2]
								+ b4/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+3];
							dQL_M0 = dLeft + dRight;
						}
						else{
							//隣接画素にて補間
							dQL_M0 =  2/3.*Image.Dt[iS][iM-1] + 1/3.*Image.Dt[iS][iM+2];
						}
					}
					//iM+1について(dQL_M1)
					//まずトゲ判定
					iLeft0  = Image.Dt[iS][iM + 1 - iToge];//線欠陥から３画素離れた画素で判定
					iRight0 = Image.Dt[iS][iM + 1 + iToge];//線欠陥から３画素離れた画素で判定
					if( iLeft0*iRight0==0 || double(iLeft0)/iRight0>iThres || double(iRight0)/iLeft0>iThres ){//トゲ
						//2ライン連続線欠陥に隣接する正常画素で直線補間
						dQL_M1 =  1/3.*Image.Dt[iS][iM-1] + 2/3.*Image.Dt[iS][iM+2];
					}
					else{//非トゲ
						if ((_LineDefectVer == iDefLineType[iL]) & (_GridVer == iGridDirectionByFFT) ||
							(_LineDefectHor == iDefLineType[iL]) & (_GridHor == iGridDirectionByFFT)) {//グリッドと線欠陥が平行なときはそのまま(横線欠陥が横グリッドと平行なときが抜けていたのを修正)170619tsuji
							dLeft =
								  b1/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs+1]
								+ b2/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs]
								+ b3/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-1]
								+ b4/2.*Image.Dt[iS+iRandom[iR++%15]][iM-iMs-2];
							dRight =
								+ b1/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+1]
								+ b2/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+2]
								+ b3/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+3]
								+ b4/2.*Image.Dt[iS+iRandom[iR++%15]][iM+iMs+4];
							dQL_M1 = dLeft + dRight;
						}
						else{
							//隣接画素にて補間
							dQL_M1 =  1/3.*Image.Dt[iS][iM-1]   + 2/3.*Image.Dt[iS][iM+2];
						}
					}
					outImage.SetVal(iS, iM,   dQL_M0);
					outImage.SetVal(iS, iM+1, dQL_M1);
					Image.SetVal(iS, iM, outImage.Dt[iS][iM]);//線欠陥の交差点の補正残差対策として、補正結果を次の補正に用いるために導入(ttsuji120814)
					Image.SetVal(iS, iM+1, outImage.Dt[iS][iM+1]);//線欠陥の交差点の補正残差対策として、補正結果を次の補正に用いるために導入(ttsuji120814)
				}
  			}
		}


		if (_LineDefectHor == iDefLineType[iL]) {//線欠陥が横であれば主副反転する
			Image.MainSub();
			outImage.MainSub();
		}

		if (b1tmp < 99) {//バッファb1tmpを使っている場合
			//補正係数を元に戻しておく
			b1 = b1tmp;
			b2 = b2tmp;
			b3 = b3tmp;
			b4 = b4tmp;
		}
	}

	delete[] iDefLineType;
	delete[] iLine;
	delete[] iWid;

	dA1 = b1;
	dA2 = b2;
	dA3 = b3;
	dA4 = b4;

	for(int i=0;i<outImage.SubPixel;i++){
		for(int j=0;j<outImage.MainPixel;j++){
			m_pTTMA->Dt[i][j] = outImage.Dt[i][j];
		}
	}

//	m_pTTMA->SaveTmaTdt("DEFECT6a.tma");

#if 0
	// 横線欠陥は略
	Image.WriteTma( AddPrefix( str_img, Prefix ));
#endif

	return true;
}

/* -------------------------------------------------------------------
* @brief	欠陥補正7(点欠陥)
* 			大小欠陥をまとめて１つのアルゴで処理
* 			線欠陥の仮埋めは従来は４で行っていたが、６の内部の前段処理として行う方がすっきりするのでそうする。
* 			端処理どうするか？
* 			大小欠陥をまとめて１つのアルゴで処理する新規アルゴ（グリッドなしのときには欠陥周囲のパターン解析を含む）
* @param	bad							欠陥マップデータ
* @param	iGridDirection				縦：０、横：１、なし：２
* @param	bPatternAnalysis
* @param	iPixelNumPatternAnalysis
* @param	dThPatternExist
* @param	bModifyWeight
* @retval   true:成功 /false:失敗
* @date     2013/03/31	ttsuji
 --------------------------------------------------------------------- */
bool CSimars::bDefectCorrection7(TTMA& bad, const int iGridDirection, const bool bPatternAnalysis,
		const int iPixelNumPatternAnalysis, const double dThPatternExist, const bool bModifyWeight){

	if (0 == iGridDirection) {//縦グリッドならば主副反転
		bad.MainSub();
		m_pTTMA->MainSub();
	}

	for(int iMCnt = 0; iMCnt < m_pTTMA->MainPixel; ++iMCnt){
		for(int iSCnt = 0; iSCnt < m_pTTMA->SubPixel; ++iSCnt){
			//サイズオーバーの点欠陥以外の点欠陥を処理（線欠陥も除く）
			if (bad.Dt[iSCnt][iMCnt] > 0 && bad.Dt[iSCnt][iMCnt] != 0x2000 && bad.Dt[iSCnt][iMCnt] != 0xEEEE) {
				if (2 != iGridDirection) {//グリッドあり
					//欠陥に対して水平方向のみの画素で、かつ欠陥に隣接した画素から補間（横グリッドと想定し、グリッドの向きのみから補間）（要は欠陥補正2改と同じアルゴ）
					//　→　エッジある画素でのトゲなくなり、かつスムーズ領域での良化も保持しており、両領域で従来補正4より改善した！
					const int iMSearchInterval = 1;//1画素ずつ欠陥周囲の正常画素を探索
					if (!bDefectCorrectionByOnlyHor2Pixel(bad, iSCnt, iMCnt, iMSearchInterval)){
						CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByOnlyHor2Pixel()", "Error", MB_OK);
						if (0 == iGridDirection) {//縦グリッドならば主副反転
							bad.MainSub();
							m_pTTMA->MainSub();
						}
						return false;
					}
				}
				else{//グリッドなし
					const int iSearchInterval = 1;//1画素ずつ欠陥周囲の正常画素を探索
					if (bPatternAnalysis) {
						if (!bDefectCorrectionWithPatternAnalysis(bad, iSCnt, iMCnt, iSearchInterval, iPixelNumPatternAnalysis, dThPatternExist)) {
							CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionWithPatternAnalysis()", "Error", MB_OK);
							return false;
						}
					}
					else{
						if (!bDefectCorrectionByHorVer4Pixel(bad, iSCnt, iMCnt, iSearchInterval, bModifyWeight)){
							CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByHorVer4Pixel()", "Error", MB_OK);
							return false;
						}
					}
				}
			}
		}
	}

	//元に戻す
	if (0 == iGridDirection) {//縦グリッドならば主副反転
		bad.MainSub();
		m_pTTMA->MainSub();
	}

	return true;


}
//---------------------------------------------------------------------------
/*
欠陥に対して水平方向のみの画素で、かつ欠陥に隣接した画素のみから、その距離に応じて補間
（横グリッドと想定し、グリッドの向きのみから補間）
 */
//欠陥に対して水平方向のみの２画素で、かつ欠陥に隣接した画素のみから、その距離に応じて補間（横グリッドと想定し、グリッドの向きのみから補間）
bool CSimars::bDefectCorrectionByOnlyHor2Pixel(TTMA& bad, const int iSCor, const int iMCor, const int iMSearchInterval){

	int iQLleft = 0;
	int iQLright = 0;
	int iM1 = 0;
	int iM2 = 0;
	bool bLeft = false;
	bool bRight = false;
	int iMShift = 0;

	//左探索
	if(iMCor - iMSearchInterval >= 0){
		iMShift = iMSearchInterval;
		while (0 != bad.Dt[iSCor][iMCor - iMShift] && (iMCor - iMShift) > 0){//横線欠陥の場合に画像幅の分までiMShiftがインクリメントされる無駄処理が入ってしまう。
//		while (0 != bad.Dt[iSCor][iMCor - iMShift] && (iMCor - iMShift) > 0 && iMShift <= 2){//大サイズ欠陥の尾の部分が対応できないので補正画質NG
			iMShift += iMSearchInterval;
		}
		iM1 = iMCor - iMShift;
		if (0 != bad.Dt[iSCor][iM1]) {//端まで欠陥画素がある場合
			bLeft = false;
		}
		else if (iM1 >= 0 && iM1 < bad.MainPixel) {
			iQLleft = m_pTTMA->Dt[iSCor][iM1];
			bLeft = true;
		}
		else{
			bLeft = false;
		}
	}

	//右探索
	if(iMCor + iMSearchInterval < bad.MainPixel - 1){
		iMShift = iMSearchInterval;
//		while (0 != bad.Dt[iSCor][iMCor + iMShift] && (iMCor + iMShift) < bad.MainPixel){//横線欠陥の場合に画像幅の分までiMShiftがインクリメントされる無駄処理が入ってしまう。
//		while (0 != bad.Dt[iSCor][iMCor + iMShift] && (iMCor + iMShift) < bad.MainPixel && iMShift <= 2){//大サイズ欠陥の尾の部分が対応できないので補正画質NG
		while (0 != bad.Dt[iSCor][iMCor + iMShift] && (iMCor + iMShift) < bad.MainPixel - 1){//横線欠陥の場合に画像幅の分までiMShiftがインクリメントされる無駄処理が入ってしまう。
			iMShift += iMSearchInterval;
		}
		iM2 = iMCor + iMShift;
		if (0 != bad.Dt[iSCor][iM2]) {//端まで欠陥画素がある場合
			bRight = false;
		}
		else if (iM2 >= 0 && iM2 < bad.MainPixel) {
			iQLright = m_pTTMA->Dt[iSCor][iM2];
			bRight = true;
		}
		else{
			bRight = false;
		}
	}


	//補正に用いる正常画素が左右ともに見つかった場合
	if (bLeft && bRight) {
		if (iM2 - iM1) {
			m_pTTMA->Dt[iSCor][iMCor] = (int)((iQLleft * (iM2 - iMCor) + iQLright * (iMCor - iM1)) / (float)(iM2 - iM1));
		}
		else{
			m_pTTMA->Dt[iSCor][iMCor] = iQLleft;
		}
	}
	//画像端などで左右の片方しか正常画素が見つからなかった場合
#if 0
	//片側1画素のdupe（→補正精度が悪いため不採用）
	else if (bLeft) {
		m_pTTMA->Dt[iSCor][iMCor] = iQLleft;
	}
	else if (bRight) {
		m_pTTMA->Dt[iSCor][iMCor] = iQLright;
	}
	else{
		return false;
	}
#else
	//上下左右の正常３画素を用いて補間（→こちらを採用）
	else {
		int iSearchInterval = iMSearchInterval;
//		bool bModifyWeight = true;//上下左右のウェイトを1/dに（補正精度良いがFPGAメモリブロック食うため不採用）
		bool bModifyWeight = false;//上下と左右のウェイト比を1:1に（これを採用）
		if (!bDefectCorrectionByHorVer4Pixel(bad, iSCor, iMCor, iSearchInterval, bModifyWeight)){
			CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByHorVer4Pixel()", "Error", MB_OK);
			return false;
		}
	}
#endif

	return true;
}
//---------------------------------------------------------------------------
/*
  欠陥に対して上下左右方向のみの４画素で、かつ欠陥に隣接した画素のみから、その距離に応じて補間

*/
//欠陥に対して上下左右方向のみの４画素で、かつ欠陥に隣接した画素のみから、その距離に応じて補間
bool CSimars::bDefectCorrectionByHorVer4Pixel(TTMA& bad, const int iSCor, const int iMCor, const int iSearchInterval, const bool bModifyWeight){

	int iQLleft = 0;
	int iQLright = 0;
	int iQLup = 0;
	int iQLdown = 0;
	int iM1 = 0;
	int iM2 = 0;
	int iS1 = 0;
	int iS2 = 0;
	bool bLeft = false;
	bool bRight = false;
	bool bUp = false;
	bool bDown = false;

	//欠陥の上下左右の隣接正常画素を探索
	SearchNormal4PixelsArroundBadPixel(bad, iSCor, iMCor, iSearchInterval, bLeft, bRight, bUp, bDown, iM1, iM2, iS1, iS2, iQLleft, iQLright, iQLup, iQLdown);


	 //上下と左右からの寄与を１：１とする
	//（→グリッドありのときにかけると、補正残差が従来補正425並に目立つようになった）
	if (!bModifyWeight){
		float fQLver, fQLhor;
		//補正に用いる正常画素が左右ともに見つかった場合
		if (bLeft && bRight) {
			if (iM2 - iM1) {
				fQLhor = (iQLleft * (iM2 - iMCor) + iQLright * (iMCor - iM1)) / (float)(iM2 - iM1);
			}
			else{
				fQLhor = iQLleft;
			}
		}
		//画像端などで左右の片方しか正常画素が見つからなかった場合
		else if (bLeft) {
			fQLhor = iQLleft;
		}
		else if (bRight) {
			fQLhor = iQLright;
		}

		//補正に用いる正常画素が上下ともに見つかった場合
		if (bUp && bDown) {
			if (iS2 - iS1) {
				fQLver = (iQLup * (iS2 - iSCor) + iQLdown * (iSCor - iS1)) / (float)(iS2 - iS1);
			}
			else{
				fQLver = iQLup;
			}
		}
		//画像端などで上下の片方しか正常画素が見つからなかった場合
		else if (bUp) {
			fQLver = iQLup;
		}
		else if (bDown) {
			fQLver = iQLdown;
		}

		//上下左右の統合
		m_pTTMA->SetVal(iSCor, iMCor, (fQLhor + fQLver)/2);
	}
	//上下も左右もともに欠陥画素からの距離に応じた重みとする
	//（→グリッドありのときにかけると、補正残差が従来補正425並に目立つようになった。上下、左右のウェイトを半分としたときとは変化視認できず）
	else{
		float fNorm   = 0;
		float fWleft  = 0;
		float fWright = 0;
		float fWup    = 0;
		float fWdown  = 0;

		//上下左右の4方向の正常画素に対する補間係数を計算
		CalcNormAndWeight(iSCor, iMCor, bLeft, bRight, bUp, bDown, iM1, iM2, iS1, iS2, fNorm, fWleft, fWright, fWup, fWdown);
		//補間値
		m_pTTMA->SetVal(iSCor, iMCor, fWup*iQLup + fWdown*iQLdown + fWleft*iQLleft + fWright*iQLright);
	}

    return true;

}
//---------------------------------------------------------------------------
/*
	欠陥の上下左右の隣接正常画素を探索
*/
void CSimars::SearchNormal4PixelsArroundBadPixel(TTMA& bad, const int iSCor, const int iMCor, const int iSearchInterval,
			bool& bLeft, bool& bRight, bool& bUp, bool& bDown, int& iM1, int& iM2, int& iS1, int& iS2,
			int& iQLleft, int& iQLright, int& iQLup, int& iQLdown){

	int iMShift = 0;
	int iSShift = 0;

	//左探索
	if(iMCor - iSearchInterval >= 0){
		iMShift = iSearchInterval;
		while (0 != bad.Dt[iSCor][iMCor - iMShift] && (iMCor - iMShift) > 0){
			iMShift += iSearchInterval;
		}
		iM1 = iMCor - iMShift;
		if (0 != bad.Dt[iSCor][iM1]) {//端まで欠陥画素がある場合
			bLeft = false;
		}
		else if (iM1 >= 0 && iM1 < bad.MainPixel) {
			iQLleft = m_pTTMA->Dt[iSCor][iM1];
			bLeft = true;
		}
		else{
			bLeft = false;
		}
	}
	//右探索
	if(iMCor + iSearchInterval < bad.MainPixel - 1){
		iMShift = iSearchInterval;
//		while (0 != bad.Dt[iSCor][iMCor + iMShift] && (iMCor + iMShift) < bad.MainPixel){
		while (0 != bad.Dt[iSCor][iMCor + iMShift] && (iMCor + iMShift) < bad.MainPixel - 1){
			iMShift += iSearchInterval;
		}
		iM2 = iMCor + iMShift;
		if (0 != bad.Dt[iSCor][iM2]) {//端まで欠陥画素がある場合
			bRight = false;
		}
		else if (iM2 >= 0 && iM2 < bad.MainPixel) {
			iQLright = m_pTTMA->Dt[iSCor][iM2];
			bRight = true;
		}
		else{
			bRight = false;
		}
	}
	//上探索
	if(iSCor - iSearchInterval >= 0){
		iSShift = iSearchInterval;
		while (0 != bad.Dt[iSCor - iSShift][iMCor] && (iSCor - iSShift) > 0){
			iSShift += iSearchInterval;
		}
		iS1 = iSCor - iSShift;
		if (0 != bad.Dt[iS1][iMCor]) {//端まで欠陥画素がある場合
			bUp = false;
		}
		else if (iS1 >= 0 && iS1 < bad.SubPixel) {
			iQLup = m_pTTMA->Dt[iS1][iMCor];
			bUp = true;
		}
		else{
			bUp = false;
		}
	}
	//下探索
	if(iSCor + iSearchInterval < bad.SubPixel - 1){
		iSShift = iSearchInterval;
//		while (0 != bad.Dt[iSCor + iSShift][iMCor] && (iSCor + iSShift) < bad.SubPixel){ // iSCor+iSShift = bad.SubPixelとなったときに実行エラー！
		while (0 != bad.Dt[iSCor + iSShift][iMCor] && (iSCor + iSShift) < bad.SubPixel - 1){
			iSShift += iSearchInterval;
		}
		iS2 = iSCor + iSShift;
		if (0 != bad.Dt[iS2][iMCor]) {//端まで欠陥画素がある場合
			bDown = false;
		}
		else if (iS2 >= 0 && iS2 < bad.SubPixel) {
			iQLdown = m_pTTMA->Dt[iS2][iMCor];
			bDown = true;
		}
		else{
			bDown = false;
		}
	}
}
/* -------------------------------------------------------------------
* @brief	パターン認識欠陥補正
* 			欠陥に対して上下左右および斜め45度方向の８画素の周辺の正常画素を解析して、
* 			パターンを認識して補正に用いる画素を選択する
* @param	bad							欠陥マップデータ
* @param	iSCor						対象画素の副走査位置
* @param	iMCor						対象画素の主走査位置
* @param	iSearchInterval				検索画素単位(基本的には1とする)
* @param	dThPatternExist				0.3とか0.4とか？ダイアログの値を使用していて実際の値が分からない。
* @retval   true:成功 /false:失敗
* @date     2013/03/31より前	作者不明
 --------------------------------------------------------------------- */
bool CSimars::bDefectCorrectionWithPatternAnalysis(TTMA& bad, const int iSCor, const int iMCor, const int iSearchInterval, const int iPixelNumPatternAnalysis, const double dThPatternExist)
{
	DEFECTARROUNDPIXELS defect;

	//欠陥の上下左右および斜めの8方向の最近接の正常画素を探索
	SearchNormal8PixelsArroundBadPixel(bad, iSCor, iMCor, iSearchInterval, defect);

	int iNumOfAnlsPix = iPixelNumPatternAnalysis;//パターン解析に用いる上下左右の各方向の正常画素の数
//	const int iNumOfAnlsPix = 3;//パターン解析に用いる上下左右の各方向の正常画素の数
//	const int iNumOfAnlsPix = 4;//パターン解析に用いる上下左右の各方向の正常画素の数

	int iNumOfLeftPix 		= 1;//左方向のパターン解析用の画素数
	int iNumOfRightPix 		= 1;//右方向のパターン解析用の画素数
	int iNumOfUpPix 		= 1;//上方向のパターン解析用の画素数
	int iNumOfDownPix 		= 1;//下方向のパターン解析用の画素数
	int iNumOfUpLeftPix 	= 1;//左上方向のパターン解析用の画素数
	int iNumOfUpRightPix 	= 1;//右上方向のパターン解析用の画素数
	int iNumOfDownLeftPix 	= 1;//左下方向のパターン解析用の画素数
	int iNumOfDownRightPix 	= 1;//右下方向のパターン解析用の画素数
	int iMShift;
	int iSShift;
	float fNorm   = 0;
	float fWleft  = 0;
	float fWright = 0;
	float fWup    = 0;
	float fWdown  = 0;

	bool bAnalysisPossible = true;

	Vct vUp, vDown, vLeft, vRight, vUpLeft, vUpRight, vDownLeft, vDownRight;//パターン解析に用いる上下左右の各方向の正常画素の集合

	//いずれか１方向でも補間画素が見つからない場合はパターン認識しない
	if (!defect.Left.bExist || !defect.Right.bExist || !defect.Up.bExist || !defect.Down.bExist || !defect.UpLeft.bExist || !defect.UpRight.bExist || !defect.DownLeft.bExist || !defect.DownRight.bExist) {
		bAnalysisPossible = false;
	}
	else {
		//左探索
		defect.Left.dAnalysisQL[0] = defect.Left.iQL;
		iMShift = 1;
		while (iNumOfLeftPix < iNumOfAnlsPix && (defect.Left.iM - iMShift) > 0){
			if (!bad.Dt[iSCor][defect.Left.iM - iMShift]) {
				defect.Left.dAnalysisQL[iNumOfLeftPix] = m_pTTMA->Dt[iSCor][defect.Left.iM - iMShift];
				iNumOfLeftPix++;
			}
			iMShift++;
		}
		if (iNumOfLeftPix != iNumOfAnlsPix) {//所定の個数だけパターン解析用の画素数が見つからなかった場合
			defect.Left.bAnalysisPossible = false;
			goto calc;
		}
		defect.Left.bAnalysisPossible = true;
		defect.Left.iNumOfAnlsPix = iNumOfLeftPix;
		vLeft.set(defect.Left.dAnalysisQL, iNumOfAnlsPix);
		defect.Left.dVariance = vLeft.GetVariance();
		defect.Left.dStdEv = vLeft.GetStandardDeviation();

		//右探索
		defect.Right.dAnalysisQL[0] = defect.Right.iQL;
		iMShift = 1;
		while (iNumOfRightPix < iNumOfAnlsPix && (defect.Right.iM + iMShift) < bad.MainPixel - 1){
			if (!bad.Dt[iSCor][defect.Right.iM + iMShift]) {
				defect.Right.dAnalysisQL[iNumOfRightPix] = m_pTTMA->Dt[iSCor][defect.Right.iM + iMShift];
				iNumOfRightPix++;
			}
			iMShift++;
		}
		if (iNumOfRightPix != iNumOfAnlsPix) {//所定の個数だけパターン解析用の画素数が見つからなかった場合
			defect.Right.bAnalysisPossible = false;
			goto calc;
		}
		defect.Right.bAnalysisPossible = true;
		defect.Right.iNumOfAnlsPix = iNumOfRightPix;
		vRight.set(defect.Right.dAnalysisQL, iNumOfAnlsPix);
		defect.Right.dVariance = vRight.GetVariance();
		defect.Right.dStdEv = vRight.GetStandardDeviation();

		//上探索
		defect.Up.dAnalysisQL[0] = defect.Up.iQL;
		iSShift = 1;
		while (iNumOfUpPix < iNumOfAnlsPix && (defect.Up.iS - iSShift) > 0){
			if (!bad.Dt[defect.Up.iS - iSShift][iMCor]) {
				defect.Up.dAnalysisQL[iNumOfUpPix] = m_pTTMA->Dt[defect.Up.iS - iSShift][iMCor];
				iNumOfUpPix++;
			}
			iSShift++;
		}
		if (iNumOfUpPix != iNumOfAnlsPix) {//所定の個数だけパターン解析用の画素数が見つからなかった場合
			defect.Up.bAnalysisPossible = false;
			goto calc;
		}
		defect.Up.bAnalysisPossible = true;
		defect.Up.iNumOfAnlsPix = iNumOfUpPix;
		vUp.set(defect.Up.dAnalysisQL, iNumOfAnlsPix);
		defect.Up.dVariance = vUp.GetVariance();
		defect.Up.dStdEv = vUp.GetStandardDeviation();

		//下探索
		defect.Down.dAnalysisQL[0] = defect.Down.iQL;
		iSShift = 1;
		while (iNumOfDownPix < iNumOfAnlsPix && (defect.Down.iS + iSShift) < bad.SubPixel - 1){
			if (!bad.Dt[defect.Down.iS + iSShift][iMCor]) {
				defect.Down.dAnalysisQL[iNumOfDownPix] = m_pTTMA->Dt[defect.Down.iS + iSShift][iMCor];
				iNumOfDownPix++;
			}
			iSShift++;
		}
		if (iNumOfDownPix != iNumOfAnlsPix) {//所定の個数だけパターン解析用の画素数が見つからなかった場合
			defect.Down.bAnalysisPossible = false;
			goto calc;
		}
		defect.Down.bAnalysisPossible = true;
		defect.Down.iNumOfAnlsPix = iNumOfDownPix;
		vDown.set(defect.Down.dAnalysisQL, iNumOfAnlsPix);
		defect.Down.dVariance = vDown.GetVariance();
		defect.Down.dStdEv = vDown.GetStandardDeviation();

		//左上探索
		defect.UpLeft.dAnalysisQL[0] = defect.UpLeft.iQL;
		iSShift = 1;
		iMShift = 1;
		while (iNumOfUpLeftPix < iNumOfAnlsPix && (defect.UpLeft.iM - iMShift) > 0 && (defect.UpLeft.iS - iSShift) > 0){
			if (!bad.Dt[defect.UpLeft.iS - iSShift][defect.UpLeft.iM - iMShift]) {
				defect.UpLeft.dAnalysisQL[iNumOfUpLeftPix] = m_pTTMA->Dt[defect.UpLeft.iS - iSShift][defect.UpLeft.iM - iMShift];
				iNumOfUpLeftPix++;
			}
			iSShift++;
			iMShift++;
		}
		if (iNumOfUpLeftPix != iNumOfAnlsPix) {//所定の個数だけパターン解析用の画素数が見つからなかった場合
			defect.UpLeft.bAnalysisPossible = false;
			goto calc;
		}
		defect.UpLeft.bAnalysisPossible = true;
		defect.UpLeft.iNumOfAnlsPix = iNumOfUpLeftPix;
		vUpLeft.set(defect.UpLeft.dAnalysisQL, iNumOfAnlsPix);
		defect.UpLeft.dVariance = vUpLeft.GetVariance();
		defect.UpLeft.dStdEv = vUpLeft.GetStandardDeviation();

		//右上探索
		defect.UpRight.dAnalysisQL[0] = defect.UpRight.iQL;
		iSShift = 1;
		iMShift = 1;
		while (iNumOfUpRightPix < iNumOfAnlsPix && (defect.UpRight.iM + iMShift) < bad.MainPixel - 1 && (defect.UpRight.iS - iSShift) > 0){
			if (!bad.Dt[defect.UpRight.iS - iSShift][defect.UpRight.iM + iMShift]) {
				defect.UpRight.dAnalysisQL[iNumOfUpRightPix] = m_pTTMA->Dt[defect.UpRight.iS - iSShift][defect.UpRight.iM + iMShift];
				iNumOfUpRightPix++;
			}
			iSShift++;
			iMShift++;
		}
		if (iNumOfUpRightPix != iNumOfAnlsPix) {//所定の個数だけパターン解析用の画素数が見つからなかった場合
			defect.UpRight.bAnalysisPossible = false;
			goto calc;
		}
		defect.UpRight.bAnalysisPossible = true;
		defect.UpRight.iNumOfAnlsPix = iNumOfUpRightPix;
		vUpRight.set(defect.UpRight.dAnalysisQL, iNumOfAnlsPix);
		defect.UpRight.dVariance = vUpRight.GetVariance();
		defect.UpRight.dStdEv = vUpRight.GetStandardDeviation();

		//左下探索
		defect.DownLeft.dAnalysisQL[0] = defect.DownLeft.iQL;
		iSShift = 1;
		iMShift = 1;
		while (iNumOfDownLeftPix < iNumOfAnlsPix && (defect.DownLeft.iM - iMShift) > 0 && (defect.DownLeft.iS + iSShift) < bad.SubPixel - 1){
			if (!bad.Dt[defect.DownLeft.iS + iSShift][defect.DownLeft.iM - iMShift]) {
				defect.DownLeft.dAnalysisQL[iNumOfDownLeftPix] = m_pTTMA->Dt[defect.DownLeft.iS + iSShift][defect.DownLeft.iM - iMShift];
				iNumOfDownLeftPix++;
			}
			iSShift++;
			iMShift++;
		}
		if (iNumOfDownLeftPix != iNumOfAnlsPix) {//所定の個数だけパターン解析用の画素数が見つからなかった場合
			defect.DownLeft.bAnalysisPossible = false;
			goto calc;
		}
		defect.DownLeft.bAnalysisPossible = true;
		defect.DownLeft.iNumOfAnlsPix = iNumOfDownLeftPix;
		vDownLeft.set(defect.DownLeft.dAnalysisQL, iNumOfAnlsPix);
		defect.DownLeft.dVariance = vDownLeft.GetVariance();
		defect.DownLeft.dStdEv = vDownLeft.GetStandardDeviation();

		//右下探索
		defect.DownRight.dAnalysisQL[0] = defect.DownRight.iQL;
		iSShift = 1;
		iMShift = 1;
		while (iNumOfDownRightPix < iNumOfAnlsPix && (defect.DownRight.iM + iMShift) < bad.MainPixel - 1 && (defect.DownRight.iS + iSShift) < bad.SubPixel - 1){
			if (!bad.Dt[defect.DownRight.iS + iSShift][defect.DownRight.iM + iMShift]) {
				defect.DownRight.dAnalysisQL[iNumOfDownRightPix] = m_pTTMA->Dt[defect.DownRight.iS + iSShift][defect.DownRight.iM + iMShift];
				iNumOfDownRightPix++;
			}
			iSShift++;
			iMShift++;
		}
		if (iNumOfDownRightPix != iNumOfAnlsPix) {//所定の個数だけパターン解析用の画素数が見つからなかった場合
			defect.DownRight.bAnalysisPossible = false;
			goto calc;
		}
		defect.DownRight.bAnalysisPossible = true;
		defect.DownRight.iNumOfAnlsPix = iNumOfDownRightPix;
		vDownRight.set(defect.DownRight.dAnalysisQL, iNumOfAnlsPix);
		defect.DownRight.dVariance = vDownRight.GetVariance();
		defect.DownRight.dStdEv = vDownRight.GetStandardDeviation();
	}

	calc://label
	//上下左右斜めの8方向に対して、解析用の所定数の正常画素数が見つからなかった場合
	if (!bAnalysisPossible) {
		//上下左右の4方向の正常画素に対する補間係数を計算
		CalcNormAndWeight(iSCor, iMCor, defect.Left.bExist, defect.Right.bExist, defect.Up.bExist, defect.Down.bExist, defect.Left.iM, defect.Right.iM, defect.Up.iS, defect.Down.iS, fNorm, fWleft, fWright, fWup, fWdown);
		//補間値
		m_pTTMA->SetVal(iSCor, iMCor, fWup*defect.Up.iQL + fWdown*defect.Down.iQL + fWleft*defect.Left.iQL + fWright*defect.Right.iQL);
	}
	else{//8方向の解析可能な場合
		double dAveStdEvUp, dAveStdEvUpLeft, dAveStdEvLeft, dAveStdEvUpRight;//欠陥を横切る4方向の標準偏差の平均値
		defect.dStdEv[_Vertical] = (defect.Up.dStdEv + defect.Down.dStdEv) / 2;
		defect.dStdEv[_Horizontal] = (defect.Left.dStdEv + defect.Right.dStdEv) / 2;
		defect.dStdEv[_RightDown] = (defect.UpLeft.dStdEv + defect.DownRight.dStdEv) / 2;
		defect.dStdEv[_RightUp]= (defect.UpRight.dStdEv + defect.DownLeft.dStdEv) / 2;

		//max, minの計算
		double max = 0;
		double min = 65535;
		for (int iDir = 0; iDir < 4; iDir++) {
			if (max < defect.dStdEv[iDir]) {
				max = defect.dStdEv[iDir];
				defect.indexMax = iDir;
			}
		}
		for (int iDir = 0; iDir < 4; iDir++) {
			if (min > defect.dStdEv[iDir]) {
				min = defect.dStdEv[iDir];
				defect.indexMin = iDir;
			}
		}

		//maxとminの比
		defect.indexCorDir = -1;//初期値
		double dRate;
		if (max){
			dRate = min/max;
		}
		else{
			dRate = 1.;
        }
//		const double dThMaxMin = 0.4;//0.4以下であればminとなる方向の2画素のみで補間する
//		const double dThMaxMin = 0.3;//0.4以下であればminとなる方向の2画素のみで補間する
		double dThMaxMin = dThPatternExist;
		if (dRate < dThMaxMin) {
			defect.indexCorDir = defect.indexMin;
			switch (defect.indexCorDir) {
			case _Horizontal:
				if (!bDefectCorrectionByOnlyHor2Pixel(bad, iSCor, iMCor, iSearchInterval)){
					CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByOnlyHor2Pixel()", "Error", MB_OK);
					return false;
				}
				break;
			case _Vertical:
				if (!bDefectCorrectionByOnlyVer2Pixel(bad, iSCor, iMCor, iSearchInterval)){
					CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByOnlyVer2Pixel()", "Error", MB_OK);
					return false;
				}
				break;
			case _RightUp:
				if (!bDefectCorrectionByOnlyRightUp2Pixel(bad, iSCor, iMCor, iSearchInterval)){
					CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByOnlyRightUp2Pixel()", "Error", MB_OK);
					return false;
				}
				break;
			case _RightDown:
				if (!bDefectCorrectionByOnlyRightDown2Pixel(bad, iSCor, iMCor, iSearchInterval)){
					CApplication::GetInstance()->MessageBox("Error in bDefectCorrectionByOnlyRightDown2Pixel()", "Error", MB_OK);
					return false;
				}
				break;
			default:
				return false;
			}
		}
		else{//突出した方向がなければ上下左右の4方向からの補間
			//上下左右の4方向の正常画素に対する補間係数を計算
			CalcNormAndWeight(iSCor, iMCor, defect.Left.bExist, defect.Right.bExist, defect.Up.bExist, defect.Down.bExist, defect.Left.iM, defect.Right.iM, defect.Up.iS, defect.Down.iS, fNorm, fWleft, fWright, fWup, fWdown);
			//補間値
			m_pTTMA->SetVal(iSCor, iMCor, fWup*defect.Up.iQL + fWdown*defect.Down.iQL + fWleft*defect.Left.iQL + fWright*defect.Right.iQL);
        }
    }

	return true;
}



//---------------------------------------------------------------------------
/*
	欠陥に対して垂直方向のみの２画素で、かつ欠陥に隣接した画素のみから、その距離に応じて補間（縦グリッドと想定し、グリッドの向きのみから補間）
*/
bool CSimars::bDefectCorrectionByOnlyVer2Pixel(TTMA& bad, const int iSCor, const int iMCor, const int iSSearchInterval)
{
	int iQLup = 0;
	int iQLdown = 0;
	int iS1 = 0;
	int iS2 = 0;
	bool bUp = false;
	bool bDown = false;
	int iSShift = 0;

	//上探索
	if(iSCor - iSSearchInterval >= 0){
		iSShift = iSSearchInterval;
		while (0 != bad.Dt[iSCor - iSShift][iMCor] && (iSCor - iSShift) > 0){
			iSShift += iSSearchInterval;
		}
		iS1 = iSCor - iSShift;
		if (0 != bad.Dt[iS1][iMCor]) {//端まで欠陥画素がある場合
			bUp = false;
		}
		else if (iS1 >= 0 && iS1 < bad.SubPixel) {
			iQLup = m_pTTMA->Dt[iS1][iMCor];
			bUp = true;
		}
		else{
			bUp = false;
		}
	}

	//下探索
	if(iSCor + iSSearchInterval < bad.SubPixel - 1){
		iSShift = iSSearchInterval;
		while (0 != bad.Dt[iSCor + iSShift][iMCor] && (iSCor + iSShift) < bad.SubPixel - 1){
			iSShift += iSSearchInterval;
		}
		iS2 = iSCor + iSShift;
		if (0 != bad.Dt[iS2][iMCor]) {//端まで欠陥画素がある場合
			bDown = false;
		}
		else if (iS2 >= 0 && iS2 < bad.SubPixel) {
			iQLdown = m_pTTMA->Dt[iS2][iMCor];
			bDown = true;
		}
		else{
			bDown = false;
		}
	}


	//補正に用いる正常画素が上下ともに見つかった場合
	if (bUp && bDown) {
		if (iS2 - iS1) {
			m_pTTMA->Dt[iSCor][iMCor] = (int)((iQLup * (iS2 - iSCor) + iQLdown * (iSCor - iS1)) / (float)(iS2 - iS1));
		}
		else{
			m_pTTMA->Dt[iSCor][iMCor] = iQLup;
		}
	}
	//画像端などで上下の片方しか正常画素が見つからなかった場合
	else if (bUp) {
		m_pTTMA->Dt[iSCor][iMCor] = iQLup;
	}
	else if (bDown) {
		m_pTTMA->Dt[iSCor][iMCor] = iQLdown;
	}
	else{
		return false;
    }

	return true;
}
//---------------------------------------------------------------------------
/*
	欠陥に対して右上がり方向のみの２画素で、かつ欠陥に隣接した画素のみから、その距離に応じて補間
*/
bool CSimars::bDefectCorrectionByOnlyRightUp2Pixel(TTMA& bad, const int iSCor, const int iMCor, const int iSearchInterval)
{
	int iQLupright = 0;
	int iQLdownleft = 0;
	int iS1 = 0;
	int iS2 = 0;
	int iM1 = 0;
	int iM2 = 0;
	bool bUpRight = false;
	bool bDownLeft = false;
	int iSShift = 0;
	int iMShift = 0;

	//右上探索
	if(iMCor + iSearchInterval < bad.MainPixel - 1 && iSCor - iSearchInterval >= 0){
		iMShift = iSShift = iSearchInterval;
		while (0 != bad.Dt[iSCor - iSShift][iMCor + iMShift] && (iMCor + iMShift) < bad.MainPixel - 1 && (iSCor - iSShift) > 0){
			iMShift += iSearchInterval;
			iSShift += iSearchInterval;
		}
		iS1 = iSCor - iSShift;
		iM2 = iMCor + iMShift;
		if (0 != bad.Dt[iS1][iM2]) {//端まで欠陥画素がある場合
			bUpRight = false;
		}
		else if (iM2 >= 0 && iM2 < bad.MainPixel && iS1 >= 0 && iS1 < bad.SubPixel) {
			iQLupright = m_pTTMA->Dt[iS1][iM2];
			bUpRight = true;
		}
		else{
			bUpRight = false;
		}
	}

	//左下探索
	if(iMCor - iSearchInterval >= 0 && iSCor + iSearchInterval < bad.SubPixel - 1){
		iMShift = iSShift = iSearchInterval;
		while (0 != bad.Dt[iSCor + iSShift][iMCor - iMShift] && (iMCor - iMShift) > 0 && (iSCor + iSShift) < bad.SubPixel - 1){
			iMShift += iSearchInterval;
			iSShift += iSearchInterval;
		}
		iS2 = iSCor + iSShift;
		iM1 = iMCor - iMShift;
		if (0 != bad.Dt[iS2][iM1]) {//端まで欠陥画素がある場合
			bDownLeft = false;
		}
		else if (iM1 >= 0 && iM1 < bad.MainPixel && iS2 >= 0 && iS2 < bad.SubPixel) {
			iQLdownleft = m_pTTMA->Dt[iS2][iM1];
			bDownLeft = true;
		}
		else{
			bDownLeft = false;
		}
	}

	//補正に用いる正常画素が右上、左下ともに見つかった場合
	if (bUpRight && bDownLeft) {
		if (iS2 - iS1 && iM2 - iM1) {
			m_pTTMA->Dt[iSCor][iMCor] = (int)((iQLupright * (iS2 - iSCor) + iQLdownleft * (iSCor - iS1)) / (float)(iS2 - iS1));
//			Dt[iSCor][iMCor] = (int)((iQLupright * (iM2 - iMCor) + iQLdownleft * (iMCor - iM1)) / (float)(iM2 - iM1));//幾何学的関係から、SでもMでもいずれも同じ結果になるはず。
		}
		else{
			m_pTTMA->Dt[iSCor][iMCor] = iQLupright;
		}
	}
	//画像端などで右上、左下の片方しか正常画素が見つからなかった場合
	else if (bUpRight) {
		m_pTTMA->Dt[iSCor][iMCor] = iQLupright;
	}
	else if (bDownLeft) {
		m_pTTMA->Dt[iSCor][iMCor] = iQLdownleft;
	}
	else{
		return false;
    }

	return true;
}
//---------------------------------------------------------------------------
/*
	欠陥に対して右下がり方向のみの２画素で、かつ欠陥に隣接した画素のみから、その距離に応じて補間
*/
bool CSimars::bDefectCorrectionByOnlyRightDown2Pixel(TTMA& bad, const int iSCor, const int iMCor, const int iSearchInterval)
{
	int iQLupleft = 0;
	int iQLdownright = 0;
	int iS1 = 0;
	int iS2 = 0;
	int iM1 = 0;
	int iM2 = 0;
	bool bUpLeft = false;
	bool bDownRight = false;
	int iSShift = 0;
	int iMShift = 0;

	//左上探索
	if(iMCor - iSearchInterval >= 0 && iSCor - iSearchInterval >= 0){
		iMShift = iSShift = iSearchInterval;
		while (0 != bad.Dt[iSCor - iSShift][iMCor - iMShift] && (iMCor - iMShift) > 0 && (iSCor - iSShift) > 0){
			iMShift += iSearchInterval;
			iSShift += iSearchInterval;
		}
		iS1 = iSCor - iSShift;
		iM1 = iMCor - iMShift;
		if (0 != bad.Dt[iS1][iM1]) {//端まで欠陥画素がある場合
			bUpLeft = false;
		}
		else if (iM1 >= 0 && iM1 < bad.MainPixel && iS1 >= 0 && iS1 < bad.SubPixel) {
			iQLupleft = m_pTTMA->Dt[iS1][iM1];
			bUpLeft = true;
		}
		else{
			bUpLeft = false;
		}
	}


	//右下探索
	if(iMCor + iSearchInterval < bad.MainPixel - 1 && iSCor + iSearchInterval < bad.SubPixel - 1){
		iMShift = iSShift = iSearchInterval;
		while (0 != bad.Dt[iSCor + iSShift][iMCor + iMShift] && (iMCor + iMShift) < bad.MainPixel - 1 && (iSCor + iSShift) < bad.SubPixel - 1){
			iMShift += iSearchInterval;
			iSShift += iSearchInterval;
		}
		iS2 = iSCor + iSShift;
		iM2 = iMCor + iMShift;
		if (0 != bad.Dt[iS2][iM2]) {//端まで欠陥画素がある場合
			bDownRight = false;
		}
		else if (iM2 >= 0 && iM2 < bad.MainPixel && iS2 >= 0 && iS2 < bad.SubPixel) {
			iQLdownright = m_pTTMA->Dt[iS2][iM2];
			bDownRight = true;
		}
		else{
			bDownRight = false;
		}
	}


	//補正に用いる正常画素が左上、右下ともに見つかった場合
	if (bUpLeft && bDownRight) {
		if (iS2 - iS1 && iM2 - iM1) {
			m_pTTMA->Dt[iSCor][iMCor] = (int)((iQLupleft * (iS2 - iSCor) + iQLdownright * (iSCor - iS1)) / (float)(iS2 - iS1));
//			Dt[iSCor][iMCor] = (int)((iQLupleft * (iM2 - iMCor) + iQLdownright * (iMCor - iM1)) / (float)(iM2 - iM1));//幾何学的関係から、SでもMでもいずれも同じ結果になるはず。
		}
		else{
			m_pTTMA->Dt[iSCor][iMCor] = iQLupleft;
		}
	}
	//画像端などで左上、右下の片方しか正常画素が見つからなかった場合
	else if (bUpLeft) {
		m_pTTMA->Dt[iSCor][iMCor] = iQLupleft;
	}
	else if (bDownRight) {
		m_pTTMA->Dt[iSCor][iMCor] = iQLdownright;
	}
	else{
		return false;
    }

	return true;
}














//---------------------------------------------------------------------------
/*
	上下左右の4方向の正常画素に対する補間係数を計算
*/
void CSimars::CalcNormAndWeight(int iSCor, int iMCor, bool bLeft, bool bRight, bool bUp, bool bDown, int iM1, int iM2, int iS1, int iS2, float& fNorm, float& fWleft, float& fWright, float& fWup, float& fWdown)
{
	//規格化定数計算
	if (bUp) {
		if (0 == iSCor - iS1) {
			;
			int a = 0;
		}
		else{
			fNorm += (float)1 / (iSCor - iS1);
		}
	}
	if (bDown) {
		if (0 == iS2 - iSCor) {
			;
			int a = 0;
		}
		else{
			fNorm += (float)1 / (iS2 - iSCor);
        }
	}
	if (bLeft) {
		if (0 == iMCor - iM1) {
			;
			int a = 0;
		}
		else{
			fNorm += (float)1 / (iMCor - iM1);
		}
	}
	if (bRight) {
		if (0 == iM2 - iMCor) {
			;
			int a = 0;
		}
		else{
			fNorm += (float)1 / (iM2 - iMCor);
        }
	}

	//荷重計算
	if (bUp) {
		fWup    = 1 / fNorm / (iSCor - iS1);
	}
	if (bDown) {
		fWdown  = 1 / fNorm / (iS2 - iSCor);
	}
	if (bLeft) {
		fWleft  = 1 / fNorm / (iMCor - iM1);
	}
	if (bRight) {
		fWright = 1 / fNorm / (iM2 - iMCor);
	}

#if 1
	//Weightを浮動小数点ではなく、512階調にして画質評価（FPGA実装の観点）→官能評価では差異視認されず。
	const int iNorm = 512;
	int iWupMod = (int)(fWup * iNorm + 0.5);
	int iWdownMod = (int)(fWdown * iNorm + 0.5);
	int iWleftMod = (int)(fWleft * iNorm + 0.5);
	int iWrightMod = (int)(fWright * iNorm + 0.5);

	//階調を粗くして評価
	fWup = (float)iWupMod / iNorm;
	fWdown = (float)iWdownMod / iNorm;
	fWleft = (float)iWleftMod / iNorm;
	fWright = (float)iWrightMod / iNorm;
#endif
	return;
}


//---------------------------------------------------------------------------
/*
	欠陥の上下左右および斜め45度方向の８画素の周辺の正常画素を解析

*/
void CSimars::SearchNormal8PixelsArroundBadPixel(TTMA& bad, const int iSCor, const int iMCor, const int iSearchInterval, DEFECTARROUNDPIXELS& defect)
{
	int iM1, iM2, iS1, iS2;
	int iMShift = 0;
	int iSShift = 0;

	//初期化
    defect.Left.bExist = false;
	defect.Right.bExist = false;
	defect.Up.bExist = false;
	defect.Down.bExist = false;
	defect.UpLeft.bExist = false;
	defect.UpRight.bExist = false;
	defect.DownLeft.bExist = false;
    defect.DownRight.bExist = false;


	//左探索
	if(iMCor - iSearchInterval >= 0){
		iMShift = iSearchInterval;
		while (0 != bad.Dt[iSCor][iMCor - iMShift] && (iMCor - iMShift) > 0){
			iMShift += iSearchInterval;
		}
		iM1 = iMCor - iMShift;
		if (0 != bad.Dt[iSCor][iM1]) {//端まで欠陥画素がある場合
			defect.Left.bExist = false;
		}
		else if (iM1 >= 0 && iM1 < bad.MainPixel) {
			defect.Left.iQL = m_pTTMA->Dt[iSCor][iM1];
			defect.Left.iS  = iSCor;
			defect.Left.iM  = iM1;
			defect.Left.bExist = true;
		}
		else{
			defect.Left.bExist = false;
		}
	}

	//右探索
	if(iMCor + iSearchInterval < bad.MainPixel - 1){
		iMShift = iSearchInterval;
//		while (0 != bad.Dt[iSCor][iMCor + iMShift] && (iMCor + iMShift) < bad.MainPixel){
		while (0 != bad.Dt[iSCor][iMCor + iMShift] && (iMCor + iMShift) < bad.MainPixel - 1){
			iMShift += iSearchInterval;
		}
		iM2 = iMCor + iMShift;
		if (0 != bad.Dt[iSCor][iM2]) {//端まで欠陥画素がある場合
			defect.Right.bExist = false;
		}
		else if (iM2 >= 0 && iM2 < bad.MainPixel) {
			defect.Right.iQL = m_pTTMA->Dt[iSCor][iM2];
			defect.Right.iS  = iSCor;
			defect.Right.iM  = iM2;
			defect.Right.bExist = true;
		}
		else{
			defect.Right.bExist = false;
		}
	}

	//上探索
	if(iSCor - iSearchInterval >= 0){
		iSShift = iSearchInterval;
		while (0 != bad.Dt[iSCor - iSShift][iMCor] && (iSCor - iSShift) > 0){
			iSShift += iSearchInterval;
		}
		iS1 = iSCor - iSShift;
		if (0 != bad.Dt[iS1][iMCor]) {//端まで欠陥画素がある場合
			defect.Up.bExist = false;
		}
		else if (iS1 >= 0 && iS1 < bad.SubPixel) {
			defect.Up.iQL = m_pTTMA->Dt[iS1][iMCor];
			defect.Up.iS  = iS1;
			defect.Up.iM  = iMCor;
			defect.Up.bExist = true;
		}
		else{
			defect.Up.bExist = false;
		}
	}
	//下探索
	if(iSCor + iSearchInterval < bad.SubPixel - 1){
		iSShift = iSearchInterval;
//		while (0 != bad.Dt[iSCor + iSShift][iMCor] && (iSCor + iSShift) < bad.SubPixel){ // iSCor+iSShift = bad.SubPixelとなったときに実行エラー！
		while (0 != bad.Dt[iSCor + iSShift][iMCor] && (iSCor + iSShift) < bad.SubPixel - 1){
			iSShift += iSearchInterval;
		}
		iS2 = iSCor + iSShift;
		if (0 != bad.Dt[iS2][iMCor]) {//端まで欠陥画素がある場合
			defect.Down.bExist = false;
		}
		else if (iS2 >= 0 && iS2 < bad.SubPixel) {
			defect.Down.iQL = m_pTTMA->Dt[iS2][iMCor];
			defect.Down.iS  = iS2;
			defect.Down.iM  = iMCor;
			defect.Down.bExist = true;
		}
		else{
			defect.Down.bExist = false;
		}
	}

	//左上探索
	if(iMCor - iSearchInterval >= 0 && iSCor - iSearchInterval >= 0){
		iMShift = iSShift = iSearchInterval;
		while (0 != bad.Dt[iSCor - iSShift][iMCor - iMShift] && (iMCor - iMShift) > 0 && (iSCor - iSShift) > 0){
			iMShift += iSearchInterval;
			iSShift += iSearchInterval;
		}
		iS1 = iSCor - iSShift;
		iM1 = iMCor - iMShift;
		if (0 != bad.Dt[iS1][iM1]) {//端まで欠陥画素がある場合
			defect.UpLeft.bExist = false;
		}
		else if (iM1 >= 0 && iM1 < bad.MainPixel && iS1 >= 0 && iS1 < bad.SubPixel) {
			defect.UpLeft.iQL = m_pTTMA->Dt[iS1][iM1];
			defect.UpLeft.iS  = iS1;
			defect.UpLeft.iM  = iM1;
			defect.UpLeft.bExist = true;
		}
		else{
			defect.UpLeft.bExist = false;
		}
	}

	//右上探索
	if(iMCor + iSearchInterval < bad.MainPixel - 1 && iSCor - iSearchInterval >= 0){
		iMShift = iSShift = iSearchInterval;
		while (0 != bad.Dt[iSCor - iSShift][iMCor + iMShift] && (iMCor + iMShift) < bad.MainPixel - 1 && (iSCor - iSShift) > 0){
			iMShift += iSearchInterval;
			iSShift += iSearchInterval;
		}
		iS1 = iSCor - iSShift;
		iM2 = iMCor + iMShift;
		if (0 != bad.Dt[iS1][iM2]) {//端まで欠陥画素がある場合
			defect.UpRight.bExist = false;
		}
		else if (iM2 >= 0 && iM2 < bad.MainPixel && iS1 >= 0 && iS1 < bad.SubPixel) {
			defect.UpRight.iQL = m_pTTMA->Dt[iS1][iM2];
			defect.UpRight.iS  = iS1;
			defect.UpRight.iM  = iM2;
			defect.UpRight.bExist = true;
		}
		else{
			defect.UpRight.bExist = false;
		}
	}

	//左下探索
	if(iMCor - iSearchInterval >= 0 && iSCor + iSearchInterval < bad.SubPixel - 1){
		iMShift = iSShift = iSearchInterval;
		while (0 != bad.Dt[iSCor + iSShift][iMCor - iMShift] && (iMCor - iMShift) > 0 && (iSCor + iSShift) < bad.SubPixel - 1){
			iMShift += iSearchInterval;
			iSShift += iSearchInterval;
		}
		iS2 = iSCor + iSShift;
		iM1 = iMCor - iMShift;
		if (0 != bad.Dt[iS2][iM1]) {//端まで欠陥画素がある場合
			defect.DownLeft.bExist = false;
		}
		else if (iM1 >= 0 && iM1 < bad.MainPixel && iS2 >= 0 && iS2 < bad.SubPixel) {
			defect.DownLeft.iQL = m_pTTMA->Dt[iS2][iM1];
			defect.DownLeft.iS  = iS2;
			defect.DownLeft.iM  = iM1;
			defect.DownLeft.bExist = true;
		}
		else{
			defect.DownLeft.bExist = false;
		}
	}

	//右下探索
	if(iMCor + iSearchInterval < bad.MainPixel - 1 && iSCor + iSearchInterval < bad.SubPixel - 1){
		iMShift = iSShift = iSearchInterval;
		while (0 != bad.Dt[iSCor + iSShift][iMCor + iMShift] && (iMCor + iMShift) < bad.MainPixel - 1 && (iSCor + iSShift) < bad.SubPixel - 1){
			iMShift += iSearchInterval;
			iSShift += iSearchInterval;
		}
		iS2 = iSCor + iSShift;
		iM2 = iMCor + iMShift;
		if (0 != bad.Dt[iS2][iM2]) {//端まで欠陥画素がある場合
			defect.DownRight.bExist = false;
		}
		else if (iM2 >= 0 && iM2 < bad.MainPixel && iS2 >= 0 && iS2 < bad.SubPixel) {
			defect.DownRight.iQL = m_pTTMA->Dt[iS2][iM2];
			defect.DownRight.iS  = iS2;
			defect.DownRight.iM  = iM2;
			defect.DownRight.bExist = true;
		}
		else{
			defect.DownRight.bExist = false;
		}
	}

}


