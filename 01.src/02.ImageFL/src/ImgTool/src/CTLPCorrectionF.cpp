/*
 * CTLPCorrectionF.cpp
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#include "CTLPCorrectionF.h"
#include "CLPCorrectionS.h"
#include "CApplication.h"
#include "CTTMA.h"
#include "CTIIMG.h"
#include "CSIMARSF.h"
#include "utility.h"
#include <sstream>

CTLPCorrectionF::CTLPCorrectionF() {

	m_bRot180Exe = true;
	m_bUpDownExe = true;
	m_bEachCorImgOut = true;
	m_bLeftRightExe = true;
	m_bOffsetExe = true;
	m_bOffsetGeta = true;
	m_bGainExe = true;
	m_bSenseExe = true;
	m_bDefect7Exe = true;
	m_bAntiSens = true;
	m_bPatternAnalysis = true;
	m_bModifyWeight = true;
	m_bDefect6Exe = true;
	m_bPreCor2Line = true;
	m_bDetectPeak = true;
	m_bInverseGainExe = true;
	m_bInverseGainExe2 = true;
	m_bDetectEdgePosExe = true;
	m_bDetectUpperEdge = true;
	m_bDetectLowerEdge = true;
	m_bGainCalibParamExe = true;
	m_bForwardGainExe = true;
	m_bbExeForwardSensCor = true;
	m_bGlassEdgeCorExe = true;
	m_bCorUpperGlassEdge = true;
	m_bCorLowerGlassEdge = true;
	m_bSintiEdgeCorExe = true;
	m_bCorUpperSintiEdge = true;
	m_bCorLowerSintiEdge = true;
	m_bAutoCalcTanzakuHeight = true;
	m_bTanzakuYokokasaExe = true;
	m_bTanzakuYokokasaForUpperEdge = true;
	m_bTanzakuYokokasaForLowerEdge = true;
	m_bExecGranularityRemoval = true;
	m_bDefect3ArroundSintiEdgeExe = true;
	m_bCorUpperDefect = true;
	m_bCorLowerDefect = true;
	m_bSValueModExe = true;
	m_bClipExe = true;
	m_bSharpnessFilterExe = true;
	m_bUpDownExe2 = true;
	m_bLeftRightExe2 = true;
	m_bRot180Exe2 = true;
	m_bLogExe = true;
	m_bCheckAntiLog = true;
	m_bBitChangeExe = true;
	m_bSetBit = true;

}

CTLPCorrectionF::~CTLPCorrectionF() {
	// TODO Auto-generated destructor stub
}
//段差補正処理
//bool CTLPCorrectionF::bCorrection(TSIMARS& io_img, string in_str){
bool CTLPCorrectionF::bCorrection(TTMA& io_img, string in_str){

	m_StrCor = "";

	//画像回転/反転処理
	if (!setImageDirectionBefore(io_img, in_str)){
		return false;
	}

	// オフセット補正
	if (!offsetCorrection(io_img, in_str)){
		return false;
	}

	// ゲイン補正（乗算）
	if (!gainCorrection(io_img, in_str)){
		return false;
	}

	//欠陥補正7     130331 ttsuji
	if (!defectCorrection7(io_img, in_str)){
		return false;
	}

	//欠陥補正6　(takeshi_kuwabara)
	if (!defectCorrection6(io_img, in_str)){
		return false;
	}

	//逆ゲイン補正
	if (!invGainCorrection(io_img, in_str)){
		return false;
	}

	//段差検出
	if (!detectEdge(io_img, in_str)){
		return false;
	}

	//逆ゲインキャリブ画像の平行四辺形シフト
	if (!shiftInvGainSquare(io_img, in_str)){
		return false;
	}

	//順ゲイン補正
	if (!forwardGainCorrection(io_img, in_str)){
		return false;
	}

	//ガラス段差補正
	if (!glassEdgeCorrection(io_img, in_str)){
		return false;
	}

	//シンチ段差補正
	if (!sintiEdgeCorrection(io_img, in_str)){
		return false;
	}

	//短冊横傘
	if (!tanzakuYokokasa(io_img, in_str)){
		return false;
	}

	//粒状抑制処理
	if (!formatForSmoothing(io_img, in_str)){
		return false;
	}

	//欠陥補正3もどき（シンチ段差付近のみ）
	if (!defectArroundSintiEdge2Correction(io_img, in_str)){
		return false;
	}

	//S値適正化補正(4倍補正)
	if (!adaptSValueCorrection(io_img, in_str)){
		return false;
	}

	//高周波横傘
	if (!highFreqYokokasa(io_img, in_str)){
		return false;
	}

	//鮮鋭度補正
	if (!sharpnessFilter(io_img, in_str)){
		return false;
	}

	//Log/Bit変換
	if (!logBitTrans(io_img, in_str)){
		return false;
	}

	return true;

}
void CTLPCorrectionF::ProgressStepIt(){




}
//事前画像回転/反転処理
bool CTLPCorrectionF::setImageDirectionBefore(TTMA& io_img, string in_str){

	// 180度回転
	if (m_bRot180Exe){

		io_img.UpDown();
		io_img.RtoL();
		m_StrCor = "rot" + m_StrCor;

		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}
		ProgressStepIt();
	}

	// 上下反転
	if (m_bUpDownExe){

		io_img.UpDown();
		m_StrCor = "t" + m_StrCor;

		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}
		ProgressStepIt();
	}

	// 左右反転
	if (m_bLeftRightExe){

		io_img.RtoL();
		m_StrCor = "r" + m_StrCor;

		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}
		ProgressStepIt();
	}

	return true;

}

//オフセット補正処理
bool CTLPCorrectionF::offsetCorrection(TTMA& io_img, string in_str){

	TTMA OffsetImg;
	if (m_bOffsetExe){
		if(OffsetImg.ReadTma(m_OffsetFile) == false){
			CApplication::GetInstance()->MessageBox("オフセットファイルが開けませんでした", "Error", MB_OK);
			return false;
		}

		//補正画像と被補正画像のサイズ判定
		if (OffsetImg.MainPixel != io_img.MainPixel || OffsetImg.SubPixel != io_img.SubPixel) {
			CApplication::GetInstance()->MessageBox("オフセットキャリブ画像と被補正画像のサイズが合っていません。", "Error", MB_OK);
			return false;
		}

		//ゲタを履かせる場合(ttsuji)
		int i, j;
		if (m_bOffsetGeta) {
			int geta = m_nOffsetGeta;
			//			io_img -= (OffsetImg - geta);//クリップ動作が複数回繰り返されるため好ましくない
			for (i = 0 ; i < io_img.SubPixel ; i++){
				for (j = 0 ; j < io_img.MainPixel ; j++){
					io_img.Dt[i][j] = CLIP((io_img.Dt[i][j] - OffsetImg.Dt[i][j] + geta), 0, io_img.DtMax);
				}
			}
			std::ostringstream oss;
			oss << m_nOffsetGeta;
			m_StrCor = "o(" + oss.str() + ")" + m_StrCor;
		}
		else{
			io_img -= OffsetImg;
			m_StrCor = "o" + m_StrCor;
		}

		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}
		ProgressStepIt();
	}

	return true;
}

//ゲイン補正(乗算)&感度補正係数乗算
bool CTLPCorrectionF::gainCorrection(TTMA& io_img, string in_str){

	TTMA ShadingImg;
	if (m_bGainExe){

		if(!ShadingImg.ReadTma(m_GainMapFile)){
			CApplication::GetInstance()->MessageBox("ゲインキャリブファイルが開けませんでした", "Error", MB_OK);
			return false;
		}
		//補正画像と被補正画像のサイズ判定
		if (ShadingImg.MainPixel != io_img.MainPixel || ShadingImg.SubPixel != io_img.SubPixel) {
			CApplication::GetInstance()->MessageBox("ゲインキャリブ画像と被補正画像のサイズが合っていません。", "Error", MB_OK);
			return false;
		}

		//感度補正係数
		int iCoeSense = (int)(32768 * m_fCoeSense);//U1.15(32768が1に相当（SPCに合わせるために切り捨て！）)

		//規格化QL
		int iShosuKirisute;
		switch (m_nComboGainNormQL) {
		case 0://32768QLがゲイン1倍のとき
			iShosuKirisute = 16384;//14bit切り捨て
			break;
		case 1://8192QLがゲイン1倍のとき
			iShosuKirisute = 4092;//12bit切り捨て
			break;
		default:
			CApplication::GetInstance()->MessageBox("ゲイン1倍のQL値が不正です", "Error", MB_OK);
			return false;
		}

		CSimars	simarsObj;
		simarsObj.SetTTMA(&io_img);

		//パネル種ごとに場合分け (ゲイン補正に加え、感度補正も同時に実行)
		switch (m_nComboPanelType) {
		case 0://SELENE, ROOTS
			if (!simarsObj.bGainCorrectionSPC(ShadingImg, iCoeSense, iShosuKirisute)){
				CApplication::GetInstance()->MessageBox("Bmapを用いたSPCゲイン補正でエラーが発生しました。", "Error", MB_OK);
				return false;
			}
			break;
		case 1://AXIS, 77G
			if (!simarsObj.bGainCorrectionAXIS(ShadingImg, iCoeSense, _PANEL_AXIS)){
				CApplication::GetInstance()->MessageBox("Bmapを用いたゲイン補正でエラーが発生しました。", "Error", MB_OK);
				return false;
			}
			break;
		case 2://47C, 77C
			if (!simarsObj.bGainCorrectionAXIS(ShadingImg, iCoeSense, _PANEL_47C)){
				CApplication::GetInstance()->MessageBox("Bmapを用いたゲイン補正でエラーが発生しました。", "Error", MB_OK);
				return false;
			}
			break;
		case 3://2430C
			if (!simarsObj.bGainCorrection2430(ShadingImg, iCoeSense, _PANEL_2430, m_nInputClipQL)){
				CApplication::GetInstance()->MessageBox("Bmapを用いたゲイン補正でエラーが発生しました。", "Error", MB_OK);
				return false;
			}
			break;
		case 4://HM
			if (!simarsObj.bGainCorrection2430(ShadingImg, iCoeSense, _PANEL_HM, m_nInputClipQL)){
				CApplication::GetInstance()->MessageBox("Bmapを用いたゲイン補正でエラーが発生しました。", "Error", MB_OK);
				return false;
			}
			break;
		case 5://DEVO2 1417GOS
			if (!simarsObj.bGainCorrection2430(ShadingImg, iCoeSense, _PANEL_DEVO2_1417_GOS, m_nInputClipQL)){
				CApplication::GetInstance()->MessageBox("Bmapを用いたゲイン補正でエラーが発生しました。", "Error", MB_OK);
				return false;
			}
			break;
		case 6://DEVO2 1417CsI
			if (!simarsObj.bGainCorrection2430(ShadingImg, iCoeSense, _PANEL_DEVO2_1417_CSI, m_nInputClipQL)){
				CApplication::GetInstance()->MessageBox("Bmapを用いたゲイン補正でエラーが発生しました。", "Error", MB_OK);
				return false;
			}
			break;
		case 7://DEVO2 1717GOS
			if (!simarsObj.bGainCorrection2430(ShadingImg, iCoeSense, _PANEL_DEVO2_1717_GOS, m_nInputClipQL)){
				CApplication::GetInstance()->MessageBox("Bmapを用いたゲイン補正でエラーが発生しました。", "Error", MB_OK);
				return false;
			}
			break;
		case 8://DEVO2 1717CsI
			if (!simarsObj.bGainCorrection2430(ShadingImg, iCoeSense, _PANEL_DEVO2_1717_CSI, m_nInputClipQL)){
				CApplication::GetInstance()->MessageBox("Bmapを用いたゲイン補正でエラーが発生しました。", "Error", MB_OK);
				return false;
			}
			break;
		}

		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
//			if ("1" == Kando->Text) {
//				m_StrCor = "s" + m_StrCor;
//			}
//			else{
//				m_StrCor = "S(" + Kando->Text + ")s" + m_StrCor;
//			}
			m_StrCor = "S(" + (string)"1"+ ")s" + m_StrCor;

			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}
		ProgressStepIt();
	}

	//-----------------------
	// 感度補正係数乗算
	//
	if (m_bSenseExe){
		if (!m_bGainExe){//ゲイン補正時に逆数画像の乗算を指定した場合、ここでの感度補正は不要
			if (!m_bAntiSens) {
				io_img *= m_fCoeSense;
			}
			else{
				io_img /= m_fCoeSense;
			}
		}
		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			m_StrCor = "S" + m_StrCor;
			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}
		ProgressStepIt();
	}

	return true;
}

//欠陥補正7
bool CTLPCorrectionF::defectCorrection7(TTMA& io_img, string in_str){

	TTMA bad;
	int iGridDirection = 2;//縦：０、横：１、なし：２

	CSimars	simarsObj;
	simarsObj.SetTTMA(&io_img);

	if(m_bDefect7Exe){

		if(!bad.ReadTma(m_DefectMapFile)){
			CApplication::GetInstance()->MessageBox("欠陥マップファイルが開けませんでした", "Error", MB_OK);
			return false;
		}
		//補正画像と被補正画像のサイズ判定
		if (bad.MainPixel != io_img.MainPixel || bad.SubPixel != io_img.SubPixel) {
			CApplication::GetInstance()->MessageBox("欠陥マップ画像と被補正画像のサイズが合っていません。", "Error", MB_OK);
			return false;
		}
		//グリッドありのときはグリッドのリス目方向の画素のみから補間）(ttsuji 120809)
		if (!simarsObj.bDefectCorrection7(bad, iGridDirection, m_bPatternAnalysis, m_nPixelNumPatternAnalysis, m_dThPatternExist, m_bModifyWeight)){
			CApplication::GetInstance()->MessageBox("欠陥補正7でエラーが発生しました", "Error", MB_OK);
			return false;
		}
		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			m_StrCor = "d7" + m_StrCor;
			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}
		ProgressStepIt();
	 }

	return true;
}

//欠陥補正6
bool CTLPCorrectionF::defectCorrection6(TTMA& io_img, string in_str){

	if(m_bDefect6Exe){
		float fHonsuu;
		CSimars	simarsObj;
		simarsObj.SetTTMA(&io_img);
//		if (!simarsObj.bDefectCorrection6a(in_str, m_CoordMapFile, m_bDetectPeak, m_iGridDirection, fHonsuu, m_b1, m_b2, m_b3, m_b4, m_iTh+1.e-6, m_bPreCor2Line)) {
//			CApplication::GetInstance()->MessageBox("欠陥補正6でエラーが発生しました", "Error", MB_OK);
//			return false;
//		}
		//GUIに補正係数を反映
//		a1->Text = FloatToStr(b1);
//		a2->Text = FloatToStr(b2);
//		a3->Text = FloatToStr(b3);
//		a4->Text = FloatToStr(b4);
//		GridDirection->ItemIndex = iGridDirection;
//		Label78->Caption = "本数：" + FloatToStr(fHonsuu);
//		SetGUI_GridHonsuuCombo(fHonsuu, iGridDirection);

		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			m_StrCor = "d6" + m_StrCor;
			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}
		ProgressStepIt();
	 }

	return true;
}

//逆ゲイン補正
bool CTLPCorrectionF::invGainCorrection(TTMA& io_img, string in_str){

	double dKando32768 = m_fKando * 32768;//感度補正係数が32768倍になるようにビットシフト
	int iNormBitSense = 15;//感度補正係数の規格化べき

//	if(InverseGainExe->Checked){
	if(m_bInverseGainExe || m_bInverseGainExe2){

		if(m_bInverseGainExe && m_bInverseGainExe2){
			CApplication::GetInstance()->MessageBox("乗算処理か除算処理のいずれかを選択してください。", "Error", MB_OK);
			return false;
		}

		TTMA InverseGain;
		double dQL = m_fComboInvGainNormQL;
		int iThDiv = m_ClipDiv;
		bool bClipDiv = m_bCheckClipDiv;
		bool bInvSensCor = m_bExeInvSensCor;
//		double dKando32768 = Kando->Text.ToDouble() * 32768;//32768倍になるようにビットシフト
		double dInvSensitivity = 1.0 / dKando32768;
		unsigned long long tmpPixel;
//		int iNormBitSense = 15;//感度補正係数の規格化べき
//		int iNormInvGainQL = round(log2l(dQL));//感度補正係数の規格化べき
		int iNormInvGainQL = 11;//log2でコンパイラエラーとなるため。


		//逆ゲイン補正画像のファイル名チェック（処理ミス対策）
		if(m_bInverseGainExe){
			if (string::npos == ExtractFileName(m_InvGainMapFile).find("BmapLPIG_Mul")){
				if (IDYES == CApplication::GetInstance()->MessageBox("逆ゲインキャリブ画像のファイル名に\"BmapLPIG_Mul\"という文字列が含まれています。計算を中止しますか？", "警告", MB_YESNO)){
					return false;
				}
			}
		}
		else if (m_bInverseGainExe2){
			if (string::npos == ExtractFileName(m_InvGainMapFile).find("BmapLPIG_Mul") || string::npos == ExtractFileName(m_InvGainMapFile).find("InvGain00.xxx")){
				if (IDYES == CApplication::GetInstance()->MessageBox("逆ゲインキャリブ画像のファイル名に\"BmapLPIG_Mul\"、または\"InvGain00.xxx\"という文字列が含まれていません。計算を中止しますか？", "警告", MB_YESNO)){
					return false;
				}
			}
		}

		if(!InverseGain.ReadTma(m_InvGainMapFile)){
			CApplication::GetInstance()->MessageBox("逆ゲイン補正画像ファイルが開けませんでした", "Error", MB_OK);
			return false;
		}
		//補正画像と被補正画像のサイズ判定
		if (InverseGain.MainPixel != io_img.MainPixel || InverseGain.SubPixel != io_img.SubPixel) {
			CApplication::GetInstance()->MessageBox("逆ゲインキャリブ画像と被補正画像のサイズが合っていません。", "Error", MB_OK);
			return false;
		}

		//除算処理の場合
		if(m_bInverseGainExe){
			for (int iS = 0; iS < io_img.SubPixel; iS++){
				for (int iM = 0; iM < io_img.MainPixel; iM++){
					if (InverseGain.Dt[iS][iM]) {
						//非補正画像QLが飽和していると逆ゲイン補正により、キャリブのシンチ段差、ガラス段差を作ってしまい、NG
						if (!bClipDiv) {
							io_img.SetVal(iS, iM, dQL * io_img.Dt[iS][iM] / InverseGain.Dt[iS][iM]);
						}
						else{
							//非補正画像QLが飽和しているときの対策（clip処理）
							if (io_img.Dt[iS][iM] >= iThDiv) {//逆ゲイン補正される元画像がiTh以上であれば、QLを変えない。
								continue;
							}
							else{
								io_img.SetVal(iS, iM, dQL * io_img.Dt[iS][iM] / InverseGain.Dt[iS][iM]);
							}
						}
					}
					else{
						io_img.SetVal(iS, iM, 0);
					}
				}
			}
		}
		//乗算処理の場合
		else if (m_bInverseGainExe2){
			for (int iS = 0; iS < io_img.SubPixel; iS++){
				for (int iM = 0; iM < io_img.MainPixel; iM++){
					if (!bClipDiv) {
						//非補正画像QLが飽和しているとゲイン補正により、キャリブのシンチ段差、ガラス段差を作ってしまい、NG
#if 0	 //四捨五入
						io_img.SetVal(iS, iM, io_img.Dt[iS][iM] * InverseGain.Dt[iS][iM] / dQL);
#elif 0	//切捨て（MCコンペア用）
						io_img.Dt[iS][iM] = CLIP((int)(io_img.Dt[iS][iM] * InverseGain.Dt[iS][iM] / dQL), 0, io_img.DtMax);
#elif 1 //逆感度補正を追加(MCコンペア用) 150210 ttsuji実装
						switch (bInvSensCor) {
						case true://逆感度補正あり
							tmpPixel = (((unsigned long long)io_img.Dt[iS][iM] * InverseGain.Dt[iS][iM]) << iNormBitSense ) >> iNormInvGainQL;
							tmpPixel = (unsigned long long)((double)tmpPixel * dInvSensitivity + 0.5);
							io_img.Dt[iS][iM] = CLIP(( unsigned short )tmpPixel, 0, io_img.DtMax);
							break;
						case false://逆感度補正なし
							io_img.Dt[iS][iM] = CLIP((int)(io_img.Dt[iS][iM] * InverseGain.Dt[iS][iM] / dQL), 0, io_img.DtMax);
							break;
						}
#endif
					}
					else{
						//非補正画像QLが飽和しているときの対策（clip処理）
						if (io_img.Dt[iS][iM] >= iThDiv) {//逆ゲイン補正される元画像がiTh以上であれば、QLを変えない。
							continue;
						}
						else{
							io_img.SetVal(iS, iM, io_img.Dt[iS][iM] * InverseGain.Dt[iS][iM] / dQL);
						}
					}
				}
			}
		}


		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			if(m_bInverseGainExe){
				m_StrCor = "i(" +  to_string(m_fComboInvGainNormQL) + ")" + m_StrCor;
			}
			else if (m_bInverseGainExe2){
				m_StrCor = "i(Mul" +  to_string(m_fComboInvGainNormQL) + ")" + m_StrCor;
        	}
			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}
		ProgressStepIt();
   }

	return true;

}

//段差補正
bool CTLPCorrectionF::detectEdge(TTMA& io_img, string in_str){

	if(m_bDetectEdgePosExe){

		if (m_bDetectUpperEdge) {
			if (!detectEdgeWithHough(io_img, in_str, true)) {
				CApplication::GetInstance()->MessageBox("上パネルの段差検出に失敗しました", "Error", MB_OK);
				return false;
			}
		}
		if (m_bDetectLowerEdge) {
			io_img.UpDown();//上下反転
			if (!detectEdgeWithHough(io_img, in_str, false)) {
				CApplication::GetInstance()->MessageBox("下パネルの段差検出に失敗しました", "Error", MB_OK);
				io_img.UpDown();//上下反転
				return false;
			}
			io_img.UpDown();//上下反転
		}
		ProgressStepIt();
	}

	return true;
}

//逆ゲインキャリブ画像の平行四辺形シフト
bool CTLPCorrectionF::shiftInvGainSquare(TTMA& io_img, string in_str){
#if 0
	if (m_bGainCalibParamExe){

		//補正画像のファイル名チェック（処理ミス対策）
//		if (!ExtractFileName(InverseGainMapFile2->Text).AnsiPos("BmapLPIG_m(m33s33)")){
//		if (!ExtractFileName(InverseGainMapFile2->Text).AnsiPos("m(m33s33)_BmapLP")){ //ttsuji 140925 コンペアでの変更により
		if (!ExtractFileName(InverseGainMapFile2->Text).AnsiPos("m(m33s33)_BmapLP") && !ExtractFileName(InverseGainMapFile2->Text).AnsiPos("m(m33s33)_Gain00.xxx")){ //ttsuji 140925 コンペアでの変更により
			if (IDYES == CApplication::GetInstance()->MessageBox("平行四辺形シフト元画像のファイル名に\"m(m33s33)_BmapLP\"、または\"m(m33s33)_Gain00.xxx\"という文字列が含まれていません。計算を中止しますか？", "警告", MB_YESNO)){
				return false;
			}
		}

		ParallelogramShiftCalibImage(in_str);
		ProgressStepIt();
	}
#endif
	return true;
}

//順ゲイン補正
bool CTLPCorrectionF::forwardGainCorrection(TTMA& io_img, string in_str){
#if 0
#define _RANDOM_SUPERIMPOSE

//順ゲイン補正
	if (m_bForwardGainExe) {

		TTMA ForwardGainImg;
		ForwardGainImg.ReadTma(ForwardGainMapFile->Text);

#ifdef _RANDOM_SUPERIMPOSE
    	//乱数ふりかけ順ゲイン補正（年輪ムラ対策）
		int iClipQL = 100000;//dummyの値
		//感度補正係数（dummyの値）
		int iCoeSense = 32768;//U1.15(32768が1に相当（SPCに合わせるために切り捨て！）)

//		if (!io_img.bForwardGainCorrectionLP(ForwardGainImg, iCoeSense, iClipQL)){
		if (!io_img.bForwardGainCorrectionLP(ForwardGainImg, iCoeSense, iClipQL, m_bbExeForwardSensCor, dKando32768, iNormBitSense)){
			CApplication::GetInstance()->MessageBox("順ゲイン補正でエラーが発生しました。", "Error", MB_OK);
			return false;
		}
#else
		double dQL = ComboForwardGainNormQL->Text.ToDouble();
		int iThMul = ClipMul->Text.ToInt();
		bool bClipMul = CheckClipMul->Checked;
		if (!dQL) {
			CApplication::GetInstance()->MessageBox("規格化QLが不正です。non-zeroにて指定してください。", "Error", MB_OK);
			return false;
		}
		for (int iS = 0; iS < io_img.SubPixel; iS++){
			for (int iM = 0; iM < io_img.MainPixel; iM++){
				if (!bClipMul) {
					//非補正画像QLが飽和しているとゲイン補正により、キャリブのシンチ段差、ガラス段差を作ってしまい、NG
 #if 0	 //四捨五入
					io_img.SetVal(iS, iM, io_img.Dt[iS][iM] * ForwardGainImg.Dt[iS][iM] / dQL);
 #else	//切捨て（MCコンペア用）
					io_img.Dt[iS][iM] = CLIP((int)(io_img.Dt[iS][iM] * ForwardGainImg.Dt[iS][iM] / dQL), 0, io_img.DtMax);
 #endif
				}
				else{
					//非補正画像QLが飽和しているときの対策（clip処理）
					if (io_img.Dt[iS][iM] >= iThMul) {//ゲイン補正される元画像がiTh以上であれば、QLを変えない。
						continue;
					}
					else{
						io_img.SetVal(iS, iM, io_img.Dt[iS][iM] * ForwardGainImg.Dt[iS][iM] / dQL);
					}
				}
			}
		}
#endif
		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			double dShiftU0   = round(GetValueMatrix(true,  7, 5)) / 10.;
			double dShiftUEnd = round(GetValueMatrix(true,  7, 6)) / 10.;
			double dShiftD0   = round(GetValueMatrix(false, 7, 5)) / 10.;
			double dShiftDEnd = round(GetValueMatrix(false, 7, 6)) / 10.;
			m_StrCor = "f(" +  FloatToStr((float)dShiftU0) + "," + FloatToStr((float)dShiftUEnd) + "," + FloatToStr((float)dShiftD0) + "," + FloatToStr((float)dShiftDEnd) + ")" + m_StrCor;
			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}
		ProgressStepIt();
	}

#endif
	return true;
}

//ガラス段差補正
bool CTLPCorrectionF::glassEdgeCorrection(TTMA& io_img, string in_str){
#if 0
	if (m_bGlassEdgeCorExe) {

		const int iMainMedianGlassProfile = MainMedianGlassProfile->Text.ToInt();
		const bool bDetectKanajakuForGlassCor = DetectKanajakuForGlassCor->Checked;
		const int iThQLSubDiffForScaleSearchGlass = ThQLSubDiffForScaleSearchGlass->Text.ToInt();
		const int iSMargin = SMargin->Text.ToInt();
		if (m_bCorUpperGlassEdge) {
			CheckMemberHensuuGlassEdge(io_img, true);//メンバー変数が入っているかチェックし、入ってなければ計算して格納する
			CheckMemberHensuuSintiEdge(io_img, true);//メンバー変数が入っているかチェックし、入ってなければ計算して格納する
			GlassEdgeCorrection(io_img, in_str, iMainMedianGlassProfile, true, bDetectKanajakuForGlassCor, iThQLSubDiffForScaleSearchGlass, iSMargin);
		}
		if (m_bCorLowerGlassEdge) {
			CheckMemberHensuuGlassEdge(io_img, false);//メンバー変数が入っているかチェックし、入ってなければ計算して格納する
			CheckMemberHensuuSintiEdge(io_img, false);//メンバー変数が入っているかチェックし、入ってなければ計算して格納する
			GlassEdgeCorrection(io_img, in_str, iMainMedianGlassProfile, false, bDetectKanajakuForGlassCor, iThQLSubDiffForScaleSearchGlass, iSMargin);
		}
		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			int iYustart(0), iYuend(0), iYdstart(0), iYdend(0);
			if (CorUpperGlassEdge->Checked) {
				iYustart = round(GetValueMatrix(true,  5, 5) / 10.);
				iYuend   = round(GetValueMatrix(true,  5, 6) / 10.);
			}
			if (CorLowerGlassEdge->Checked) {
				iYdstart = round(GetValueMatrix(false,  5, 5) / 10.);
				iYdend   = round(GetValueMatrix(false,  5, 6) / 10.);
			}
   #if 0
			m_StrCor = "g(" + IntToStr(iYustart) + "," + IntToStr(iYuend) + "," + IntToStr(iYdstart) + "," + IntToStr(iYdend) + "," + MainMedianGlassProfile->Text + ")" + m_StrCor;
			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
   #else 	//シンチ段差前に文字列を短縮し、すべて一括処理しても長すぎないファイル名になるようにする(140908 ttsuji)
			if (bDetectKanajakuForGlassCor) {
				m_StrCor = "g(" + IntToStr(iYustart) + "," + IntToStr(iYuend) + "," + IntToStr(iYdstart) + "," + IntToStr(iYdend) + "," + MainMedianGlassProfile->Text + ",K" + ThQLSubDiffForScaleSearchGlass->Text + ")";
			}
			else{
				m_StrCor = "g(" + IntToStr(iYustart) + "," + IntToStr(iYuend) + "," + IntToStr(iYdstart) + "," + IntToStr(iYdend) + "," + MainMedianGlassProfile->Text + ")";
			}
//			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor);
			in_str = ExtractFilePath(in_str) + m_StrCor + ".tma";
			io_img.SaveTmaTdt(in_str);

   #endif
		}
		ProgressStepIt();
	}
#endif
	return true;
}

//シンチ段差補正
bool CTLPCorrectionF::sintiEdgeCorrection(TTMA& io_img, string in_str){
#if 0
	bool bExistPbScale = false;//Pbスケールの目盛が検出されているかのフラグ
	if (m_bSintiEdgeCorExe) {
		const bool bCorHorShift      = CorHorShift->Checked;
		const int iMainShiftMax      = MainShiftMax->Text.ToInt();
		const int iSubNumMatch       = SubNumMatch->Text.ToInt();
		const int iMainMedianMatch   = MainMedianMatch->Text.ToInt();
		const bool bGPRMatch         = GPRMatch->Checked;
		const int iMainMedianDiffImg = MainMedianDiffImg->Text.ToInt();
		const int iSubMedianDiffImg  = SubMedianDiffImg->Text.ToInt();
		const int iLimitSearchRange  = LimitSearchRange->Text.ToInt();
		const bool bUetyousei        = Uetyousei->Checked;
		const int iUeOffset          = UeOffset->Text.ToInt();
		const int iSitaOffsetOrg     = SitaOffset->Text.ToInt();
		int iSitaOffset = iSitaOffsetOrg;//白帯検出の有無によって変わる場合の変数
		const float fUpperLimitOfPanelSensRatio = (float)UpperLimitOfPanelSensRatio->Text.ToDouble();
		const float fLowerLimitOfPanelSensRatio = (float)LowerLimitOfPanelSensRatio->Text.ToDouble();
		const int iSubRangeOnlyMidPanel = SubRangeOnlyMidPanel->Text.ToInt();
		const int iItplSubOffset     = ItplSubOffset->Text.ToInt();
		const int iMainMedianDCItpl  = MainMedianDCItpl->Text.ToInt();
		const float fThQLRateUD      = (float)ThQLRateUD->Text.ToDouble();
		const float fMagicPara       = (float)MagicPara->Text.ToDouble();
		const bool bDetectKanajakuMemori = DetectKanajakuMemori->Checked;
		const int iMedianHasiSyori = MedianHasiSyori->ItemIndex;
		const int iThQLSubDiffForScaleSearch = ThQLSubDiffForScaleSearch->Text.ToInt();
		const int iSubAveNum = SubAveNum->Text.ToInt();
		const int iMarginXMed = MarginXMed->Text.ToInt();
		const double dKandoTop    = KandoTop->Text.ToDouble();
		const double dKandoCenter = KandoCenter->Text.ToDouble();
       	const int iYSearchEnd = YSearchEnd->Text.ToInt();
/*
#ifdef _TIME
			//計算時間計測
		clock_t	startTime, stopTime;
		startTime = clock();
#endif
*/
		TTMA ImageUp, ImageDown;
		if (m_bCorUpperSintiEdge) {
			if(!ImageUp.ReadTma(FN_UpperImage->Text)){
				CApplication::GetInstance()->MessageBox("シンチ段差補正の上画像ファイルが開けませんでした", "Error", MB_OK);
				return false;
			}
			CheckMemberHensuuSintiEdge(io_img, true);//メンバー変数が入っているかチェックし、入ってなければ計算して格納する
			io_img.LPSintiEdgeCorrection(ImageUp, FN_UpperImage->Text, in_str,
			bCorHorShift, iMainShiftMax, iSubNumMatch, iMainMedianMatch, bGPRMatch,
										iMainMedianDiffImg, iSubMedianDiffImg, iLimitSearchRange, bUetyousei, iUeOffset, iSitaOffset, fUpperLimitOfPanelSensRatio, fLowerLimitOfPanelSensRatio,
										iSubRangeOnlyMidPanel, iItplSubOffset, iMainMedianDCItpl, fThQLRateUD,
										 fMagicPara, bDetectKanajakuMemori, iMedianHasiSyori, iThQLSubDiffForScaleSearch,
										 iSubAveNum, iMarginXMed, dKandoTop, dKandoCenter, iYSearchEnd, bExistPbScale, true);
		}
		if (m_bCorLowerSintiEdge) {
			if(!ImageDown.ReadTma(FN_LowerImage->Text)){
				CApplication::GetInstance()->MessageBox("シンチ段差補正の下画像ファイルが開けませんでした", "Error", MB_OK);
				return false;
			}
			CheckMemberHensuuSintiEdge(io_img, false);//メンバー変数が入っているかチェックし、入ってなければ計算して格納する
			io_img.LPSintiEdgeCorrection(ImageDown, FN_UpperImage->Text, in_str, bCorHorShift, iMainShiftMax, iSubNumMatch, iMainMedianMatch, bGPRMatch,
										iMainMedianDiffImg, iSubMedianDiffImg, iLimitSearchRange, bUetyousei, iUeOffset, iSitaOffset, fUpperLimitOfPanelSensRatio, fLowerLimitOfPanelSensRatio,
										iSubRangeOnlyMidPanel, iItplSubOffset, iMainMedianDCItpl, fThQLRateUD,
										fMagicPara, bDetectKanajakuMemori, iMedianHasiSyori, iThQLSubDiffForScaleSearch,
										iSubAveNum, iMarginXMed, dKandoTop, dKandoCenter, iYSearchEnd, bExistPbScale, true);
		}
/*
#ifdef _TIME
			stopTime = clock();
			double time = (double)(stopTime-startTime)/CLOCKS_PER_SEC;
			Application->MessageBox(("シンチ段差補正：\n" + FloatToStr(time) + " sec").c_str(), "OK", MB_OK);
#endif
*/
		//白帯の有無、長さに応じて短冊横傘のパラメータを設定
		if (m_bAutoCalcTanzakuHeight) {
			const int iYstartUpperTanzaku  = YstartUpperTanzaku->Text.ToInt();
			const int iYendUpperTanzaku    = YendUpperTanzaku->Text.ToInt();
			int iYSintiMin = (iYstartUpperTanzaku < iYendUpperTanzaku) ? iYstartUpperTanzaku : iYendUpperTanzaku - 1;
			int iUpperHeight, iLowerHeight;
			if (iSitaOffsetOrg == iSitaOffset) {
//				iUpperHeight = iLowerHeight = 70;
				iUpperHeight = 150;
				iLowerHeight = 150;//70では不足なので、150に変更(140816 ttsuji)
			}
			else if (iSitaOffset < 100) {
				iUpperHeight = 150;
				iLowerHeight = 200;
			}
			else{
				iUpperHeight = 150;
#if 0
				iLowerHeight = 400;
#else           //VG対応によりパラメータ変更(151022 ttsuji)
				iLowerHeight = 500;
#endif
			}
			DeltaYUpTanzaku->Text = (iUpperHeight < iYSintiMin) ? iUpperHeight : iYSintiMin;
			DeltaYDownTanzaku->Text = iLowerHeight;
			ShiroobiLen->Text = iSitaOffset;
#if 0
			SubMedianTanzaku->Text = IntToStr((int)((DeltaYUpTanzaku->Text.ToInt() + DeltaYDownTanzaku->Text.ToInt() - 1) / 10.) * 10 + 1);//196なら191になる（19.6を切り捨てて10倍して１たす）
#else       //VG対応によりパラメータ変更(151022 ttsuji)
			if (iSitaOffset < 100) {
				SubMedianTanzaku->Text = IntToStr((int)((DeltaYUpTanzaku->Text.ToInt() + DeltaYDownTanzaku->Text.ToInt() - 1) / 10.) * 10 + 1);//196なら191になる（19.6を切り捨てて10倍して１たす）
			}
			else{
             	SubMedianTanzaku->Text = 541;//決めうちの値とする
			}
#endif

			//Pbスケール目盛検出結果に応じてクリップ値設定
			if (bExistPbScale) {
				ClipValTanzaku->Text = IntToStr(200);
			}
			else{
				ClipValTanzaku->Text = IntToStr(500);
			}
		}

		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			io_img.SaveTmaTdt(in_str);//in_strを上書きしているため
		}

		ProgressStepIt();
	}
#endif
	return true;
}

//短冊横傘
bool CTLPCorrectionF::tanzakuYokokasa(TTMA& io_img, string in_str){
#if 0
	if (m_bTanzakuYokokasaExe) {

		const int iDeltaYUpTanzaku     = DeltaYUpTanzaku->Text.ToInt();
		const int iDeltaYDownTanzaku   = DeltaYDownTanzaku->Text.ToInt();
		const int iSubMedianTanzaku    = SubMedianTanzaku->Text.ToInt();
		const int iSubMoveAveTanzaku   = SubMoveAveTanzaku->Text.ToInt();
		const int iClipValTanzaku      = ClipValTanzaku->Text.ToInt();
		const int iComboMainDivTanzaku = ComboMainDivTanzaku->Text.ToInt();
		const int iYstartUpperTanzaku  = YstartUpperTanzaku->Text.ToInt();
		const int iYendUpperTanzaku    = YendUpperTanzaku->Text.ToInt();
		const int iYstartLowerTanzaku  = YstartLowerTanzaku->Text.ToInt();
		const int iYendLowerTanzaku    = YendLowerTanzaku->Text.ToInt();
		const bool bLogYokokasa        = LogYokokasa->Checked;
		const bool bDebugYokokasa      = Debug_Yokokasa->Checked;
		const bool bWriteYokokasaClipValTanzaku = WriteYokokasaClipValTanzaku->Checked;
		const bool bCalcSubLowFrequency = CalcSubLowFrequency->Checked;
		const bool bBlend              = Blend->Checked;
		const int iYRangeNotBlend      = YRangeNotBlend->Text.ToInt();
		const int iShiroobiLen         = ShiroobiLen->Text.ToInt();

		//フィルターのファイル名チェック（処理ミス対策）
#if 0
		if (iClipValTanzaku != 200) {
			if (IDYES == CApplication::GetInstance()->MessageBox("クリップ値が200以外の値です。計算を中止しますか？", "警告", MB_YESNO)){
#else	//パラメータ変更
//		if (iClipValTanzaku != 500) {
		if (iClipValTanzaku != 500 && iClipValTanzaku != 200) {
			if (IDYES == CApplication::GetInstance()->MessageBox("クリップ値が200, 500以外の値です。計算を中止しますか？", "警告", MB_YESNO)){
#endif
				return false;
			}
		}
#if 0
		if (!ExtractFileName(TanzakuYokokasaMainFilter->Text).AnsiPos("0.05")){
			if (IDYES == CApplication::GetInstance()->MessageBox("主フィルターのファイル名に\"0.05\"という文字列が含まれていません。計算を中止しますか？", "警告", MB_YESNO)){
#else	//パラメータ変更
		if (!ExtractFileName(TanzakuYokokasaMainFilter->Text).AnsiPos("0.02")){
			if (IDYES == CApplication::GetInstance()->MessageBox("主フィルターのファイル名に\"0.02\"という文字列が含まれていません。計算を中止しますか？", "警告", MB_YESNO)){
#endif
				return false;
			}
		}
#if 0
		if (!ExtractFileName(TanzakuYokokasaSubFilter->Text).AnsiPos("0.02")){
			if (IDYES == CApplication::GetInstance()->MessageBox("主フィルターのファイル名に\"0.02\"という文字列が含まれていません。計算を中止しますか？", "警告", MB_YESNO)){
#else	//パラメータ変更
		if (!ExtractFileName(TanzakuYokokasaSubFilter->Text).AnsiPos("0.18")){
			if (IDYES == CApplication::GetInstance()->MessageBox("主フィルターのファイル名に\"0.18\"という文字列が含まれていません。計算を中止しますか？", "警告", MB_YESNO)){
#endif
				return false;
			}
		}
		if (!ExtractFileName(TanzakuYokokasaHighCutFilter->Text).AnsiPos("inf")){
			if (IDYES == CApplication::GetInstance()->MessageBox("ハイカットフィルターのファイル名に\"inf\"という文字列が含まれていません。計算を中止しますか？", "警告", MB_YESNO)){
				return false;
			}
		}
		if (!ExtractFileName(TanzakuYokokasaMainFilter2->Text).AnsiPos("0.09")){
			if (IDYES == CApplication::GetInstance()->MessageBox("主フィルター2のファイル名に\"0.09\"という文字列が含まれていません。計算を中止しますか？", "警告", MB_YESNO)){
				return false;
			}
		}
#if 0
		if (!ExtractFileName(TanzakuYokokasaSubFilter2->Text).AnsiPos("0.02")){
			if (IDYES == CApplication::GetInstance()->MessageBox("副フィルター2のファイル名に\"0.02\"という文字列が含まれていません。計算を中止しますか？", "警告", MB_YESNO)){
#else	//パラメータ変更
		if (!ExtractFileName(TanzakuYokokasaSubFilter2->Text).AnsiPos("0.01")){
			if (IDYES == CApplication::GetInstance()->MessageBox("副フィルター2のファイル名に\"0.01\"という文字列が含まれていません。計算を中止しますか？", "警告", MB_YESNO)){
#endif
				return false;
			}
		}
		if (!ExtractFileName(TanzakuYokokasaHighCutFilter2->Text).AnsiPos("0.18")){
			if (IDYES == CApplication::GetInstance()->MessageBox("HighCutフィルター2のファイル名に\"0.18\"という文字列が含まれていません。計算を中止しますか？", "警告", MB_YESNO)){
				return false;
			}
		}



		if (m_bTanzakuYokokasaForUpperEdge) {
#ifdef _TIME
			//計算時間計測
			clock_t	startTime, stopTime;
			startTime = clock();
#endif
			if (!io_img.LPTanzakuYokokasa(in_str, iYstartUpperTanzaku, iYendUpperTanzaku, iDeltaYUpTanzaku, iDeltaYDownTanzaku, iSubMedianTanzaku, iSubMoveAveTanzaku, iClipValTanzaku, iComboMainDivTanzaku, TanzakuYokokasaMainFilter->Text, TanzakuYokokasaSubFilter->Text, TanzakuYokokasaHighCutFilter->Text, bCalcSubLowFrequency, TanzakuYokokasaMainFilter2->Text, TanzakuYokokasaSubFilter2->Text, TanzakuYokokasaHighCutFilter2->Text, bBlend, iYRangeNotBlend, iShiroobiLen, bLogYokokasa, true, bDebugYokokasa, bWriteYokokasaClipValTanzaku)){
				CApplication::GetInstance()->MessageBox("UC短冊横傘でエラーが発生しました", "Error", MB_OK);
				return false;
			}
#ifdef _TIME
			stopTime = clock();
			double time = (double)(stopTime-startTime)/CLOCKS_PER_SEC;
			CApplication::GetInstance()->MessageBox(("短冊横傘：\n" + FloatToStr(time) + " sec").c_str(), "OK", MB_OK);
#endif

		}
		if (m_bTanzakuYokokasaForLowerEdge) {
			if (!io_img.LPTanzakuYokokasa(in_str, iYstartLowerTanzaku, iYendLowerTanzaku, iDeltaYUpTanzaku, iDeltaYDownTanzaku, iSubMedianTanzaku, iSubMoveAveTanzaku, iClipValTanzaku, iComboMainDivTanzaku, TanzakuYokokasaMainFilter->Text, TanzakuYokokasaSubFilter->Text, TanzakuYokokasaHighCutFilter->Text, bCalcSubLowFrequency, TanzakuYokokasaMainFilter2->Text, TanzakuYokokasaSubFilter2->Text, TanzakuYokokasaHighCutFilter2->Text, bBlend, iYRangeNotBlend, iShiroobiLen, bLogYokokasa, false, bDebugYokokasa, bWriteYokokasaClipValTanzaku)){
				CApplication::GetInstance()->MessageBox("LC短冊横傘でエラーが発生しました", "Error", MB_OK);
				return false;
			}
		}

		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			int iFPD1 = ExtractFileName(TanzakuYokokasaSubFilter2->Text).Pos("FPD1_");//"FPD1"を含む場合、その文字列の開始位置を返す
			if (iFPD1) {//"FPD1"を含む場合
#if 1
 #if 0
				m_StrCor = "y(" + ClipValTanzaku->Text + "," + ComboMainDivTanzaku->Text + "," +  ExtractFileName(TanzakuYokokasaSubFilter->Text).SubString(iFPD1+5, 4) + "," + ExtractFileName(TanzakuYokokasaMainFilter->Text).SubString(iFPD1+5, 4) + "," + DeltaYDownTanzaku->Text + "," + IntToStr(iSubMedianTanzaku) + ")";
 #else			//短冊横傘二度掛け
//				m_StrCor = "y(2wo1(m0.09)" + ClipValTanzaku->Text + "," + IntToStr(iSubMedianTanzaku) + ")";
//				m_StrCor = "y(2wo1(m0.09,s0.02)" + ClipValTanzaku->Text + "," + IntToStr(iSubMedianTanzaku) + ")";
				m_StrCor = "y(2wo1(m" + ExtractFileName(TanzakuYokokasaMainFilter2->Text).SubString(iFPD1+5, 4) + ",s" + ExtractFileName(TanzakuYokokasaSubFilter2->Text).SubString(iFPD1+5, 4) + "," + ClipValTanzaku->Text + "," + IntToStr(iSubMedianTanzaku) + ")";
 #endif
#else
				if (false == EdgeDetect->Checked) {
					m_StrCor = "y(" + ClipValTanzaku->Text + "," + ComboMainDivTanzaku->Text + "," +  ExtractFileName(TanzakuYokokasaSubFilter->Text).SubString(iFPD1+5, 4) + "," + ExtractFileName(TanzakuYokokasaMainFilter->Text).SubString(iFPD1+5, 4) + "," + DeltaYDownTanzaku->Text + "," + IntToStr(iSubMedianTanzaku) + ")" + m_StrCor;
				}
				else{
					m_StrCor = "y(" + ClipValTanzaku->Text + "," + ComboMainDivTanzaku->Text + "," +  ExtractFileName(TanzakuYokokasaSubFilter->Text).SubString(iFPD1+5, 4) + "," + ExtractFileName(TanzakuYokokasaMainFilter->Text).SubString(iFPD1+5, 4) + "," + DeltaYDownTanzaku->Text + "," + IntToStr(iSubMedianTanzaku) + ")(" + IntToStr(iDiv1LineKasa) + "," + IntToStr(iThMDifQL) + "," + IntToStr(iThBaseQL) + ")" + m_StrCor;
				}
#endif
			}
			else{//"FPD1"を含まない場合
				m_StrCor = "y(" + ClipValTanzaku->Text + "," + ComboMainDivTanzaku->Text + ")" + m_StrCor;
			}

			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}
		ProgressStepIt();
	}
#endif
	return true;
}

//粒状抑制処理
bool CTLPCorrectionF::formatForSmoothing(TTMA& io_img, string in_str){
#if 0
	if (m_bExecGranularityRemoval) {
#ifdef _TIME
		//計算時間計測
		clock_t	startTime, stopTime;
		startTime = clock();
#endif

		const int iYShiftPlus = YShiftPlus->Text.ToInt();
		const int iYShiftMinus = YShiftMinus->Text.ToInt();

		TSIMARS ImgUpper, ImgLower;
		ImgUpper = io_img;
		ImgLower = io_img;
		FormatForSmoothing(io_img, in_str, ImgUpper, ImgLower, iYShiftPlus, iYShiftMinus);

		const int iMaskSize1 = MaskSize1->Text.ToInt();
		const int iMaskSize2 = MaskSize2->Text.ToInt();
		const int iMaskSize3 = MaskSize3->Text.ToInt();
		const double dThEnergy1 = ThEnergy1->Text.ToDouble();
		const double dThEnergy2 = ThEnergy2->Text.ToDouble();
		const double dThEnergy3 = ThEnergy3->Text.ToDouble();
		const double dSelfWeight1 = SelfWeight1->Text.ToDouble();
		const double dSelfWeight2 = SelfWeight2->Text.ToDouble();
		const double dSelfWeight3 = SelfWeight3->Text.ToDouble();

		//1倍、0.5倍、0.25倍の各周波数画像からノイズ画像を抽出し、合成する
		ImgUpper.LPGranularityReduction(in_str, iMaskSize1, dThEnergy1, dSelfWeight1, iMaskSize2, dThEnergy2, dSelfWeight2, iMaskSize3, dThEnergy3, dSelfWeight3, iYShiftMinus);

		//画像の上下連結
		io_img = ImgUpper;
		io_img.DownConnect(ImgLower);
#ifdef _TIME
		stopTime = clock();
		double time = (double)(stopTime-startTime)/CLOCKS_PER_SEC;
		Application->MessageBox(("粒状抑制処理：\n" + FloatToStr(time) + " sec").c_str(), "OK", MB_OK);
#endif

		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			io_img.SaveTmaTdt(AddPrefix(in_str, "r"));
		}
		ProgressStepIt();
	}
#endif
	return true;
}

//欠陥補正3もどき
bool CTLPCorrectionF::defectArroundSintiEdge2Correction(TTMA& io_img, string in_str){
#if 0
	if (m_bDefect3ArroundSintiEdgeExe) {
		const int iDefect3SubRange    = Defect3SubRange->Text.ToInt();
		const int iDefect3MedianMask1 = Defect3MedianMask1->Text.ToInt();
		const int iDefect3MedianMask2 = Defect3MedianMask2->Text.ToInt();
		const int iDefect3MedianMask3 = Defect3MedianMask3->Text.ToInt();
		const float fDefect3ThQL1     = (float)Defect3ThQL1->Text.ToDouble();
		const float fDefect3ThQL2     = (float)Defect3ThQL2->Text.ToDouble();
		const float fDefect3ThQL3     = (float)Defect3ThQL3->Text.ToDouble();
		const bool bLimitBaseQL       = LimitBaseQL->Checked;
		const int iThBaseQL           = ThBaseQL->Text.ToInt();

		if (m_bCorUpperDefect) {
			CheckMemberHensuuSintiEdge(io_img, true);//メンバー変数が入っているかチェックし、入ってなければ計算して格納する
			io_img.LPCorrectDefectArroundSintiEdge2(in_str, iDefect3SubRange, iDefect3MedianMask1, fDefect3ThQL1, iDefect3MedianMask2, fDefect3ThQL2, iDefect3MedianMask3, fDefect3ThQL3, bLimitBaseQL, iThBaseQL, true);
		}
		if (m_bCorLowerDefect) {
			CheckMemberHensuuSintiEdge(io_img, false);//メンバー変数が入っているかチェックし、入ってなければ計算して格納する
			io_img.LPCorrectDefectArroundSintiEdge2(in_str, iDefect3SubRange, iDefect3MedianMask1, fDefect3ThQL1, iDefect3MedianMask2, fDefect3ThQL2, iDefect3MedianMask3, fDefect3ThQL3, bLimitBaseQL, iThBaseQL, false);
		}

		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
//			m_StrCor = "m(" + Defect3MedianMask3->Text + "," + FloatToStrF((float)Defect3ThQL3->Text.ToDouble(), ffFixed, 3, 2) + "," + Defect3MedianMask2->Text + "," + Defect3MedianMask1->Text + ")" + m_StrCor;//FloatToStrF(x,ffFixed,3,3) → 実数ｘの値を有効数字３桁小数点以下３桁の文字列にする
			m_StrCor = "m(" + Defect3MedianMask3->Text + "," + Defect3MedianMask2->Text + "," + FloatToStrF((float)Defect3ThQL2->Text.ToDouble(), ffFixed, 3, 1) + "," + Defect3MedianMask1->Text + ")" + m_StrCor;//FloatToStrF(x,ffFixed,3,3) → 実数ｘの値を有効数字３桁小数点以下３桁の文字列にする
			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}

		ProgressStepIt();
	}
#endif
	return true;
}

//S値適正化補正(4倍補正)
bool CTLPCorrectionF::adaptSValueCorrection(TTMA& io_img, string in_str){

#if 0
	if (m_bSValueModExe) {

		int iSValueMod = 4;//４倍乗算
		io_img *= iSValueMod;

		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			m_StrCor = "A" + m_StrCor;
			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}

		ProgressStepIt();
	}


	//-----------------------
	//クリップ処理
		if (m_bClipExe) {

			int iClipValue = ClipValue->Text.ToInt();
			double dKando = Kando->Text.ToDouble();

			for (int iS = 0; iS < io_img.SubPixel; iS++){
				for (int iM = 0; iM < io_img.MainPixel; iM++){
					//非補正画像QLが飽和しているときの対策（clip処理）
					if (io_img.Dt[iS][iM] <= iClipValue * dKando) {//QL値がiClipValue * dKando以下であれば、QLを変えない。
						continue;
					}
					else{
						io_img.SetVal(iS, iM, iClipValue * dKando);
					}
				}
			}


			//中間画像出力モードの場合
			if (m_bEachCorImgOut) {
				m_StrCor = "C" + IntToStr(int(iClipValue * dKando + 0.5)) + m_StrCor;
				io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
			}

			ProgressStepIt();
		}
#endif
	return true;
}

//高周波横傘
bool CTLPCorrectionF::highFreqYokokasa(TTMA& io_img, string in_str){

#if 0
	//高周波横傘     //  20141222 Kuwabara ここから   要修正：シンチ段差位置の自動のときの座標読み取りを入れ忘れて固定で処理されている


			const int iHighDeltaYUp        = DeltaYUpTanzaku2->Text.ToInt();
			const int iHighDeltaYDown      = DeltaYDownTanzaku2->Text.ToInt();
			const int iHighClipVal         = ClipValTanzaku2->Text.ToInt();
			const int iHighComboDiv        = ComboMainDivTanzaku2->Text.ToInt();
			const int iHighYstartUpper     = YstartUpperTanzaku->Text.ToInt();//ログ短冊横傘のGUIの流用
			const int iHighYendUpper       = YendUpperTanzaku->Text.ToInt();//ログ短冊横傘のGUIの流用
			const int iHighYstartLower     = YstartLowerTanzaku->Text.ToInt();//ログ短冊横傘のGUIの流用
			const int iHighYendLower       = YendLowerTanzaku->Text.ToInt();//ログ短冊横傘のGUIの流用
			const bool bDebugYokokasa      = Debug_Yokokasa2->Checked;
			const bool bWriteHighClipVal   = WriteYokokasaClipValTanzaku2->Checked;
			const int iHighSubMoveAve       = SubMoveAveTanzaku2->Text.ToInt();

			//フィルターのファイル名チェック（処理ミス対策）
			if (iHighClipVal != 90) {
				if (IDYES == CApplication::GetInstance()->MessageBox("クリップ値が90以外の値です。計算を中止しますか？", "警告", MB_YESNO)){
					return false;
				}
			}
	#if 0
			if (!ExtractFileName(HighFilter->Text).AnsiPos("0.18")){
				if (IDYES == Application->MessageBox("主フィルターのファイル名に\"0.18\"という文字列が含まれていません。計算を中止しますか？", "警告", MB_YESNO)){
	#else	//パラメータ変更
			if (!ExtractFileName(TanzakuYokokasa2MainFilter->Text).AnsiPos("0.18")){
				if (IDYES == CApplication::GetInstance()->MessageBox("主フィルターのファイル名に\"0.18\"という文字列が含まれていません。計算を中止しますか？", "警告", MB_YESNO)){
	#endif
					return false;
				}
			}
			if (!ExtractFileName(TanzakuYokokasa2SubFilter->Text).AnsiPos("0.23")){
				if (IDYES == CApplication::GetInstance()->MessageBox("副フィルターのファイル名に\"0.23\"という文字列が含まれていません。計算を中止しますか？", "警告", MB_YESNO)){
					return false;
				}
			}
			if (!ExtractFileName(TanzakuYokokasa2HighCutFilter->Text).AnsiPos("inf")){
				if (IDYES == CApplication::GetInstance()->MessageBox("ハイカットフィルターのファイル名に\"inf\"という文字列が含まれていません。計算を中止しますか？", "警告", MB_YESNO)){
					return false;
				}
			}

	//		if (!io_img.bLPHighkasa(in_str, iHighYstartUpper, iHighYendLower, iHighDeltaYUp, iHighDeltaYDown, iHighSubMedian, iHighClipVal, iHighComboDiv, TanzakuYokokasa2MainFilter->Text, TanzakuYokokasa2SubFilter->Text, TanzakuYokokasa2HighCutFilter->Text, false, false, bDebugYokokasa, bWriteHighClipVal)){
			if (!io_img.bLPHighkasa(in_str, iHighYstartUpper, iHighYendUpper, iHighDeltaYUp, iHighDeltaYDown, iHighSubMoveAve, iHighClipVal, iHighComboDiv, TanzakuYokokasa2MainFilter->Text, TanzakuYokokasa2SubFilter->Text, TanzakuYokokasa2HighCutFilter->Text, false, false, bDebugYokokasa, bWriteHighClipVal)){
				CApplication::GetInstance()->MessageBox("高周波横傘でエラーが発生しました", "Error", MB_OK);
				return false;
			}

	/* Bottom側はメンテしていない
			if (TanzakuYokokasaForLowerEdge->Checked) {
				if (!io_img.LPTanzakuYokokasa(in_str, iYstartLowerTanzaku, iYendLowerTanzaku, iDeltaYUpTanzaku, iDeltaYDownTanzaku, iSubMedianTanzaku, iClipValTanzaku, iComboMainDivTanzaku, TanzakuYokokasaMainFilter->Text, TanzakuYokokasaSubFilter->Text, TanzakuYokokasaHighCutFilter->Text, bLogYokokasa, false, bDebugYokokasa, bWriteYokokasaClipValTanzaku)){
					Application->MessageBox("LC短冊横傘でエラーが発生しました", "Error", MB_OK);
					return false;
				}
			}
	*/


			//中間画像出力モードの場合
			if (m_bEachCorImgOut) {
				int iFPD1 = ExtractFileName(TanzakuYokokasa2SubFilter->Text).Pos("FPD1_");//"FPD1"を含む場合、その文字列の開始位置を返す
				if (iFPD1) {//"FPD1"を含む場合
	#if 1
	//				m_StrCor = "y(" + ClipValTanzaku->Text + "," + ComboMainDivTanzaku->Text + "," +  ExtractFileName(TanzakuYokokasaSubFilter->Text).SubString(iFPD1+5, 4) + "," + ExtractFileName(TanzakuYokokasaMainFilter->Text).SubString(iFPD1+5, 4) + "," + DeltaYDownTanzaku->Text + "," + IntToStr(iSubMedianTanzaku) + ")" + m_StrCor;
					m_StrCor = "h(" + ClipValTanzaku2->Text + "," + ComboMainDivTanzaku2->Text + "," +  ExtractFileName(TanzakuYokokasa2SubFilter->Text).SubString(iFPD1+5, 4) + "," + ExtractFileName(TanzakuYokokasa2MainFilter->Text).SubString(iFPD1+5, 4) + "," + DeltaYDownTanzaku->Text + ")";
	#else
					if (false == EdgeDetect->Checked) {
						m_StrCor = "h(" + ClipValTanzaku2->Text + "," + ComboMainDivTanzaku2->Text + "," +  ExtractFileName(TanzakuYokokasa2SubFilter->Text).SubString(iFPD1+5, 4) + "," + ExtractFileName(TanzakuYokokasa2MainFilter->Text).SubString(iFPD1+5, 4) + "," + DeltaYDownTanzaku->Text + ")" + m_StrCor;
					}
					else{
						m_StrCor = "h(" + ClipValTanzaku2->Text + "," + ComboMainDivTanzaku2->Text + "," +  ExtractFileName(TanzakuYokokasa2SubFilter->Text).SubString(iFPD1+5, 4) + "," + ExtractFileName(TanzakuYokokasa2MainFilter->Text).SubString(iFPD1+5, 4) + "," + DeltaYDownTanzaku->Text + ")" + m_StrCor;
					}
	#endif
				}
				else{//"FPD1"を含まない場合
					m_StrCor = "h(" + ClipValTanzaku2->Text + "," + ComboMainDivTanzaku2->Text + ")" + m_StrCor;
				}

				io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
			}
			ProgressStepIt();
		}
#endif
	return true;
}

//鮮鋭度フィルタ
bool CTLPCorrectionF::sharpnessFilter(TTMA& io_img, string in_str){

#if 0
	if (m_bSharpnessFilterExe){

		float filter[101];
		fstream file;
		int ct;
		file.open(FilterFileName->Text.c_str(), ios::in);
		if(file.fail()){
			Application->MessageBox(
			  (FilterFileName->Text + " がopenできません").c_str(),
			  "Error", MB_OK);
			return false;
		}

		ct = -1;
		while(!file.eof()){
			++ct;
			file >> filter[ct];
		}
		file.close();

		switch (FilterNormalization->ItemIndex) {
		case 0://1で規格化したフィルター
			io_img.Filter(filter, ct, true);
			io_img.Filter(filter, ct, false);
			break;
		case 1://2冪で規格化したフィルター
			int iFilter[101];
			for (int i = 0; i < ct; i++) {
				if (filter[i] >= 0) {
					iFilter[i] = (int)(filter[i] + 0.5);
				}
				else{
					iFilter[i] = (int)(filter[i] - 0.5);
				}
			}
			io_img.Filter_long(iFilter, ct, true);
			io_img.Filter_long(iFilter, ct, false);
		}

		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			m_StrCor = "f" + m_StrCor;
			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}
		ProgressStepIt();
	}
#endif
	return true;
}

//事後画像回転/反転処理
bool CTLPCorrectionF::setImageDirectionAfter(TTMA& io_img, string in_str){

#if 0
	//-----------------------
	// 上下反転
	//
	if (m_bUpDownExe2){
		io_img.UpDown();

		m_StrCor = "t" + m_StrCor;

		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}
		ProgressStepIt();
	}

	//-----------------------
	// 左右反転
	//
	if (m_bLeftRightExe2){

		io_img.RtoL();

		m_StrCor = "r" + m_StrCor;

		//中間画像出力モードの場合
		if (m_bEachCorImgOut) {
			io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
		}
		ProgressStepIt();
	}
	//-----------------------
	// 180度回転
	//
		if (m_bRot180Exe2){
			io_img.UpDown();
			io_img.RtoL();

			m_StrCor = "rot" + m_StrCor;

			//中間画像出力モードの場合
			if (m_bEachCorImgOut) {
				io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
			}
			ProgressStepIt();
		}
#endif
	return true;
}

//Log/Bit変換
bool CTLPCorrectionF::logBitTrans(TTMA& io_img, string in_str){
#if 0
	//-----------------------
	//-----------------------
	//-----------------------
	// Log変換
	//
		if (m_bLogExe){

			if (!m_bCheckAntiLog) {
	//			io_img.Log(MidVal->Text.ToDouble(), Keta->Text.ToDouble());
	        	//閾値処理付LOG変換
				io_img.LogThreshold(MidVal->Text.ToDouble(), Keta->Text.ToDouble(), LogTh->Text.ToInt());
			}
			else{
	#if 1
				//BENEOスペシャル
				if (14 == io_img.Bit){
					io_img.BitChange(16);
				}
	#endif
				io_img.LogToLinear2(io_img.Bit, MidVal->Text.ToDouble(), Keta->Text.ToInt());
	        }

			//中間画像出力モードの場合
			if (m_bEachCorImgOut) {
				m_StrCor = "L" + m_StrCor;
				io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
			}
			ProgressStepIt();
		}
	//-----------------------
	// Bit変換
	//
		if (m_bBitChangeExe){
			if (!m_bSetBit) {
				io_img.BitChange(Bit->Text.ToInt());
			}
			else{
				io_img.SetBit(Bit->Text.ToInt());//bit数を変えるだけ（QL値はそのまま）
			}

			//中間画像出力モードの場合
			if (m_bEachCorImgOut) {
				m_StrCor = "B" + m_StrCor;
				io_img.SaveTmaTdt(ExtractFilePath(in_str) + m_StrCor + ExtractFileName(in_str));
			}
			//ProgressStepIt();
		}
#endif
		return true;

}



//---------------------------------------------------------------------------
//曝射画像からHough変換を用いて段差検出

#define _DEBUG_HOUGH
#define _DETECT_ONLY_PLUS_EDGE //Hough変換の際に、マイナス方向のエッジはカウントしない
#define _EXCESS_THQL //QL閾値を越えている程度を評価

bool CTLPCorrectionF::detectEdgeWithHough(TTMA& img, string str, bool bUpperEdge)
{

	//キャリブ画像の段差位置をセット（ガラス段差位置はキャリブ画像と曝射画像のシンチ段差位置のシフトから求めるため、キャリブ時のシンチ段差位置情報が必要）
	string strUD;
	double dA, dB, dC, dDummy;
	int iDummy;
	if (bUpperEdge) {
		if (NULL == m_CalibSintiEdgeU.y10) {
//			dA = GetValueMatrix(true,  1, 1);
//			dB = GetValueMatrix(true,  1, 2);
//			dC = GetValueMatrix(true,  1, 3);
			m_CalibSintiEdgeU.MainPixel = img.MainPixel;
			m_CalibSintiEdgeU.SetLPEdgeInf(m_dA1, m_dB1, m_dC1, dDummy, dDummy, dDummy, dDummy, iDummy, iDummy);
		}
		if (NULL == m_CalibGlassEdgeU.y10) {
//			dA = GetValueMatrix(true,  2, 1);
//			dB = GetValueMatrix(true,  2, 2);
//			dC = GetValueMatrix(true,  2, 3);
			m_CalibGlassEdgeU.MainPixel = img.MainPixel;
			m_CalibGlassEdgeU.SetLPEdgeInf(m_dA2, m_dB2, m_dC2, dDummy, dDummy, dDummy, dDummy, iDummy, iDummy);
		}
		strUD = "(UC)";
	}
	else{
		//未対応とする	ttaneichi
		return false;
	}

	const int iYStart = m_iYStart;
	const int iYEnd   = m_iYEnd;
	int iThDiffQL = m_ThDiffQL;
	const int iDetectLineNum = m_iDetectLineNum;
	const int iSubMedianHough = m_iSubMedianHough;

	string strPre = "F" + strUD;
	TTMA FormatOffImg;
	FormatOffImg = img;

	int iFormatH = iYEnd - iYStart + 1;//石丸さん指摘により修正(141125 ttsuji)
	if (iFormatH <= 0) {
		CApplication::GetInstance()->MessageBox("検出開始と終了のY座標を適切に指定してください。", "Error", MB_OK);
		return false;
	}

 #ifdef _EXCESS_THQL
	const float fExcessQLThreshold = m_ThQLMetal;//fExcessQLThresholdを越えたピークは金属エッジの可能性あり、さらに検査する
	const int iSShiftPixel = m_iSShiftPixel;// fExcessQLThresholdを越えたピークの際に、counter画像上でiSShiftPixelだけ下方にシフトした画素を調査する
//	const int iSMargin = 1;//1pix探索する範囲を設けたマージン
	const int iSMargin = m_iSMargin;//金属判定時に探索するガラス段差相当位置の範囲
	const float fRateGlassToSinti = m_fRateGlassToSinti;//counter画像上でiSShiftPixelだけ下方にシフトした画素とシフト前の画素の画素値の比の閾値
	FormatOffImg.Format(iFormatH + iSShiftPixel + iSMargin, FormatOffImg.MainPixel, -iYStart, 0);//上短冊の画像を抽出
 #else
	FormatOffImg.Format(iFormatH, FormatOffImg.MainPixel, -iYStart, 0);//上短冊の画像を抽出
 #endif

 #ifdef _DEBUG_HOUGH
	FormatOffImg.WriteTma( AddPrefix(str, strPre) );
 #endif

	//Log変換  →LOG変換すると、横スジを拾ってしまうことがあり、精度良くない
	const double dMidVal = m_MidValHough;
	const double dKeta = m_KetaHough;
	const int iLogTh = m_LogThHough;
	if (m_bPreLogTrans) {
		strPre = "L" + strPre;
		FormatOffImg.LogThreshold(dMidVal, dKeta, iLogTh);
//		FormatOffImg.BitChange(14);//140926 ttsuji　実装忘れのまま評価したため、コメントアウトする
 #ifdef _DEBUG_HOUGH
		FormatOffImg.WriteTma( AddPrefix(str, strPre) );
 #endif
	}

	//副微分
	strPre = "D" + strPre;
	FormatOffImg.DifSubUp();
 #ifdef _DEBUG_HOUGH
	FormatOffImg.WriteTma( AddPrefix(str, strPre) );
 #endif


	//副微分した後、副Medianして横スジを薄める
	if (m_bExeSubMedianHough) {
		strPre = "m(s" + to_string(m_iSubMedianHough) + ")" + strPre;
		//端処理はどれでも変わらない(Hough変換はy=0はしていないため)
 #if 0
		FormatOffImg.SubMedianAveExpand(iSubMedianHough);//平均値で延長
 #elif 0
		FormatOffImg.SubMedian(iSubMedianHough);//可変長Median
 #elif 0
		FormatOffImg.SubMedian(iSubMedianHough);
		FormatOffImg.SubPixelExpand(iSubMedianHough / 2);//端1画素は2画素目のMedian値で置換
 #else  //上端のy=1をy=2のMedian値で置換(141125 ttsuji)
		FormatOffImg.SubMedian(iSubMedianHough);
		FormatOffImg.SubPixelExpand(iSubMedianHough / 2);//端1画素は2画素目のMedian値で置換
		const int iSReplace = 1;//y=Replace(=1)の画素をy=Replace + 1(=2)のMedian値で置換
		for (int iM = 0; iM < FormatOffImg.MainPixel; iM++){
			FormatOffImg.Dt[iSReplace][iM] = FormatOffImg.Dt[iSReplace + 1][iM];
		}
 #endif
		FormatOffImg.WriteTma( AddPrefix(str, strPre) );
	}


	//Hough変換
//#define PI 3.141592
#define PI 3.141592653589793238462643383279//30桁(コンペア不一致対策として 141205 ttsuji)

	int counter_max;
	const int iTHETA_RESOLUTION = m_ThetaResolution;//thetaの範囲は、0～π[rad]を0から1023までで分割する
	const int iRHO_RESOLUTION = m_RhoMax * 2;//rhoの範囲は、-3000からまで2999まで（HoughImg.MainPixelが2832、HoughImg.SubPixelが大きくても300程度なので、Rhoの絶対値は3000あれば十分）
	const int iLNUMBER_MAX = iDetectLineNum;//検出する直線の数の最大
    const int iCounterTh = m_ThCounter;//カウンターの閾値

	int iTheta, iRho;
	int iThetaMax, iRhoMax, count;
	int iThetaCut, iRhoCut;
	float fExcessThQLMax;

	//直線検出のためのカウンタを用意する
	TIIMG counter;
	counter.NewDt(iTHETA_RESOLUTION, iRHO_RESOLUTION);//NewDt(M,S)の順
	counter.Fill(0);

#ifdef _EXCESS_THQL
	TFIMG fExcessThQL;
	fExcessThQL.NewDt(iTHETA_RESOLUTION, iRHO_RESOLUTION);//NewDt(M,S)の順
	fExcessThQL.Fill(0);
#endif

	//ハフ逆変換（直線）の結果を表示する画像
	TTMA HoughImg;

	//sinとcosのテーブルを用意する
	double* dSin = new double[iTHETA_RESOLUTION];
	double* dCos = new double[iTHETA_RESOLUTION];
	double dPai = PI / iTHETA_RESOLUTION;
// #define _DEBUG_OUT_SIN_COS

 #ifdef _DEBUG_OUT_SIN_COS
	FILE* fp1 = fopen((ExtractFilePath(str) + "dSin.txt").c_str(), "wt");
	FILE* fp2 = fopen((ExtractFilePath(str) + "dCos.txt").c_str(), "wt");
	fprintf(fp1, "iTheta	dSin\n");
	fprintf(fp2, "iTheta	dCos\n");
 #endif
	for(iTheta = 0; iTheta < iTHETA_RESOLUTION; iTheta++){
		double dRad = iTheta * dPai;
		dSin[iTheta] = sin(dRad);
		dCos[iTheta] = cos(dRad);
 #ifdef _DEBUG_OUT_SIN_COS
		fprintf(fp1, "%d,%lf\n", iTheta, dSin[iTheta]);
		fprintf(fp2, "%d,%lf\n", iTheta, dCos[iTheta]);
 #endif
	}
 #ifdef _DEBUG_OUT_SIN_COS
	fclose(fp1);
	fclose(fp2);
 #endif

	int* iYStartCandidate = new int[iLNUMBER_MAX];
	int* iYEndCandidate = new int[iLNUMBER_MAX];
	bool* bCandidate = new bool[iLNUMBER_MAX];
	const int iThDeltaY = m_ThDeltaY;//検出した直線のΔYがこの閾値を越えると（傾きすぎると）候補からはずす（パネル間のヨーイング仕様がiYstart-iYend が±1mm(=±6.7pix)以内であるため10pixで十分）
//	const int iThDeltaY = 10;//検出した直線のΔYがこの閾値を越えると（傾きすぎると）候補からはずす（パネル間のヨーイング仕様がiYstart-iYend が±1mm(=±6.7pix)以内であるため10pixで十分）
//	const int iThDeltaY = 70;//シンチ傾けすぎて撮影したときのためのパラメータ（131023_LP斜入角のいじめ評価用）
    const int iRhoNeighbor = m_RhoNeighbor;
	const int iThetaNeighbor = m_ThetaNeighbor;
	bool bHoughAgain = false;//閾値を変えての2回目のHough変換を実施したかいなかのフラグ

	string strAddPrefix = AddPrefix(str, "HF(" + to_string(iThDiffQL) + "," + to_string(iLNUMBER_MAX) + ")" + strUD);
	ofstream TextFile( ChangeFileExt(strAddPrefix, ".txt").c_str() );

	HoughStart://label
	counter.Fill(0);
	fExcessThQL.Fill(0);
	HoughImg = img;


	//ハフ変換（直線）の実行
	for (int iM = 0;iM < FormatOffImg.MainPixel; iM++){
 #ifdef _EXCESS_THQL
//		for (int iS = 1; iS < iFormatH; iS++){//副微分画像のy=0は無効値なので１から (bug！　3R-15踵骨長手斜入でシンチ段差検出不可)
		for (int iS = 1; iS < FormatOffImg.SubPixel; iS++){//シンチ段差候補がiYEndギリギリにある場合にガラス段差相当位置のピークをチェックできるようにするため(150813 ttsuji)

 #else
//		for(int iS = 0; iS < FormatOffImg.SubPixel; iS++){
		for (int iS = 1; iS < FormatOffImg.SubPixel; iS++){//副微分画像のy=0は無効値なので１から
 #endif
			int iQL;
			switch (m_bDetectOnlyPlusEdge) {
			case true://ベースQLからの差分(スパナ横エッジ誤検出対策として、絶対値はとらない)(141109 ttsuji)
				iQL = FormatOffImg.Dt[iS][iM] - FormatOffImg.DtMax/2 - 1;//ベースQLからの差分(スパナ横エッジ誤検出対策として、絶対値はとらない)(141109 ttsuji)
				break;
			case false:
				iQL = abs(FormatOffImg.Dt[iS][iM] - FormatOffImg.DtMax/2 - 1);//ベースQLからの差分の絶対値
				break;
			}
/*
 #ifdef _DETECT_ONLY_PLUS_EDGE
			int iQL = FormatOffImg.Dt[iS][iM] - FormatOffImg.DtMax/2 - 1;//ベースQLからの差分(スパナ横エッジ誤検出対策として、絶対値はとらない)(141109 ttsuji)
 #else
			int iQL = abs(FormatOffImg.Dt[iS][iM] - FormatOffImg.DtMax/2 - 1);//ベースQLからの差分の絶対値
 #endif
*/
			if (iQL > iThDiffQL) {
				for(iTheta = 0; iTheta < iTHETA_RESOLUTION; iTheta++){
//					iRho = (int)(iM * dCos[iTheta] + iS * dSin[iTheta] + 0.5);
					iRho = round(iM * dCos[iTheta] + iS * dSin[iTheta]);//負の数の四捨五入を修正することでMCと一致(151002 ttsuji)
 #if 1
					if (iRho == 0) {

					}
 #endif
					if (0 > iTheta || iTHETA_RESOLUTION <= iTheta) {
						CApplication::GetInstance()->MessageBox("Thetaのレンジオーバー", "Error", MB_OK);
						delete[] dSin;
						delete[] dCos;
						return false;
					}
					if (0 > iRho + iRHO_RESOLUTION/2 || iRHO_RESOLUTION <= iRho + iRHO_RESOLUTION/2) {
						CApplication::GetInstance()->MessageBox("Rhoのレンジオーバー", "Error", MB_OK);
						delete[] dSin;
						delete[] dCos;
						return false;
					}
					counter.Dt[iTheta][iRho + iRHO_RESOLUTION/2]++;//画像data（Dt[iM][iS]の順）
 #ifdef _EXCESS_THQL
					fExcessThQL.Dt[iTheta][iRho + iRHO_RESOLUTION/2] += (iQL - iThDiffQL);//閾値をどれだけオーバーしているかを記録する　　画像data（Dt[iM][iS]の順）
 #endif
				}
			}
		}
	}


 #ifdef _EXCESS_THQL
	//閾値をどれだけ越えているかの程度を規格化
	for (iTheta = 0; iTheta < iTHETA_RESOLUTION; iTheta++){
		for (iRho = -iRHO_RESOLUTION/2; iRho < iRHO_RESOLUTION/2; iRho++){
			if (!counter.Dt[iTheta][iRho + iRHO_RESOLUTION/2]) {
            	continue;
			}
			fExcessThQL.Dt[iTheta][iRho + iRHO_RESOLUTION/2] /= counter.Dt[iTheta][iRho + iRHO_RESOLUTION/2];//閾値をどれだけオーバーしているかを記録する　　画像data（Dt[iM][iS]の順）
		}
	}
 #endif

 #ifdef _DEBUG_HOUGH
	//Hough空間の画像化
	counter.SaveTmaTdt( AddPrefix(str, "Counter(" + to_string(iThDiffQL) + "," + to_string(iLNUMBER_MAX) + ")" + strPre) );
 #endif


	TextFile << "YStart\t"<< m_iYStart << endl;
	TextFile << "YEnd\t"<< m_iYEnd << endl;
	TextFile << "ThDiffQL\t"<< m_ThDiffQL << endl;
	TextFile << "DetectLineNum\t"<< m_iDetectLineNum << endl;
	TextFile << "PreLogTrans\t"<< m_bPreLogTrans << endl;
 #ifdef _EXCESS_THQL
	TextFile << "id\tiM\tiS\tiCounter\tfExcessThQL\tTheta\tRho\t"<< endl;
 #else
	TextFile << "id\tiM\tiS\tiCounter\t"<< endl;
 #endif

	//ハフ逆変換（直線）の実行
	for (int n = 0; n < iLNUMBER_MAX; n++){
		bCandidate[n] = false;//初期化

		//counterが最大になるtheta_maxとrho_maxを求める
		counter_max = 0;
		for (iTheta = 0; iTheta < iTHETA_RESOLUTION; iTheta++){
			for(iRho = -iRHO_RESOLUTION/2; iRho < iRHO_RESOLUTION/2; iRho++){
				if (counter.Dt[iTheta][iRho + iRHO_RESOLUTION/2] > counter_max){
					counter_max = counter.Dt[iTheta][iRho + iRHO_RESOLUTION/2];
					iThetaMax = iTheta;
					iRhoMax = iRho;
					count = counter_max;
 #ifdef _EXCESS_THQL
					fExcessThQLMax = fExcessThQL.Dt[iTheta][iRho + iRHO_RESOLUTION/2];
 #endif
				}
			}
		}
 /*
		//counter.Dt[iThetaMax][iRhoMax]の近傍を0にする
		for (int i = -iThetaNeighbor; i <= iThetaNeighbor; i++){
			for (int j = -iRhoNeighbor; j <= iRhoNeighbor; j++){
				iThetaCut = iThetaMax + i;
				iRhoCut = iRhoMax + j;
				if (iThetaCut < 0){
					iThetaCut += iTHETA_RESOLUTION;
					iRhoCut = -iRhoCut;
				}
				else if (iThetaCut > iTHETA_RESOLUTION - 1){
					iThetaCut -= iTHETA_RESOLUTION;
					iRhoCut = -iRhoCut;
				}
				counter.Dt[iThetaCut][iRhoCut + iRHO_RESOLUTION/2] = 0;//削除する
			}
		}
*/

/*		//直線を形成するピクセルがMainPixelのある一定倍未満であったら検出しない
		if (count < FormatOffImg.MainPixel*0.6){
			continue;
		}
*/

		//逆ハフ変換（直線）した結果を表示する
		int iM, iS;
		if (iThetaMax != 0){//垂直の線を除く
			for (iM = 0; iM < FormatOffImg.MainPixel; iM++){
				iS = (int)((iRhoMax - iM * dCos[iThetaMax]) / dSin[iThetaMax] + 0.5);
 #if 0 //コメントアウトする（検出した直線の情報がテキスト出力されないため） (151002 ttsuji)
//if 1
				if (iS >= FormatOffImg.SubPixel || iS < 0){
					continue;
				}
//				FormatOffImg.SetVal(iS, iM, FormatOffImg.DtMax);
				HoughImg.SetVal(iS + iYStart, iM, HoughImg.DtMax);
 #endif

				if (0 == iM){
					iYStartCandidate[n] = iS + iYStart;
 #ifdef _EXCESS_THQL
//					TextFile << n << "\t" << iM << "\t" << iS + iYStart << "\t" << counter_max << "\t" << fExcessThQL.Dt[iThetaMax][iRhoMax + iRHO_RESOLUTION/2] << "\t" << iThetaMax << "\t" << iRhoMax + iRHO_RESOLUTION/2 << endl;
					TextFile << n << "\t" << iM << "\t" << iS + iYStart << "\t" << counter_max << "\t" << fExcessThQLMax << "\t" << iThetaMax << "\t" << iRhoMax + iRHO_RESOLUTION/2 << endl;
 #else
					TextFile << n << "\t" << iM << "\t" << iS + iYStart << "\t" << counter_max << endl;
 #endif
				}
				else if (FormatOffImg.MainPixel - 1 == iM) {
					iYEndCandidate[n] = iS + iYStart;
 #ifdef _EXCESS_THQL
//					TextFile << n << "\t" << iM << "\t" << iS + iYStart << "\t" << counter_max << "\t" << fExcessThQL.Dt[iThetaMax][iRhoMax + iRHO_RESOLUTION/2] << "\t" << iThetaMax << "\t" << iRhoMax + iRHO_RESOLUTION/2 << endl;
					TextFile << n << "\t" << iM << "\t" << iS + iYStart << "\t" << counter_max << "\t" << fExcessThQLMax << "\t" << iThetaMax << "\t" << iRhoMax + iRHO_RESOLUTION/2 << endl;
 #else
					TextFile << n << "\t" << iM << "\t" << iS + iYStart << "\t" << counter_max << endl;
 #endif
				}
			}
		}
		if (iThetaMax != iTHETA_RESOLUTION/2){//水平の線を除く
			for (iS = 0; iS < FormatOffImg.SubPixel; iS++){
				iM = (int)((iRhoMax - iS * dSin[iThetaMax]) / dCos[iThetaMax] + 0.5);
 #if 0 //コメントアウトする（検出した直線の情報がテキスト出力されないため） (151002 ttsuji)
//if 1
				if (iM >= FormatOffImg.MainPixel || iM < 0){
					continue;
				}
//				FormatOffImg.SetVal(iS, iM, FormatOffImg.DtMax);
				HoughImg.SetVal(iS + iYStart, iM, HoughImg.DtMax);
 #endif
/*				if (0 == iM || FormatOffImg.MainPixel - 1 == iM) {
					TextFile << n << "\t" << iM << "\t" << iS + iYStart << endl;
				}*/
				if (0 == iM){
					iYStartCandidate[n] = iS + iYStart;
 #ifdef _EXCESS_THQL
					TextFile << n << "\t" << iM << "\t" << iS + iYStart << "\t" << counter_max << "\t" << fExcessThQL.Dt[iThetaMax][iRhoMax + iRHO_RESOLUTION/2] << "\t" << iThetaMax << "\t" << iRhoMax + iRHO_RESOLUTION/2 << endl;
 #else
					TextFile << n << "\t" << iM << "\t" << iS + iYStart << "\t" << counter_max << endl;
 #endif
				}
				else if (FormatOffImg.MainPixel - 1 == iM) {
					iYEndCandidate[n] = iS + iYStart;
 #ifdef _EXCESS_THQL
					TextFile << n << "\t" << iM << "\t" << iS + iYStart << "\t" << counter_max << "\t" << fExcessThQL.Dt[iThetaMax][iRhoMax + iRHO_RESOLUTION/2] << "\t" << iThetaMax << "\t" << iRhoMax + iRHO_RESOLUTION/2 << endl;
 #else
					TextFile << n << "\t" << iM << "\t" << iS + iYStart << "\t" << counter_max << endl;
 #endif
				}
			}
		}

 #ifdef _EXCESS_THQL //150813 3R-15（踵骨長手斜入）のシンチ段差誤検出対応
//		if (iYStartCandidate[n] > iYEnd + 1 || iYEndCandidate[n] > iYEnd + 1) {//検出したｙ座標が指定した探索範囲の上限iYEnd＋１を越える場合は無効とし、nもデクリメントする（150813 ttsuji）
		if (iYStartCandidate[n] > iYEnd || iYEndCandidate[n] > iYEnd) {//検出したｙ座標が指定した探索範囲の上限iYEndを越える場合は無効とする。nデクリメントはしない（151002 ttsuji）
  #if 0 	//nデクリメントすると、なかなかｙレンジを満たす直線候補が見つからないため、コメントアウト（151002 ttsuji）
			n--;//検出した直線は無効とし、nのカウントにも含めない
  #endif
			TextFile << "invalid iS" << endl;
		}
		else if (abs(iYEndCandidate[n] - iYStartCandidate[n]) > iThDeltaY) {//傾き大の場合（検出した直線の傾きにて候補か否かを判別）
 #else
		if (abs(iYEndCandidate[n] - iYStartCandidate[n]) > iThDeltaY) {//傾き大の場合（検出した直線の傾きにて候補か否かを判別）
 #endif
			bCandidate[n] = false;

 #if 0 //test(傾き大のものはN数から除外するよう修正)(160511 ttsuji)
			n--;
 #endif
		}
		else{//傾き小の場合
			switch (m_bExcessThQL) {
				case true://微分QL閾値を大幅に超えたエッジの場合は金属エッジの可能性あるため、画像の下方にガラス段差らしきピークがあるかcounter画像をサーチ
					if (fExcessThQLMax >= fExcessQLThreshold){
						int iCounterTh = round(counter.Dt[iThetaMax][iRhoMax + iRHO_RESOLUTION/2] * fRateGlassToSinti);
						TextFile << n << "\tOverExcessQLThreshold\t" << fExcessQLThreshold << "\tiCounterTh\t" << iCounterTh << endl;
						for(int iRhoSearch = iRhoMax + iSShiftPixel - iSMargin; iRhoSearch <= iRhoMax + iSShiftPixel + iSMargin; iRhoSearch++){
//							TextFile << n << "\tcounter.Dt[iThetaMax][iRhoSearch + iRHO_RESOLUTION/2]\t" << counter.Dt[iThetaMax][iRhoSearch + iRHO_RESOLUTION/2] << endl;
							TextFile << n << "\tiThetaMax\t" << iThetaMax << "\tiRhoSearch\t" << iRhoSearch << "\tcounter.Dt[iThetaMax][iRhoSearch + iRHO_RESOLUTION/2]\t" << counter.Dt[iThetaMax][iRhoSearch + iRHO_RESOLUTION/2] << endl;
							if (counter.Dt[iThetaMax][iRhoSearch + iRHO_RESOLUTION/2] >= iCounterTh) {//ガラス段差相当の位置にシフトしたcounter画素値が大きければ金属エッジではないとみなす（141203 ttsuji）
								bCandidate[n] = true;
 #if 0 //ガラス段差相当位置の画素値を全チェックするため、とりあえず全部ループを回す（計算速度は二の次）
								break;
 #endif
							}
						}
					}
					else{//微分QL閾値が顕著には大きくない場合はシンチ段差とする
						bCandidate[n] = true;
					}
					break;

				case false:
					bCandidate[n] = true;
					break;
			}
/*
 #ifdef _EXCESS_THQL
			//微分QL閾値を大幅に超えたエッジの場合は金属エッジの可能性あるため、画像の下方にガラス段差らしきピークがあるかcounter画像をサーチ
			if (fExcessThQLMax >= fExcessQLThreshold){
				int iCounterTh = round(counter.Dt[iThetaMax][iRhoMax + iRHO_RESOLUTION/2] * fRateGlassToSinti);
				TextFile << n << "\tOverExcessQLThreshold\t" << fExcessQLThreshold << "\tiCounterTh\t" << iCounterTh << endl;
				for(int iRhoSearch = iRhoMax + iSShiftPixel - iSMargin; iRhoSearch <= iRhoMax + iSShiftPixel + iSMargin; iRhoSearch++){
					TextFile << n << "\tcounter.Dt[iThetaMax][iRhoSearch + iRHO_RESOLUTION/2]\t" << counter.Dt[iThetaMax][iRhoSearch + iRHO_RESOLUTION/2] << endl;
					if (counter.Dt[iThetaMax][iRhoSearch + iRHO_RESOLUTION/2] >= iCounterTh) {//ガラス段差相当の位置にシフトしたcounter画素値が大きければ金属エッジではないとみなす（141203 ttsuji）
						bCandidate[n] = true;
						break;
					}
				}
			}
			else{//微分QL閾値が顕著には大きくない場合はシンチ段差とする
				bCandidate[n] = true;
			}
 #else
			bCandidate[n] = true;
 #endif
*/
		}

		//counter.Dt[iThetaMax][iRhoMax]の近傍を0にする
		for (int i = -iThetaNeighbor; i <= iThetaNeighbor; i++){
			for (int j = -iRhoNeighbor; j <= iRhoNeighbor; j++){
				iThetaCut = iThetaMax + i;
				iRhoCut = iRhoMax + j;
				if (iThetaCut < 0){
					iThetaCut += iTHETA_RESOLUTION;
					iRhoCut = -iRhoCut;
				}
				else if (iThetaCut > iTHETA_RESOLUTION - 1){
					iThetaCut -= iTHETA_RESOLUTION;
					iRhoCut = -iRhoCut;
				}
				counter.Dt[iThetaCut][iRhoCut + iRHO_RESOLUTION/2] = 0;//削除する
			}
		}


		//直線を形成するピクセルが60個未満になったら表示しない
		if (count < iCounterTh){
			break;
		}
	}
#ifdef _DEBUG_HOUGH
//	FormatOffImg.WriteTma( AddPrefix(str, "HF(" + IntToStr(iThDiffQL) + "," + IntToStr(iLNUMBER_MAX) + ")" + strPre) );
#endif

#if 1
	HoughImg.WriteTma( AddPrefix(str, "HF(" + to_string(iThDiffQL) + "," + to_string(iLNUMBER_MAX) + ")" + strUD) );
#endif

	//LNUMBER_MAX個の候補から１つを選抜する
	int iYStartDetect(-1), iYEndDetect(-1);//最終的に検出されたシンチ段差のY座標
	int iDeltaYSintiGlass = 17;//シンチ段差とガラス段差のY差分の大体の値
	int iYKousaSintiGlass = 3;//シンチをTFTへ貼り付けるときの貼り付け公差0.5mm = 3pix
	bool bFlag = false;//
	if (m_bGlassSintiPriority) {
		for (int n = 0; n < iLNUMBER_MAX; n++){
			if (!bCandidate[n]) {
				continue;
			}
			for (int m = n + 1; m < iLNUMBER_MAX; m++){
				int idYStart = iYStartCandidate[n] - iYStartCandidate[m];
				int idYEnd = iYEndCandidate[n] - iYEndCandidate[m];
				//自分よりもyの負の方向にiDeltaYSintiGlass付近の線分があれば、その線分がシンチ段差とみなす
				if (iDeltaYSintiGlass - iYKousaSintiGlass < idYStart && idYStart < iDeltaYSintiGlass + iYKousaSintiGlass &&
					iDeltaYSintiGlass - iYKousaSintiGlass < idYEnd && idYEnd < iDeltaYSintiGlass + iYKousaSintiGlass) {
					iYStartDetect = iYStartCandidate[m];
					iYEndDetect = iYEndCandidate[m];
					bFlag = true;
				}
			}
		}
	}
	if (!bFlag) {
		for (int n = 0; n < iLNUMBER_MAX; n++){
			if (bCandidate[n]) {
				iYStartDetect = iYStartCandidate[n];
				iYEndDetect = iYEndCandidate[n];
				break;
			}
		}
	}

	TextFile << "\niYStartDetect\tiYEndDetect" << endl;
	TextFile << iYStartDetect << "\t" << iYEndDetect << endl;


	//候補が求まらなかった場合は一回だけ閾値を100にして再度Hough変換
	if (iYStartDetect < 0 || iYEndDetect < 0) {
		switch (bHoughAgain) {
		case false://Hough変換2回目を未実施の場合
			iThDiffQL = 100;
			bHoughAgain = true;
			TextFile << endl;
			goto HoughStart;
		case true://既にHough変換2回目を実施済みの場合
			//ゲイン伽リブのa,b,cを出力する
			//未実装

			delete[] dSin;
			delete[] dCos;
			delete[] iYStartCandidate;
			delete[] iYEndCandidate;
			TextFile.close();
			//imgのメンバ変数の値の設定

			//ViewGUILowerEdgeDetectionResult(img, false);//GUIに検出結果を表示
			return true;
		}
	}

	delete[] dSin;
	delete[] dCos;
	delete[] iYStartCandidate;
	delete[] iYEndCandidate;
	TextFile.close();

	//シンチ段差とガラス段差の位置を検出
//	const int iRangeSearch = 2;//Hough変換の検出pix誤差で規定
	const int iRangeSearch = m_SubExpandRange2;//Hough変換の検出pix誤差で規定
	int iY0 = iYStartDetect > iYEndDetect ? iYEndDetect - iRangeSearch : iYStartDetect - iRangeSearch;
	int iYE = iYStartDetect > iYEndDetect ? iYStartDetect + iRangeSearch : iYEndDetect + iRangeSearch;
	int iX0 = m_XStart;
	int iXE = m_XEnd;
	int iMMedBeforeSubDiff = m_MMedBeforeSubDiff;
	int iMMedAfterSubDiff = m_MMedAfterSubDiff;
	int iThBaseQLSubDiff = m_ThBaseQLSubDiff;

	CLPCorrectionS	LPCorrectObj;
	LPCorrectObj.SetTTMA(&img);
	LPCorrectObj.DetectLPSintiGlassEdgePosition(str, iY0, iYE, iX0, iXE, 1, iMMedBeforeSubDiff, iMMedAfterSubDiff, iThBaseQLSubDiff, true, false);//Hough変換結果を元に副微分処理実行
	//ViewGUIUpperEdgeDetectionResult(img, false);//GUIに検出結果を表示			//TODO Edge情報をメンバーへコピーする

	return true;
}
