/*
 * CLPCorrectionS.cpp
 *
 *  Created on: 2021/04/06
 *      Author: 10097663
 */

#include "CLPCorrectionS.h"
#include "CApplication.h"
#include "CTIIMG.h"
#include "CLeastSquare.h"
#include "utility.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <limits>

using namespace std;

CLPCorrectionS::CLPCorrectionS() {
	m_pTTMA = NULL;

}

CLPCorrectionS::~CLPCorrectionS() {
	// TODO Auto-generated destructor stub
}

CLPCorrectionS::CLPCorrectionS(TTMA *pTTMA) {
	m_pTTMA = pTTMA;

}

//TTMAオブジェクトの設定
void CLPCorrectionS::SetTTMA(TTMA *pTTMA){

	m_pTTMA = pTTMA;

}

void CLPCorrectionS::DetectLPSintiGlassEdgePosition(string FileName, int iY0, int iYE, int iX0, int iXE, int iSMedBeforeSubDiff, int iMMedBeforeSubDiff, int iMMedAfterSubDiff, int iThBaseQLSubDiff, bool bUpperEdge, bool bGainCalib){
	TTMA SintiProfile, GlassProfile;
	TTMA MedSintiProfile, MedGlassProfile;

//	const int iEdge=17; // シンチエッジとガラスエッジの座標差
	const int iEdge=13; // シンチエッジとガラスエッジの座標差(150305 ttsuji　変更漏れ)

	string strUL  = (bUpperEdge) ? "U" : "L";

	ofstream mofile( ChangeFileExt(FileName, "_" + strUL + "(" + to_string(iY0) + "-" + to_string(iYE) + ")"".txt").c_str() );
	mofile << ExtractFileName(FileName).c_str() << "\tsinti 10times\t" << endl;	//プロファイルを主方向メディアンしたものも表示（ttsuji 131025）


#if 0
	//Log変換(ttsuji 131024)  →Log変換すると、骨のエッジは検出しなくなるが、逆に横スジを拾ってしまうようになった。
	const double dMidVal = 948.;
	const double dKeta = 4.;
	const int iLogTh = 60000;
	Image.LogThreshold(dMidVal, dKeta, iLogTh);
	Image.WriteTma( AddPrefix(FileName, "L") );
#endif

	TTMA Image;
	Image = *m_pTTMA;
#if 1
	//可変長メディアン
	if (bGainCalib) {
		Image.SubMedian(iSMedBeforeSubDiff);//ゲインキャリブのときのみ、2ライン横線欠陥を除去するために導入
	}
	Image.MainMedian(iMMedBeforeSubDiff);//これをはずすと検出段差にノイズが多くNG（ttsuji 130928）
//	Image.WriteTma( AddPrefix(FileName, "MMed" + IntToStr(iMMedBeforeSubDiff)));
#else
	//平均延長メディアン
	if (bGainCalib) {
		Image.SubMedianAveExpand(iSMedBeforeSubDiff);//ゲインキャリブのときのみ、2ライン横線欠陥を除去するために導入
	}
	Image.MainMedianAveExpand(iMMedBeforeSubDiff);//これをはずすと検出段差にノイズが多くNG（ttsuji 130928）
#endif


	if ( iXE >= Image.MainPixel || iXE < 0 ){
		iXE = Image.MainPixel - 1;
	}

	SintiProfile.NewDt(1, Image.MainPixel, Image.Bit);
	SintiProfile.FillValue(0);
	GlassProfile.NewDt(1, Image.MainPixel, Image.Bit);
	GlassProfile.FillValue(0);
	MedSintiProfile.NewDt(1, Image.MainPixel, Image.Bit);
	MedSintiProfile.FillValue(0);
	MedGlassProfile.NewDt(1, Image.MainPixel, Image.Bit);
	MedGlassProfile.FillValue(0);


	for (int iM = iX0; iM <= iXE; iM++){
		//まずはシンチエッジを探す
		int dQL=0;
		int iedge=0;    // シンチエッジ位置
		int dQLmax=0;

 #if 0  //debug
		if (iM == 1055) {
        	int  ccc = 0;
		}
 #endif

#if 0   //腰椎RS200などの大線量の素抜けでは副微分絶対値ではシンチ段差が検出できないため
		for (int iS = iY0; iS < iYE - 1; iS++){
//		  dQL = Image.Dt[iS+1][iM]-Image.Dt[iS][iM];
		  dQL = abs(Image.Dt[iS+1][iM]-Image.Dt[iS][iM]);//完全に単調減少しかない画像パターンのときは負の値となるため、これを考慮して絶対値をつけた。(ttsuji 131021)
		  if ( dQL > dQLmax ){
			iedge = iS;
			dQLmax = dQL;
		  }
		}

		if (0 == dQLmax) {//dQLmax=0のとき（iY0～iYEまで同じQL値で最大値がなかった場合）←完全に65535QLで飽和している場合は問題ないが、60000QL前後で微妙な場合でもシンチ段差が正しく検出できないため、これではNG。
//			SintiProfile.Dt[0][iM] = (int)(10 * (iY0 + iYE) / 2.);//画像data（Dt[iS][iM]の順）
			SintiProfile.Dt[0][iM] = 0;//0にすることで、副微分最大値が見つからなかったことを示す。主Medianフィルターの対象としたいため、あえてダミーの値を代入する。
		}
		else{
			// 半値を探す
			int i10edge=0; // シンチエッジ位置×１０
//			float wQL=0.;
			double wQL=0.;
			wQL = (Image.Dt[iedge+4][iM]+Image.Dt[iedge-3][iM])/2.;
			i10edge=int(10.*( iedge + (wQL-Image.Dt[iedge][iM])/(dQLmax)   ) ) ;
			SintiProfile.Dt[0][iM] = i10edge;//画像data（Dt[iS][iM]の順）
//			mofile << iM << "\t" << i10edge << "\t" << dQLmax << endl;
		}
#else   //完全に65535QLで飽和しておらず60000QL前後でシンチ段差が正しく検出できない場合の対策
 #ifndef _SUNUKE_NIJUUDANSA
		for (int iS = iY0; iS < iYE - 1; iS++){
			if (Image.Dt[iS][iM] >= iThBaseQLSubDiff) {//iY0-iYEの副領域の中に1画素でもiThBaseQLSubDiff以上の画素があった場合は、そのｘは飽和領域とみなして、副微分は計算しない
				SintiProfile.Dt[0][iM] = 0;//0にすることで、副微分最大値が見つからなかったことを示す。主Medianフィルターの対象としたいため、あえてダミーの値を代入する。
				dQLmax = 0;// 140719 ttsuji　石丸さんに指摘されて追加
				break;// 140719 ttsuji　石丸さんに指摘されて追加
			}
			else{//飽和していない領域とみなして、副微分を計算する
				dQL = abs(Image.Dt[iS+1][iM]-Image.Dt[iS][iM]);//完全に単調減少しかない画像パターンのときは負の値となるため、これを考慮して絶対値をつけた。(ttsuji 131021)
				if ( dQL > dQLmax ){
					iedge = iS;
					dQLmax = dQL;
				}
			}
		}
 #else  //素抜けに偽の二重段差を生成する不具合対策（141116 tsuji）
		int iS = iY0;
		bool bFlagSaturation = false;
		while (Image.Dt[iS][iM] >= iThBaseQLSubDiff) {//iY0-iYEの副領域の中に1画素でもiThBaseQLSubDiff以上の画素があった場合は、そのｘは飽和領域とみなして、副微分は計算しない
			if (iS < iYE - 1) {
				iS++;
			}
			else{ //iY0 <= iS <= iYE-2 の範囲内のすべての画素が閾値を越えている場合
				bFlagSaturation = true;
				dQLmax = 0;// 140719 ttsuji　石丸さんに指摘されて追加
				break;// 140719 ttsuji　石丸さんに指摘されて追加
			}
		}

		for (int iS = iY0; iS < iYE - 1; iS++){
			if (bFlagSaturation) {
				break;
			}
			else{//飽和していない領域とみなして、副微分を計算する
				dQL = abs(Image.Dt[iS+1][iM]-Image.Dt[iS][iM]);//完全に単調減少しかない画像パターンのときは負の値となるため、これを考慮して絶対値をつけた。(ttsuji 131021)
				if ( dQL > dQLmax ){
					iedge = iS;
					dQLmax = dQL;
				}
			}
		}
 #endif


 #ifdef _SUNUKE_NIJUUDANSA
		if (bFlagSaturation) {//bFlagSaturation=trueのとき（iY0～iYEまでThBaseQL以上であった場合）
 #else
		if (0 == dQLmax) {//dQLmax=0のとき（iY0～iYEまで同じQL値で最大値がなかった場合）
 #endif
 //			SintiProfile.Dt[0][iM] = (int)(10 * (iY0 + iYE) / 2.);//画像data（Dt[iS][iM]の順）
			SintiProfile.Dt[0][iM] = 0;//0にすることで、副微分最大値が見つからなかったことを示す。主Medianフィルターの対象としたいため、あえてダミーの値を代入する。
		}
		else{
			// 半値を探す
			int i10edge=0; // シンチエッジ位置×１０
			float wQL=0.;
			wQL = (Image.Dt[iedge+4][iM]+Image.Dt[iedge-3][iM])/2.;
 #if 0
			i10edge = int(10. * (iedge + (wQL - Image.Dt[iedge][iM]) / (dQLmax))) ;
 #else
			if (!dQLmax) {//dQLmax=0のとき（iY0～iYEまで同じQL値で最大値がなかった場合）(実行エラー対策として追加 141201 ttsuji)
				SintiProfile.Dt[0][iM] = 0;//0にすることで、副微分最大値が見つからなかったことを示す。主Medianフィルターの対象としたいため、あえてダミーの値を代入する。
			}
			else{
				i10edge = round(10. * (iedge + (wQL - Image.Dt[iedge][iM]) / (dQLmax)));//四捨五入に変更 (140912 ttsuji)
				SintiProfile.Dt[0][iM] = i10edge;//画像data（Dt[iS][iM]の順）
//				mofile << iM << "\t" << i10edge << "\t" << dQLmax << endl;
			}
 #endif
		}

#endif

		if (bGainCalib){//ゲインキャリブ時は続けてガラス段差を探索する（撮影時は一旦シンチ段差位置を確定させてから、キャリブ時のシンチ段差位置からのシフトから、撮影画像のガラス段差位置を求める）
			int edge=0;
			dQLmax=0;
//			for( int iS=iedge-5+iEdge; iS<iedge+4+iEdge; iS++){
			for( int iS=iedge-5+iEdge; iS <= iedge+5+iEdge; iS++){//150305 ttsuji　変更漏れ
//				dQL = Image.Dt[iS+1][iM]-Image.Dt[iS][iM];
				dQL = abs(Image.Dt[iS+1][iM]-Image.Dt[iS][iM]);//完全に単調減少しかない画像パターンのときは負の値となるため、これを考慮して絶対値をつけた。(ttsuji 131021)
				if( dQL>dQLmax ) {
					edge=iS;
					dQLmax=dQL;
				}
			}

			if (0 == dQLmax) {//dQLmax=0のとき（iY0～iYEまで同じQL値で最大値がなかった場合）
//				GlassProfile.Dt[0][iM] = SintiProfile.Dt[0][iM] + 10 * iEdge;//画像data（Dt[iS][iM]の順）
				GlassProfile.Dt[0][iM] = 0;//0にすることで、副微分最大値が見つからなかったことを示す。主Medianフィルターの対象としたいため、あえてダミーの値を代入する。
			}
			else{
				// 半値を探す
				int i10edge_g=0; // ガラスエッジ位置×１０
				float wQL=0.;
				wQL = (Image.Dt[edge+4][iM]+Image.Dt[edge-3][iM])/2.;
 #if 0
				i10edge_g = int(10.*( edge + (wQL-Image.Dt[edge][iM])/(dQLmax)   ) )  ;
 #else			//四捨五入に変更 (140912 ttsuji)
				i10edge_g = round(10. * (edge + (wQL - Image.Dt[edge][iM]) / (dQLmax)))  ;
 #endif
				GlassProfile.Dt[0][iM] = i10edge_g;//画像data（Dt[iS][iM]の順）
//				mofile << iM << "\t" << i10edge << "\t" << dQLmax << endl;
//				mofile << iM << "\t" << i10edge << "\t" << i10edge_g << endl;
//				mofile << iM << "\t" << i10edge << "\t" << 10*iedge << "\t" << i10edge_g << "\t" << 10*edge<< endl;//10倍精度の検証（ttsuji 130928）
			}
		}
	}

//	int iMMedian = 201;
//	int iMMedian = 501;
	MedSintiProfile = SintiProfile;
	MedGlassProfile = GlassProfile;
#if 1
	MedSintiProfile.MainMedian(iMMedAfterSubDiff);//可変長メディアン
#else
	MedSintiProfile.MainMedianAveExpand(iMMedAfterSubDiff);//平均値延長メディアン
#endif
//	MedSintiProfile.WriteTma( AddPrefix(FileName, "Med" + IntToStr(iMMedian) + "SintiProfile(" + YStart->Text + "-" + YEnd->Text + ")") );
#if 1
	MedGlassProfile.MainMedian(iMMedAfterSubDiff);
#else
	MedGlassProfile.MainMedianAveExpand(iMMedAfterSubDiff);//平均値延長メディアン
#endif
//	MedGlassProfile.WriteTma( AddPrefix(FileName, "Med" + IntToStr(iMMedian) + "GlassProfile(" + YStart->Text + "-" + YEnd->Text + ")") );

	//テキストファイルに書き込み
	for( int iM=iX0; iM<=iXE; iM++){
		mofile << iM << "\t" << SintiProfile.Dt[0][iM] << "\t" << GlassProfile.Dt[0][iM] << "\t" << MedSintiProfile.Dt[0][iM] << "\t" << MedGlassProfile.Dt[0][iM] << endl;//プロファイルを主方向メディアンしたものも表示（ttsuji 131025）
	}
	mofile.close();

	string strAddPrefix = AddPrefix(FileName, "LeastSquare" + strUL + "(" + to_string(iY0) + "," + to_string(iYE) + ")");
	ofstream LSfile( ChangeFileExt(strAddPrefix, ".txt").c_str() );
	LSfile << ExtractFileName(FileName).c_str() << "\tシンチ段差\tガラス段差" << endl;

/*
#if 0 //AXIS試作機は、x=0～300くらいは上パネルと下パネルが重なっていないため
	const int iNumOfSampleCut = 300;
#else //DEVO2試作機は、上パネルと中パネルは左右方向に完全に重複しているため
	const int iNumOfSampleCut = 0;
#endif
	const int iNumOfSample = iXE - iX0 + 1 - iNumOfSampleCut;
	double* dM = new double[iNumOfSample];
	double* dMedSintiProfile = new double[iNumOfSample];
	double* dMedGlassProfile = new double[iNumOfSample];
*/

	//検出した主プロファイルを最小二乗法近似
/*
	for (int iM = 0; iM < iNumOfSample; iM++) {

		dM[iM] = (double)(iNumOfSampleCut + iM);
		dMedSintiProfile[iM] = (double)MedSintiProfile.Dt[0][iNumOfSampleCut + iM];
		dMedGlassProfile[iM] = (double)MedGlassProfile.Dt[0][iNumOfSampleCut + iM];
	}
	CLeastSquare LSSinti(iNumOfSample, dM, dMedSintiProfile, _TWO_DIM);
	CLeastSquare LSGlass(iNumOfSample, dM, dMedGlassProfile, _TWO_DIM);
*/

	int iCntSinti = 0;//最小二乗法サンプル点の数
	int iCntGlass = 0;//最小二乗法サンプル点の数
	double *dMSinti(NULL), *dMGlass(NULL), *dMedSintiProfile(NULL), *dMedGlassProfile(NULL);
	vector<double> vSinti, vGlass;

	dMSinti = new double[iXE - iX0 + 1];
	dMedSintiProfile = new double[iXE - iX0 + 1];
	for (int iM = 0; iM < Image.MainPixel; iM++) {
 #if 0	//Median前のシンチ段差主プロファイルがnon-zeroのものをサンプル点とすると、Medianによってnon-zero→0になって点もサンプル点に含まれ、最小二乗近似を狂わせる不具合あり(3R-7 画像)
		if (SintiProfile.Dt[0][iM]) {
 #else 	//Median後のシンチ段差主プロファイルがnon-zeroのものをサンプル点とするよう修正（140902 ttsuji）
		if (MedSintiProfile.Dt[0][iM]) {
 #endif
			dMSinti[iCntSinti] = (double)iM;
			dMedSintiProfile[iCntSinti] = (double)MedSintiProfile.Dt[0][iM];
			iCntSinti++;
		}
	}

 #if 0 //debug
	ofstream mofile2( ChangeFileExt(FileName, "_check.txt").c_str() );
	for (int iCnt = 0; iCnt < iCntSinti; iCnt++) {
		mofile2 << iCnt << "\t" << dMSinti[iCnt] << "\t" << dMedSintiProfile[iCnt] << endl;
	}
	mofile2.close();
 #endif

	CLeastSquare LSSinti(iCntSinti, dMSinti, dMedSintiProfile, _TWO_DIM);
	if (!LSSinti.Calc()){
		CApplication::GetInstance()->MessageBox("シンチプロファイルの最小二乗法計算でエラーが発生しました。", "Error", MB_OK);
		if (dMSinti)			delete[] dMSinti;
		if (dMedSintiProfile)	delete[] dMedSintiProfile;
		LSfile.close();
		return;
	}
	vSinti = LSSinti.GetSolve();//シンチ段差位置を最小二乗近似から計算


	double dYGstart, dYGend, dYSstart, dYSend, dYSMax, dYSMin, dYGMax, dYGMin, dA, dB, dC;
	int iXS_atmax, iXS_atmin, iXG_atmax, iXG_atmin;
	if (bGainCalib) {//ゲインキャリブ時はガラス段差も最小二乗近似で計算する
		dMGlass = new double[iXE - iX0 + 1];
		dMedGlassProfile = new double[iXE - iX0 + 1];
		for (int iM = 0; iM < Image.MainPixel; iM++) {
			if (GlassProfile.Dt[0][iM]) {
				dMGlass[iCntGlass] = (double)iM;
				dMedGlassProfile[iCntGlass] = (double)MedGlassProfile.Dt[0][iM];
				iCntGlass++;
			}
		}
		CLeastSquare LSGlass(iCntGlass, dMGlass, dMedGlassProfile, _TWO_DIM);
		if (!LSGlass.Calc()){
			CApplication::GetInstance()->MessageBox("ガラスプロファイルの最小二乗法計算でエラーが発生しました。", "Error", MB_OK);
			if (dMSinti)			delete[] dMSinti;
			if (dMGlass)			delete[] dMGlass;
			if (dMedSintiProfile)	delete[] dMedSintiProfile;
			if (dMedGlassProfile)	delete[] dMedGlassProfile;
			LSfile.close();
			return;
		}
		vGlass = LSGlass.GetSolve();//ガラス段差位置を最小二乗近似から計算

		switch (bUpperEdge) {
		case true:
			//SetLPEdgeInf(m_CalibSintiEdgeU, vSinti, dYSstart, dYSend, dYSMax, dYSMin, iXS_atmax, iXS_atmin);
			//SetLPEdgeInf(m_CalibGlassEdgeU, vGlass, dYGstart, dYGend, dYGMax, dYGMin, iXG_atmax, iXG_atmin);
			m_CalibSintiEdgeU.SetLPEdgeInf(vSinti, dYSstart, dYSend, dYSMax, dYSMin, iXS_atmax, iXS_atmin);
			m_CalibGlassEdgeU.SetLPEdgeInf(vGlass, dYGstart, dYGend, dYGMax, dYGMin, iXG_atmax, iXG_atmin);
			break;
		case false:
			//SetLPEdgeInf(m_CalibSintiEdgeD, vSinti, dYSstart, dYSend, dYSMax, dYSMin, iXS_atmax, iXS_atmin);
			//SetLPEdgeInf(m_CalibGlassEdgeD, vGlass, dYGstart, dYGend, dYGMax, dYGMin, iXG_atmax, iXG_atmin);
			m_CalibSintiEdgeD.SetLPEdgeInf(vSinti, dYSstart, dYSend, dYSMax, dYSMin, iXS_atmax, iXS_atmin);
			m_CalibGlassEdgeD.SetLPEdgeInf(vGlass, dYGstart, dYGend, dYGMax, dYGMin, iXG_atmax, iXG_atmin);
			break;
		}
		dA = vGlass[2];
		dB = vGlass[1];
		dC = vGlass[0];
	}
	else{//撮影時のガラス段差は、撮影時のシンチ段差に、キャリブ時のシンチ段差とガラス段差の差分を加算して計算
		double dY0, dYend, dDummy;
		int iDummy;
		if (bUpperEdge) {
			//SetLPEdgeInf(m_SintiEdgeU, vSinti, dYSstart, dYSend, dYSMax, dYSMin, iXS_atmax, iXS_atmin);
			m_SintiEdgeU.SetLPEdgeInf(vSinti, dYSstart, dYSend, dYSMax, dYSMin, iXS_atmax, iXS_atmin);

			dA = m_SintiEdgeU.a - m_CalibSintiEdgeU.a;
			dB = m_SintiEdgeU.b - m_CalibSintiEdgeU.b;
			dC = m_SintiEdgeU.c - m_CalibSintiEdgeU.c;
			//SetLPEdgeInf(m_EdgeShiftU, dA, dB, dC, dY0, dYend, dDummy, dDummy, iDummy, iDummy);//平行四辺形シフトに利用
			m_EdgeShiftU.SetLPEdgeInf(dA, dB, dC, dY0, dYend, dDummy, dDummy, iDummy, iDummy);//平行四辺形シフトに利用

			dA = m_SintiEdgeU.a + m_CalibGlassEdgeU.a - m_CalibSintiEdgeU.a;
			dB = m_SintiEdgeU.b + m_CalibGlassEdgeU.b - m_CalibSintiEdgeU.b;
			dC = m_SintiEdgeU.c + m_CalibGlassEdgeU.c - m_CalibSintiEdgeU.c;
			//SetLPEdgeInf(m_GlassEdgeU, dA, dB, dC, dYGstart, dYGend, dYGMax, dYGMin, iXG_atmax, iXG_atmin);//撮影時のガラス段差は、撮影時のシンチ段差に、キャリブ時のシンチ段差とガラス段差の差分を加算して計算
			m_GlassEdgeU.SetLPEdgeInf(dA, dB, dC, dYGstart, dYGend, dYGMax, dYGMin, iXG_atmax, iXG_atmin);//撮影時のガラス段差は、撮影時のシンチ段差に、キャリブ時のシンチ段差とガラス段差の差分を加算して計算
		}
		else{
			//SetLPEdgeInf(m_SintiEdgeD, vSinti, dYSstart, dYSend, dYSMax, dYSMin, iXS_atmax, iXS_atmin);
			m_SintiEdgeD.SetLPEdgeInf(vSinti, dYSstart, dYSend, dYSMax, dYSMin, iXS_atmax, iXS_atmin);

			dA = m_SintiEdgeD.a - m_CalibSintiEdgeD.a;
			dB = m_SintiEdgeD.b - m_CalibSintiEdgeD.b;
			dC = m_SintiEdgeD.c - m_CalibSintiEdgeD.c;
			//SetLPEdgeInf(m_EdgeShiftD, dA, dB, dC, dY0, dYend, dDummy, dDummy, iDummy, iDummy);//平行四辺形シフトに利用
			m_EdgeShiftD.SetLPEdgeInf(dA, dB, dC, dY0, dYend, dDummy, dDummy, iDummy, iDummy);//平行四辺形シフトに利用

			dA = m_SintiEdgeD.a + m_CalibGlassEdgeD.a - m_CalibSintiEdgeD.a;
			dB = m_SintiEdgeD.b + m_CalibGlassEdgeD.b - m_CalibSintiEdgeD.b;
			dC = m_SintiEdgeD.c + m_CalibGlassEdgeD.c - m_CalibSintiEdgeD.c;
			//SetLPEdgeInf(m_GlassEdgeD, dA, dB, dC, dYGstart, dYGend, dYGMax, dYGMin, iXG_atmax, iXG_atmin);//撮影時のガラス段差は、撮影時のシンチ段差に、キャリブ時のシンチ段差とガラス段差の差分を加算して計算
			m_GlassEdgeD.SetLPEdgeInf(dA, dB, dC, dYGstart, dYGend, dYGMax, dYGMin, iXG_atmax, iXG_atmin);//撮影時のガラス段差は、撮影時のシンチ段差に、キャリブ時のシンチ段差とガラス段差の差分を加算して計算
		}
	}
 /*
	double dYGstart, dYGend, dYSstart, dYSend, dMax, dMin;
	int x_atmax, x_atmin;
	switch (bUpperEdge) {
	case true:
		SetEdgeInf(m_SintiEdgeU, vSinti, dYSstart, dYSend);
		SetEdgeInf(m_GlassEdgeU, vGlass, dYGstart, dYGend);
		break;
	case false:
		SetEdgeInf(m_SintiEdgeD, vSinti, dYSstart, dYSend);
		SetEdgeInf(m_GlassEdgeD, vGlass, dYGstart, dYGend);
		break;
	}
*/
/*
	LSfile << "a\t" << m_SintiEdge.a << "\t" << m_GlassEdge.a << endl;
	LSfile << "b\t" << m_SintiEdge.b << "\t" << m_GlassEdge.b << endl;
	LSfile << "c\t" << m_SintiEdge.c << "\t" << m_GlassEdge.c << endl << endl;
*/
/*
	LSfile << "a\t" << vSinti[2] << "\t" << vGlass[2] << endl;
	LSfile << "b\t" << vSinti[1] << "\t" << vGlass[1] << endl;
	LSfile << "c\t" << vSinti[0] << "\t" << vGlass[0] << endl << endl;
	LSfile << "x\tシンチ段差\tガラス段差" << endl;
*/
	LSfile << "a\t" << vSinti[2] << "\t" << dA << endl;
	LSfile << "b\t" << vSinti[1] << "\t" << dB << endl;
	LSfile << "c \t" << fixed << setprecision(5) << vSinti[0] << "\t" << fixed << setprecision(5) << dC << endl << endl;

/*
	LSfile << "0" << "\t"  << m_SintiEdge.ystart << "\t" << m_GlassEdge.ystart << endl;
	LSfile << round(Image.MainPixel - 1) << "\t"  << m_SintiEdge.yend << "\t" << m_GlassEdge.yend << endl;
*/
	LSfile << "x\tシンチ段差\tガラス段差" << endl;
	LSfile << "0" << "\t"  << fixed << setprecision(5) << dYSstart << "\t" << fixed << setprecision(5) << dYGstart << endl;
	LSfile << round(Image.MainPixel - 1) << "\t"  << fixed << setprecision(5) << dYSend << "\t" << fixed << setprecision(5) << dYGend << endl;
	LSfile << "min\t" << iXS_atmin << "\t"  << fixed << setprecision(5) << dYSMin << "\t" << iXG_atmin << "\t"  << fixed << setprecision(5) << dYGMin << endl;
	LSfile << "max\t" << iXS_atmax << "\t"  << fixed << setprecision(5) << dYSMax << "\t" << iXG_atmax << "\t"  << fixed << setprecision(5) << dYGMax << endl;
	LSfile.close();

	if (dMSinti)			delete[] dMSinti;
	if (dMGlass)			delete[] dMGlass;
	if (dMedSintiProfile)	delete[] dMedSintiProfile;
	if (dMedGlassProfile)	delete[] dMedGlassProfile;

}
void CLPCorrectionS::InitializeLPEdgeInf(LPEDGEINF& EdgeInf){

}
void CLPCorrectionS::LPSintiEdgeCorrection(TTMA& ImageUp, string FileNameImageUp, string& FileName, bool bCorHorShift, int iMainShiftMax, int iSubNumMatch, int iMainMedianMatch,
						bool bGPRMatch, int iMainMedianDiffImg, int iSubMedianDiffImg, int iLimitSearchRange, bool bUetyousei, int iUeOffset, int& iSitaOffset,
						float fUpperLimitOfPanelSensRatio, float fLowerLimitOfPanelSensRatio, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iMainMedianDCItpl,
						float fThQLRateUD, float fMagicPara, bool bDetectKanajakuMemori, int iMedianHasiSyori, int iThQLSubDiffForScaleSearch, int iSubAveNum,
						int iMarginXMed, double dKandoTop, double dKandoCenter, int iYSearchEnd, bool& bExistPbScale, bool bUpperEdge){
#if 0
#ifdef _TIME1
			//計算時間計測
		clock_t	startTime, stopTime;
		startTime = clock();
#endif

	AnsiString StrPre = "";//接頭辞
	double* delta = new double[MainPixel];
	int* edgeY = new int[MainPixel];
	TFIMG ImageRateOfUpToDown;
	LPCalcSintiEdge(delta, edgeY, FileName, bUpperEdge);//シンチ段差の段差量を計算する
	int iYSintiEnd = round((bUpperEdge ? m_SintiEdgeU.ymax : m_SintiEdgeD.ymax) / 10.);//シンチ段差の下端
//	int iYSintiEnd = round((bUpperEdge ? m_SintiEdgeU.ymax : m_SintiEdgeD.ymax) / 10.) + 1;//シンチ段差の下端
	int iMShiftDetect;//上画像と中画像の左右位置ずれ
	int iWidthROI = 500;//とりあえず
	int iWidthOverlap = 100;//とりあえず
	float fSensitivityRateDtoU;//tmp
	TTMA ImageUpBeforeLRShift = ImageUp;//左右シフト前のImageUpを保存
	TTMA ImageUpAfterLRShift = ImageUp;//左右シフト後のImageUpを保存

#ifdef _ONLY_NO_OVERLAP_COR   //非重複領域の補正のみ行いたい場合(MCデバッグ用)
	iMShiftDetect = 1;//LongPanel.logより抜粋
	iSitaOffset = 0;//LongPanel.logより抜粋
	fSensitivityRateDtoU = 0.993656;//LongPanel.logより抜粋

#else //通常はこちら
	//上画像と中画像の絵の重複領域を抽出して、ログ変換後に主Medianをかけて、左右位置ずれを正規化相互相関にて検出し、上画像を左右シフトする（ImageUpは左右シフト後の画像に変換される）
	if (bCorHorShift) {
//		LPDetectHorizontalMisalignmentAndShiftImageUp(ImageUp, FileNameImageUp, iMShiftDetect, FileName, iMainShiftMax, iSubNumMatch, iMainMedianMatch, iSubRangeOnlyMidPanel, iYSintiEnd);
		LPDetectHorizontalMisalignmentAndShiftImageUpMultiROI(ImageUp, FileNameImageUp, iMShiftDetect, FileName, iMainShiftMax, iSubNumMatch, iMainMedianMatch, iSubRangeOnlyMidPanel, iYSintiEnd, iWidthROI, iWidthOverlap, dKandoTop, dKandoCenter, bGPRMatch);

//		iMShiftDetect = 0;//強制上書きテスト(150529 ttsuji)
	}



	//上画像と中画像の感度の違いを検出して上画像の感度を修正する
//	float fSensitivityRateDtoU;
	if (bUetyousei) {
		ImageRateOfUpToDown.NewDt(MainPixel, 1);//M,Sの順
//		LPAdjustEndPanelSensitivity(ImageRateOfUpToDown, ImageUp, edgeY, FileName, iLimitSearchRange, iUeOffset, iSitaOffset, fUpperLimitOfPanelSensRatio, fLowerLimitOfPanelSensRatio, iMainShiftMax, fSensitivityRateDtoU);//上画像ImageUpの感度を中画像に合うように調整
		LPAdjustEndPanelSensitivity2(ImageRateOfUpToDown, ImageUp, edgeY, FileName, iLimitSearchRange, iUeOffset, iSitaOffset, fUpperLimitOfPanelSensRatio, fLowerLimitOfPanelSensRatio, iMainShiftMax, fSensitivityRateDtoU, iYSearchEnd);//上画像ImageUpの感度を中画像に合うように調整

		//シンチ段差プロファイルを主Median11
		for (int iM = 0; iM < MainPixel; iM++){
			ImageRateOfUpToDown.Dt[iM][0] = delta[iM];
		}
		const int iPMedian = 11;
		ImageRateOfUpToDown.MainMedian(iPMedian);
 #ifdef _LP_DEBUG_OUTPUT
		ImageRateOfUpToDown.bSaveAsText(ExtractFilePath(FileName) + "Med_delta.txt", true);
 #endif
		for (int iM = 0; iM < MainPixel; iM++){
			delta[iM] = ImageRateOfUpToDown.Dt[iM][0];
		}
// #ifdef _LP_DEBUG_OUTPUT
		//MCコンペア用にTop画像の処理を追加（上画像（左右位置ずらしなし）にクリッピング処理を入れて出力）(151004 ttsuji)
		//Topパネルに感度補正係数を乗算
		for (int iS = 0; iS < ImageUpBeforeLRShift.SubPixel; iS++){
			for (int iM = 0; iM < ImageUpBeforeLRShift.MainPixel; iM++){
				ImageUpBeforeLRShift.SetVal(iS, iM, ImageUpBeforeLRShift.Dt[iS][iM] * fSensitivityRateDtoU);
			}
		}
		AnsiString strImageUpPanelSensTC = "A(" + FloatToStrF(fSensitivityRateDtoU,ffFixed,7,6) + ")ImageUp.tma";//有効数字7桁、小数点以下6桁を表示
		AnsiString strImageUpPanelSensTCBeforeLRShift = "A(" + FloatToStrF(fSensitivityRateDtoU,ffFixed,7,6) + ")ImageUpBeforeLRShift.tma";//有効数字7桁、小数点以下6桁を表示
		ImageUpBeforeLRShift.WriteTma(ExtractFilePath(FileName) + strImageUpPanelSensTCBeforeLRShift);

		//S値適正化補正
		int iSValueMod = 4;//４倍乗算
		ImageUpAfterLRShift  *= iSValueMod;
		ImageUpBeforeLRShift *= iSValueMod;
		ImageUpAfterLRShift.WriteTma(ExtractFilePath(FileName) + "A" + strImageUpPanelSensTC);
		ImageUpBeforeLRShift.WriteTma(ExtractFilePath(FileName) + "A" + strImageUpPanelSensTCBeforeLRShift);

		//クリップ処理
		int iClipValue = 52000;
		for (int iS = 0; iS < ImageUpAfterLRShift.SubPixel; iS++){
			for (int iM = 0; iM < ImageUpAfterLRShift.MainPixel; iM++){
				//非補正画像QLが飽和しているときの対策（clip処理）
				if (ImageUpAfterLRShift.Dt[iS][iM] <= iClipValue * dKandoTop * fSensitivityRateDtoU) {//QL値がiClipValue * dKando * fSensitivityRateDtoU 以下であれば、QLを変えない。
					continue;
				}
				else{
					ImageUpAfterLRShift.SetVal(iS, iM, iClipValue * dKandoTop * fSensitivityRateDtoU);
				}
			}
		}
		ImageUpAfterLRShift.WriteTma(ExtractFilePath(FileName) + "C(" + IntToStr(round(iClipValue * dKandoTop * fSensitivityRateDtoU)) + ")A" + strImageUpPanelSensTC);

		for (int iS = 0; iS < ImageUpBeforeLRShift.SubPixel; iS++){
			for (int iM = 0; iM < ImageUpBeforeLRShift.MainPixel; iM++){
				//非補正画像QLが飽和しているときの対策（clip処理）
				if (ImageUpBeforeLRShift.Dt[iS][iM] <= iClipValue * dKandoTop * fSensitivityRateDtoU) {//QL値がiClipValue * dKando * fSensitivityRateDtoU 以下であれば、QLを変えない。
					continue;
				}
				else{
					ImageUpBeforeLRShift.SetVal(iS, iM, iClipValue * dKandoTop * fSensitivityRateDtoU);
				}
			}
		}
		ImageUpBeforeLRShift.WriteTma(ExtractFilePath(FileName) + "C(" + IntToStr(round(iClipValue * dKandoTop * fSensitivityRateDtoU)) + ")A" + strImageUpPanelSensTCBeforeLRShift);
// #endif
	}
#endif

#ifdef _TIME1
	stopTime = clock();
	double time = (double)(stopTime-startTime)/CLOCKS_PER_SEC;
	Application->MessageBox(("シンチ段差補正A,B,C：\n" + FloatToStr(time) + " sec").c_str(), "OK", MB_OK);
#endif


#ifdef _TIME2
	//計算時間計測
	clock_t	startTime2, stopTime2;
	startTime2 = clock();
#endif


 #if 0 //tmp
	iSitaOffset = 100;//強制的に白帯長さを100にする
 #endif

	const int iKasa = 30000;//CorrImgのアンダーフローを防ぐため導入（ttsuji 131003）
//	const int iKasa = 50000;//CorrImgのアンダーフローを防ぐため導入（ttsuji 131003）
//	const int iKasa = 4000;//CorrImgのアンダーフロー、オーバーフローを防ぐため修正（ttsuji 140407）
	TTMA CorrImg;

 #if 0 //検出した白帯長さをDC成分補間に反映するのをやめた。（DC補間長さを長くしても完全にはキレイにならない画像もあるため、DC補間はできるだけ短く抑え、白帯は短冊横傘を強くかけて落とすとする）140523
	iItplSubOffset = iSitaOffset;//感度比算出で計算した白帯の長さをDC成分補間の補間長さに反映
 #elif 0
	iItplSubOffset = 3;
 //	iItplSubOffset = 50;//150622ttsuji 決めうち（VG対応でグリッドなし腰椎側面の白帯対策）
//	iItplSubOffset = 46;//150622ttsuji 決めうち（VG対応でグリッドなし腰椎側面の白帯対策）
//	iItplSubOffset = 30;//150622ttsuji 決めうち（VG対応でグリッドなし腰椎側面の白帯対策）
 #elif 1
	if (iSitaOffset >= 100) {
//	if (iSitaOffset >= 0) {//強制的にDC補間長さを50pixとするため
//		iItplSubOffset = 50;//白帯長さが100以上のときのみDC補間を長い範囲で実施する（VG対応でグリッドなし腰椎側面の白帯対策）(150623ttsuji)
//		iItplSubOffset = 30;//白帯長さが100以上のときのみDC補間を長い範囲で実施する（VG対応でグリッドなし腰椎側面の白帯対策）(150623ttsuji)
//		iItplSubOffset = 20;//白帯長さが100以上のときのみDC補間を長い範囲で実施する（VG対応でグリッドなし腰椎側面の白帯対策）(150623ttsuji)
		iItplSubOffset = 10;//白帯長さが100以上のときのみDC補間を長い範囲で実施する（VG対応でグリッドなし腰椎側面の白帯対策）(150623ttsuji)
//		iItplSubOffset = 3;//白帯長さが100以上のときのみDC補間を長い範囲で実施する（VG対応でグリッドなし腰椎側面の白帯対策）(150623ttsuji)
	}
 #endif
	CorrImg.NewDt(iYSintiEnd + iItplSubOffset + 1, MainPixel, Bit);
//	CorrImg.NewDt(iYSintiEnd + iItplSubOffset + 2, MainPixel, Bit);//マージン込み確保
//	CorrImg.NewDt(iYSintiEnd + iItplSubOffset + 3, MainPixel, Bit);//マージン込み確保(140816に iYSintiEnd + iItplSubOffset + 1　に戻した。ttsuji )
	CorrImg.FillValue(iKasa);

	TTMA QLRateImg;//上下の画像のQL比（左右位置ずれ検出のため） ←結局アルゴが複雑になりそうだったので使わなかった。
	const double dNormQL = 8192.;//8192QLが1倍
	QLRateImg.NewDt(iYSintiEnd + iItplSubOffset + 1, MainPixel, Bit);
	QLRateImg.FillValue(0);

#ifndef _ONLY_NO_OVERLAP_COR
	//上画像のうち、中画像と同じ絵が存在する領域に対して中画像との差分をとって、主Median、副Medianして上画像との差分画像の低周波情報を取得
	LPCorrectOverlapAreaReferToImageUp(ImageUp, CorrImg, QLRateImg, FileName, edgeY, iKasa, iYSintiEnd, dNormQL, iSubRangeOnlyMidPanel, iMainMedianDiffImg, iSubMedianDiffImg, iMShiftDetect);
#endif

	//上画像低周波情報で補正した中画像のうち、上画像情報が存在しない補正領域は中画像のDC成分を線形補間にて計算する。その際、DC成分線形補間の端点のQL比がある閾値以上であれば、濃度の濃い画素の濃度を下げた上で線形補間する。
	//DC成分線形補間した画像を中画像から減算して補正画像を作成し、これに主Medianをかけて、補正量の主方向スムージングを行う。これを最終的な補正画像として、中画像から減算する。
//	LPCorrectNonOverlapAreaByItplDC(CorrImg, FileName, edgeY, iKasa, iYSintiEnd, iSubRangeOnlyMidPanel, iItplSubOffset, iMainMedianDCItpl, fThQLRateUD, fMagicPara);
//	LPCorrectNonOverlapAreaByItplDC2(CorrImg, FileName, edgeY, iKasa, iYSintiEnd, iSubRangeOnlyMidPanel, iItplSubOffset, iMainMedianDCItpl, fThQLRateUD, fMagicPara);
#ifndef _HEIKOHSHIHENKEI_HOKAN
	LPCorrectNonOverlapAreaByItplDC3(CorrImg, FileName, edgeY, iKasa, iYSintiEnd, iSubRangeOnlyMidPanel, iItplSubOffset, iMainMedianDCItpl, fThQLRateUD);
#else
	LPCorrectNonOverlapAreaByItplDC4(CorrImg, FileName, edgeY, iKasa, iYSintiEnd, iSubRangeOnlyMidPanel, iItplSubOffset, iMainMedianDCItpl, bDetectKanajakuMemori, iMedianHasiSyori, iThQLSubDiffForScaleSearch, iSubAveNum, iMarginXMed, bExistPbScale);
#endif


#ifdef _TIME2
	stopTime2 = clock();
	double time2 = (double)(stopTime2-startTime2)/CLOCKS_PER_SEC;
	Application->MessageBox(("シンチ段差補正D,E：\n" + FloatToStr(time2) + " sec").c_str(), "OK", MB_OK);
#endif

	if(bUetyousei){
//		StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + ",-2,3," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "," + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ")";
		if (bCorHorShift) {
//			 StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + "," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "," + FloatToStrF(fThQLRateUD,ffFixed,2,1) + "," +  FloatToStrF(fMagicPara,ffFixed,2,1) + "," + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ",A" + FloatToStrF(fSensitivityRateDtoU,ffFixed,3,2) + "," + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) + "NCC"  + IntToStr(iMShiftDetect) + ")";//FloatToStrF(x,ffFixed,3,3) → 実数ｘの値を有効数字３桁小数点以下３桁の文字列にする
			if (bDetectKanajakuMemori) {
#if 0
				StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + "," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "K" + IntToStr(iThQLSubDiffForScaleSearch) + "," + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ",A" + FloatToStrF(fSensitivityRateDtoU,ffFixed,3,2) + "," + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) + "N"  + IntToStr(iMShiftDetect) + ")";//FloatToStrF(x,ffFixed,3,3) → 実数ｘの値を有効数字３桁小数点以下３桁の文字列にする
#else			//矩形アーチファクト対策
				StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + "," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "K(-1,-3," + IntToStr(iThQLSubDiffForScaleSearch) + ")" + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ",A" + FloatToStrF(fSensitivityRateDtoU,ffFixed,3,2) + "," + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) + "N"  + IntToStr(iMShiftDetect) + ")";//FloatToStrF(x,ffFixed,3,3) → 実数ｘの値を有効数字３桁小数点以下３桁の文字列にする
#endif
			}
			else{
				StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + "," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "," + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ",A" + FloatToStrF(fSensitivityRateDtoU,ffFixed,3,2) + "," + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) + "N"  + IntToStr(iMShiftDetect) + ")";//FloatToStrF(x,ffFixed,3,3) → 実数ｘの値を有効数字３桁小数点以下３桁の文字列にする
			}
		}
		else{
			 StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + "," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "," + FloatToStrF(fThQLRateUD,ffFixed,2,1) + "," +  FloatToStrF(fMagicPara,ffFixed,2,1) + "," + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ",A" + FloatToStrF(fSensitivityRateDtoU,ffFixed,3,2) + "," + IntToStr(iUeOffset) + "," + IntToStr(iSitaOffset) +  ")";
		}
	}
	else{
		if (bCorHorShift) {
			 StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + "," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "," + FloatToStrF(fThQLRateUD,ffFixed,2,1) + "," +  FloatToStrF(fMagicPara,ffFixed,2,1) + "," + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ",ANCC"  + IntToStr(iMShiftDetect) + ")";
		}
		else{
			 StrPre = "S(" + IntToStr(edgeY[0]) + "," + IntToStr(edgeY[MainPixel-1]) + "," + IntToStr(iSubRangeOnlyMidPanel) + "," + IntToStr(iItplSubOffset) + "," + IntToStr(iMainMedianDCItpl) + "," + FloatToStrF(fThQLRateUD,ffFixed,2,1) + "," +  FloatToStrF(fMagicPara,ffFixed,2,1) + "," + IntToStr(iMainMedianDiffImg) + "," + IntToStr(iSubMedianDiffImg) + ",A)";
		}
	}
#if 0
	FileName = AddPrefix(FileName, StrPre);
#else
	FileName = ExtractFilePath(FileName) + StrPre + ".tma";
#endif

	delete[] edgeY;
	delete[] delta;
#endif
}
//シンチ段差の段差量を計算する
void CLPCorrectionS::LPCalcSintiEdge(double* delta, int* edgeY, string FileName, bool bUpperEdge){

}
//上画像の感度を中画像に合うように調整
void CLPCorrectionS::LPAdjustEndPanelSensitivity(TFIMG& ImageRateOfUpToDown, TTMA& ImageUp, int* edgeY, string FileName, int iLimitSearchRange, int iUeOffset, int& iSitaOffset, float fUpperLimitOfPanelSensRatio, float fLowerLimitOfPanelSensRatio, int iMainShiftMax, float& fSensitivityRateDtoU){

}
//上画像の感度を中画像に合うように調整(150326 ttsuji 日赤の臨床画像を受けて修正)
void CLPCorrectionS::LPAdjustEndPanelSensitivity2(TFIMG& ImageRateOfUpToDown, TTMA& ImageUp, int* edgeY, string FileName, int iLimitSearchRange, int iUeOffset, int& iSitaOffset, float fUpperLimitOfPanelSensRatio, float fLowerLimitOfPanelSensRatio, int iMainShiftMax, float& fSensitivityRateDtoU, int iYSearchEnd){

}
//上画像と中画像の絵の重複領域を抽出して、ログ変換後に主Medianをかけて、左右位置ずれを正規化相互相関にて検出し、上画像を左右シフトする
void CLPCorrectionS::LPDetectHorizontalMisalignmentAndShiftImageUp(TTMA& ImageUp, string FileNameImageUp, int& iMShiftDetect, string FileName, int iMainShiftMax, int iSubNumMatch, int iMainMedianMatch, int iSubRangeOnlyMidPanel, int iYSintiEnd){

}
//上画像と中画像の絵の重複領域を抽出して、ログ変換後に主方向微分して、左右位置ずれを正規化相互相関にて検出し、上画像を左右シフトする
void CLPCorrectionS::LPDetectHorizontalMisalignmentAndShiftImageUpMultiROI(TTMA& ImageUp, string FileNameImageUp, int& iMShiftDetect, string FileName, int iMainShiftMax, int iSubNumMatch, int iMainMedianMatch, int iSubRangeOnlyMidPanel, int iYSintiEnd, int iWidthROI, int iWidthOverlap, double dKandoTop, double dKandoCenter, bool bGPRMatch){

#if 0
	if (iWidthROI > MainPixel) {
		Application->MessageBox("ROIの幅の設定が不正です。", "Error", MB_OK);
		return;
	}
	if (iWidthROI < iWidthOverlap) {
		Application->MessageBox("ROIの幅の設定が不正です。", "Error", MB_OK);
		return;
	}

	//上・中画像の絵が重複している短冊領域のみ抽出（パターンマッチングに使用）
	TTMA FImageUp, FImageDown;
	FImageUp = ImageUp;
	FImageDown = *this;

	//左右端から30pixを除外（左右位置ずれの最大量）
//	FImageUp.Format(iSubNumMatch, ImageUp.MainPixel - 2*iMainShiftMax, -(ImageUp.SubPixel - 1 - iSubNumMatch),  -iMainShiftMax);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）//ｙが1画素ずれてトリミングしていたbug
	FImageUp.Format(iSubNumMatch, ImageUp.MainPixel - 2*iMainShiftMax, -(ImageUp.SubPixel - iSubNumMatch),  -iMainShiftMax);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）
	FImageDown.Format(iSubNumMatch, MainPixel - 2*iMainShiftMax, -(iYSintiEnd - iSubRangeOnlyMidPanel - iSubNumMatch),  -iMainShiftMax);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）
 #ifdef _LP_DEBUG_OUTPUT
	FImageUp.WriteTma(ExtractFilePath(FileName) + "FImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "FImageDown.tma");
 #endif

#if 0 //メ品証ランニング評価で出た案件対応（141117 kuwabara） →素抜けの飽和縦スジパターンが相互相関の誤検出に影響するためクリッピングする
	const int iClipQL = 43000;//飽和スジは45000QLで見えてくるので、43000QLでクリップ処理を入れる。
	FImageUp.Clip(0, iClipQL);
	FImageDown.Clip(0, iClipQL);
 #ifdef _LP_DEBUG_OUTPUT
	FImageUp.WriteTma(ExtractFilePath(FileName) + "CFImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "CFImageDown.tma");
 #endif
#else //パネルの感度補正係数を反映し、パネルごとに異なるclippingQLとする (150915 ttsuji)
	const int iClipQL = 52000;
	int iClipUp   = (int)(iClipQL * dKandoTop / 4);//切捨て
	int iClipDown = (int)(iClipQL * dKandoCenter / 4);//切捨て
	FImageUp.Clip(0, iClipUp);
	FImageDown.Clip(0, iClipDown);
 #ifdef _LP_DEBUG_OUTPUT
	FImageUp.WriteTma(ExtractFilePath(FileName) + "C(" + IntToStr(iClipUp) + ")FImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "C(" + IntToStr(iClipDown) + ")FImageDown.tma");
 #endif
#endif


	//ログ変換してから相互相関を計算する
	const double dMidVal = 948.;
	const double dKeta = 4.;
	const int iLogTh = 60000;
	AnsiString StrPre = "";
 #ifndef _LOWDOSE_LINEAR_TABLE
	FImageUp.LogThreshold(dMidVal, dKeta, iLogTh);
	FImageUp.BitChange(14);
	FImageDown.LogThreshold(dMidVal, dKeta, iLogTh);
	FImageDown.BitChange(14);
 #else
	FImageUp.Lin16ToLog14_LowDoseLinear(dMidVal, dKeta);
	FImageDown.Lin16ToLog14_LowDoseLinear(dMidVal, dKeta);
 #endif

 #ifdef _LP_DEBUG_OUTPUT
//	FImageUp.WriteTma(ExtractFilePath(FileName) + "LFImageUp.tma");
//	FImageDown.WriteTma(ExtractFilePath(FileName) + "LFImageDown.tma");
	FImageUp.WriteTma(ExtractFilePath(FileName) + "LCFImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "LCFImageDown.tma");
 #endif


 #ifdef _SKINLINE_KUBOMI_TAISAKU //スキンラインくぼみ対策（LCFImageかDLCFImageにかける）(180517ttsuji)
	//GPRをかけて直接線分のグリッドリス目を除去する
	if (bGPRMatch) {
  #if 0
		FImageUp.GPR();
		FImageDown.GPR();
  #else   //副フィルターのtap数を短くする
  /*
  GPR(const float fMainFilter[],
			const int iMainFilterNum,
			const float fSubFilter[],
			const int iSubFilterNum)
  */
		const float fMainFilter[] =
			{-0.001708984, 0.001464844, 0.002197266,-0.002441406,-0.003173828,
			  0.004150391, 0.003417969,-0.007324219,-0.002929688, 0.011474609,
			  0.000488281,-0.016357422, 0.004394531, 0.021728516,-0.012451172,
			 -0.026855469, 0.025634766, 0.031494141,-0.048583984,-0.03515625,
			  0.096435547, 0.037597656,-0.314941406, 0.462890625,-0.314941406,
			  0.037597656, 0.096435547,-0.03515625 ,-0.048583984, 0.031494141,
			  0.025634766,-0.026855469,-0.012451172, 0.021728516, 0.004394531,
			 -0.016357422, 0.000488281, 0.011474609,-0.002929688,-0.007324219,
			  0.003417969, 0.004150391,-0.003173828,-0.002441406, 0.002197266,
			  0.001464844,-0.001708984};
		const int iMainFilterNum = 47;

		const float fSubFilter[] = {0.0156071,
									0.0631302,
									0.1292630,
									0.1870300,
									0.2099400,
									0.1870300,
									0.1292630,
									0.0631302,
									0.0156071};
		const int iSubFilterNum = 9;

		FImageUp.GPR(fMainFilter, iMainFilterNum, fSubFilter, iSubFilterNum);
		FImageDown.GPR(fMainFilter, iMainFilterNum, fSubFilter, iSubFilterNum);
  #endif

  #ifdef _LP_DEBUG_OUTPUT
		FImageUp.WriteTma(ExtractFilePath(FileName) + "GLCFImageUp.tma");
		FImageDown.WriteTma(ExtractFilePath(FileName) + "GLCFImageDown.tma");
  #endif
 	}
 #endif

	//左右方向へ微分
	FImageUp.DifMainUp();
	FImageDown.DifMainUp();
	FImageUp.Format(iSubNumMatch, FImageUp.MainPixel - 1, 0, -1);
	FImageDown.Format(iSubNumMatch, FImageDown.MainPixel - 1, 0, -1);
 #ifdef _LP_DEBUG_OUTPUT
//	FImageUp.WriteTma(ExtractFilePath(FileName) + "DLFImageUp.tma");
//	FImageDown.WriteTma(ExtractFilePath(FileName) + "DLFImageDown.tma");
	FImageUp.WriteTma(ExtractFilePath(FileName) + "DLCFImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "DLCFImageDown.tma");
 #endif

 #if 0
	//主方向Medianでノイズ落とす
	FImageUp.MainMedian(iMainMoveAveMatch);
	FImageDown.MainMedian(iMainMoveAveMatch);
 #else
	//主方向移動平均でノイズ落とす(主方向微分のときはこれが精度良い)
	FImageUp.MainMoveAve(iMainMoveAveMatch);
	FImageDown.MainMoveAve(iMainMoveAveMatch);
 #endif

 #ifdef _LP_DEBUG_OUTPUT
//	FImageUp.WriteTma(ExtractFilePath(FileName) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLFImageUp.tma");
//	FImageDown.WriteTma(ExtractFilePath(FileName) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLFImageDown.tma");
	FImageUp.WriteTma(ExtractFilePath(FileName) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLCFImageUp.tma");
	FImageDown.WriteTma(ExtractFilePath(FileName) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLCFImageDown.tma");
 #endif

//	const double dThNCC = 0.4;
	const double dThNCC = 0.3;//ROIにエッジがあり、正しく相関が出ていることを示す閾値(default)
//	const double dThNCC = 0.1;
//	const double dThNCC = 1.;
	double* dNCC = new double[2 * iMainShiftMax + 1];//Δx = -30～+30までスキャンする
	for (int i = 0; i < 2 * iMainShiftMax + 1; i++) {
   		dNCC[i] = -100.;//デフォルトを負の値とする
	}
	double dNCCMaxX0, dNCCMaxXE;
	double dQLaveUp;
	double dQLaveDown;
	double dNormUp = 0.;
	double dNormDown = 0.;
	double dNormUp2, dNormDown2;
	TTMA FImageUpROIX0, FImageDownROIX0, FImageUpROIXE, FImageDownROIXE;
	bool bFindEdgeX0 = false;//X0方向のROIから有効なエッジを見つけたらtrue
	bool bFindEdgeXE = false;//XE方向のROIから有効なエッジを見つけたらtrue
    int iXROI;
	int iROICnt = 0;//ROIを修正する試行回数
	int iMShiftDetectX0(0), iMShiftDetectXE(0);

 #ifdef _LP_DEBUG_OUTPUT
	FILE* fpRel = fopen((ExtractFilePath(FileName) + "NormalizedCrossCorrelationX0.txt").c_str(), "wt");
	fprintf(fpRel, "%s\n", FileNameImageUp.c_str());
	fprintf(fpRel, "%s\n", FileName.c_str());
 #endif


	//X=0側のROIについて上・中２画像の正規化相互相関（NormalizedCrossCorrelation）を計算
	do{
		iXROI = iROICnt * (iWidthROI - iWidthOverlap);
		FImageUpROIX0   = FImageUp;
		FImageDownROIX0 = FImageDown;
		FImageUpROIX0.Format(FImageUpROIX0.SubPixel, iWidthROI, 0,  -iXROI);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）
		FImageDownROIX0.Format(FImageDownROIX0.SubPixel, iWidthROI, 0,  -iXROI);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）
 #ifdef _LP_DEBUG_OUTPUT
//		FImageUpROIX0.WriteTma(ExtractFilePath(FileName)   + "F" + IntToStr(iROICnt * (iWidthROI - iWidthOverlap)) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLFImageUp.tma");
//		FImageDownROIX0.WriteTma(ExtractFilePath(FileName) + "F" + IntToStr(iROICnt * (iWidthROI - iWidthOverlap)) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLFImageDown.tma");
		FImageUpROIX0.WriteTma(ExtractFilePath(FileName)   + "F" + IntToStr(iROICnt * (iWidthROI - iWidthOverlap)) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLCFImageUp.tma");
		FImageDownROIX0.WriteTma(ExtractFilePath(FileName) + "F" + IntToStr(iROICnt * (iWidthROI - iWidthOverlap)) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLCFImageDown.tma");
 #endif

		//ROI画像に相互相関
		dQLaveUp   = FImageUpROIX0.dAve();
		dQLaveDown = FImageDownROIX0.dAve();
		for (int iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
			dNormUp = 0.;
			dNormDown = 0.;
//			double dQLaveUp   = FImageUp.dAve(0, iMainShiftMax/2, FImageUp.SubPixel - 1, FImageUp.MainPixel - 1 - iMainShiftMax/2);
//			double dQLaveDown = FImageDown.dAve(0, iMainShiftMax/2, FImageDown.SubPixel - 1, FImageDown.MainPixel - 1 - iMainShiftMax/2);
			for(int iS = 0; iS < iSubNumMatch; iS++){
//			for(int iM = 0; iM < FImageUp.MainPixel - iMainShiftMax; iM++){
				for(int iM = CLIP(-iMShift, 0, FImageUpROIX0.MainPixel); iM < CLIP(FImageUpROIX0.MainPixel - iMShift, 0, FImageUpROIX0.MainPixel); iM++){
					double dDeltaUp   = (FImageUpROIX0.Dt[iS][iM] - dQLaveUp);
					double dDeltaDown = (FImageDownROIX0.Dt[iS][iM + iMShift] - dQLaveDown);
					dNCC[iMShift + iMainShiftMax] += dDeltaUp * dDeltaDown;
					dNormUp     += dDeltaUp * dDeltaUp;
					dNormDown   += dDeltaDown * dDeltaDown;
				}
			}
			dNormUp2 = sqrt(dNormUp);
			dNormDown2 = sqrt(dNormDown);
//			if (0. == (dNormUp2 * dNormDown2)) {//一律同じQLで分散ゼロの場合 →相互相関が定義できず、発散するためcontinueする
			if (0 == round(dNormUp2 * dNormDown2)) {//一律同じQLで分散ゼロの場合 →相互相関が定義できず、発散するためcontinueする
				break;
			}
			dNCC[iMShift + iMainShiftMax] /= (dNormUp2 * dNormDown2);
		}
		//相互相関の最大値を探索
		int iMShift;
		dNCCMaxX0 = 0.;
		for (iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
			if (dNCCMaxX0 < dNCC[iMShift + iMainShiftMax]) {
				dNCCMaxX0 = dNCC[iMShift + iMainShiftMax];
				iMShiftDetectX0 = iMShift;
			}
		}

 #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpRel, "x = %d - %d\n", iXROI, iXROI + iWidthROI);
		fprintf(fpRel, "dQLaveUp	dQLaveDown\n");
		fprintf(fpRel, "%lf	%lf\n", dQLaveUp, dQLaveDown);
		fprintf(fpRel, "MShiftMax	NCCMax\n");
		fprintf(fpRel, "%d	%lf\n", iMShiftDetectX0, dNCCMaxX0);
		fprintf(fpRel, "MShift	NCC\n");
		for (int iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
			fprintf(fpRel, "%d	%lf\n", iMShift, dNCC[iMShift + iMainShiftMax]);
		}
		fprintf(fpRel, "\n\n");
 #endif

		//相互相関値の値によりエッジを適切に検出できているか判断
		if (dNCCMaxX0 >= dThNCC) {
			bFindEdgeX0 = true;
			break;
		}
		iROICnt++;

	} while (!bFindEdgeX0 && iROICnt * (iWidthROI - iWidthOverlap) < ImageUp.MainPixel / 2);
//	} while (!bFindEdgeX0 && iROICnt * (iWidthROI - iWidthOverlap) < ImageUp.MainPixel / 2 && 0. != (dNormUp2 * dNormDown2));
//	} while (!bFindEdgeX0 && iROICnt * iXROI < ImageUp.MainPixel / 2);

 #ifdef _LP_DEBUG_OUTPUT
	fclose(fpRel);
 #endif



	//Xendの側のROIで相互相関を計算
	iROICnt = 0;//ROIを修正する試行回数

	for (int i = 0; i < 2 * iMainShiftMax + 1; i++) {
		dNCC[i] = -100.;//デフォルトを負の値とする
	}

 #ifdef _LP_DEBUG_OUTPUT
	FILE* fpRel2 = fopen((ExtractFilePath(FileName) + "NormalizedCrossCorrelationXE.txt").c_str(), "wt");
	fprintf(fpRel2, "%s\n", FileNameImageUp.c_str());
	fprintf(fpRel2, "%s\n", FileName.c_str());
 #endif


	//X=End側のROIについて上・中２画像の正規化相互相関（NormalizedCrossCorrelation）を計算
	do{
//		iXROI = FImageUp.MainPixel - 1 - iWidthROI - iROICnt * (iWidthROI - iWidthOverlap);
		iXROI = FImageUp.MainPixel - iWidthROI - iROICnt * (iWidthROI - iWidthOverlap); //トリミングが1画素ずれていたbug修正(141216 ttsuji)
		FImageUpROIXE   = FImageUp;
		FImageDownROIXE = FImageDown;
		FImageUpROIXE.Format(FImageUpROIXE.SubPixel, iWidthROI, 0,  -iXROI);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）
		FImageDownROIXE.Format(FImageDownROIXE.SubPixel, iWidthROI, 0,  -iXROI);//中画像の上短冊を抽出（短手斜入による左右端の画欠けを考慮して30pix両端を除外してトリミング）
 #ifdef _LP_DEBUG_OUTPUT
//		FImageUpROIXE.WriteTma(ExtractFilePath(FileName)   + "F" + IntToStr(iXROI) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLFImageUp.tma");
//		FImageDownROIXE.WriteTma(ExtractFilePath(FileName) + "F" + IntToStr(iXROI) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLFImageDown.tma");
		FImageUpROIXE.WriteTma(ExtractFilePath(FileName)   + "F" + IntToStr(iXROI) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLCFImageUp.tma");
		FImageDownROIXE.WriteTma(ExtractFilePath(FileName) + "F" + IntToStr(iXROI) + "M(m" + IntToStr(iMainMoveAveMatch) + ")DLCFImageDown.tma");
 #endif

		//ROI画像に相互相関
		dQLaveUp   = FImageUpROIXE.dAve();
		dQLaveDown = FImageDownROIXE.dAve();
		for (int iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
			dNormUp = 0.;
			dNormDown = 0.;
//			double dQLaveUp   = FImageUp.dAve(0, iMainShiftMax/2, FImageUp.SubPixel - 1, FImageUp.MainPixel - 1 - iMainShiftMax/2);
//			double dQLaveDown = FImageDown.dAve(0, iMainShiftMax/2, FImageDown.SubPixel - 1, FImageDown.MainPixel - 1 - iMainShiftMax/2);
			for(int iS = 0; iS < iSubNumMatch; iS++){
//			for(int iM = 0; iM < FImageUp.MainPixel - iMainShiftMax; iM++){
				for(int iM = CLIP(-iMShift, 0, FImageUpROIXE.MainPixel); iM < CLIP(FImageUpROIXE.MainPixel - iMShift, 0, FImageUpROIXE.MainPixel); iM++){
					double dDeltaUp   = (FImageUpROIXE.Dt[iS][iM] - dQLaveUp);
					double dDeltaDown = (FImageDownROIXE.Dt[iS][iM + iMShift] - dQLaveDown);
					dNCC[iMShift + iMainShiftMax] += dDeltaUp * dDeltaDown;
					dNormUp     += dDeltaUp * dDeltaUp;
					dNormDown   += dDeltaDown * dDeltaDown;
				}
			}
			dNormUp2 = sqrt(dNormUp);
			dNormDown2 = sqrt(dNormDown);
			if (0. == (dNormUp2 * dNormDown2)) {//一律同じQLで分散ゼロの場合 →相互相関が定義できず、発散するためcontinueする
				break;
			}
			dNCC[iMShift + iMainShiftMax] /= (dNormUp2 * dNormDown2);
		}
		//相互相関の最大値を探索
		int iMShift;
		dNCCMaxXE = 0.;
		for (iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
			if (dNCCMaxXE < dNCC[iMShift + iMainShiftMax]) {
				dNCCMaxXE = dNCC[iMShift + iMainShiftMax];
				iMShiftDetectXE = iMShift;
			}
		}

 #ifdef _LP_DEBUG_OUTPUT
		fprintf(fpRel2, "x = %d - %d\n", iXROI, iXROI + iWidthROI);
		fprintf(fpRel2, "dQLaveUp	dQLaveDown\n");
		fprintf(fpRel2, "%lf	%lf\n", dQLaveUp, dQLaveDown);
		fprintf(fpRel2, "MShiftMax	NCCMax\n");
		fprintf(fpRel2, "%d	%lf\n", iMShiftDetectXE, dNCCMaxXE);
		fprintf(fpRel2, "MShift	NCC\n");
		for (int iMShift = -iMainShiftMax; iMShift <= iMainShiftMax; iMShift++) {
			fprintf(fpRel2, "%d	%lf\n", iMShift, dNCC[iMShift + iMainShiftMax]);
		}
		fprintf(fpRel2, "\n\n");
 #endif

		//相互相関値の値によりエッジを適切に検出できているか判断
		if (dNCCMaxXE >= dThNCC) {
			bFindEdgeXE = true;
			break;
		}
		iROICnt++;

	} while (!bFindEdgeXE && iROICnt * (iWidthROI - iWidthOverlap) < ImageUp.MainPixel / 2);
//	} while (!bFindEdgeXE && iROICnt * (iWidthROI - iWidthOverlap) < ImageUp.MainPixel / 2  && 0. != (dNormUp2 * dNormDown2));

	delete[] dNCC;
 #ifdef _LP_DEBUG_OUTPUT
	fclose(fpRel2);
 #endif



	if (bFindEdgeX0 & bFindEdgeXE) {//画像の左右端それぞれから正しくエッジ検出できた場合
		iMShiftDetect = round((iMShiftDetectX0 + iMShiftDetectXE) / 2.);//X=0側のエッジの左右シフトとX＝END側のエッジの左右シフトとを平均処理する
	}
#if 1 //桑原さん指摘（照射野を狭く絞って左右のいずれかによせて短手斜入されたケースでずれる）により修正（141117 ttsuji）
	else if (bFindEdgeX0) {
		iMShiftDetect = iMShiftDetectX0;
	}
	else if (bFindEdgeXE) {
		iMShiftDetect = iMShiftDetectXE;
	}
#endif
	else{
		iMShiftDetect = 0;//適切に求まっていないため、0を出力する
	}

#if 0 //手動で左右位置ずれ量を指定(tmp)
	iMShiftDetect = 0;
#endif

	//上画像を中画像と絵のパターンが合うように左右シフト
#if 0 //白で埋める
	ImageUp.Format(ImageUp.SubPixel, ImageUp.MainPixel, 0, iMShiftDetect);//中画像の上短冊を抽出 bug(左右シフトしてできた空間を真っ白で埋めていた)
#elif 1 //黒で埋める
	ImageUp.Format(ImageUp.SubPixel, ImageUp.MainPixel, 0, iMShiftDetect, ImageUp.DtMax);//中画像の上短冊を抽出 (左右シフトしてできた空間を真っ黒で埋めるよう修正)
#else //Dupe拡大して補間する (141201 ttsuji)
	const int iMMedDupe = 5;//tmp
	TTMA MedImageUp;
	if (iMShiftDetect) {
		MedImageUp = ImageUp;
		MedImageUp.MainMedian(iMMedDupe);
 #ifdef _LP_DEBUG_OUTPUT
		MedImageUp.WriteTma(ExtractFilePath(FileName) + "ImageUpMMed(" + IntToStr(iMMedDupe) + ").tma");
 #endif
	}
	ImageUp.Format(ImageUp.SubPixel, ImageUp.MainPixel, 0, iMShiftDetect, ImageUp.DtMax);//中画像の上短冊を抽出 (左右シフトしてできた空間を真っ黒で埋めるよう修正)
	//Top画像の下端をDupe拡大する(Centerパネルの重複領域の段差補正用にのみ行い、合成時はDupe拡大していない黒塗りしたものを使うべき？)
	for (int iS = 0; iS < ImageUp.SubPixel; iS++) {
		if (iMShiftDetect > 0) {//空白領域はxのStart側
			for (int iM = 0; iM < iMShiftDetect; iM++) {
				ImageUp.SetVal(iS, iM, MedImageUp.Dt[iS][iMMedDupe/2]);
			}
		}
		else if (iMShiftDetect < 0) {//空白領域はxのEnd側
			for (int iM = MedImageUp.MainPixel - 1; iM > MedImageUp.MainPixel - 1 + iMShiftDetect; iM--) {
				ImageUp.SetVal(iS, iM, MedImageUp.Dt[iS][MedImageUp.MainPixel - 1 - iMMedDupe/2]);
			}
		}
	}

#endif

 #ifdef _LP_DEBUG_OUTPUT
	ImageUp.WriteTma(ExtractFilePath(FileName) + "ImageUpMShift" + IntToStr(iMShiftDetect) + ".tma");
	WriteTma(ExtractFilePath(FileName) + "ImageDown.tma");
 #endif
#endif
}
void CLPCorrectionS::LPCorrectOverlapAreaReferToImageUp(TTMA& ImageUp, TTMA& CorrImg, TTMA& QLRateImg, string FileName, int* edgeY, int iKasa, int iYSintiEnd, double dNormQL, int iSubRangeOnlyMidPanel, int iMainMedianDiffImg, int iSubMedianDiffImg, int iMShiftDetect){

}
//斜めエッジをQL比閾値判定で検出し、黒い方の画素値を白くしてDC補間（斜めエッジ周辺に高周波横スジ発生させる副作用あり）
void CLPCorrectionS::LPCorrectNonOverlapAreaByItplDC(TTMA& CorrImg, string FileName, int* edgeY, int iKasa, int iYSintiEnd, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iMainMedianDCItpl, float& fThQLRateUD, float& fMagicPara){

}
//斜めエッジをQL比閾値判定で検出し、その箇所だけDC補間の長さを短くする（実装途中）
void CLPCorrectionS::LPCorrectNonOverlapAreaByItplDC2(TTMA& CorrImg, string FileName, int* edgeY, int iKasa, int iYSintiEnd, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iMainMedianDCItpl, float& fThQLRateUD, float& fMagicPara){

}
//斜めエッジをQL比閾値判定で検出し、かつ副微分で斜めエッジのｙ座標を特定し、斜めエッジの上側と下側とでそれぞれ三角補間する
void CLPCorrectionS::LPCorrectNonOverlapAreaByItplDC3(TTMA& CorrImg, string FileName, int* edgeY, int iKasa, int iYSintiEnd, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iMainMedianDCItpl, float& fThQLRateUD){

}
//斜めエッジをQL比閾値判定で検出し、かつ副微分で斜めエッジのｙ座標を特定し、斜めエッジに平行に平行四辺形補間する
void CLPCorrectionS::LPCorrectNonOverlapAreaByItplDC4(TTMA& CorrImg, string FileName, int* edgeY, int iKasa, int iYSintiEnd, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iMainMedianDCItpl, bool bDetectKanajakuMemori, int iMedianHasiSyori, int iThQLSubDiffForScaleSearch, int iSubAveNum, int iMarginXMed, bool& bExistPbScale){

}
//DC補間領域内の横長の金属エッジを探索する(141108 ttsuji 金尺の目盛り消失対策として)
void CLPCorrectionS::LPDetectPbScaleWhiteBandXROI(TTMA MedLogImg, string FileName, int* edgeY, int iDx, int iThQLSubProf, int iThQLMainProf, int iSubRangeOnlyMidPanel, int iItplSubOffset, int& iNumOfROI, ROI* xROI){

}
void CLPCorrectionS::LPDetectPbScaleEdgeXROI(TTMA MedLogImg, string FileName, int* edgeY, int iDx, int iThQLSubProf, int iThQLMainProf, int iSubRangeOnlyMidPanel, int iItplSubOffset, int& iNumOfXROI, ROI* XROI, int& iNumOfXROIMed, ROI* XROIMed, bool& bExistPbScale){

}
void CLPCorrectionS::LPDetectPbScaleEdgeXROI_Glass(TTMA MedLogImg, string FileName, int* edgeY, int* edgeYSinti, int iDx, int iThQLSubProf, int iThQLMainProf, int iSubOffset, int& iNumOfXROI, ROI* XROI, int iSMargin){

}
//探索した金属エッジが存在するｘ領域のROIごとに必要なMedianマスクを算出し、Median処理しなおす。
void CLPCorrectionS::LPMainMedianForEachROI(TTMA& CorrImg, int* edgeY, int iNumOfXROI, ROI* XROI, int iMainMedianDCItpl, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iKasa){

}
//探索した金属エッジが存在するｘ領域のROIごとに必要なMedianマスクを算出し、Median処理しなおす。
void CLPCorrectionS::LPMainMedianAndCopyForEachROI(TTMA& CorrImg, string FileName, int* edgeY, int iNumOfXROI, ROI* XROI, int iNumOfXROIMed, ROI* XROIMed, int iMainMedianDCItpl, int iSubRangeOnlyMidPanel, int iItplSubOffset, int iKasa, int iMedianHasiSyori, int iMarginXMMed){

}


