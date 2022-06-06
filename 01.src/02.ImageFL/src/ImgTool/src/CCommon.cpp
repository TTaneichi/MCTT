/*
 * CCommon.cpp
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#include "CCommon.h"
#include "CApplication.h"
#include "utility.h"

#include <fstream>
#include <iostream>
#include <io.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//---------------------------------------------------------------------------
/*
	上書きの確認
    	戻り値　true -> 上書きOK
        		false-> 上書き拒否が入力された
*/
bool TCommon::OverWrite(const string fileName)
{
    if(access(fileName.c_str(), 0) == 0){
    	if(CApplication::GetInstance()->MessageBox((fileName + "を上書きしますか？").c_str(), "Warning", MB_YESNO) == IDNO)
            return(false);
    }
    return(true);
}
//---------------------------------------------------------------------------
/*
	Directory　抽出
    	戻り値　Directory 最後に'\'まで含む
*/
string TCommon::GetDir(const string fileName)
{
    char fn[100];		//SaveFileName
    char *ptr, *pos;

	strcpy(fn, fileName.c_str());
    ptr = fn;
    while((pos = strchr(ptr , '\\')) != NULL){
    	ptr = pos + 1;
    }
    *ptr = '\0';
    return(string(fn));
}
//---------------------------------------------------------------------------
/*
	FileName　抽出
*/
string TCommon::GetFileName(const string fileName)
{
    char fn[100];		//SaveFileName
    char *ptr, *pos;

	strcpy(fn, fileName.c_str());
    ptr = fn;
    while((pos = strchr(ptr , '\\')) != NULL){
    	ptr = pos + 1;
    }
    return(string(ptr));
}
//---------------------------------------------------------------------------
/*
	拡張子除去
*/
string TCommon::RemExt(const string fileName)
{
    char fn[100];		//SaveFileName
    char *ptr;

    strcpy(fn, fileName.c_str());
    ptr = strchr(fn , '.');
    if(ptr == NULL) return(fileName);
    ++ptr;
    while(strchr(ptr, '.') != NULL){
    	ptr = strchr(ptr , '.');
        ++ptr;
    }
    --ptr;
    *ptr = 0;
    return(string(fn));
}
//---------------------------------------------------------------------------
/*
	拡張子抽出
*/
string TCommon::GetExt(const string fileName)
{
	char fn[100];		//SaveFileName
	char *ptr;

	strcpy(fn, fileName.c_str());
	ptr = strchr(fn , '.');
	if(ptr == NULL) return(fileName);
	++ptr;
	while(strchr(ptr, '.') != NULL){
		ptr = strchr(ptr , '.');
		++ptr;
	}
	return(string(ptr));
}
//---------------------------------------------------------------------------
//浮動小数点型のコンペア(ttsuji追加 190418)

bool TCommon::IsEqual(double dA, double dB, double dEpsilon)
{
	if (dEpsilon < 0) {
		dEpsilon *= -1.;//絶対値とする
	}

	if (dA - dEpsilon <= dB && dB <= dA + dEpsilon) {
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------
//ディレクトリのコピー (ttsuji追加 190419)

bool TCommon::CopyDirectory(const string& strSourcePath, const string& strTargetPath)
{
	// 入力値チェック
	if ("" == strSourcePath || "" == strTargetPath){
		return false;
	}

	//コピー先のディレクトリが存在しなければ作成する
	if (!DirectoryExists(strTargetPath)){
		if (!CreateDir(strTargetPath)){
			string strEvent = "ディレクトリの作成に失敗しました。\n" + strTargetPath;
			CApplication::GetInstance()->MessageBox(strEvent.c_str(), "Error", MB_OK);
			return false;
		}
	}

#if 0				//TODO とりあえずコメントアウト
	// ディレクトリ内のファイル走査開始
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile;
	hFindFile = FindFirstFile((strSourcePath + "\\*").c_str(), &FindFileData);
	if(hFindFile == INVALID_HANDLE_VALUE){
		string strEvent = "コピー元ディレクトリ内のファイル走査に失敗しました。\n" + strSourcePath;
		CApplication::GetInstance()->MessageBox(strEvent.c_str(), "Error", MB_OK);
		return false;//ファイル無し
	}


	do {
		if( '.' != FindFileData.cFileName[0] ) {
			string strCopySource = strSourcePath + "\\" + FindFileData.cFileName;
			string strCopyTarget = strTargetPath + "\\" + FindFileData.cFileName;

			if( FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes )
			{	// ディレクトリなら再起呼び出しでコピー
				if( !CopyDirectory( strCopySource, strCopyTarget ) )
				{
					FindClose( hFindFile );
					return false;
				}
			}
			else
			{	// ファイルならWin32API関数を用いてコピー
				if( !CopyFile( strCopySource.c_str(), strCopyTarget.c_str(), false ) )
				{
					FindClose( hFindFile );
					return false;
				}
			}
		}
	} while( FindNextFile( hFindFile, &FindFileData ) );

	FindClose( hFindFile );

#endif

	return true;
}
//---------------------------------------------------------------------------
//ディレクトリの削除(ttsuji追加 190419)

bool TCommon::DeleteDirectory(string StrDirectory)
{
	bool retVal =true;

#if 0			//TODO とりあえずコメントアウト
	WIN32_FIND_DATA FindFileData = { 0 };
	HANDLE hFind;
	const string SearchName = StrDirectory + "\\*.*";
	hFind = FindFirstFile(SearchName.c_str(), &FindFileData);

	int iFindNum = 0;
	const int iFileNum = 23;//"C:\ip_debug_img"フォルダ内に出力されるファイル数23
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			//If a found file is . or .. then skip
			if(strcmp(FindFileData.cFileName,".") != 0 && strcmp(FindFileData.cFileName,"..") != 0){
				//The path should be absolute path
				string StrFound = StrDirectory + "\\" + FindFileData.cFileName;

				//If the file is a directory
				if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0){
					DeleteDirectory(StrFound);
				}
				//If the file is a file
				else {
					DeleteFile(StrFound.c_str());
				}
			}
		} while(FindNextFile(hFind, &FindFileData) != 0);
	}

	FindClose(hFind);

	//ディレクトリ削除
	if (_rmdir(StrDirectory.c_str())) {
		cout << ("ディレクトリの削除に失敗しました。\n" + StrDirectory).c_str() << endl;
		retVal = false;
	}
#endif

	return retVal;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------
//指定したディレクトリ下のサブフォルダを含むファイル数をカウントする (ttsuji追加 190610)

bool TCommon::CountFileNumUnderDirectory(const string& StrPath, int& iFileNum)
{
#if 0

	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile;

	hFindFile = FindFirstFile((StrPath + "\\*").c_str(), &FindFileData);
	if(hFindFile == INVALID_HANDLE_VALUE){
    	iFileNum = 0;
		return false;//ファイル無し
	}


	do {
		string sFileName = FindFileData.cFileName;
		string FilePath = StrPath + "\\" + sFileName;
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {//ディレクトリの場合の処理
			if (sFileName != "." && sFileName != "..") {
				if (!CountFileNumUnderDirectory(FilePath, iFileNum)) {//サブフォルダ検索
					FindClose(hFindFile);
					return false;
				}
			}
		} else {//ファイルの場合の処理
			iFileNum++;
		}
	} while(FindNextFile(hFindFile, &FindFileData));

	FindClose( hFindFile );

#endif

	return true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------
/*
//vector型データのテキストファイル出力 (ttsuji追加 200213)

bool __fastcall TCommon::SaveText(std::vector<float>& vec, const String fileName, bool bLineFeed)
{
	fstream file(fileName.c_str(), ios::out|ios::trunc);
	if (file.fail()){
		return false;
	}

	char cInterval;
	if (bLineFeed) {
		cInterval = '\n';
	}
	else{
		cInterval = '\t';
	}

	for (vector<float>::iterator it = vec. begin(); it != vec.end(); ++it){
		file << *it << cInterval;
	}

	if(file.fail()){
		return false;
	}
	return true;
}
*/
//------------------------------------------------------------------------------------------------------------------------------------------------------
//vector型データのテキストファイル出力 (ttsuji追加 200218)
//cInterval = '\n', '\t', ' ', ',' //改行、タブ区切り	、スペース、カンマなどの文字を挿入

bool TCommon::SaveText(std::vector<float>& vec, string fileName, const char cInterval)
{
	fstream file(fileName.c_str(), ios::out|ios::trunc);
	if (file.fail()){
		return false;
	}

	for (vector<float>::iterator it = vec. begin(); it != vec.end(); ++it){
		file << *it << cInterval;
	}

	if(file.fail()){
		return false;
	}
	return true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------
//vector型データのテキストファイル出力 (ttsuji追加 200218)
//vec[i1][i2]に対して、i1を横にふって、i2を縦にふってテキスト出力する

bool TCommon::SaveText(std::vector<std::vector<float> >& vec, const string fileName)
{
	fstream OutFile;
	OutFile.open(fileName.c_str(), ios::out|ios::trunc);
	if (OutFile.fail()){
		CApplication::GetInstance()->MessageBox(("ファイルのオープンエラーが発生しました。\n" + fileName).c_str(), "Error", MB_OK);
		return false;
	}
	for (int i2 = 0; i2 < vec[0].size(); i2++){
		OutFile << i2;
		for (int i1 = 0; i1 < vec.size(); i1++) {
			OutFile << '\t' << vec[i1][i2];
		}
		OutFile << endl;
	}
	OutFile.close();

	return true;
}
////------------------------------------------------------------------------------------------------------------------------------------------------------
////TImageコンポーネントにTTMAインスタンスの画像を縮小して表示
//
//void __fastcall TCommon::ShowTImage(TImage *tImage, TTMA& Image)
//{
//	// TBitmapオブジェクトを作成
//	Graphics::TBitmap *BM = new Graphics::TBitmap;
//
//	//BMの画像サイズ、ビット深さを指定
//	BM->PixelFormat = pf24bit;
//	BM->Width = Image.MainPixel;
//	BM->Height= Image.SubPixel;
//
//	//TImageのサイズに収まるように1/iRatio倍に縮小して表示（間引き）
//	int iRatioW = (int)(BM->Width / tImage->Width);//切り捨て
//	int iRatioH = (int)(BM->Height / tImage->Height);//切り捨て
//	int iRatio = (iRatioW < iRatioH) ? iRatioH : iRatioW;//Maxを選択
//
//	int iShift = 8;//右ビットシフト量
//	switch (Image.Bit) {
//	case 16:
//		iShift = 8;
//		break;
//	case 14:
//		iShift = 6;
//		break;
//	}
//
//	unsigned char* p_ucLine;// 8bitのpix値のポインタ
//	for (int iS = 0; iS < BM->Height; iS++){
//		// ScanLineで1行分まとめて取得
//		p_ucLine = (unsigned char*)BM->ScanLine[iS];
//		int i=0;
//		for (int iM = 0; iM < BM->Width; iM++){
//#if 0       //ピクセル等倍表示
//			p_ucLine[i] = (Image.DtMax - Image.Dt[iS][iM]) >> iShift;//白黒反転してiShift bit右シフト
//#else
//			//1/iRatio倍表示（間引き）
//			if (iS * iRatio >= BM->Height || iM * iRatio >= BM->Width) {
//				continue;
//			}
//			p_ucLine[i] = (Image.DtMax - Image.Dt[iS * iRatio][iM * iRatio]) >> iShift;//白黒反転してiShift bit右シフト
//#endif
//			p_ucLine[i + 1] = p_ucLine[i + 2] = p_ucLine[i];//RGB同じpix値とする
//			i+=3;
//		}
//	}
//
//	tImage->Picture->Assign(BM);	// tImageにコピー
//	tImage->Refresh();
////	tImage->Invalidate();
////	tImage->Update();
//	delete BM;			// オブジェクト解放
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------
////入力画像をA倍強調して表示(A * (Dt - B) + C のBは画像平均QL、CはQLMaxの半値)
//
//void __fastcall TCommon::ShowTImageABC(TImage *tImage, TTMA& img, int iA)
//{
//	TTMA imgABC;
//	imgABC = img;
//	double dAve = img.dAve();
//	int iC = img.DtMax / 2;
//
//	for (int iS = 0; iS < img.SubPixel; iS++) {
//		for (int iM = 0; iM < img.MainPixel; iM++) {
//			imgABC.SetVal(iS, iM, iA * (img.Dt[iS][iM] - dAve) + iC);
//		}
//	}
//
//	ShowTImage(tImage, imgABC);
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------
////入力画像をA倍強調して表示(A * (Dt - B) + C のBは画像平均QL、CはQLMaxの半値)
//
//void __fastcall TCommon::ShowTImageABC(TImage *tImage, TTMA& img, double dA, double dB, double dC)
//{
//	TTMA imgABC;
//	imgABC = img;
//
//	for (int iS = 0; iS < img.SubPixel; iS++) {
//		for (int iM = 0; iM < img.MainPixel; iM++) {
//			imgABC.SetVal(iS, iM, dA * (img.Dt[iS][iM] - dB) + dC);
//		}
//	}
//
//	ShowTImage(tImage, imgABC);
//}
//------------------------------------------------------------------------------------------------------------------------------------------------------

