/*
 * CTTMA.cpp
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#include "CTTMA.h"

#include <fcntl.h>
#include <io.h>
#include <math.h>
#include <sys\stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CTTMA.h"
#include "CTFIMG.h"
//#include "Common.h"
#include "utility.h"
#include "CApplication.h"

//TCommon Common;
//---------------------------------------------------------------------------
/*

*/
float TSIMGH::MitsudoToPixelSize(char* psMitsudo)
{
    if(strcmp(psMitsudo, "67") == 0){
        return(0.15);
    }
    return(1.0 / atoi(psMitsudo));
}
//---------------------------------------------------------------------------
/*
	Read "*.tma"
       	引数	File Name

        戻り値	成功->true
        		失敗->false
*/
bool TSIMGH::ReadTma(const string fileName)
{
    int handle;		//file handler
    char *head_ptr;
    char dummy;
    char cDummy[HEADDER_SIZE];

//    if(ExtractFileExt(fileName).AnsiCompareIC(".tma") != 0){
//    	CApplication::GetInstance()->MessageBox("File が tma ではありません", "Error", MB_OK);
//        return(false);
//    }

    DeleteDt();		//data消去

//Read Headder File
    if ((handle = open(fileName.c_str(), O_RDONLY|O_BINARY)) == -1){
    	CApplication::GetInstance()->MessageBox((fileName + " openできませんでした").c_str(), "Error", MB_OK);
    	return(false);
    }
    if ((read(handle, Head, HEADDER_SIZE)) == -1){
    	CApplication::GetInstance()->MessageBox((fileName + " readできませんでした").c_str(), "Error", MB_OK);
    	close(handle);
        return(false);
    }
    close(handle);

	dummy = Head[X_FILE_SIZE];		//主走査画素数
    Head[X_FILE_SIZE] = '\0';
    MainPixel = atoi(Head);
    Head[X_FILE_SIZE] = dummy;

    dummy = Head[X_FILE_SIZE + Y_FILE_SIZE];		//副走査画素数
    Head[X_FILE_SIZE + Y_FILE_SIZE] = '\0';
    head_ptr = Head + X_FILE_SIZE;
    SubPixel = atoi(head_ptr);
    Head[X_FILE_SIZE + Y_FILE_SIZE] = dummy;

	mainGasoMitsudo[0] = Head[10];
    mainGasoMitsudo[1] = Head[11];
    mainGasoMitsudo[2] = 0;
	SetMainPixelSize(MitsudoToPixelSize(mainGasoMitsudo));

    subGasoMitsudo[0] = Head[12];
    subGasoMitsudo[1] = Head[13];
    subGasoMitsudo[2] = 0;
	SetSubPixelSize(MitsudoToPixelSize(subGasoMitsudo));

    if(bSetBit(Head[BIT] - '0' + 10) == false){		//画像data bit
        return(false);
    }
    if(Head[iEDR_POS] == '0'){
        iEDR = iEDR_OFF;
    }else if(Head[iEDR_POS] == '1'){
        iEDR = iEDR_FIX;
    }else if(Head[iEDR_POS] == '2'){
        iEDR = iEDR_SEMIAUTO;
    }else{
        iEDR = iEDR_OFF;
    }

    for(int iCnt = 0; iCnt < iSk_BYTE; ++iCnt){
        cDummy[iCnt] = Head[iSk_POS + iCnt];
    }
    cDummy[iSk_BYTE] = 0;
    iSk = atoi(cDummy);

    for(int iCnt = 0; iCnt < iL_BYTE; ++iCnt){
        cDummy[iCnt] = Head[iL_POS + iCnt];
    }
    cDummy[iL_BYTE] = 0;
    iL = atoi(cDummy);

    if(Head[COMP_LOC] == '0'){
		return(ReadDt(ChangeFileExt(fileName, ".tdt")));		//圧縮なし
    }else{
        return(false);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*
	Read "*.tma"
		引数	File Name

        戻り値	成功->true
        		失敗->false
*/
bool TTMA::ReadTma(const string fileName, const bool bShowError)
{
	if(ExtractFileExt(fileName) != ".tma"){
		if(bShowError){
			CApplication::GetInstance()->MessageBox("File が tma ではありません", "Error", MB_OK);
		}
		return(false);
	}

    int iCompress = iReadHeadder(fileName);
    if (iCompress < 0){
		if(bShowError){
			CApplication::GetInstance()->MessageBox((fileName + " readできませんでした").c_str(), "Error", MB_OK);
		}
        return(false);
    }

    if(iCompress == '0'){
    	return(ReadDt(ChangeFileExt(fileName, ".tdt")));		//圧縮なし
    }else if(iCompress == 'D'){
    	return(ReadDtD(ChangeFileExt(fileName, ".tdt")));		//新圧縮
    }else{
    	if (bShowError){
    		CApplication::GetInstance()->MessageBox("圧縮無しと新圧縮Fileしか扱えません", "Error", MB_OK);
    	}
        return(false);
    }
}
//---------------------------------------------------------------------------
/*
	Save "*.tma" headder file only
       	引数	File Name

        戻り値	成功->true
        		失敗->false
*/
bool TTMA::SaveTma(const string fileName, const bool bVerify)
{
    int i;
    int handle;
    char ySize[Y_FILE_SIZE + 1];
    string fn;		//fileName
    char cDummy[HEADDER_SIZE];

    fn = ChangeFileExt(fileName, ".tma");

// 	//上書きするか確認
//	if(bVerify){
//		if(Common.OverWrite(fn) == false){
//			return(false);//上書き拒否
//		}
//	}

//Hedder File出力
	sprintf(Head, "%05d", MainPixel);
    sprintf(ySize, "%05d", SubPixel);

    strcat(Head, ySize);
    SetGasoMitsudo();
    strcat(Head, mainGasoMitsudo);
    strcat(Head, subGasoMitsudo);

    for (i = 14 ; i < 24 ; ++i)
        Head[i] = 0;
    Head[COMP_LOC] = '0';
    for (i = 25 ; i < 256 ; ++i)
        Head[i] = ' ';
    Head[BIT] = char('0' + Bit - 10);		//画像data bit数 (bit - 10)の文字

    if(iEDR == iEDR_OFF){
        Head[iEDR_POS] = '0';
    }else if(iEDR == iEDR_FIX){
        Head[iEDR_POS] = '1';
    }else if(iEDR == iEDR_SEMIAUTO){
        Head[iEDR_POS] = '2';
    }else{
        Head[iEDR_POS] = '0';
    }

    sprintf(cDummy, "%03d", iSk);
    for(int iCnt = 0; iCnt < iSk_BYTE; ++iCnt){
        Head[iSk_POS + iCnt] = cDummy[iCnt];
    }

    sprintf(cDummy, "%03d", iL);
    for(int iCnt = 0; iCnt < iL_BYTE; ++iCnt){
		Head[iL_POS + iCnt] = cDummy[iCnt];
    }

#if 0 //ttsuji (open関数だとなぜかアクセスバイオレーションの例外エラーが出てコードガード使用できない)
	if ((handle = open(fn.c_str(), O_RDWR|O_CREAT|O_BINARY|O_TRUNC, S_IWRITE)) == -1){
		CApplication::GetInstance()->MessageBox((fn + " openできませんでした").c_str(), "", MB_OK);
		return(false);
	}
	if ((write(handle, Head, HEADDER_SIZE)) == -1){
		CApplication::GetInstance()->MessageBox((fn + " writeできませんでした").c_str(), "", MB_OK);
		close(handle);
		return(false);
	}
	close(handle);
#else
	FILE* fp = fopen(fn.c_str(), "wb");
	if (!fp) {
		CApplication::GetInstance()->MessageBox((fn + " openできませんでした").c_str(), "", MB_OK);
		return false;
	}
	size_t size = fwrite(Head, sizeof(char), HEADDER_SIZE, fp);
	if (size < HEADDER_SIZE) {
		CApplication::GetInstance()->MessageBox((fn + " writeできませんでした").c_str(), "", MB_OK);
		fclose(fp);
		return false;
	}
	fclose(fp);
#endif

	return(true);
}
//---------------------------------------------------------------------------
//Tdtファイル作成関数(ttsuji追加)
bool TTMA::SaveTmaTdt(const string fileName)
{
	if (!WriteTma(fileName)) {
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
/*
	Write "*.tma"
       	引数	File Name

        戻り値	成功->true
        		失敗->false
*/
bool TTMA::WriteTma(const string fileName, const bool bVerify)
{
    int i;
	int handle;
    string fn;		//fileName

	if (!SaveTma(fileName, bVerify)){
    	return false;//上書き拒否などでtmaファイル保存されなかった場合はtdtも保存しない！(ttsuji)
	}

//画像data File出力
	fn = ChangeFileExt(fileName, ".tdt");
#if 0 //ttsuji (open関数だとなぜかアクセスバイオレーションの例外エラーが出てコードガード使用できない)
	if ((handle = open(fn.c_str(), O_RDWR|O_CREAT|O_BINARY|O_TRUNC, S_IWRITE)) == -1){
		CApplication::GetInstance()->MessageBox((fn + " openできませんでした").c_str(), "", MB_OK);
		return(false);
	}

	for(i = 0 ; i < SubPixel ; ++i){
		if ((write(handle, Dt[i], MainPixel * 2)) == -1){
			CApplication::GetInstance()->MessageBox((fn + " writeできませんでした").c_str(), "", MB_OK);
			close(handle);
			return(false);
		}
	}
	close(handle);
#else
	FILE* fp = fopen(fn.c_str(), "wb");
	if (!fp) {
		CApplication::GetInstance()->MessageBox((fn + " openできませんでした").c_str(), "", MB_OK);
		return false;
	}
	for(i = 0 ; i < SubPixel ; ++i){
		size_t size = fwrite(Dt[i], sizeof(unsigned short int), MainPixel, fp);
		if (size < (unsigned int)MainPixel) {
			CApplication::GetInstance()->MessageBox((fn + " writeできませんでした").c_str(), "", MB_OK);
			fclose(fp);
			return false;
		}
	}
	fclose(fp);
#endif
    return(true);
}
////---------------------------------------------------------------------------
///*
//	ヘッダ付の動画データ（複数フレーム画像データ）(*.sif)を、
//	ヘッダなしの複数の画像データに分解する(ttsuji追加)
//*/
//bool TTMA::ReadFramesSkipHead(const string fileName, const int NumOfFrame)
//{
//#if 0		//TODO とりあえずコメントアウト
//	if (!MainPixel || !SubPixel) {
//		CApplication::GetInstance()->MessageBox("主副画素数の設定が不正です", "Error", MB_OK);
//		return false;
//	}
//	if (1 > NumOfFrame) {
//		CApplication::GetInstance()->MessageBox("分解フレーム数の設定が不正です", "Error", MB_OK);
//		return false;
//	}
//	NewDt(SubPixel, MainPixel, Bit);
//
//	ifstream inFile(fileName.c_str(), ios::in | ios::binary);
//	if(!inFile){
//		CApplication::GetInstance()->MessageBox((fileName + " openできませんでした").c_str(), "Error", MB_OK);
//		return false;
//	}
//	// ファイルサイズ計算
//	long file_size, head_size;
//	inFile.seekg(0, ios::end);
//	file_size = inFile.tellg();
//	inFile.seekg(0);
//	head_size = file_size - MainPixel * SubPixel * 2 * NumOfFrame;
//	if(head_size < 0){
//		CApplication::GetInstance()->MessageBox("不正なファイルサイズの画像です", "Error", MB_OK);
//		inFile.close();
//		return false;
//	}
//
//	int i, iFrame;
//	char tmp;//1byte
//	unsigned short int data;//2byte
//	for (i = 0; i < head_size; i++){
//		inFile.read((unsigned char*)&tmp, sizeof(char));//読み飛ばし
//	}
//	for (iFrame = 0 ; iFrame < NumOfFrame ; iFrame++){
//		string str = fileName;
//		for (i = 0 ; i < SubPixel ; i++){
//			inFile.read((unsigned char*)Dt[i], sizeof(unsigned short int) * MainPixel);
//		}
//		SaveTmaTdt(str.Insert("_" + IntToStr(iFrame + 1), str.Length() - 3));//".tmaの前に文字挿入"
//	}
//	inFile.close();
//#endif
//
//	return true;
//}
////---------------------------------------------------------------------------
///*
//     べた画像EDRもどき
//*/
//void TTMA::AntiEDR()
//{
//    AntiEDR(float(iGetS()), iL / 100.0, 12);
//}
////---------------------------------------------------------------------------
///*
//     べた画像EDRもどき
//*/
//void TTMA::AntiEDR(const int iS, const float fL, const int iNewBit)
//{
//    if(Bit < iNewBit){        //計算はbit数が多きい方で実施
//        BitChange(iNewBit);
//    }
//    double dA = fL / 4.0f;
//    double dB = DtMax >> 1;
//    double dC = (DtMax + 1) / 4.0 * log10(200.0 / iS) + dB;
//
//    TTMA table;
//
//    table.NewDt(1, DtMax + 1, Bit);
//    for(int iCnt = 0; iCnt <= DtMax; ++iCnt){
//        table.SetVal(0, iCnt, dA * (iCnt - dB) + dC);
//    }
//    for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
//        for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
//            Dt[iSCnt][iMCnt] = table.Dt[0][Dt[iSCnt][iMCnt]];
//        }
//    }
//    if(Bit > iNewBit){
//        BitChange(iNewBit);
//    }
//    iEDR = iEDR_OFF;
//    SetSkWithS(iS);
//    SetL(fL);
//}
////---------------------------------------------------------------------------
///*
//     べた画像EDRもどき
//*/
//void TTMA::BetaEDR(const float fMag)
//{
//    int iMCnt, iSCnt;
//    TTMA org;
//    org.NewDt(SubPixel, MainPixel, Bit, fMainPixelSize());
//    for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
//        for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
//            org.Dt[iSCnt][iMCnt] = Dt[iSCnt][iMCnt];
//        }
//    }
//
//    // --- 定数 ---
//    const int iBit = 10;
//    const float fMEDIAN = 1.0f;   // 除去する異常値の長さ[mm]
//    const float fFIELD  = 100.0f; // 計算領域の大きさ[mm]
//    int iMedianLength   = ((int) (fMEDIAN/m_fMainPixelSize+0.5)) * 2 + 1;
//                                  // メディアンフィルターの大きさ[pix]
//
//    // --- メディアンフィルターをかける ---
//    MainMedian(iMedianLength);
//    SubMedian( iMedianLength); // 副方向も主方向と同じ長さを使う
//
//    // --- 計算領域の下端・上端のpixelを求める。端数は四捨五入 ---
//    int iMainMin = (int) (MainPixel/2 - fFIELD/m_fMainPixelSize/2 + 0.5);
//    int iSubMin  = (int) (SubPixel/2  - fFIELD/m_fSubPixelSize /2 + 0.5);
//    int iMainMax = iMainMin + (int) (fFIELD/m_fMainPixelSize +0.5);
//    int iSubMax  = iSubMin  + (int) (fFIELD/m_fSubPixelSize  +0.5);
//    // (計算領域が画像より大きいときは画像に合わせる)
//    if( iMainMin < 0 ) iMainMin = 0;
//    if( iSubMin  < 0 ) iSubMin  = 0;
//    if( iMainMax >= MainPixel ) iMainMax = MainPixel-1;
//    if( iSubMax  >= SubPixel  ) iSubMax  = SubPixel-1;
//
//    // --- 計算領域内の平均値を求める ---
//    float fAve = dAve( iSubMin, iMainMin, iSubMax, iMainMax );
//
//    // --- DCを調整する ---
//    // 平均値は 2^(変換前ビット数)/2-1 = DtMax に割り当てる。
//    for( int iSub=0; iSub<SubPixel; iSub++){
//        for ( int iMain=0; iMain<MainPixel; iMain++){
//            SetVal( iSub, iMain, fMag * (org.Dt[iSub][iMain]-fAve) + DtMax/2 );
//        }
//    }
//
//    // --- ビット変換 ---
//    BitChange( iBit );
//}
////---------------------------------------------------------------------------
///*
//
//*/
//bool TTMA::bReadFromText(const string& fileName, const int iSkipLine,
//        const char cMark,       //text　区切り文字
//        TTMA& maxSizeImg)       //読み取り最大画素数と画像情報が設定されていること　画像dataは破壊される
//{
//    fstream file(fileName.c_str(), ios::in);
//    if(file.fail()){
//        return(false);
//    }
//
//    const int iMAX_LEN = 21 * maxSizeImg.MainPixel;
//    char *tempChar = new char[iMAX_LEN];
//
//    for(int iCnt = 0; iCnt < iSkipLine; ++iCnt){
//        file.getline(tempChar, iMAX_LEN);
//    }
//
//    maxSizeImg.FillValue(0);
//    int iSCnt = 0;
//    int iMMax = 0;
//    while(!file.eof()){
//        file.getline(tempChar, iMAX_LEN);
//        char *pt = tempChar;
//        if(*pt < ' '){
//            break;
//        }
//        char *pos;
//        int iMCnt = 0;
//        while((pos = strchr(pt, cMark)) != NULL){
//            *pos = 0;
//            maxSizeImg.SetVal(iSCnt, iMCnt, atof(pt));
//            pt = pos + 1;
//            while(*pt == ' '){
//                ++pt;
//            }
//            ++iMCnt;
//            if(iMCnt >= maxSizeImg.MainPixel - 1){
//                break;
//            }
//        }
//        maxSizeImg.SetVal(iSCnt, iMCnt, atof(pt));
//        ++iMCnt;
//        ++iSCnt;
//        if(iSCnt >= maxSizeImg.SubPixel){
//            break;
//        }
//        if(iMCnt > iMMax){
//            iMMax = iMCnt;
//        }
//    }
//
//    NewDt(iSCnt, iMMax, maxSizeImg.Bit, maxSizeImg.fMainPixelSize(), maxSizeImg.iGetEDR(), maxSizeImg.iGetSk(), maxSizeImg.iGetL());
//    for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
//        for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
//            Dt[iSCnt][iMCnt] = maxSizeImg.Dt[iSCnt][iMCnt];
//        }
//    }
//    delete[] tempChar;
//
//    return(true);
//}
////---------------------------------------------------------------------------
///*
//   旧ＵＳＭ処理
//*/
//bool TTMA::bUSM(const int iRn, const float fRe)
//{
//   if(iRn < 0 || iRn > 9){
//      return(false);
//   }                                //iRnのチェック
//   if(fRe < 0.0f || fRe > 16.0f){
//      return(false);                //fReのチェック
//   }
//
//   int iMASK[4][10] = {{319, 229, 159, 115, 81, 57, 41, 29, 21, 15},
//                       {127, 115, 81, 57, 41, 29, 21, 15, 11, 7},
//                       {109, 77, 55, 39, 27, 19, 13, 9, 7, 5},
//                       {81, 57, 41, 29, 21, 15, 11, 7, 5, 3}};  //マスクサイズ
//
//   int iMainDens, iSubDens;
//
//   if(fMainPixelSize() == 0.05f){
//      iMainDens = 0;
//   }else if(fMainPixelSize() == 0.1f){
//      iMainDens = 1;
//   }else if(fMainPixelSize() == 0.15f){
//      iMainDens = 2;
//   }else if(fMainPixelSize() == 0.2f){
//      iMainDens = 3;
//   }else{
//      return(false);
//   }
//
//   if(fSubPixelSize() == 0.05f){
//      iSubDens = 0;
//   }else if(fSubPixelSize() == 0.1f){
//      iSubDens = 1;
//   }else if(fSubPixelSize() == 0.15f){
//      iSubDens = 2;
//   }else if(fSubPixelSize() == 0.2f){
//      iSubDens = 3;
//   }else{
//      return(false);
//   }
//
//   TFIMG tAve;
//   Copy(tAve);
//
//   int iMainMask, iSubMask;
//   iMainMask = iMASK[iMainDens][iRn];
//   iSubMask = iMASK[iSubDens][iRn];
//   tAve.MainMoveAve(iMainMask);
//   tAve.SubMoveAve(iSubMask);
//
//   for(int iSubCnt = 0; iSubCnt < SubPixel; iSubCnt++){
//      for(int iMainCnt = 0; iMainCnt < MainPixel; iMainCnt++){
//         SetVal(iSubCnt, iMainCnt,
//                    Dt[iSubCnt][iMainCnt] + fRe * (Dt[iSubCnt][iMainCnt] - tAve.Dt[iMainCnt][iSubCnt]));
//      }
//   }
//   return(true);
//}
////---------------------------------------------------------------------------
///*
//*/
//void TTMA::EDR(const int iS, const float fL, const int iNewBit)
//{
//    if(iEDR != iEDR_OFF){
//        AntiEDR();
//    }
//
//    if(Bit < iNewBit){
//        BitChange(iNewBit);
//    }
//
//    double dA = 4.0 / fL;
//    double dC = DtMax >> 1;
//    double dB = (DtMax + 1) / 4.0 * log10(200.0 / iS) + dC;
//
//    CTDAT table;
//    table.NewDt(1, DtMax + 1, Bit);
//    for(int iCnt = 0; iCnt <= DtMax; ++iCnt){
//        table.SetVal(0, iCnt, dA * (iCnt - dB) + dC);
//    }
//    for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
//        for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
//            Dt[iSCnt][iMCnt] = table.Dt[0][Dt[iSCnt][iMCnt]];
//        }
//    }
//
//    if(Bit > iNewBit){
//        BitChange(iNewBit);
//    }
//
//    iEDR = iEDR_FIX;
//    SetSkWithS(iS);
//    SetL(fL);
//}
////---------------------------------------------------------------------------
///*
//*/
//void TTMA::EDR()
//{
//    EDR(iGetS(), fGetL());
//}
////---------------------------------------------------------------------------
//int TTMA::iGetS()
//{
//    return(int(200.0 / pow(10.0, iSk / 100.0 - 2.0) + 0.5));
//}
////---------------------------------------------------------------------------
///*
//
//*/
int TTMA::iReadHeadder(string fileName)
{
    const int iERROR = -1;

    int handle;		//file handler
    char *head_ptr;
    char dummy;
	char cDummy[HEADDER_SIZE];

//Read Headder File

	FILE* fp = fopen(fileName.c_str(), "rb");
	if (!fp) {
		return (iERROR);
	}
	size_t size = fread(Head, sizeof(char), HEADDER_SIZE, fp);
	if (!size) {
		 fclose(fp);
		 return (iERROR);
	}
	fclose(fp);
	DeleteDt();

	dummy = Head[X_FILE_SIZE];		//主走査画素数
	Head[X_FILE_SIZE] = '\0';
	MainPixel = atoi(Head);
	Head[X_FILE_SIZE] = dummy;

	dummy = Head[X_FILE_SIZE + Y_FILE_SIZE];		//副走査画素数
	Head[X_FILE_SIZE + Y_FILE_SIZE] = '\0';
	head_ptr = Head + X_FILE_SIZE;
	SubPixel = atoi(head_ptr);
	Head[X_FILE_SIZE + Y_FILE_SIZE] = dummy;

	mainGasoMitsudo[0] = Head[10];
	mainGasoMitsudo[1] = Head[11];
	mainGasoMitsudo[2] = 0;
	SetMainPixelSize(MitsudoToPixelSize(mainGasoMitsudo));

	subGasoMitsudo[0] = Head[12];
	subGasoMitsudo[1] = Head[13];
	subGasoMitsudo[2] = 0;
	SetSubPixelSize(MitsudoToPixelSize(subGasoMitsudo));

	SetBit(Head[BIT] - '0' + 10);		//画像data bit

	if(Head[iEDR_POS] == '0'){
		iEDR = iEDR_OFF;
	}else if(Head[iEDR_POS] == '1'){
		iEDR = iEDR_FIX;
	}else if(Head[iEDR_POS] == '2'){
		iEDR = iEDR_SEMIAUTO;
	}else{
		iEDR = iEDR_OFF;
	}

	for(int iCnt = 0; iCnt < iSk_BYTE; ++iCnt){
		cDummy[iCnt] = Head[iSk_POS + iCnt];
	}
	cDummy[iSk_BYTE] = 0;
	iSk = atoi(cDummy);

	for(int iCnt = 0; iCnt < iL_BYTE; ++iCnt){
		cDummy[iCnt] = Head[iL_POS + iCnt];
	}
	cDummy[iL_BYTE] = 0;
	iL = atoi(cDummy);

	return(Head[COMP_LOC]);
}
////---------------------------------------------------------------------------
///*
//
//*/
float TTMA::MitsudoToPixelSize(char* psMitsudo)
{
	if(strcmp(psMitsudo, "67") == 0){
		return(0.15);
	}
#if 0 //ttsuji（ゼロ割の実行エラー回避）
	return(1.0 / atoi(psMitsudo));
#else
	int iMitsudo = atoi(psMitsudo);
	if (!iMitsudo) {
		 return (0.0);
	}
	return (1.0/iMitsudo);
#endif
}
//---------------------------------------------------------------------------
/*

*/
void TTMA::NewDt(const int sub, const int main, const int bit, const float fPixelSize,
            const int iEDRVal,
            const int iSkVal,
//            const int iLVal)
            const int iLVal, const string cComment)
{
    CTDAT::NewDt(sub, main, bit);
    SetMainPixelSize(fPixelSize);
    SetSubPixelSize(fPixelSize);
    iEDR = iEDRVal;
    iSk = iSkVal;
    iL = iLVal;
    Comment = cComment;
}
////---------------------------------------------------------------------------
///*
//	Read "*.SCN"
//       	引数	File Name
//
//        戻り値	成功->true
//        		失敗->false
//*/
//bool TTMA::ReadSCN(const string fileName)
//{
//    if(ExtractFileExt(fileName) != ".hed"  && ExtractFileExt(fileName) != ".scn"){
//    	CApplication::GetInstance()->MessageBox("File が SCN ではありません", "Error", MB_OK);
//        return(false);
//    }
//
//    int iCompress = iReadHeadder(fileName);
//    if (iCompress < 0){
//    	CApplication::GetInstance()->MessageBox((fileName + " readできませんでした").c_str(), "Error", MB_OK);
//        return(false);
//    }
//
//    if(iCompress == '0'){
//    	return(ReadDt(ChangeFileExt(fileName, ".tdt")));		//圧縮なし
//    }else if(iCompress == 'D'){
//    	return(ReadDtD(ChangeFileExt(fileName, ".tdt")));		//新圧縮
//    }else{
//    	CApplication::GetInstance()->MessageBox("圧縮無しと新圧縮Fileしか扱えません", "Error", MB_OK);
//        return(false);
//    }
///*
//    const int ATLAS_FILE = 25;      //ATLAS画像の.hedのbyte数は25byte
//
//    int handle;		//file handler
//    char *head_ptr;
//    char dummy;
//    int fileLen;		//file byte数
//    char cDummy[HEADDER_SIZE];
//
//    DeleteDt();		//data消去
//
////Read Headder File
//    if ((handle = open(fileName.c_str(), O_RDONLY|O_BINARY)) == -1){
//    	CApplication::GetInstance()->MessageBox((fileName + " openできませんでした").c_str(), "Error", MB_OK);
//    	return(false);
//    }
//    if ((read(handle, Head, HEADDER_SIZE)) == -1){
//    	CApplication::GetInstance()->MessageBox((fileName + " readできませんでした").c_str(), "Error", MB_OK);
//    	close(handle);
//        return(false);
//    }
//    fileLen = filelength(handle);		//file byte数
//    close(handle);
//
//	dummy = Head[X_FILE_SIZE];		//主走査画素数
//    Head[X_FILE_SIZE] = '\0';
//    MainPixel = atoi(Head);
//    Head[X_FILE_SIZE] = dummy;
//
//    dummy = Head[X_FILE_SIZE + Y_FILE_SIZE];		//副走査画素数
//    Head[X_FILE_SIZE + Y_FILE_SIZE] = '\0';
//    head_ptr = Head + X_FILE_SIZE;
//    SubPixel = atoi(head_ptr);
//    Head[X_FILE_SIZE + Y_FILE_SIZE] = dummy;
//
//    mainGasoMitsudo[0] = Head[10];
//    mainGasoMitsudo[1] = Head[11];
//    mainGasoMitsudo[2] = 0;
//    SetMainPixelSize(MitsudoToPixelSize(mainGasoMitsudo));
//
//    subGasoMitsudo[0] = Head[12];
//    subGasoMitsudo[1] = Head[13];
//    subGasoMitsudo[2] = 0;
//    SetSubPixelSize(MitsudoToPixelSize(subGasoMitsudo));
//
//    if(Head[iEDR_POS] == '0'){
//        iEDR = iEDR_OFF;
//    }else if(Head[iEDR_POS] == '1'){
//        iEDR = iEDR_FIX;
//    }else if(Head[iEDR_POS] == '2'){
//        iEDR = iEDR_SEMIAUTO;
//    }else{
//        iEDR = iEDR_OFF;
//    }
//
//    for(int iCnt = 0; iCnt < iSk_BYTE; ++iCnt){
//        cDummy[iCnt] = Head[iSk_POS + iCnt];
//    }
//    cDummy[iSk_BYTE] = 0;
//    iSk = atoi(cDummy);
//
//    for(int iCnt = 0; iCnt < iL_BYTE; ++iCnt){
//        cDummy[iCnt] = Head[iL_POS + iCnt];
//    }
//    cDummy[iL_BYTE] = 0;
//    iL = atoi(cDummy);
//
//
//    if(fileLen == ATLAS_FILE){
//        SetBit(16);		//ATLAS画像16bit
//    }else{
//        SetBit(Head[BIT] - '0' + 10);		//画像data bit
//    }
//
//    if(Head[COMP_LOC] == '0'){
//    	return(ReadDt(ChangeFileExt(fileName, ".dat")));		//圧縮なし
//    }else if(Head[COMP_LOC] == 'D'){
//    	return(ReadDtD(ChangeFileExt(fileName, ".dat")));		//新圧縮
//    }else{
//    	CApplication::GetInstance()->MessageBox("圧縮無しと新圧縮Fileしか扱えません", "Error", MB_OK);
//        return(false);
//    }
//*/
//}
////---------------------------------------------------------------------------
///*
//	Write "*.scn"
//       	引数	File Name
//
//        戻り値	成功->true
//        		失敗->false
//*/
//bool TTMA::WriteSCN(const string fileName)
//{
//    int i;
//    char scn[1024];
//    int handle;
//    char ySize[Y_FILE_SIZE + 1];
//    string fn;		//fileName
//    char cDummy[HEADDER_SIZE];
//
//    fn = ChangeFileExt(fileName, ".hed");
////	if(Common.OverWrite(fn) == false) return(false);
//
//    for(i = 0 ; i < 1024 ; ++i)
//    	scn[i] = 0;
//
////Hedder File出力
//	sprintf(Head, "%05d", MainPixel);
//    sprintf(ySize, "%05d", SubPixel);
//
//    strcat(Head, ySize);
//    SetGasoMitsudo();
//    strcat(Head, mainGasoMitsudo);
//    strcat(Head, subGasoMitsudo);
//
//    for (i = 14 ; i < 24 ; ++i)
//        Head[i] = 0;
//    Head[COMP_LOC] = '0';
//    for (i = 25 ; i < 256 ; ++i)
//        Head[i] = ' ';
//    Head[BIT] = (char)('0' + Bit - 10);		//画像data bit数 (bit - 10)の文字
//
//    if(iEDR == iEDR_OFF){
//        Head[iEDR_POS] = '0';
//    }else if(iEDR == iEDR_FIX){
//        Head[iEDR_POS] = '1';
//    }else if(iEDR == iEDR_SEMIAUTO){
//        Head[iEDR_POS] = '2';
//    }else{
//        Head[iEDR_POS] = '0';
//    }
//
//    sprintf(cDummy, "%03d", iSk);
//    for(int iCnt = 0; iCnt < iSk_BYTE; ++iCnt){
//        Head[iSk_POS + iCnt] = cDummy[iCnt];
//    }
//
//    sprintf(cDummy, "%03d", iL);
//    for(int iCnt = 0; iCnt < iL_BYTE; ++iCnt){
//        Head[iL_POS + iCnt] = cDummy[iCnt];
//    }
//
//    if ((handle = open(fn.c_str(), O_RDWR|O_CREAT|O_BINARY|O_TRUNC, S_IWRITE)) == -1){
//    	CApplication::GetInstance()->MessageBox((fn + " openできませんでした").c_str(), "", MB_OK);
//    	return(false);
//    }
//    if ((write(handle, Head, HEADDER_SIZE)) == -1){
//    	CApplication::GetInstance()->MessageBox((fn + " writeできませんでした").c_str(), "", MB_OK);
//    	close(handle);
//        return(false);
//    }
//    close(handle);
///*
//    for(i = 0 ; i < HEADDER_SIZE ; ++i)
//    	scn[i] = Head[i];
//
//    fn = ChangeFileExt(fileName, ".scn");
//
//    if ((handle = open(fn.c_str(), O_RDWR|O_CREAT|O_BINARY|O_TRUNC, S_IWRITE)) == -1){
//    	CApplication::GetInstance()->MessageBox((fn + " openできませんでした").c_str(), "", MB_OK);
//    	return(false);
//    }
//    if ((write(handle, scn, 1024)) == -1){
//    	CApplication::GetInstance()->MessageBox((fn + " writeできませんでした").c_str(), "", MB_OK);
//    	close(handle);
//        return(false);
//    }
//    close(handle);
//*/
////画像data File出力
//    fn = ChangeFileExt(fileName, ".dat");
//
//    if ((handle = open(fn.c_str(), O_RDWR|O_CREAT|O_BINARY, S_IWRITE)) == -1){
//    	CApplication::GetInstance()->MessageBox((fn + " openできませんでした").c_str(), "", MB_OK);
//    	return(false);
//    }
//
//	for(i = 0 ; i < SubPixel ; ++i){
//    	if ((write(handle, Dt[i], MainPixel * 2)) == -1){
//    		CApplication::GetInstance()->MessageBox((fn + " writeできませんでした").c_str(), "", MB_OK);
//    		close(handle);
//        	return(false);
//        }
//    }
//
//    close(handle);
//
//    return(true);
//}
//---------------------------------------------------------------------------
/*
	画素密度の設定
        引数    10.0  10本読み
                20.0  20本読み
                6.7  6.7本読み
                5.0    5本読み
*/
bool TTMA::SetGasoMitsudo(const float fMitsudo)
{
    if(fMitsudo == 10.0f){
        strcpy(mainGasoMitsudo , "10");
        strcpy(subGasoMitsudo , "10");
        m_fMainPixelSize = m_fSubPixelSize = 0.1f;
        return(true);
    }else if(fMitsudo == 5.0f){
        strcpy(mainGasoMitsudo , "05");
        strcpy(subGasoMitsudo , "05");
        m_fMainPixelSize = m_fSubPixelSize = 0.2f;
        return(true);
    }else if(fMitsudo == 20.0f){
        strcpy(mainGasoMitsudo , "20");
        strcpy(subGasoMitsudo , "20");
        m_fMainPixelSize = m_fSubPixelSize = 0.05f;
        return(true);
    }else if(fMitsudo == 6.7f){
        strcpy(mainGasoMitsudo , "67");
        strcpy(subGasoMitsudo , "67");
        m_fMainPixelSize = m_fSubPixelSize = 0.15f;
        return(true);
    }else{
		return(false);
    }
}
//---------------------------------------------------------------------------
/*
	画素密度の設定
        引数    10  10本読み
                20  20本読み
                67  6.7本読み
                 5  5本読み
*/
bool TTMA::SetGasoMitsudo(const int iMitsudo)
{
    if( iMitsudo == 5 || iMitsudo == 10 || iMitsudo == 20 || iMitsudo == 67){
	    sprintf(mainGasoMitsudo, "%02d", iMitsudo);
        strcpy(subGasoMitsudo , mainGasoMitsudo);
        SetMainPixelSize(MitsudoToPixelSize(mainGasoMitsudo));
        SetSubPixelSize(MitsudoToPixelSize(subGasoMitsudo));
        return(true);
    }else{
        return(false);
    }
}
//---------------------------------------------------------------------------
/*
	画素密度の設定
*/
void TTMA::SetGasoMitsudo()
{
    if(m_fMainPixelSize == 0.15f){
        strcpy(mainGasoMitsudo , "67");
	}else{
#if 0
		int iMitsudo = int(1.0 / m_fMainPixelSize + 0.5);
		sprintf(mainGasoMitsudo, "%02d", iMitsudo);
#else
		//エラー処理（160509 ttsuji）
		if (m_fMainPixelSize) {
			int iMitsudo = int(1.0 / m_fMainPixelSize + 0.5);
			sprintf(mainGasoMitsudo, "%02d", iMitsudo);
		}
#endif
	}
	if(m_fSubPixelSize == 0.15f){
		strcpy(subGasoMitsudo , "67");
	}else{
#if 0
		int iMitsudo = int(1.0 / m_fSubPixelSize + 0.5);
		sprintf(subGasoMitsudo, "%02d", iMitsudo);
#else
		//エラー処理（160509 ttsuji）
		if (m_fSubPixelSize) {
		int iMitsudo = int(1.0 / m_fSubPixelSize + 0.5);
		sprintf(subGasoMitsudo, "%02d", iMitsudo);
		}
#endif
	}
}
////---------------------------------------------------------------------------
//void TTMA::SetEDR(const int iVal)
//{
//    if(iVal == iEDR_OFF){
//        iEDR = iEDR_OFF;
//    }else if(iVal == iEDR_FIX){
//        iEDR = iEDR_FIX;
//    }else if(iVal == iEDR_SEMIAUTO){
//        iEDR = iEDR_SEMIAUTO;
//    }else{
//        iEDR = iEDR_OFF;
//    }
//}
//---------------------------------------------------------------------------
/*

*/
void TTMA::SetSkWithS(const int iSVal)
{
    double dR = 200.0 / iSVal;
    iSk = int((log10(dR) + 2.0) * 100.0 + 0.5);
}
//---------------------------------------------------------------------------
/*
	constructor
*/
TTMA::TTMA()
{
	DtExist = HistExist = false;

//    strcpy(mainGasoMitsudo , "10");
//    strcpy(subGasoMitsudo , "10");
}
//---------------------------------------------------------------------------
/*
	copy constructor
*/
TTMA::TTMA(const TFIMG &copy)
{
//    DtExist = HistExist = false;
    NewDt(copy.SubPixel, copy.MainPixel, 16);
    for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
    	for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
        	SetVal(iSCnt, iMCnt, copy.Dt[iMCnt][iSCnt]);
        }
    }
}
//---------------------------------------------------------------------------
/*
	copy constructor
*/
TTMA::TTMA(const TTMA &copy)
{
    NewDt(copy.SubPixel, copy.MainPixel, copy.Bit, copy.fMainPixelSize(), copy.iEDR, copy.iSk, copy.iL);
	for(int i = 0 ; i < SubPixel ; ++i)
		for(int j = 0 ; j < MainPixel ; ++j)
			Dt[i][j] = copy.Dt[i][j];

}
//---------------------------------------------------------------------------
/*
	operator = TDAT
*/
TTMA& TTMA::operator =(const CTDAT &copy)
{
	int i,j;

    NewDt(copy.SubPixel, copy.MainPixel, copy.Bit);
    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	Dt[i][j] = copy.Dt[i][j];

    return(*this);
}
//---------------------------------------------------------------------------
/*
	operator = TTIF
*/
TTMA& TTMA::operator =(const TTIF &copy)
{
	int i,j;

    NewDt(copy.SubPixel, copy.MainPixel, copy.Bit);
    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	Dt[i][j] = copy.Dt[i][j];

    return(*this);
}
//---------------------------------------------------------------------------
/*
	operator = TTMA
*/
TTMA& TTMA::operator =(TTMA &copy)
{
	int i,j;

//    strcpy(mainGasoMitsudo , copy.mainGasoMitsudo);
//    strcpy(subGasoMitsudo , copy.subGasoMitsudo);

    NewDt(copy.SubPixel, copy.MainPixel, copy.Bit, copy.fMainPixelSize(), copy.iEDR, copy.iSk, copy.iL);
    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	Dt[i][j] = copy.Dt[i][j];

    return(*this);
}
////---------------------------------------------------------------------------
///*
//	operator = TLNSR
//*/
//TTMA& TTMA::operator =(const TLNSR &copy)
//{
//	int i,j;
//
//    NewDt(copy.SubPixel, copy.MainPixel, copy.Bit);
//    for(i = 0 ; i < SubPixel ; ++i)
//    	for(j = 0 ; j < MainPixel ; ++j)
//        	Dt[i][j] = copy.Dt[i][j];
//
//    return(*this);
//}
////---------------------------------------------------------------------------
///*
//	operator = TDTA
//*/
//TTMA& TTMA::operator =(const TDTA &copy)
//{
//	int i,j;
//
//    NewDt(copy.SubPixel, copy.MainPixel, copy.Bit);
//    for(i = 0 ; i < SubPixel ; ++i)
//    	for(j = 0 ; j < MainPixel ; ++j)
//        	Dt[i][j] = copy.Dt[i][j];
//
//    return(*this);
//}
////---------------------------------------------------------------------------
///*
//	operator = TBMP24
//*/
//TTMA& TTMA::operator =(const TBMP24 &copy)
//{
//	int i,j;
//
//    NewDt(copy.SubPixel, copy.MainPixel, copy.Bit);
//    for(i = 0 ; i < SubPixel ; ++i)
//    	for(j = 0 ; j < MainPixel ; ++j)
//        	Dt[i][j] = copy.Dt[i][j];
//
//    return(*this);
//}
////---------------------------------------------------------------------------
///*
//	operator = TFIMG
//*/
////アンダーフロー、オーバーフローの可能性あり(ttsuji)
//TTMA& TTMA::operator =(const TFIMG &copy)
//{
//	int iMCnt, iSCnt;
//
//    if(DtExist == false){
//        Bit = 16;
//    }
//    NewDt(copy.SubPixel, copy.MainPixel, Bit);
//
//    for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
//    	for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
//            SetVal(iSCnt, iMCnt, copy.Dt[iMCnt][iSCnt]);
//        }
//    }
//
//    return(*this);
//}
