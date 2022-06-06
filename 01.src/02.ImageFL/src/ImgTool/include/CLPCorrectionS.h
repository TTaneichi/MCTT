/*
 * CLPCorrectionS.h
 *
 *  Created on: 2021/04/06
 *      Author: 10097663
 */

#ifndef CLPCORRECTIONS_H_
#define CLPCORRECTIONS_H_

#include "CTTMA.h"
#include "stdio.h"
#include "CSIMARSF.h"

class CLPCorrectionS {
private:
	TTMA	*m_pTTMA;

public:

	CLPCorrectionS();

	CLPCorrectionS(TTMA *pTTMA);

	virtual ~CLPCorrectionS();

	//TTMAオブジェクトの設定
	void SetTTMA(TTMA *pTTMA);

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



public:
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

};

#endif /* CLPCORRECTIONS_H_ */
