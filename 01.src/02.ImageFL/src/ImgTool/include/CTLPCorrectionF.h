/*
 * CTLPCorrectionF.h
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#ifndef CTLPCORRECTIONF_H_
#define CTLPCORRECTIONF_H_

#include "CSIMARSF.h"
#include "CTDAT.h"
#include <string>

using namespace std;

class TSIMARS;

class CTLPCorrectionF {
public:
	CTLPCorrectionF();
	virtual ~CTLPCorrectionF();

	//段差補正処理
//	bool bCorrection(TSIMARS& io_img, string str);
	bool bCorrection(TTMA& io_img, string str);

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


private:
	bool m_bRot180Exe;			//180度回転
	bool m_bUpDownExe;			//上下反転
	bool m_bEachCorImgOut;		//各補正処理後の画像出力
	bool m_bLeftRightExe;		//左右入れ替え
	bool m_bOffsetExe;			//オフセット補正
	bool m_bOffsetGeta;			//オフセットげた
	bool m_bGainExe;			//ゲイン処理
	bool m_bSenseExe;			//感度補正係数乗算
	bool m_bDefect7Exe;			//欠陥補正7
	bool m_bAntiSens;			//
	bool m_bPatternAnalysis;	//
	bool m_bModifyWeight;		//
	bool m_bDefect6Exe;			//
	bool m_bPreCor2Line;
	bool m_bDetectPeak;
	bool m_bInverseGainExe;		//逆ゲイン補正
	bool m_bInverseGainExe2;	//逆ゲイン補正2
	bool m_bCheckClipDiv;
	bool m_bExeInvSensCor;
	bool m_bPreLogTrans;
	bool m_bExeSubMedianHough;
	bool m_bDetectOnlyPlusEdge;
	bool m_bExcessThQL;
	bool m_bGlassSintiPriority;
	bool m_bDetectEdgePosExe;	//
	bool m_bDetectUpperEdge;	//
	bool m_bDetectLowerEdge;
	bool m_bGainCalibParamExe;	//
	bool m_bForwardGainExe;		//
	bool m_bbExeForwardSensCor;	//順感度補正するか否かのフラグ
	bool m_bGlassEdgeCorExe;	//
	bool m_bCorUpperGlassEdge;	//
	bool m_bCorLowerGlassEdge;	//
	bool m_bSintiEdgeCorExe;	//
	bool m_bCorUpperSintiEdge;	//
	bool m_bCorLowerSintiEdge;	//
	bool m_bAutoCalcTanzakuHeight;	//
	bool m_bTanzakuYokokasaExe;	//
	bool m_bTanzakuYokokasaForUpperEdge;	//
	bool m_bTanzakuYokokasaForLowerEdge;	//
	bool m_bExecGranularityRemoval;
	bool m_bDefect3ArroundSintiEdgeExe;
	bool m_bCorUpperDefect;
	bool m_bCorLowerDefect;
	bool m_bSValueModExe;
	bool m_bClipExe;
	bool m_bSharpnessFilterExe;
	bool m_bUpDownExe2;
	bool m_bLeftRightExe2;
	bool m_bRot180Exe2;
	bool m_bLogExe;
	bool m_bCheckAntiLog;
	bool m_bBitChangeExe;
	bool m_bSetBit;



	string m_StrCor;
	int m_nOffsetGeta;			//Offsetのげた値
	float m_fCoeSense;			//感度
	int m_nComboGainNormQL;		//
	int m_nComboPanelType;		//パネルタイプ
	int m_nInputClipQL;			//入力クリップQL
	string m_OffsetFile;		//オフセットデータファイル
	string m_GainMapFile;		//ゲインマップファイル
	string m_DefectMapFile;		//欠陥マップ
	string m_CoordMapFile;		//線欠陥アドレスを記載したテキストファイル
	string m_InvGainMapFile;	//

	int m_nPixelNumPatternAnalysis;
	double m_dThPatternExist;
	double m_b1;
	double m_b2;
	double m_b3;
	double m_b4;
	double m_iTh;
	int m_iGridDirection;
	double m_fKando;			//感度補正係数
	double m_fComboInvGainNormQL;	//
	int m_ClipDiv;

	//キャリブ画像の段差位置
	double m_dA1, m_dA2;
	double m_dB1, m_dB2;
	double m_dC1, m_dC2;

	int m_iYStart;
	int m_iYEnd;
	int m_ThDiffQL;
	int m_iDetectLineNum;
	int m_iSubMedianHough;
	float m_ThQLMetal;
	int m_iSShiftPixel;
	int m_iSMargin;
	float m_fRateGlassToSinti;
	double m_MidValHough;
	double m_KetaHough;
	double m_LogThHough;
	int m_ThetaResolution;
	int m_RhoMax;
	int m_ThCounter;
	int m_ThDeltaY;
	int m_RhoNeighbor;
	int m_ThetaNeighbor;
	int m_SubExpandRange2;
	int m_XStart;
	int m_XEnd;
	int m_MMedBeforeSubDiff;
	int m_MMedAfterSubDiff;
	int m_ThBaseQLSubDiff;


private:
	void ProgressStepIt();

	//事前画像回転/反転処理
	bool setImageDirectionBefore(TTMA& io_img, string in_str);

	//オフセット補正処理
	bool offsetCorrection(TTMA& io_img, string in_str);

	//ゲイン補正(乗算)&感度補正係数乗算
	bool gainCorrection(TTMA& io_img, string in_str);

	//欠陥補正7
	bool defectCorrection7(TTMA& io_img, string in_str);

	//欠陥補正6
	bool defectCorrection6(TTMA& io_img, string in_str);

	//逆ゲイン補正
	bool invGainCorrection(TTMA& io_img, string in_str);

	//段差補正
	bool detectEdge(TTMA& io_img, string in_str);

	//逆ゲインキャリブ画像の平行四辺形シフト
	bool shiftInvGainSquare(TTMA& io_img, string in_str);

	//順ゲイン補正
	bool forwardGainCorrection(TTMA& io_img, string in_str);

	//ガラス段差補正
	bool glassEdgeCorrection(TTMA& io_img, string in_str);

	//シンチ段差補正
	bool sintiEdgeCorrection(TTMA& io_img, string in_str);

	//短冊横傘
	bool tanzakuYokokasa(TTMA& io_img, string in_str);

	//粒状抑制処理
	bool formatForSmoothing(TTMA& io_img, string in_str);

	//欠陥補正3もどき
	bool defectArroundSintiEdge2Correction(TTMA& io_img, string in_str);

	//S値適正化補正(4倍補正)
	bool adaptSValueCorrection(TTMA& io_img, string in_str);

	//高周波横傘
	bool highFreqYokokasa(TTMA& io_img, string in_str);

	//鮮鋭度フィルタ
	bool sharpnessFilter(TTMA& io_img, string in_str);

	//事後画像回転/反転処理
	bool setImageDirectionAfter(TTMA& io_img, string in_str);

	//Log/Bit変換
	bool logBitTrans(TTMA& io_img, string in_str);



	//曝射画像からHough変換を用いて段差検出
	bool detectEdgeWithHough(TTMA& img, string str, bool bUpperEdge);

};

#endif /* CTLPCORRECTIONF_H_ */
