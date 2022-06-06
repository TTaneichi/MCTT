/*
 * CSIMARSF.h
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#ifndef CSIMARSF_H_
#define CSIMARSF_H_

#include <vector>
#include <stdio.h>
#include <math.h>
#include <string>

#include "CTQUADRANGLE.h"
#include "TRECTANGLE.h"
#include "CTPENTAGON.h"
#include "CTDAT.h"
#include "CTTMA.h"
#include "CTFIMG.h"
#include "CVct.h"

using namespace std;

#define	Sisuu	(0)
#define	Beki	(1)

//欠陥を貫通する4方向のindex
#define _Horizontal	(0)
#define _Vertical	(1)
#define _RightUp	(2)
#define _RightDown	(3)


//カセッテDRのパネル種
#define	_PANEL_AXIS				(0)
#define	_PANEL_77G				(1)
#define	_PANEL_47C				(2)
#define	_PANEL_77C				(3)
#define	_PANEL_2430 			(4)
#define	_PANEL_HM				(5)
#define	_PANEL_DEVO2_1417_GOS 	(6)
#define	_PANEL_DEVO2_1417_CSI 	(7)
#define	_PANEL_DEVO2_1717_GOS   (8)
#define	_PANEL_DEVO2_1717_CSI 	(9)

//LongPanel段差補正の金尺目盛検出パターン
#define	_W2B				(0)
#define	_B2W				(1)
#define	_W2B2W				(2)
#define	_B2W2B				(3)

//DEVO3ラグキャリブ用
#define _INT_VER_TABLE//2DLUTの縦方向の数値はintで指定

//線欠陥補正のエッジ検出方法
#define	_USING_CTF_AREA		(0)
#define	_USING_SOBEL_FILTER	(1)


typedef struct _ROI {
	int 	x1;
	int 	y1;
	int 	x2;
	int		y2;
	double*	QLave;//NumOfFrame個確保
	double* Dose;//NumOfFrame個確保
	int		Pattern;//LongPanel段差補正の金尺目盛検出対応で追加(141111 ttsuji)
} ROI;

typedef struct _LAG {
//	int    	NumOfROI;
	int    	NumOfROI_X;//ｘ方向のROI数
	int    	NumOfROI_Y;//ｙ方向のROI数
	ROI*	X_ROI;//NumOfROI個確保
	ROI*	Pb_ROI;//NumOfROI個確保
	int 	NumOfFrame;
	double*	time;//NumOfFrame個確保
	double	QL_per_mR;//1mR当たりのQL
	double*	A;//exp(At+B) NumOfROI個確保
	double* B;//exp(At+B) NumOfROI個確保
	double* Coef2;//相関係数の二乗 NumOfROI個確保
	double  DeltaTime;//前読みと本読みの時間差
	double* ZanzouCoef;//残像補正係数  NumOfROI個確保
	int		Model;//ラグ減衰モデル（指数、冪）
} LAG;

typedef struct _LAGS{
	LAG* 	Lag;//ある温度、ある蓄積時間におけるLAG
	int		NumOfTemp;//実測した温度の数
	int		NumOfChargeTime;//実測した蓄積時間の数
	double* Temperature;//ラグを実測した温度の配列
	int*	ChargeTime;//ラグを実測した蓄積時間の配列
} LAGS;


typedef struct _DEFECTARROUNDPIXEL {
	bool bExist;//最近接の正常画素が存在するか
	int iM;//最近接の正常画素の主座標
	int iS;//最近接の正常画素の副座標
	int iQL;//最近接の正常画素のQL値
	bool bAnalysisPossible;//最近接の正常画素の集団が存在し、パターン解析可能か
	int iNumOfAnlsPix;//パターン解析のための周辺の正常画素の数（1～16）
	double dAnalysisQL[16];//パターン解析のための周辺の正常画素のQL配列
	double dVariance;//パターン解析のための分散
	double dStdEv;//パターン解析のための標準偏差
} DEFECTARROUNDPIXEL;


typedef struct _DEFECTARROUNDPIXELS {
	DEFECTARROUNDPIXEL Left;
	DEFECTARROUNDPIXEL Right;
	DEFECTARROUNDPIXEL Up;
	DEFECTARROUNDPIXEL Down;
	DEFECTARROUNDPIXEL UpLeft;
	DEFECTARROUNDPIXEL UpRight;
	DEFECTARROUNDPIXEL DownLeft;
	DEFECTARROUNDPIXEL DownRight;
	double dStdEv[4];//欠陥を貫通する4方向の標準偏差
	int indexMax;//標準偏差の最大値となる方向のインデックス
	int indexMin;//標準偏差の最小値となる方向のインデックス
	int indexCorDir;//欠陥を補正するのに用いる正常画素の方向のインデックス
} DEFECTARROUNDPIXELS;


typedef struct _DIFFERENTIALARROUNDPIXEL {
	double dDiff;//ある方向の微分値（ラプラシアン）
} DIFFERENTIALARROUNDPIXEL;


typedef struct _DIFFERENTIALARROUNDPIXELS {
	DIFFERENTIALARROUNDPIXEL Left2Right;
	DIFFERENTIALARROUNDPIXEL Up2Down;
	DIFFERENTIALARROUNDPIXEL UpLeft2DownRight;
	DIFFERENTIALARROUNDPIXEL UpRight2DownLeft;
	double dEnergy;//エネルギー
	double dVmX;//エッジの方向をあらわす仮想空間のベクトルx座標
	double dVmY;//エッジの方向をあらわす仮想空間のベクトルy座標
	double dVmNorm;//エッジの方向をあらわす仮想空間のベクトルのノルム
	double dConfidence;//線らしさの信頼性
	double dOrientation;//ベクトルの向き
	int indexMax;//微分値が最大となる方向のインデックス
	int indexMin;//微分値が最小となる方向のインデックス
} DIFFERENTIALARROUNDPIXELS;





typedef struct _DXA_PROFILE {
	int 	iWidth;
	int		iY1;//ROIの平均化y1座標
	int		iY2;//ROIの平均化y2座標
	double*	dProfCsI;//ＣｓＩのプロファイル（SubPixel個確保）
	double* dProfGOS;//ＧＯＳのプロファイル（SubPixel個確保）
	double* dProfSubtract;//差分ＱＬのプロファイル（SubPixel個確保）
	double* dProfBaseLine;//差分ＱＬのベースラインのプロファイル
	double* dProfBoneContrast;//差分画像の骨部コントラストＱＬのプロファイル
	double	dQLSoft_X0;//差分画像のX0における軟部QL
	double	dQLSoft_X1;//差分画像のX1における軟部QL
	double	dQLSoft_X;//差分画像の軟部QL
	double	dQLBoneContrast;//差分画像の骨部コントラストQL
	double	dCsIQLSoft_X0;//CsI画像のX0における軟部QL
	double	dCsIQLSoft_X1;//CsI画像のX1における軟部QL
	double	dCsIQLSoft_X;//CsI画像の軟部QL
} DXA_PROFILE;


typedef struct _DXA_OBLIQUE_ROI {
	TQUADRANGLE QuadBone; //椎体方向ベクトルの向きに応じた斜めの四角形
	TQUADRANGLE QuadSoftL;//椎体方向ベクトルの向きに応じた斜めの四角形
	TQUADRANGLE QuadSoftR;//椎体方向ベクトルの向きに応じた斜めの四角形
	double	dESQLSoftL;//差分画像のX0における軟部QL
	double	dESQLSoftR;//差分画像のX1における軟部QL
	double	dESQLSoft;//差分画像の軟部QL
	double	dESQLBone;//差分画像の骨部QL
	double	dESQLBoneContrast;//差分画像の骨部コントラストQL
	double	dCsIQLSoftL;//CsI画像のX0における軟部QL
	double	dCsIQLSoftR;//CsI画像のX1における軟部QL
	double	dCsIQLSoft;//CsI画像の軟部QL
} DXA_OBLIQUE_ROI;


typedef struct _DXA_INTEGRATED_SOFTROI_OBLIQUE_BONEROI {
	int 	iTopX;   //着目の椎体の上側の椎間
	int 	iTopY;   //着目の椎体の上側の椎間
	int 	iBottomX;//着目の椎体の下側の椎間
	int 	iBottomY;//着目の椎体の下側の椎間
	int 	iLeftX;  //着目の椎体の左側（x0側）の境界
	int 	iLeftY;  //着目の椎体の左側（x0側）の境界
	int 	iRightX; //着目の椎体の右側（xEnd側）の境界
	int 	iRightY; //着目の椎体の右側（xEnd側）の境界
	TQUADRANGLE QuadBone;//骨部ROI（斜めの長方形）
	TRECTANGLE RectSoftL;//L234の左右境界の最外点から所定距離外側に確保した鉛直水平のROI
	TRECTANGLE RectSoftR;//L234の左右境界の最外点から所定距離外側に確保した鉛直水平のROI
	double	dESQLSoftL;//差分画像のX0における軟部QL
	double	dESQLSoftR;//差分画像のX1における軟部QL
	double	dESQLSoft;//差分画像の軟部QL
	double	dESQLBone;//差分画像の骨部QL
	double	dESQLBoneContrast;//差分画像の骨部コントラストQL
	double	dCsIQLSoftL;//CsI画像のX0における軟部QL
	double	dCsIQLSoftR;//CsI画像のX1における軟部QL
	double	dCsIQLSoft;//CsI画像の軟部QL
	double	dCsIQLBone;//CsI画像の骨部QL
	double	dCsIQLBoneContrast;//CsI画像の骨部コントラストQL
	double	dGOSQLSoftL;//GOS画像のX0における軟部QL
	double	dGOSQLSoftR;//GOS画像のX1における軟部QL
	double	dGOSQLSoft;//GOS画像の軟部QL
	double	dGOSQLBone;//GOS画像の骨部QL
	double	dGOSQLBoneContrast;//GOS画像の骨部コントラストQL
	double	dQLSoft;//撮影画像の軟部QL（CsI画像またはES画像）
	double	dBMD;//BMD値
	double	dBMDCalib;//キャリブの基準BMD値の骨コントラストQL
} DXA_INTEGRATED_SOFTROI_OBLIQUE_BONEROI;


typedef struct _DXA_INTEGRATED_SOFTROI_BONEMAP {
	int 	iTopX;   //着目の椎体の上側の椎間
	int 	iTopY;   //着目の椎体の上側の椎間
	int 	iBottomX;//着目の椎体の下側の椎間
	int 	iBottomY;//着目の椎体の下側の椎間
	int 	iLeftX;  //着目の椎体の左側（x0側）の境界
	int 	iLeftY;  //着目の椎体の左側（x0側）の境界
	int 	iRightX; //着目の椎体の右側（xEnd側）の境界
	int 	iRightY; //着目の椎体の右側（xEnd側）の境界
	TTMA 	BoneMap;//2pix binning画像の1416x1418サイズの画像
	TQUADRANGLE QTopBottomLine;//着目椎体の上端下端の直線を規定する四角形
	Vct		vUnitTop;//椎体の上側の椎間法線単位ベクトル
	Vct		vUnitBottom;//椎体の下側の椎間法線単位ベクトル
	TRECTANGLE RectSoftL;//L234の左右境界の最外点から所定距離外側に確保した鉛直水平のROI
	TRECTANGLE RectSoftR;//L234の左右境界の最外点から所定距離外側に確保した鉛直水平のROI
	double	dESQLSoftL;//差分画像のX0における軟部QL
	double	dESQLSoftR;//差分画像のX1における軟部QL
	double	dESQLSoft;//差分画像の軟部QL
	double	dESQLBone;//差分画像の骨部QL
	double	dESQLBoneContrast;//差分画像の骨部コントラストQL
	double	dCsIQLSoftL;//CsI画像のX0における軟部QL
	double	dCsIQLSoftR;//CsI画像のX1における軟部QL
	double	dCsIQLSoft;//CsI画像の軟部QL
	double	dCsIQLBone;//CsI画像の骨部QL
	double	dCsIQLBoneContrast;//CsI画像の骨部コントラストQL
	double	dGOSQLSoftL;//GOS画像のX0における軟部QL
	double	dGOSQLSoftR;//GOS画像のX1における軟部QL
	double	dGOSQLSoft;//GOS画像の軟部QL
	double	dGOSQLBone;//GOS画像の骨部QL
	double	dGOSQLBoneContrast;//GOS画像の骨部コントラストQL
	double	dQLSoft;//撮影画像の軟部QL（CsI画像またはES画像）
	double	dBMD;//BMD値[g/cm^2]
	double	dBMDCalib;//キャリブの基準BMD値の骨コントラストQL
	int		iAreaBone;//骨部領域の面積[pix^2]
	double	dAreaBone;//骨部領域の面積[cm^2]
	int		iAreaSoftL;//軟部L領域の面積[pix^2]
	double	dAreaSoftL;//軟部L領域の面積[cm^2]
	int		iAreaSoftR;//軟部R領域の面積[pix^2]
	double	dAreaSoftR;//軟部R領域の面積[cm^2]
	double  dBMC;//BMC値[g]
	float   fMAS;//撮影mAs値
} DXA_INTEGRATED_SOFTROI_BONEMAP;


typedef struct _DXA_FEMUR_TOTAL {
	TTMA 	BoneMap;//4x4 binningの708x709サイズの画像
	TPENTAGON PTotal;//大腿トータルの軟部ROIを規定する五角形
	TQUADRANGLE QTotal;//大腿トータルの軟部ROIを規定する四角形
	bool 	bPentagon;//大腿トータルの軟部ROIが五角形であるかのフラグ
	TQUADRANGLE QNeck;//大腿トータルを規定するための大腿ネックROI
	TRECTANGLE RectGlobal;//グローバルROIの鉛直水平の長方形
	int		iXBoneTop;//骨領域の体側側反面の上端のX座標
	int		iYBoneTop;//骨領域の体側側反面の上端のY座標
	int		iXBodySide;//骨領域の最も体側側の点（通常大転子）の突起のX座標
	int		iYBodySide;//骨領域の最も体側側の点（通常大転子）の突起のY座標
	int		iXBodyCenter;//骨領域の最も体幹側の点のX座標
	int		iYBodyCenter;//骨領域の最も体幹側の点のY座標
	double	dESQLSoft;//差分画像の軟部QL
	double	dESQLBone;//差分画像の骨部QL
	double	dESQLBoneContrast;//差分画像の骨部コントラストQL
	double	dCsIQLSoft;//CsI画像の軟部QL
	double	dCsIQLBone;//CsI画像の骨部QL
	double	dCsIQLBoneContrast;//CsI画像の骨部コントラストQL
	double	dGOSQLSoft;//GOS画像の軟部QL
	double	dGOSQLBone;//GOS画像の骨部QL
	double	dGOSQLBoneContrast;//GOS画像の骨部コントラストQL
	double	dQLSoft;//撮影画像の軟部QL（CsI画像またはES画像）
	double	dBMD;//BMD値
	double	dBMDCalib;//キャリブの基準BMD値の骨コントラストQL
	int		iAreaBone;//骨部領域の面積[pix^2]
	double	dAreaBone;//骨部領域の面積[cm^2]
	int		iAreaSoft;//軟部領域の面積[pix^2]
	double	dAreaSoft;//軟部領域の面積[cm^2]
	double  dBMC;//BMC値[g]
	float   fMAS;//撮影mAs値

	// 下記はNeck用 Takeshi Kuwabara
	TPENTAGON PNeck;//大腿トータルを規定するための大腿ネックROIがglobal ROIで蹴られる場合
	bool    bNeckPentagon; // 大腿ネックのROIが四角形であるかのフラグ
	double	dNESQLSoft;//差分画像の軟部QL
	double	dNESQLBone;//差分画像の骨部QL
	double	dNESQLBoneContrast;//差分画像の骨部コントラストQL
	double	dNCsIQLSoft;//CsI画像の軟部QL
	double	dNCsIQLBone;//CsI画像の骨部QL
	double	dNCsIQLBoneContrast;//CsI画像の骨部コントラストQL
	double	dNGOSQLSoft;//GOS画像の軟部QL
	double	dNGOSQLBone;//GOS画像の骨部QL
	double	dNGOSQLBoneContrast;//GOS画像の骨部コントラストQL
	double	dNQLSoft;//撮影画像の軟部QL（CsI画像またはES画像）
	double	dNBMD;//BMD値
	double	dNBMDCalib;//キャリブの基準BMD値の骨コントラストQL
	int		iNAreaBone;//骨部領域の面積[pix^2]
	double	dNAreaBone;//骨部領域の面積[cm^2]
	int		iNAreaSoft;//軟部領域の面積[pix^2]
	double	dNAreaSoft;//軟部領域の面積[cm^2]
	double  dNBMC;//BMC値[g]

} DXA_FEMUR_TOTAL;


typedef struct _DXA_JIS {
	TRECTANGLE RectBone; //骨部の水平鉛直の長方形
	TRECTANGLE RectSoftL;//軟部Lの水平鉛直の長方形
	TRECTANGLE RectSoftR;//軟部Rの水平鉛直の長方形
	double	dESQLSoftL;//差分画像のLeftROIにおける軟部QL
	double	dESQLSoftR;//差分画像のRightROIにおける軟部QL
	double	dESQLSoft;//差分画像の軟部QL
	double	dESQLBone;//差分画像の骨部QL
	double	dESQLBoneContrast;//差分画像の骨部コントラストQL
	double	dCsIQLSoftL;//CsI画像のLeftROIにおける軟部QL
	double	dCsIQLSoftR;//CsI画像のRightROIにおける軟部QL
	double	dCsIQLSoft;//CsI画像の軟部QL
	double	dCsIQLBone;//CsI画像の骨部QL
	double	dCsIQLBoneContrast;//CsI画像の骨部コントラストQL
	double	dGOSQLSoftL;//GOS画像のLeftROIにおける軟部QL
	double	dGOSQLSoftR;//GOS画像のRightROIにおける軟部QL
	double	dGOSQLSoft;//GOS画像の軟部QL
	double	dGOSQLBone;//GOS画像の骨部QL
	double	dGOSQLBoneContrast;//GOS画像の骨部コントラストQL
	double	dQLSoft;//撮影画像の軟部QL（CsI画像またはES画像）
	double	dBMD;//BMD値
	double	dBMDCalib;//キャリブの基準BMD値の骨コントラストQL
	int		iArea;//骨部領域の面積[pix^2]
	double	dArea;//骨部領域の面積[cm^2]
	double  dBMC;//BMC値[g]
	float   fMAS;//撮影mAs値
} DXA_JIS;


typedef struct _DXA_CALIB_INFO {
	int 	iY1;   //着目の椎体の骨部、軟部Y
	int 	iY2;   //着目の椎体の骨部、軟部Y
	int 	iBoneX1;//着目の椎体の骨部
	int 	iBoneX2;//着目の椎体の骨部
	int 	iSoftLX1;  //着目の椎体の左側（x0側）の軟部
	int 	iSoftLX2;  //着目の椎体の左側（x0側）の軟部
	int 	iSoftRX1; //着目の椎体の右側（xEnd側）の軟部
	int 	iSoftRX2; //着目の椎体の右側（xEnd側）の軟部
	int		iNumOfUretanThickness;//ウレタンの厚み水準
	double	dMAS[5];//ウレタン撮影時のmAs値（10cm, 15cm, 20cm, 25cm, 30cm）
	double	dESQLSoftL[5];//差分画像のX0における軟部QL(mAsあたり)
	double	dESQLSoftR[5];//差分画像のX1における軟部QL(mAsあたり)
	double	dESQLSoft[5];//差分画像の軟部QL(mAsあたり)
	double	dESQLBone[5];//差分画像の骨部QL(mAsあたり)
	double	dESQLBoneContrast[5];//差分画像の骨部コントラストQL(mAsあたり)
	double	dCsIQLSoftL[5];//CsI画像のX0における軟部QL(mAsあたり)
	double	dCsIQLSoftR[5];//CsI画像のX1における軟部QL(mAsあたり)
	double	dCsIQLSoft[5];//CsI画像の軟部QL(mAsあたり)
	double	dCsIQLBone[5];//CsI画像の骨部QL(mAsあたり)
	double	dCsIQLBoneContrast[5];//CsI画像の骨部コントラストQL(mAsあたり)
	double	dGOSQLSoftL[5];//CsI画像のX0における軟部QL(mAsあたり)
	double	dGOSQLSoftR[5];//CsI画像のX1における軟部QL(mAsあたり)
	double	dGOSQLSoft[5];//CsI画像の軟部QL(mAsあたり)
	double	dGOSQLBone[5];//CsI画像の骨部QL(mAsあたり)
	double	dGOSQLBoneContrast[5];//GOS画像の骨部コントラストQL(mAsあたり)
} DXA_CALIB_INFO;


//XEBRAの直接線散乱線PSF計算対応
typedef struct _PSF_SEARCH_RESULT {
	int iBinSize;//PSFのビニングサイズ
	float fGradient;//PSF指数減衰モデルの減弱係数（縦軸ログQLの場合の傾き）
	float fIntercept;//PSF指数減衰モデルの縦軸ログQLとした場合の切片
	float fStength;//強度
//	float fSumOfRatio;//実測と計算値の比の評価領域における総和
	float fSumOfDeltaQL2;//実測と計算値の差分QLの評価領域における二乗和
//	float fSumDiffMin;//実測と計算値の差分の評価領域における総和の最小値  (宣言するとなぜかPOWオーバーフロー発生)
	TFIMG FImgPSFBin16;//16BinサイズのPSF
//	TFIMG FImgProfConvolution;//指数係数をふったPSFと直接線領域の畳込演算後画像の主プロファイルの配列(iM：プロファイルの各座標, iS: 各指数係数に対応)
} PSF_SEARCH_RESULT;


typedef struct _PSF_SEARCH_RESULTS {
	PSF_SEARCH_RESULT* PsfResult;//あるビニングサイズのPSFサーチ結果
	TFIMG FImgPSFBin16;//16Binサイズの合成PSF
	TFIMG* FImgProfConvolution;//指数係数をふったPSFと直接線領域の畳込演算後画像の主プロファイルの配列(iM：プロファイルの各座標, iS: 0のみ, 配列は各指数係数に対応)
	bool bExistConvolution;//畳込演算後プロファイルが存在しているかのフラグ
	bool bExistMeasuredData[3];//評価区間0,1,2の実測データが存在したかのフラグ
	float fStrength;//散乱線強度
	double dSumDiffMin;//実測と計算値の差分の評価領域における総和の最小値
} PSF_SEARCH_RESULTS;



//DEVO2'パネルのマップ方式熱ムラ補正対応
typedef struct _NETSUMURA_INFO {
	int iAccumTime;//蓄積時間[ms]
	double dReadT;//1ラインあたりの読み取り時間[ms]
	double dResetT;//1ラインあたりのリセット画像読み取り時間[ms]
	double dTotalReadT;//画像読み取り時間[ms]
	int iSubPixelOfReset;//Reset画像の副Pixel数（デフォルト709 = 2836/4）
	double* dAccCoefTable;//面内の蓄積時間比率
	int iResetYOfADAS;//Reset画像におけるADAS位置のy座標
	int iResetYOfFPGA;//Reset画像におけるFPGA位置のy座標
	int iResetYOfRef;//Reset画像におけるReference位置のy座標
	double dAccCoefAtADAS;//Reset画像におけるADAS位置の蓄積時間比率
	double dAccCoefAtFPGA;//Reset画像におけるFPGA位置の蓄積時間比率
	double dAccCoefAtRef;//Reset画像におけるReference位置の蓄積時間比率
	double dTauADAS;//ADAS位置のτ
	double dTauFPGA;//FPGA位置のτ
	double dAlpha;//α倍
	double dBeta;//β倍
	TRECTANGLE RectAdas;//ビニングなし画像（主1, 副1）のADAS領域の矩形ROI
	TRECTANGLE RectFpga;//ビニングなし画像（主1, 副1）のFPGA領域の矩形ROI
	TRECTANGLE RectRef; //ビニングなし画像（主1, 副1）のReference領域の矩形ROI
	TRECTANGLE RectAdasG;//G面画像（主1/16, 副1/4）のADAS領域の矩形ROI
	TRECTANGLE RectFpgaG;//G面画像（主1/16, 副1/4）のFPGA領域の矩形ROI
	TRECTANGLE RectRefG; //G面画像（主1/16, 副1/4）のReference領域の矩形ROI
	TRECTANGLE RectAdasFac;//工程の熱ムラマップ画像（主1, 副1/2）のADAS領域の矩形ROI
	TRECTANGLE RectFpgaFac;//工程の熱ムラマップ画像（主1, 副1/2）のFPGA領域の矩形ROI
	TRECTANGLE RectRefFac; //工程の熱ムラマップ画像（主1, 副1/2）のReference領域の矩形ROI

	TRECTANGLE RectAdasArray[3];   //ビニングなし画像（主1, 副1）のADAS領域の矩形ROI
	TRECTANGLE RectFpgaArray[3];   //ビニングなし画像（主1, 副1）のFPGA領域の矩形ROI
	TRECTANGLE RectRefArray[3];    //ビニングなし画像（主1, 副1）のReference領域の矩形ROI
	TRECTANGLE RectAdasGArray[3];  //G面画像（主1/16, 副1/4）のADAS領域の矩形ROI
	TRECTANGLE RectFpgaGArray[3];  //G面画像（主1/16, 副1/4）のFPGA領域の矩形ROI
	TRECTANGLE RectRefGArray[3];   //G面画像（主1/16, 副1/4）のReference領域の矩形ROI
	TRECTANGLE RectAdasFacArray[3];//工程の熱ムラマップ画像（主1, 副1/2）のADAS領域の矩形ROI
	TRECTANGLE RectFpgaFacArray[3];//工程の熱ムラマップ画像（主1, 副1/2）のFPGA領域の矩形ROI
	TRECTANGLE RectRefFacArray[3]; //工程の熱ムラマップ画像（主1, 副1/2）のReference領域の矩形ROI

	double dPercentileQLofRectAdas[3];//ADAS矩形ROI1,2,3それぞれの所定パーセンタイルのQL値
	double dPercentileQLofRectFpga[3];//FPGA矩形ROI1,2,3それぞれの所定パーセンタイルのQL値
	double dPercentileQLofRectRef[3]; //Reference矩形ROI1,2,3それぞれの所定パーセンタイルのQL値
	double dMaxQLinAdasFpgaRef[3]; //ADAS, FPGA, Reference矩形ROIのうちの最大QL値

	int iROINumofAdas;//ROI1,2,3のうち採用された番号（いずれのROIもラグが存在し、ROIが採用できない場合は0）
	int iROINumofFpga;//ROI1,2,3のうち採用された番号（いずれのROIもラグが存在し、ROIが採用できない場合は0）
	int iROINumofRef;//ROI1,2,3のうち採用された番号（いずれのROIもラグが存在し、ROIが採用できない場合は0）
	double dGLag;//前曝射のG面画像の各ROIのうちの最大のラグQL（ラグがないROIが存在すれば0)
	int iTimerLagCorOn[3];//ラグ補正ONタイマー
	int iTimerLagCorOff[3];//ラグ補正OFFタイマー

	int iTimerLagCorOnADAS[3]; //ADAS ROIのラグ補正ONタイマー
	int iTimerLagCorOffADAS[3];//ADAS ROIのラグ補正OFFタイマー
	int iTimerLagCorOnFpga[3]; //FPGA ROIのラグ補正ONタイマー
	int iTimerLagCorOffFpga[3];//FPGA ROIのラグ補正OFFタイマー
	int iTimerLagCorOnRef[3];  //Ref ROIのラグ補正ONタイマー
	int iTimerLagCorOffRef[3];//Ref ROIのラグ補正OFFタイマー

	double dMOGImgAveQLOfAdas;//オフセット補正、Median処理後のG面画像のADAS位置の平均QL(蓄積時間面内係数乗算前)
	double dMOGImgAveQLOfFpga;//オフセット補正、Median処理後のG面画像のFPGA位置の平均QL(蓄積時間面内係数乗算前)
	double dMOGImgAveQLOfRef; //オフセット補正、Median処理後のG面画像のReference位置の平均QL(蓄積時間面内係数乗算前)
	double dMOGLag1ImgAveQLOfAdas[3];//オフセット補正、Median処理後のG面Lag1画像のADAS位置の平均QL(蓄積時間面内係数乗算前)
	double dMOGLag1ImgAveQLOfFpga[3];//オフセット補正、Median処理後のG面Lag1画像のFPGA位置の平均QL(蓄積時間面内係数乗算前)
	double dMOGLag1ImgAveQLOfRef[3]; //オフセット補正、Median処理後のG面Lag1画像のReference位置の平均QL(蓄積時間面内係数乗算前)
	double dMOGLag2ImgAveQLOfAdas[3];//オフセット補正、Median処理後のG面Lag2画像のADAS位置の平均QL(蓄積時間面内係数乗算前)
	double dMOGLag2ImgAveQLOfFpga[3];//オフセット補正、Median処理後のG面Lag2画像のFPGA位置の平均QL(蓄積時間面内係数乗算前)
	double dMOGLag2ImgAveQLOfRef[3]; //オフセット補正、Median処理後のG面Lag2画像のReference位置の平均QL(蓄積時間面内係数乗算前)
	double dShimiImgAveQLOfAdas;//シミムラ補正の補正画像のADAS位置の平均QL(蓄積時間面内係数乗算前)
	double dShimiImgAveQLOfFpga;//シミムラ補正の補正画像のFPGA位置の平均QL(蓄積時間面内係数乗算前)
	double dShimiImgAveQLOfRef; //シミムラ補正の補正画像のReference位置の平均QL(蓄積時間面内係数乗算前)
	double dNetsumuraADAS;//ADAS位置の熱ムラ量
	double dNetsumuraFPGA;//FGPA位置の熱ムラ量
	double dNetsumura;//熱ムラ
	double dAmap_Adas;//工程のADAS熱ムラマップのADAS部分の熱ムラ量
	double dAmap_Fpga;//工程のADAS熱ムラマップのFPGA部分の熱ムラ量
	double dFmap_Adas;//工程のFPGA熱ムラマップのADAS部分の熱ムラ量
	double dFmap_Fpga;//工程のFPGA熱ムラマップのFPGA部分の熱ムラ量
	int iGeta;//ゲタQL


} NETSUMURA_INFO;



//DEVO3動画パネルのオフセット・ラグ分離補正対応
//typedef struct _IMG_ATTRIBUTE_INF {
class TIMG_ATTRIBUTE_INF
{
//private:
public:
	int 		iShotGoID;				//ShotGo ID
	int 		iMode;					//撮影mode(0:静止画、1:2x2（7.5fps）、2:2x2（15fps）、3:1x1（7.5fps）)
	int 		iFr;					//mode内におけるfrナンバー
	int 		iStatus;				//内部状態
	int 		iElapsedTimeSinceSWOFF;	//SW_OFFからの経過時間[ms]
	int 		iXrayOnTime;			//連続照射時間[ms]
	string	StrFileName;			//ファイル名
//    TDAT*		pImg;					//画像の画素値情報へのポインタ ←　構造体のデータサイズが膨大となるため削除
	bool		bCor;					//被補正画像かどうかのフラグ
	bool		bFirstFrOfMode;			//mode切り替わりの最初のfrかどうかのフラグ
	int			iElapsedTimeOfLagAveFinalFr;//Lag平均化の最後のfrのSW-OFF経過時間

	TIMG_ATTRIBUTE_INF();
	~TIMG_ATTRIBUTE_INF();
	void InitializeImgAttributeInf();

//} IMG_ATTRIBUTE_INF;//画像ごとの属性情報の構造体
};

/*
	画像ID4 2x2 7.5fps
　　画像ID5 2x2 15fps
　　画像ID6 1x1 7.5fps
*/

//typedef struct _IMG_ATTRIBUTE_INFS {
class TIMG_ATTRIBUTE_INFS
{
//private:
public:
	TIMG_ATTRIBUTE_INF* ImgAttInf[4];//modeKの画像の属性情報の配列へのポインタ（SW_OFFからの時間に応じた配列）(K=0,1,2,3)
	int iNumOfArray[4];//modeKの配列数(K=0,1,2,3)
	int iModeSorted[4];//取得しているmodeの順番
	int iNumOfMode;//実際に取得しているmode数
	int iModeCorrected[3];//補正対象のmode(mode1,2,3のどれか)
	int iNumOfModeCorrected;//補正対象のmode数
	int iCurrentShotGoID;//現在のShotGoID（Lag1,Lag2平均化処理用）
	int iNumOfLagT2Array[4];//LagT2のmodeごとの配列数
	int iNumOfModeCycle[4];//modeの塊が何サイクル存在するか(iNumOfLagT2Array-1となるのが通常)
	int iElapsedTimeOfFinalFr[4];//modeごとの最終frのSW-OFF経過時間
	bool bReachFinalFr[4];//LagT2の平均化画像作成処理において、modeごとの最終frまで到達したかのフラグ
	bool IsReachedFinalFr[4];//LagT2の平均化画像作成処理において、modeごとの最終frまで到達したかのフラグ
	int iFixedLagT2ID_CorO2[4];//パターン２の補正の際のmodeごとのLagT2ID（分離補正OFFタイマー直前とする）

	TIMG_ATTRIBUTE_INFS();
	~TIMG_ATTRIBUTE_INFS();
	void InitializeImgAttributeInfs();//初期化
	void DeleteImgAttributeInfs();

//} IMG_ATTRIBUTE_INFS;
};




class TLagCalib_D3
{
private:
//public:
	int    		m_iNumOfROI;//ROI数
	TROI_Lag*	m_ROI_X;//NumOfROI個確保
//	TROI_Lag*	ROI_Pb;//NumOfROI個確保
	int 		m_iNumOfFrame;
	double*		m_dTime;//NumOfFrame個確保
	double*		m_dA;//Lag=B*t^A  NumOfROI個確保
	double* 	m_dB;//Lag=B*t^A  NumOfROI個確保(ラグキャリブでは不要)
	double* 	m_dCoef2;//相関係数の二乗 NumOfROI個確保(ラグキャリブでは不要)
	int			m_iModel;//ラグ減衰モデル（べき乗）
	double		m_dAave;//AのROI平均

	//2DLUT関連
	double		m_dMagicPara1;//マジックパラメータ1(=0.25)
	int			m_iNumOfVerTable;//2DLUTの縦方向テーブル数(=2048)
	int			m_iNumOfHorTable;//2DLUTの横方向テーブル数(=512)
	int*		m_iHorTable;//2DLUTの横方向テーブル（連続照射時間に関するテーブル）
#ifdef _INT_VER_TABLE
	int*		m_iVerTable;//2DLUTの縦方向テーブル（ΔT/Tに関するテーブルを256で規格化)
#else
	double*		m_dVerTable;//2DLUTの縦方向テーブル（ΔT/Tに関するテーブル)
#endif
	double*		m_d2DLUT;//double型の2DLUT（横方向のループが優先で、次が縦のループ）
	int*		m_i2DLUT;//int型の2DLUT（横方向のループが優先で、次が縦のループ）

public:
	TLagCalib_D3();
	~TLagCalib_D3();
	void SetModel(int iModel)	{m_iModel = iModel;}
	void SetTime(int iFr, int iTime)	{m_dTime[iFr] = iTime;}
	void SetAave(double dAave)	{m_dAave = dAave;}
	void AllocateROILag(int iNumOfRoi);
	void AllocateNumOfFrame(int iNumOfFrame);
	int GetNumOfROI()	const {return m_iNumOfROI;}
	int GetNumOfFrame()	const {return m_iNumOfFrame;}
	TROI_Lag&	GetRoi(int iROI) 		{return m_ROI_X[iROI];}
	TROI_Lag	GetRoi(int iROI) const 	{return m_ROI_X[iROI];}//TLAG_D3オブジェクトがconstであった場合でもTROI_Lagにアクセスできるようにする
	double* GetTime()	const {return m_dTime;}
	double* GetA()		const {return m_dA;}
	double* GetB()		const {return m_dB;}
	double* GetCoef2()	const {return m_dCoef2;}
	int GetModel()		const {return m_iModel;}
	double GetAave() 	const {return m_dAave;}
	bool CalcLagAttenuation();

	//2DLUT関連
	void	AllocateNumOfVerTable(int iNumOfVerTable);
	void	AllocateNumOfHorTable(int iNumOfHorTable);
	void	AllocateD2DLUT(int iNumOfHorTable, int iNumOfVerTable);
	void	AllocateI2DLUT(int iNumOfHorTable, int iNumOfVerTable);
#ifdef _INT_VER_TABLE
	void	SetVerTable(int iRow, int iValue)	{m_iVerTable[iRow] = iValue;}
#else
	void	SetVerTable(int iRow, double dValue)	{m_dVerTable[iRow] = dValue;}
#endif
	void	SetHorTable(int iColumn, int iValue)	{m_iHorTable[iColumn] = iValue;}
	void	SetMagicPara1(double dMagicPara1)		{m_dMagicPara1 = dMagicPara1;}
	int		GetNumOfVerTable()	const {return m_iNumOfVerTable;}
	int		GetNumOfHorTable()	const {return m_iNumOfHorTable;}
	int		GetMagicPara1()	  	const {return m_dMagicPara1;}
	void	CalcD2DLUT();
	void	CalcI2DLUT();
	double	GetD2DLUT(int iColumn, int iRow) const;
	int		GetI2DLUT(int iColumn, int iRow) const;
	int		GetI2DLUT(int iXrayOnTime, int iT1, int iT, int& iColumn, int& iRow) const;
	void	SetD2DLUT(int iColumn, int iRow, double dLUT)	{m_d2DLUT[iRow * m_iNumOfHorTable + iColumn] = dLUT;}
	void	SetI2DLUT(int iColumn, int iRow, int iLUT)		{m_i2DLUT[iRow * m_iNumOfHorTable + iColumn] = iLUT;}
#ifdef _INT_VER_TABLE
	int		GetIVerTable(int iRow) const	{return m_iVerTable[iRow];}
#else
	double	GetDVerTable(int iRow) const	{return m_dVerTable[iRow];}
#endif
};


class LPEDGEINF {
public:
	double a;				//段差を二次関数ax2+bx+cで最小自乗近似したときのa
	double b;				//段差を二次関数ax2+bx+cで最小自乗近似したときのb
	double c;				//段差を二次関数ax2+bx+cで最小自乗近似したときのc
	double ystart;			//x=0におけるｙ（=c）
	double yend;			//x=2831におけるｙ
	double ymax;			//yのMaximum
	double ymin;			//yのMinimum
	double x_atmax;			//yがMaximumとなるx
	double x_atmin;			//yがMinimumとなるx
	int    NumOfY10;		//y10の要素数
	double* y10;			//10倍精度の段差位置ｙ座標の配列
	int MainPixel;			//追加なので、このパラメータを設定する処理を追加する必要がある。

	LPEDGEINF(){
		a = 0;
		b = 0;
		c = 0;
		ystart = 0;
		yend = 0;
		ymax = 0;
		ymin = 0;
		x_atmax = 0;
		x_atmin = 0;
		NumOfY10 = 0;
		y10 = NULL;
		MainPixel = 0;
	}

	void SetLPEdgeInf(vector<double> vec, double& dY0, double& dYend, double& dMax, double& dMin, int& x_atmax, int& x_atmin){
		//	double dMax, dMin;
		//	int x_atmax, x_atmin;
			this->a = vec[2];
			this->b = vec[1];
			this->c = vec[0];
			this->ystart = dY0 = this->c;
			this->yend = dYend = this->a * (MainPixel - 1) * (MainPixel - 1) + this->b * (MainPixel - 1) + this->c;
			this->NumOfY10 = MainPixel;
			this->y10 = new double[this->NumOfY10];
			dMax = -100000.;
			dMin = 100000.;
			x_atmax = x_atmin = -1;
			for (int iM = 0; iM < this->NumOfY10; iM++) {
				if (iM==this->NumOfY10-1) {
					int c = 0;
				}

				this->y10[iM] = this->a * iM * iM + this->b * iM + this->c;
				if (dMax < this->y10[iM]) {
					dMax = this->y10[iM];
					x_atmax = iM;
				}
				if (dMin > this->y10[iM]) {
					dMin = this->y10[iM];
					   x_atmin = iM;
				}
			}
			this->ymax = dMax;
			this->ymin = dMin;
			this->x_atmax = x_atmax;
			this->x_atmin = x_atmin;
	}
	void SetLPEdgeInf(double dA, double dB, double dC, double& dY0, double& dYend, double& dMax, double& dMin, int& x_atmax, int& x_atmin){
		//double dVec[] = {dC, dB, dA};
		//Vct vector(dVec, 3);
		vector<double> vec;
		vec.push_back(dC);
		vec.push_back(dB);
		vec.push_back(dC);
		SetLPEdgeInf(vec, dY0, dYend, dMax, dMin, x_atmax, x_atmin);
	}
	void InitializeLPEdgeInf()
	{
		this->a = 0.;
		this->b = 0.;
		this->c = 0.;
		this->ystart = 0.;
		this->yend = 0.;
		this->ymax = 0.;
		this->ymin = 0.;
		this->x_atmax = 0.;
		this->x_atmin = 0.;
		this->NumOfY10 = 0;
		this->y10 = NULL;
	}

};



class CSimars {
private:
	TTMA	*m_pTTMA;


public:
	CSimars();

	CSimars(TTMA *pTTMA);

	~CSimars();

	//TTMAオブジェクトの設定
	void SetTTMA(TTMA *pTTMA);

	//ゲイン補正（クリップ処理、乱数加算）
	bool bGainCorrectionSPC(TTMA& ShadingImg, int iCoeSense, int iShosuKirisute);
	bool bGainCorrectionAXIS(TTMA& ShadingImg, int iCoeSense, int iPanel_Kind);
	bool bGainCorrection2430(TTMA& ShadingImg, int iCoeSense, int iPanel_Kind, int iClipQL);
	bool bForwardGainCorrectionLP(TTMA& ShadingImg, int iCoeSense, int iClipQL, bool bExeForwardSensCor, double dKando32768, int iNormBitSense);

	//========欠陥補正==========
	typedef struct _SLineDefect{	//線欠陥構造体
		int type;				//欠陥種別(0:無視/1:縦/2:横)
		int lineNum;			//欠陥座標(何ライン目か)
		int width;				//欠陥太さ
	} SLineDefect;


	bool bDefectCorrection6(string str_img, string str_coord, bool bAutoDetect, int& iGridDirection, float& fHonsuu, double& dA1, double& dA2, double& dA3, double& dA4, double dTh);  //欠陥補正6 (ttsuji 120807)
	bool bDefectCorrection6a(vector<SLineDefect> lineDefect, bool bAutoDetect, int& iGridDirection, float& fHonsuu, double& dA1, double& dA2, double& dA3, double& dA4, double dTh, bool bPreCor2Line);  //欠陥補正6a (ttsuji 170619)

	//点欠陥補正処理
	bool bDefectCorrection7(TTMA& bad, const int iGridDirection, const bool bPatternAnalysis, const int iPixelNumPatternAnalysis, const double dThPatternExist, const bool bModifyWeight);//大小欠陥をまとめて１つのアルゴで処理する新規アルゴ（グリッドなしのときには欠陥周囲のパターン解析を含む）
	bool bDefectCorrectionByOnlyHor2Pixel(TTMA& bad, const int iSCor, const int iMCor, const int iMSearchInterval);//欠陥に対して水平方向のみの２画素で、かつ欠陥に隣接した画素のみから、その距離に応じて補間（横グリッドと想定し、グリッドの向きのみから補間）
	bool bDefectCorrectionByHorVer4Pixel(TTMA& bad, const int iSCor, const int iMCor, const int iSearchInterval, const bool bModifyWeight);//欠陥に対して上下左右方向のみの４画素で、かつ欠陥に隣接した画素のみから、その距離に応じて補間
	void SearchNormal4PixelsArroundBadPixel(TTMA& bad, const int iSCor, const int iMCor, const int iSearchInterval, bool& bLeft, bool& bRight, bool& bUp, bool& bDown, int& iM1, int& iM2, int& iS1, int& iS2, int& iQLleft, int& iQLright, int& iQLup, int& iQLdown);
	bool bDefectCorrectionWithPatternAnalysis(TTMA& bad, const int iSCor, const int iMCor, const int iSearchInterval, const int iPixelNumPatternAnalysis, const double dThPatternExist);//欠陥に対して上下左右方向のみの４画素の周辺の正常画素を解析して、パターンを認識して補正に用いる画素を選択する


	void CalcNormAndWeight(int iSCor, int iMCor, bool bLeft, bool bRight, bool bUp, bool bDown, int iM1, int iM2, int iS1, int iS2, float& fNorm, float& fWleft, float& fWright, float& fWup, float& fWdown);//上下左右の4方向の正常画素に対する補間係数を計算
	void SearchNormal8PixelsArroundBadPixel(TTMA& bad, const int iSCor, const int iMCor, const int iSearchInterval, DEFECTARROUNDPIXELS& defect);
	bool bDefectCorrectionByOnlyVer2Pixel(TTMA& bad, const int iSCor, const int iMCor, const int iSSearchInterval);//欠陥に対して垂直方向のみの２画素で、かつ欠陥に隣接した画素のみから、その距離に応じて補間（縦グリッドと想定し、グリッドの向きのみから補間）
	bool bDefectCorrectionByOnlyRightUp2Pixel(TTMA& bad, const int iSCor, const int iMCor, const int iSearchInterval);//欠陥に対して右上がり方向のみの２画素で、かつ欠陥に隣接した画素のみから、その距離に応じて補間
	bool bDefectCorrectionByOnlyRightDown2Pixel(TTMA& bad, const int iSCor, const int iMCor, const int iSearchInterval);//欠陥に対して右下がり方向のみの２画素で、かつ欠陥に隣接した画素のみから、その距離に応じて補間
	bool bDefectCorrectionByMultiResoPixel(TTMA& bad, const int iSCor, const int iMCor, const int iSearchMode);//欠陥に対して水平方向のみの２画素で、かつ欠陥に隣接した画素のみから、その距離に応じて補間（横グリッドと想定し、グリッドの向きのみから補間）


};


#if 0
//---------------------------------------------------------------------------
class TSIMARS : public TTMA
{
private:

//temporary
	//DLLのロードのための変数宣言
//	HINSTANCE hDll;
//
//	//DLL呼び出し関数の宣言(Unlha32.dllのUnlha関数を呼び出す)
//	typedef int WINAPI (*ProcUnlha)(const HWND _hwnd,
//									LPCSTR _szCmdLine,
//									LPSTR _szOutput,
//									const DWORD _dwSize);
//	//関数ポインタ
//	ProcUnlha Unlha;

public:
    void CorMainBadLineBothSide(int iM, int iS0, int iS1);
    void CorMainBadLineDecSide(int iM, int iS0, int iS1);
    void CorMainBadLineIncSide(int iM, int iS0, int iS1);
    void CorStep();
    void CorStep(const int iMid);
	void CorStep(const int iMm, const int iMp);
    void CorStep(const int iMm2, const int iMm1, const int iMp1, const int iMp2, const bool bDoDiagonal);
    void CorStepASIC(const int iM0, const int iM1, vector<float>& r, TSINGLE_DATA& step, const int iMPos, TFIMG& cor);
    void CorSubBadLineBothSide(int iS, int iM0, int iM1);
    void CorSubBadLineDecSide(int iS, int iM0, int iM1);
    void CorSubBadLineIncSide(int iS, int iM0, int iM1);
    void CorSubBad2Line(int iS, int iM0, int iM1);
//	bool HorUmbMainRedAmp4(const String SubHighCutFilter,
//			const String MainLowPassFilter,
//			const String SubLowPassFilter,
//			const float fClip,
//			const int iMAIN_DIV,
//			const bool bDebugYokokasa,
//			const String strImage);
	void CorStepASIC2(const int iMm, const int iMp,const int iMEDIAN,const int iTh,vector<unsigned short> a01, vector<unsigned short> a10, vector<unsigned short> a11);
	bool CorStepASIC2_DEBUG(const int iOFFSET, const int iASIC, const int iMm, const int iMp, const int iMEDIAN, const int iMOVEAVE, const float fTh, const int flag_Table, const int flag_DefStep, const int flag_LowPassFilter);
	void CorStepASIC(const int iM0, const int iM1, vector<unsigned short>& r, TSINGLE_DATA& step, const int iMPos, TFIMG& cor, const float fUPPER);
//	void CorStepASIC_DEBUG(const int iM0, const int iM1, vector<unsigned short>& r, TSINGLE_DATA& step, const int iMPos, TFIMG& cor, const float fUPPER);
	void CorStepASIC_DEBUG(const int iM0, const int iM1, vector<unsigned short>& r, TSINGLE_DATA& step, const TSINGLE_DATA& Dt, TFIMG& cor, const float fUPPER);
	bool CorStepTFT2(const int iMid,const int iSm,const int iSp,const int iMEDIAN,const float iTh,vector<unsigned short> r01, vector<unsigned short> r10);
	bool CorStepTFTforFluoro(const int iMid,const int iSm,const int iSp,const int iMEDIAN,const float iTh,vector<unsigned short> r01, vector<unsigned short> r10, bool bflagMDivStepProf, bool bflagStepThTraceBack, bool bflagStepThRatio, string in_str);//透視向けTFT段差補正検討用（主平均縮小後に主Median）(151007 ttsuji)
	bool CorStepTFT2_DEBUG(const int iMid, const int iSm, const int iSp, const int iMEDIAN, const int iMOVEAVE, const float fTh, const int flag_Table, const int flag_DefStep, const int flag_LowPassFilter);
	void CorStepTFT(const int iS0, const int iS1, vector<unsigned short>& r, TSINGLE_DATA& step, const int iSPos, TFIMG& cor, const float fUPPER);
	void CorStepTFT_ModifyThAlgo(const int iS0, const int iS1, vector<unsigned short>& r, TSINGLE_DATA& step, const int iSPos, TFIMG& cor, const float fUPPER);//BENEOのPD後に改良したアルゴ（製品搭載はされていない）
	void CorStepTFT_ModifyThAlgo2(const int iS0, const int iS1, vector<unsigned short>& r, TSINGLE_DATA& step, const int iSPos, TFIMG& cor, const float fUPPER);//透視用の段差補正（段差閾値を越えた場合のアルゴ改良）(160224 ttsuji)
	void CorStepTFT_ModifyThAlgo3(const int iS0, const int iS1, vector<unsigned short>& r, TSINGLE_DATA& step, TSINGLE_DATA& StepRatio, const int iSPos, TFIMG& cor, const float fThRatio);//透視用の段差補正（段差閾値を越えた場合のアルゴ改良（段差閾値は段差のQL比で規定））(160225 ttsuji)
	float Calc_fCoe(unsigned short int usDt, double dDt);
	float Calc_fCoe2(double dDtBorder, double dDt);
	float Calc_fCor(unsigned short int usDtCnt, unsigned short int usDtPos, double dStep, float fCoe);
	float Calc_fCor2(unsigned short int usDtCnt, double dDtPos, double dStep, float fCoe);

	bool bSuperRedUMB2_FPD1(bool bKasa, const string KasaMainFilter, const string KasaSubFilter,
			const string KasaClipFile, const int LogMidVal, const int LogKetaVal, const bool bDebugKasa, const string strImage);//FPD1向けムラ除去コンポ（16bitLOG→16bitLinear出力）フローに合わせた縦傘処理関数
	bool bSuperRedUMB2_77G(bool bKasa, const string KasaMainFilter, const string KasaSubFilter,
			const string KasaClipFile, const int LogMidVal, const int LogKetaVal, const bool bDebugKasa, const string strImage);//77G向けムラ除去コンポ（16bitLOG→16bitLinear出力）フローに合わせた縦傘処理関数
	bool bFPD1MuraJokyoCompo(bool bKasa, bool bYokokasa,
			const string KasaMainFilter,
			const string KasaSubFilter,
			const string KasaClipFile,
			const string YokokasaMainFilter,
			const string YokokasaSubFilter,
			const string YokokasaHighCutFilter,
			const double YokokasaClipVal,
			const int YokokasaMainDiv,
			const int LogMidVal,
			const int LogKetaVal,
			const bool bDebugKasa,
			const bool bDebugYokokasa,
			const string strImage);//FPD1ムラ除去コンポ（16bitLOG→16bitLinear出力）フローに合わせた縦横傘処理関数
//    void To100um();

	bool bRemVerLine(const int iMMedian, const string SubLowPassFilter, const int iClip, const int iBIN, const bool bOut);
//	bool bRemHorLine(const int iSMedian, const String MainLowPassFilter, const int iClip, const int iBIN, const bool bOut);
//	bool bRemHorLine(const int iSMedian, const String MainLowPassFilter, const int iClip, const int iBIN, const bool bOut, const bool bMainExpandMultiAve, const int iMainMedian);
	bool bRemHorLine(const int iSMedian, const string MainLowPassFilter, const int iClip, const int iBIN, const bool bOut, const string in_str, const bool bMainExpandMultiAve, const int iMainMedian, const bool bAntiClip);
	bool bExtractHorLine(const string in_str, const int in_iSMedian, const int in_iKasaageQL, const int in_iMMoveAve, const int in_iQL_Threshold, const bool in_bImageOutput, int& out_Cnt);//全幅横スジ抽出

	bool ReadImage17(const string fileName);//ttsuji追加
	bool Image17Sort();//ttsuji追加
	bool bChangeOrder(string fileName);//ttsuji追加（TMARSクラスのものを流用）
	bool ImagerExpansionForSIMARS(const string str);//ttsuji追加（HGHのイメージャ拡大DLLを使用）
	void CalcStepDt_ASIC(int flag_DefStep, int iM, int iMm, int iMp, TSINGLE_DATA& Step);//ttsuji追加
	void CalcStepDt_ASIC(int flag_DefStep, int iM, int iMm, int iMp, TSINGLE_DATA& dMm, TSINGLE_DATA& dMp, TSINGLE_DATA& Step);//ttsuji追加
	double CalcStepDt_TFT(int flag_DefStep, int iM, int iS, int iSm, int iSp);//ttsuji追加
	void LowPassFilterForStep(int flag_LowPassFilter, int iMEDIAN, int iMOVEAVE, TSINGLE_DATA& Step);

	static bool ZanzouHoseiTEST();
	static bool ItplTEST();
	static bool CalcTau(LAG& lag, int iSampleStart, int iSampleEnd);
	bool CalcTauImage(LAG& lag);

	bool Raw2Fcr(string str);//2880×2880,10bit以外は未サポート(ttsuji追加)

	//オフセット自動更新エラー判定
	bool CalcShockJudge(int iUpper_thre , int iLower_thre, int iNumThre, int& iUpper_count, int& iLower_count, int* iQLUpper, int* iXUpper, int* iYUpper, int* iQLLower, int* iXLower, int* iYLower, int& iQLmin, int& iQLmax, bool bExcludeEndPixel);//オフセット自動更新エラー判定における衝撃判定
	bool CalcScatteredXray(int iUpper_thre , int iLower_thre, int iROI_Length, double* dQLscatter, bool* bOK);//オフセット自動更新エラー判定における散乱線判定
	bool CalcEMC(int* iHist, int& iHistBroad, int iEmc_min_thre, int iEmc_broad_thre, int iKasa);

	//欠陥補正
	bool bDefectCorrection6b(string str_img, string str_coord, bool bAutoDetect, int& iGridDirection, float& fHonsuu, double& dA1, double& dA2, double& dA3, double& dA4, double dTh, bool bPreCor2Line, int iIndexEdgeDetect);  //欠陥補正6b (ttsuji 210220)
	bool bDefectCorrection4(TTMA& bad);//欠陥補正4 watanabe org
	bool bDefectCorrection4kai(TTMA& bad, const int iAlgoIndex, const int iGridDirection);//欠陥補正4改 (ttsuji 120809)
	bool bDefectCorrection2kai(TTMA& bad, const int iAlgoIndex, const int iGridDirection);//欠陥補正2改 (ttsuji120723)
	bool bDefectCorrection5(TTMA& bad);//欠陥補正5 watanabe org
	bool bDefectCorrection1(TTMA& bad);//欠陥補正1 watanabe org
	bool bDefectCorrection3(const int iMainMed, const int iSubMed, const float fTh);//欠陥補正3
	bool bDefectCorrection3by1DMedianDiff(const int iSubMed, const float fTh);//欠陥補正3改（副方向の1Dメディアン差分の閾値判定に修正）
	bool bDefectCorrection3byMedianRatio(const int iMainMed, const int iSubMed, const float fTh);//欠陥補正3改（;）
	bool bDefectCorrection3by1DMedianRatio(const int iSubMed, const float fTh);//欠陥補正3改（差分の閾値判定を比の閾値判定に修正）
	bool bDefectCorrection3byMain1DMedianRatio(const int iMainMed, const float fTh);//欠陥補正3改（差分の閾値判定を比の閾値判定に修正）
	bool bDefectCorrection3by1DMedianRatioAndBaseQL(const int iSubMed, const float fTh1, const int iThBaseQL);//欠陥補正3改（メディアン比が閾値１以上であり、かつベースQLが閾値２以上であれば、メディアン値に置換）
	bool bDefectCorrectionFFFF(TTMA& bad);//FF欠陥補正

	//傘
	bool bTateKasa(const string KasaMainFilter, const string KasaSubFilter, const int iKasaClipVal, const int iSubRed, const bool bDebugKasa, const string strImage);//77G対策を入れた16bitLinear入出力の縦傘（ログ変換を内部で行っていない）
//	bool bYokoKasa(const String SubHighCutFilter, const String MainLowPassFilter, const String SubLowPassFilter, const float fClip, const int iMAIN_DIV, const bool bDebugYokokasa, const String strImage);//77G対策を入れた16bitLinear入出力の縦傘（ログ変換を内部で行っていない）
	bool bYokoKasa(string SubHighCutFilter, string MainLowPassFilter, string SubLowPassFilter, const float fClip, const int iMAIN_DIV, const bool bDebugYokokasa, string strImage, bool bConserveBoundaryDensity, const int iSubMedianKeepBoundary);//77G対策を入れた16bitLinear入出力の縦傘（ログ変換を内部で行っていない）
	bool bYokoKasa(string SubHighCutFilter, string MainLowPassFilter, string SubLowPassFilter, const float fClip, const int iMAIN_DIV, const bool bDebugYokokasa, string strImage, bool bIntYokokasa);//田島さん作成(DEVO2 FPGA横傘コンペアのため)
	bool bYokoKasaD3(string SubHighCutFilter,string MainLowPassFilter,string SubLowPassFilter,float fClip, int iMAIN_DIV, bool bDebugYokokasa, string strImage);//福与さん作成(DEVO3 FPGA G面補正における横傘コンペアのため)
	bool bYokoKasaIntType(const string SubHighCutFilter, const string MainLowPassFilter, const string SubLowPassFilter, const int iClip, const int iMAIN_DIV, const bool bDebugYokokasa, const string strImage, bool bConserveBoundaryDensity, const int iSubMedianKeepBoundary);//LongPanelの短冊横傘用に微分画像をInt型へ変換
	bool bYokoKasaIntTypeMix2Filter(string SubHighCutFilter, string MainLowPassFilter, string SubLowPassFilter, string SubHighCutFilter2, string MainLowPassFilter2, string SubLowPassFilter2, const int iClip, const int iMAIN_DIV, const bool bDebugYokokasa, const string strImage, bool bConserveBoundaryDensity, const int iSubMedianKeepBoundary, TTMA& ioUmbImg);//LongPanelの短冊横傘用に微分画像をInt型へ変換


	//LongPanel補正
	void DetectLPSintiGlassEdgePosition(string FileName, int iY0, int iYE, int iX0, int iXE, int iSMedBeforeSubDiff, int iMMedBeforeSubDiff, int iMMedAfterSubDiff, int iThBaseQLSubDiff, bool bUpperEdge, bool bGainCalib);
	void InitializeLPEdgeInf(LPEDGEINF& EdgeInf);
	void LPSintiEdgeCorrection(TTMA& ImageUp, string FileNameImageUp, string& FileName, bool bCorHorShift, int iMainShiftMax, int iSubNumMatch, int iMainMedianMatch, bool bGPRMatch,
							int iMainMedianDiffImg, int iSubMedianDiffImg, int iLimitSearchRange, bool bUetyousei, int iUeOffset, int& iSitaOffset, float fUpperLimitOfPanelSensRatio, float fLowerLimitOfPanelSensRatio, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iMainMedianDCItpl, float fThQLRateUD, float fMagicPara, bool bDetectKanajakuMemori, int iMedianHasiSyori, int iThQLSubDiffForScaleSearch, int iSubAveNum, int iMarginXMed, double dKandoTop, double dKandoCenter, int iYSearchEnd, bool& bExistPbScale,
//							int iDefect3SubRange, int iDefect3MedianMask, float fDefect3ThQL, bool bUpperEdge);
							/*int iDefect3SubRange, int iDefect3MedianMask, float fDefect3ThQL,*/ bool bUpperEdge);
	void LPCalcSintiEdge(double* delta, int* edgeY, string FileName, bool bUpperEdge);//シンチ段差の段差量を計算する
	void LPAdjustEndPanelSensitivity(TFIMG& ImageRateOfUpToDown, TTMA& ImageUp, int* edgeY, string FileName, int iLimitSearchRange, int iUeOffset, int& iSitaOffset, float fUpperLimitOfPanelSensRatio, float fLowerLimitOfPanelSensRatio, int iMainShiftMax, float& fSensitivityRateDtoU);//上画像の感度を中画像に合うように調整
	void LPAdjustEndPanelSensitivity2(TFIMG& ImageRateOfUpToDown, TTMA& ImageUp, int* edgeY, string FileName, int iLimitSearchRange, int iUeOffset, int& iSitaOffset, float fUpperLimitOfPanelSensRatio, float fLowerLimitOfPanelSensRatio, int iMainShiftMax, float& fSensitivityRateDtoU, int iYSearchEnd);//上画像の感度を中画像に合うように調整(150326 ttsuji 日赤の臨床画像を受けて修正)
	void LPDetectHorizontalMisalignmentAndShiftImageUp(TTMA& ImageUp, string FileNameImageUp, int& iMShiftDetect, string FileName, int iMainShiftMax, int iSubNumMatch, int iMainMedianMatch, int iSubRangeOnlyMidPanel, int iYSintiEnd);//上画像と中画像の絵の重複領域を抽出して、ログ変換後に主Medianをかけて、左右位置ずれを正規化相互相関にて検出し、上画像を左右シフトする
	void LPDetectHorizontalMisalignmentAndShiftImageUpMultiROI(TTMA& ImageUp, string FileNameImageUp, int& iMShiftDetect, string FileName, int iMainShiftMax, int iSubNumMatch, int iMainMedianMatch, int iSubRangeOnlyMidPanel, int iYSintiEnd, int iWidthROI, int iWidthOverlap, double dKandoTop, double dKandoCenter, bool bGPRMatch);//上画像と中画像の絵の重複領域を抽出して、ログ変換後に主方向微分して、左右位置ずれを正規化相互相関にて検出し、上画像を左右シフトする
	void LPCorrectOverlapAreaReferToImageUp(TTMA& ImageUp, TTMA& CorrImg, TTMA& QLRateImg, string FileName, int* edgeY, int iKasa, int iYSintiEnd, double dNormQL, int iSubRangeOnlyMidPanel, int iMainMedianDiffImg, int iSubMedianDiffImg, int iMShiftDetect);
	void LPCorrectNonOverlapAreaByItplDC(TTMA& CorrImg, string FileName, int* edgeY, int iKasa, int iYSintiEnd, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iMainMedianDCItpl, float& fThQLRateUD, float& fMagicPara);//斜めエッジをQL比閾値判定で検出し、黒い方の画素値を白くしてDC補間（斜めエッジ周辺に高周波横スジ発生させる副作用あり）
	void LPCorrectNonOverlapAreaByItplDC2(TTMA& CorrImg, string FileName, int* edgeY, int iKasa, int iYSintiEnd, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iMainMedianDCItpl, float& fThQLRateUD, float& fMagicPara);//斜めエッジをQL比閾値判定で検出し、その箇所だけDC補間の長さを短くする（実装途中）
	void LPCorrectNonOverlapAreaByItplDC3(TTMA& CorrImg, string FileName, int* edgeY, int iKasa, int iYSintiEnd, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iMainMedianDCItpl, float& fThQLRateUD);//斜めエッジをQL比閾値判定で検出し、かつ副微分で斜めエッジのｙ座標を特定し、斜めエッジの上側と下側とでそれぞれ三角補間する
	void LPCorrectNonOverlapAreaByItplDC4(TTMA& CorrImg, string FileName, int* edgeY, int iKasa, int iYSintiEnd, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iMainMedianDCItpl, bool bDetectKanajakuMemori, int iMedianHasiSyori, int iThQLSubDiffForScaleSearch, int iSubAveNum, int iMarginXMed, bool& bExistPbScale);//斜めエッジをQL比閾値判定で検出し、かつ副微分で斜めエッジのｙ座標を特定し、斜めエッジに平行に平行四辺形補間する
	void LPDetectPbScaleWhiteBandXROI(TTMA MedLogImg, string FileName, int* edgeY, int iDx, int iThQLSubProf, int iThQLMainProf, int iSubRangeOnlyMidPanel, int iItplSubOffset, int& iNumOfROI, ROI* xROI);//DC補間領域内の横長の金属エッジを探索する(141108 ttsuji 金尺の目盛り消失対策として)
	void LPDetectPbScaleEdgeXROI(TTMA MedLogImg, string FileName, int* edgeY, int iDx, int iThQLSubProf, int iThQLMainProf, int iSubRangeOnlyMidPanel, int iItplSubOffset, int& iNumOfXROI, ROI* XROI, int& iNumOfXROIMed, ROI* XROIMed, bool& bExistPbScale);
	void LPDetectPbScaleEdgeXROI_Glass(TTMA MedLogImg, string FileName, int* edgeY, int* edgeYSinti, int iDx, int iThQLSubProf, int iThQLMainProf, int iSubOffset, int& iNumOfXROI, ROI* XROI, int iSMargin);

	void LPMainMedianForEachROI(TTMA& CorrImg, int* edgeY, int iNumOfXROI, ROI* XROI, int iMainMedianDCItpl, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iKasa);//探索した金属エッジが存在するｘ領域のROIごとに必要なMedianマスクを算出し、Median処理しなおす。
	void LPMainMedianAndCopyForEachROI(TTMA& CorrImg, string FileName, int* edgeY, int iNumOfXROI, ROI* XROI, int iNumOfXROIMed, ROI* XROIMed, int iMainMedianDCItpl, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iKasa, int iMedianHasiSyori, int iMarginXMMed);//探索した金属エッジが存在するｘ領域のROIごとに必要なMedianマスクを算出し、Median処理しなおす。



	void LPCorrectDefectArroundSintiEdge(string FileName, int* edgeY, int iYSintiEnd, int iDefect3SubRange, int iDefect3MedianMask, float fDefect3ThQL, bool bLimitBaseQL, int iThBaseQL);
	void LPCorrectDefectArroundSintiEdge2(string FileName, int iDefect3SubRange, int iDefect3MedianMask1, float fDefect3ThQL1, int iDefect3MedianMask2, float fDefect3ThQL2, int iDefect3MedianMask3, float fDefect3ThQL3, bool bLimitBaseQL, int iThBaseQL, bool bUpperEdge);
	bool LPTanzakuYokokasa(string FileName, int iYstartSinti, int iYendSinti, int iDeltaYUp, int iDeltaYDown, int iSubMedianTanzaku, int iSubMoveAveTanzaku, int iClipVal, int iComboMainDiv, string YokokasaMainFilter, string YokokasaSubFilter, string YokokasaHighCutFilter, bool bCalcSubLowFrequency, string YokokasaMainFilter2, string YokokasaSubFilter2, string YokokasaHighCutFilter2, bool bBlend, int iYRangeNotBlend, int iShiroobiLen, bool bLogYokokasa, bool bUpperEdge, bool bDebugYokokasa, bool bWriteYokokasaClipValTanzaku);
	void LPBlendTwoImagesForTanzakuYokokasa(string FileName, TTMA& YUp1Img, TTMA& Up1Img, int iYRangeNotBlend);
	DIFFERENTIALARROUNDPIXELS CalcDifferentialArroundPixel(TTMA& CorrImg,  int iSCnt, int iMCnt);
	DIFFERENTIALARROUNDPIXELS CalcDifferentialArroundPixel_Fast(TTMA& CorrImg,  int iSCnt, int iMCnt);
	void CalcEdgeOrientation(DIFFERENTIALARROUNDPIXELS& DifPixels);
//	void LPDCItplNanameSankakuHokan(TTMA& ItplImg, TTMA& CorrImg, int iSsample1, int iSsample2, int iMsample1, int iMsample2, double dTan, double dSAtGrpAve, double dMGrpAve, int iKasa);//斜め方向のDC成分補間(3点補間)
//	void LPDCItplNanameSankakuHokan(TTMA& ItplImg, TTMA& CorrImg, TTMA& CorrImgDebug, int iSsample1, int iSsample2, int iMsample1, int iMsample2, double dTan, double dSAtGrpAve, double dMGrpAve, int iKasa);//斜め方向のDC成分補間(3点補間)
	void LPDCItplNanameSankakuHokan(TTMA& ItplImg, TTMA& CorrImg, TTMA& CorrImgDebug, int* edgeY, int iSsample1, int iSsample2, int iMsample1, int iMsample2, double dTan, double dSAtGrpAve, double dMGrpAve, int iKasa, int iSubRangeOnlyMidPanel, int iItplSubOffset);//斜め方向のDC成分補間(3点補間)
	void LPDCItplNanameHeikohShihenkeiHokan(TTMA& ItplImg, TTMA& CorrImg, TTMA& CorrImgDebug, int* edgeY, int iSsample1, int iSsample2, int iMsample1, int iMsample2, double dTan, double dSAtGrpAve, double dMGrpAve, int iKasa, int iSubRangeOnlyMidPanel, int iItplSubOffset);//斜め方向のDC成分補間(3点補間)
	void LPGlassEdgeCor(string FileName, int iMainMedianGlassProfile, bool bUpperEdge);
	void LPGlassEdgeCorNanameEdge(string FileName, int iMainMedianGlassProfile, bool bUpperEdge);
	void LPGlassCorNanameROI(TTMA& CorrImg, double* delta, int iSsample1, int iSsample2, int iMsample1, int iMsampl2, double dTan, double dSAtGrpAve, double dMGrpAve, int iKasa, const int iSize, float* fDaikeiProf);
	void LPGlassEdgeCorNanameEdgeHS(string FileName, int iMainMedianGlassProfile, bool bUpperEdge, bool bDetectKanajakuForGlassCor);
	void LPGlassEdgeCorNanameEdgeHS2(string FileName, int iMainMedianGlassProfile, bool bUpperEdge, bool bDetectKanajakuForGlassCor, int iThQLSubDiffForScaleSearchGlass, int iSMargin);
	void LPGlassCorNanameROIHeikohShihenkei(TTMA& CorrImg, double* delta, int* edgeY, int iSsample1, int iSsample2, int iMsample1, int iMsampl2, double dTan, double dSAtGrpAve, double dMGrpAve, int iKasa, const int iSize, float* fDaikeiProf);
	void LPDetectBorderRadiationField(string FileName, TTMA& SmoothImageUp, int iThQLMDif, bool& bExist, bool& bExistE, int& iMBorder0, int& iMBorderEnd, int iSSearch0, int iSSearchEnd, int iLimitSearchRange);
	void LPCorrectTogeArroundSintiEdge(TSIMARS& NoMedImage, string FileName, double* pdSintiEdgeU10, double* pdSintiEdgeD10, int iDefect3SubRange, int iDefect3MainMedianMask, float fDefect3ThQL);
	// 20141222 Kuwabara 高周波横スジ除去
	bool bLPHighkasa(string FileName, int iYstartSinti, int iYendSinti, int iDeltaYUp, int iDeltaYDown, int iSubMedianTanzaku, int iClipVal, int iComboMainDiv, string YokokasaMainFilter, string YokokasaSubFilter, string YokokasaHighCutFilter, bool bLogYokokasa, bool bUpperEdge, bool bDebugYokokasa, bool bWriteYokokasaClipVal);

	//粒状抑制処理(150706 ttsuji)
	void LPGranularityReduction(string& FileName, int iMaskSize1, double dThEnergy1, double dSelfWeight1, int iMaskSize2, double dThEnergy2, double dSelfWeight2, int iMaskSize3, double dThEnergy3, double dSelfWeight3, int iYShiftMinus);
	void LPGranularityReductionForEachSizeImage(string FileName, TSIMARS& ImgNoise, int iMaskSize, double dThEnergy, double dSelfWeight);
	void LPGranularityReductionForEachSizeImage2(string FileName, TSIMARS& ImgNoise, int iMaskSize, double dThEnergy, double dSelfWeight);
	void LPResize05(string& FileName);//0.5倍に縮小（ガウシアンピラミッド生成）
	void LPResize2(string& FileName);//2倍に拡大（ラプラシアンピラミッド生成）
	void LPResizeExpand05(string& FileName);//0.5倍に縮小（ガウシアンピラミッド生成、フィルター端処理は平均拡張）
	void LPResizeExpand2(string& FileName);//2倍に拡大（ラプラシアンピラミッド生成、フィルター端処理は平均拡張）


	static void InitializeDXAObliqueRoi(DXA_OBLIQUE_ROI& Dxa);
	static void InitializeDXAIntegratedSoftRoiBoneMap(DXA_INTEGRATED_SOFTROI_BONEMAP& Dxa);
	static void InitializeDXAIntegratedSoftRoiObliqueBoneROI(DXA_INTEGRATED_SOFTROI_OBLIQUE_BONEROI& Dxa);
	static void InitializeDXAFemurTotal(DXA_FEMUR_TOTAL& Dxa);
	static void InitializeDxaJIS(DXA_JIS& Dxa);
	static void InitializeDxaCalib(DXA_CALIB_INFO& DxaCalib);
	static void InitializeNetsumuraInfo(NETSUMURA_INFO& Netsumura);
	static void InitializeNetsumuraInfo(NETSUMURA_INFO& Netsumura, int iSubPixelOfReset);
	static void DeleteNetsumuraInfo(NETSUMURA_INFO& Netsumura);

	//DEVO3動画のオフセット・ラグ分離補正
//	bool ReadImageAttributeInf(IMG_ATTRIBUTE_INF& ImgAttInf, string StrFileName);//画像から付帯情報を読み取って構造体に格納する
	bool ReadImageAttributeInf(TIMG_ATTRIBUTE_INF& ImgAttInf, string StrFileName);//画像から付帯情報を読み取って構造体に格納する

	//線欠陥補正の改良
	double GetOrientWeightedByEnergy(DIFFERENTIALARROUNDPIXELS* pDifPixels);//線欠陥補正対象画素の左右ROIにおけるそれぞれのEnergyの重みに応じてエッジの方向を計算
	DIFFERENTIALARROUNDPIXELS GetOrientWeightedByEnergy(DIFFERENTIALARROUNDPIXELS& DifPixels, DIFFERENTIALARROUNDPIXELS& DifPixelsU, DIFFERENTIALARROUNDPIXELS& DifPixelsD);//線欠陥補正対象画素の左右片側ROIにおける上下±1pixシフトさせたROIを含めたEnergyとエッジの方向を計算
	void CalcLaplacianFilter(int iQLOffset);
	bool DetectionCTFChartPattern(string StrFile, double& dCtfStripesAngle);//DLLを動的に呼び出してCTFチャートのパターン領域を検出し、縞の配置角度を求める
	double CalcCtfStripesAngle(string StrFile);//CTFチャートの縞の配置角度の検出
	bool HoughTrans(string Str, const bool bDetectOnlyPlusEdge, const int iThDiffQL, const int iBaseQL, const int iTHETA_RESOLUTION, const int iRHO_RESOLUTION, const int iThetaNeighbor, const int iRhoNeighbor, const int iLNUMBER_MAX, const int iCounterTh, double* dDegreeDetect, int* iRhoDetect);//Hough変換

	LPEDGEINF m_SintiEdgeU;
	LPEDGEINF m_GlassEdgeU;
	LPEDGEINF m_CalibSintiEdgeU;//キャリブ時のシンチ段差位置
	LPEDGEINF m_CalibGlassEdgeU;//キャリブ時のガラス段差位置
	LPEDGEINF m_EdgeShiftU;//キャリブ時と曝射時のシンチ段差位置のシフト
	LPEDGEINF m_SintiEdgeD;
	LPEDGEINF m_GlassEdgeD;
	LPEDGEINF m_CalibSintiEdgeD;//キャリブ時のシンチ段差位置
	LPEDGEINF m_CalibGlassEdgeD;//キャリブ時のガラス段差位置
	LPEDGEINF m_EdgeShiftD;//キャリブ時と曝射時のシンチ段差位置のシフト

	TSIMARS();
	~TSIMARS();
	TSIMARS(const int iS, const int iM, const int iBit,
			const int iEDRVal = iEDR_OFF,
			const int iSkVal = 200,
			const int iLVal = 400);
	TSIMARS& operator =(const TIIMG& copy);//ttsuji追加
	explicit TSIMARS(const CTDAT &copy);//ttsuji追加 200905
	TSIMARS& operator =(const TFIMG &copy);//ttsuji追加


};

#endif

#endif /* CSIMARSF_H_ */
