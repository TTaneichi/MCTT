/*
 * CCommon.h
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#ifndef CCOMMON_H_
#define CCOMMON_H_

#include <vector>
#include <string>

using namespace std;

//---------------------------------------------------------------------------
class TCommon
{
private:

public:
	bool OverWrite(const string fileName);
    string GetDir(const string fileName);
    string GetFileName(const string fileName);
    string RemExt(const string fileName);
	string GetExt(const string fileName);

	//浮動小数点型のコンペア
	bool IsEqual(double dA, double dB, double dEpsilon);

	//ディレクトリ操作
//	bool __fastcall CopyDirectory(LPCTSTR lpExistingDirectoryName,	LPCTSTR lpNewDirectoryName);
	bool CopyDirectory(const string& strSourcePath, const string& strTargetPath);//ディレクトリのコピー
//	bool __fastcall	DeleteDirectory(std::string StrDirectory);//ディレクトリの削除
	bool DeleteDirectory(string StrDirectory);//ディレクトリの削除
    bool CountFileNumUnderDirectory(const string& StrPath, int& Cnt);//指定したディレクトリ直下のサブフォルダを含むファイル数をカウントする

	//vector型データのテキストファイル出力
//	bool __fastcall SaveText(std::vector<float>& vec, const String fileName, bool bLineFeed);
	bool SaveText(std::vector<float>& vec, string fileName, const char cInterval = '\n');
	bool SaveText(std::vector<std::vector<float> >& vec, const string fileName);

	//TImageコンポーネントにTTMAインスタンスの画像を縮小して表示
//	void __fastcall ShowTImage(TImage *tImage, TTMA& img);//TImageコンポーネントにTTMAインスタンスの画像を縮小して表示
//	void __fastcall ShowTImageABC(TImage *tImage, TTMA& img, int iA);//入力画像をA倍強調して表示
//	void __fastcall ShowTImageABC(TImage *tImage, TTMA& img, double dA, double dB, double dC);//入力画像をA倍強調して表示

};
extern TCommon Common;

#endif /* CCOMMON_H_ */
