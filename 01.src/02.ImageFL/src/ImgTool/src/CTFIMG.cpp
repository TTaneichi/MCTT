/*
 * CTFIMG.cpp
 *
 *  Created on: 2021/04/01
 *      Author: 10097663
 */

#include "CTFIMG.h"
#include "CTDAT.h"
#include "CTSINGLEDATA.h"
#include "alloc.h"
#include "imageData.h"
#include <math.h>

/*
 Dt[][]の領域確保
 引数	main ->	副画素数
 sub ->	主画素数
 */
void TFIMG::NewDt(const int main, const int sub) {
	int i, j;

	DeleteDt();
	Dt = new float*[main];
	for (i = 0; i < main; i++) {
		Dt[i] = new float[sub];

		//takahashi 100705
		for (j = 0; j < sub; j++) {
			Dt[i][j] = 0;
		}
	}

	MainPixel = main;
	SubPixel = sub;
	DtExist = true;
}
//---------------------------------------------------------------------------
/*
 operator =(const CTDAT &copy)
 */
TFIMG& TFIMG::operator =(const CTDAT &copy) {
	int i, j;

	NewDt(copy.MainPixel, copy.SubPixel);
	for (i = 0; i < SubPixel; ++i)
		for (j = 0; j < MainPixel; ++j)
			Dt[j][i] = float(copy.Dt[i][j]);

	return (*this);
}
//---------------------------------------------------------------------------
/*
 operator =
 */
TFIMG& TFIMG::operator =(const TFIMG &copy) {
	int i, j;

	NewDt(copy.MainPixel, copy.SubPixel);
	for (i = 0; i < SubPixel; ++i)
		for (j = 0; j < MainPixel; ++j)
			Dt[j][i] = copy.Dt[j][i];

	return (*this);
}
//---------------------------------------------------------------------------
/*
 operator =
 */
#if 0
TFIMG& TFIMG::operator =(const TIIMG &copy)
{
	int i,j;

	NewDt(copy.MainPixel, copy.SubPixel);
	for(i = 0 ; i < SubPixel ; ++i)
		for(j = 0 ; j < MainPixel ; ++j)
			Dt[j][i] = (float)copy.Dt[j][i];

	return(*this);
}
#endif
//---------------------------------------------------------------------------
/*
 operator *= float
 */
TFIMG& TFIMG::operator *=(const float oper2) {
	int i, j;

	for (i = 0; i < SubPixel; ++i) {
		for (j = 0; j < MainPixel; ++j) {
			Dt[j][i] *= oper2;
		}
	}

	return (*this);
}
//---------------------------------------------------------------------------
/*
 operator /= int
 */
TFIMG& TFIMG::operator /=(const int oper2) {
	int i, j;
	float foper2 = oper2;

	for (i = 0; i < SubPixel; ++i)
		for (j = 0; j < MainPixel; ++j)
			Dt[j][i] /= foper2;

	return (*this);
}
//---------------------------------------------------------------------------
/*
 operator /= double
 */
TFIMG& TFIMG::operator /=(const double oper2) {
	int i, j;
	float foper2 = float(oper2);

	if (!oper2) {
		throw 1; //test
	}

	for (i = 0; i < SubPixel; ++i)
		for (j = 0; j < MainPixel; ++j)
			Dt[j][i] /= foper2;

	return (*this);
}
//---------------------------------------------------------------------------
/*
 operator + int
 */
//ttsuji追加
TFIMG TFIMG::operator +(const int iOffset) {
	TFIMG fimg;
	fimg.NewDt(MainPixel, SubPixel);
	int i, j;
	for (i = 0; i < SubPixel; i++) {
		for (j = 0; j < MainPixel; j++) {
			fimg.Dt[j][i] = Dt[j][i] + iOffset;
		}
	}
	return fimg;
}
//---------------------------------------------------------------------------
/*
 operator - int
 */
//ttsuji追加
TFIMG TFIMG::operator -(const int iOffset) {
	TFIMG fimg;
	fimg.NewDt(MainPixel, SubPixel);
	int i, j;
	for (i = 0; i < SubPixel; i++) {
		for (j = 0; j < MainPixel; j++) {
			fimg.Dt[j][i] = Dt[j][i] - iOffset;
		}
	}
	return fimg;
}
//---------------------------------------------------------------------------
/*
 operator * float
 */
TFIMG TFIMG::operator *(const float iOffset) {
	TFIMG fimg;
	fimg.NewDt(MainPixel, SubPixel);
	int i, j;
	for (i = 0; i < SubPixel; i++) {
		for (j = 0; j < MainPixel; j++) {
			fimg.Dt[j][i] = Dt[j][i] * iOffset;
		}
	}
	return fimg;
}
//---------------------------------------------------------------------------
/*
 copy constructor
 */
TFIMG::TFIMG(const TFIMG &copy) :
		MainPixel(copy.MainPixel), SubPixel(copy.SubPixel), DtExist(true) {
	Dt = new float*[MainPixel];
	for (int iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
		Dt[iMCnt] = new float[SubPixel];
	}
	for (int iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
		for (int iSCnt = 0; iSCnt < SubPixel; ++iSCnt) {
			Dt[iMCnt][iSCnt] = copy.Dt[iMCnt][iSCnt];
		}
	}
}
//---------------------------------------------------------------------------
/*
 constructor
 */
TFIMG::TFIMG(const int iM, const int iS) :
		MainPixel(iM), SubPixel(iS), DtExist(true) {
	Dt = new float*[iM];
	for (int iMCnt = 0; iMCnt < iM; ++iMCnt) {
		Dt[iMCnt] = new float[iS];
	}
}
//---------------------------------------------------------------------------
/*
 copy constructor
 */
TFIMG::TFIMG(const CTDAT &tdat) :
		MainPixel(tdat.MainPixel), SubPixel(tdat.SubPixel), DtExist(true) {
	Dt = new float*[MainPixel];
	for (int iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
		Dt[iMCnt] = new float[SubPixel];
	}
	for (int iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
		for (int iSCnt = 0; iSCnt < SubPixel; ++iSCnt) {
			Dt[iMCnt][iSCnt] = tdat.Dt[iSCnt][iMCnt];
		}
	}
}
//---------------------------------------------------------------------------
/*
 operator -(const TFIMG &oper)
 */
TFIMG& TFIMG::operator -(const TFIMG &oper) {
	TFIMG temp;
	int i, j;
	int main;
	int sub;

	if (MainPixel >= oper.MainPixel && SubPixel >= oper.SubPixel) {
		for (i = 0; i < oper.MainPixel; ++i) {
			for (j = 0; j < oper.SubPixel; ++j) {
				Dt[i][j] -= oper.Dt[i][j];
			}
		}
		return (*this);
	}

	temp.NewDt(MainPixel, SubPixel);
	for (i = 0; i < MainPixel; ++i) {
		for (j = 0; j < SubPixel; ++j) {
			temp.Dt[i][j] = Dt[i][j];
		}
	}

	main = MainPixel;
	sub = SubPixel;
	if (main < oper.MainPixel)
		main = oper.MainPixel;
	if (sub < oper.SubPixel)
		sub = oper.SubPixel;
	NewDt(main, sub);

	for (i = 0; i < main; ++i) {
		for (j = 0; j < sub; ++j) {
			Dt[i][j] = 0.0;
		}
	}
	for (i = 0; i < temp.MainPixel; ++i) {
		for (j = 0; j < SubPixel; ++j) {
			Dt[i][j] = temp.Dt[i][j];
		}
	}
	for (i = 0; i < oper.MainPixel; ++i) {
		for (j = 0; j < oper.SubPixel; ++j) {
			Dt[i][j] -= oper.Dt[i][j];
		}
	}
	return (*this);
}
//---------------------------------------------------------------------------
/*
 operator -(const CTDAT &oper)
 */
TFIMG& TFIMG::operator -(const CTDAT &oper) {
	TFIMG temp;
	int i, j;
	int main;
	int sub;

	if (MainPixel >= oper.MainPixel && SubPixel >= oper.SubPixel) {
		for (i = 0; i < oper.MainPixel; ++i) {
			for (j = 0; j < oper.SubPixel; ++j) {
				Dt[i][j] -= float(oper.Dt[j][i]);
			}
		}
		return (*this);
	}

	temp.NewDt(MainPixel, SubPixel);
	for (i = 0; i < MainPixel; ++i) {
		for (j = 0; j < SubPixel; ++j) {
			temp.Dt[i][j] = Dt[i][j];
		}
	}

	main = MainPixel;
	sub = SubPixel;
	if (main < oper.MainPixel)
		main = oper.MainPixel;
	if (sub < oper.SubPixel)
		sub = oper.SubPixel;
	NewDt(main, sub);

	for (i = 0; i < main; ++i) {
		for (j = 0; j < sub; ++j) {
			Dt[i][j] = 0.0;
		}
	}
	for (i = 0; i < temp.MainPixel; ++i) {
		for (j = 0; j < SubPixel; ++j) {
			Dt[i][j] = temp.Dt[i][j];
		}
	}
	for (i = 0; i < oper.MainPixel; ++i) {
		for (j = 0; j < oper.SubPixel; ++j) {
			Dt[i][j] -= float(oper.Dt[j][i]);
		}
	}
	return (*this);
}
//---------------------------------------------------------------------------
/*
 operator +=(const const float fConst)
 */
TFIMG& TFIMG::operator +=(const float fConst) {
	int i, j;

	for (i = 0; i < MainPixel; ++i) {
		for (j = 0; j < SubPixel; ++j) {
			Dt[i][j] += fConst;
		}
	}
	return (*this);
}
//---------------------------------------------------------------------------
/*
 operator +=(const TFIMG& oper2)
 */
TFIMG& TFIMG::operator +=(const TFIMG &oper2) {
	int i, j;
	TFIMG temp;
	int main;
	int sub;

	if (MainPixel >= oper2.MainPixel && SubPixel >= oper2.SubPixel) {
		for (i = 0; i < oper2.MainPixel; ++i) {
			for (j = 0; j < oper2.SubPixel; ++j) {
				Dt[i][j] += oper2.Dt[i][j];
			}
		}
		return (*this);
	}

	temp.NewDt(MainPixel, SubPixel);
	for (i = 0; i < MainPixel; ++i) {
		for (j = 0; j < SubPixel; ++j) {
			temp.Dt[i][j] = Dt[i][j];
		}
	}

	main = MainPixel;
	sub = SubPixel;
	if (main < oper2.MainPixel)
		main = oper2.MainPixel;
	if (sub < oper2.SubPixel)
		sub = oper2.SubPixel;
	NewDt(main, sub);

	for (i = 0; i < main; ++i) {
		for (j = 0; j < sub; ++j) {
			Dt[i][j] = 0.0;
		}
	}
	for (i = 0; i < temp.MainPixel; ++i) {
		for (j = 0; j < temp.SubPixel; ++j) {
			Dt[i][j] = temp.Dt[i][j];
		}
	}
	for (i = 0; i < oper2.MainPixel; ++i) {
		for (j = 0; j < oper2.SubPixel; ++j) {
			Dt[i][j] += oper2.Dt[i][j];
		}
	}
	return (*this);
}
//---------------------------------------------------------------------------
/*
 operator +=(const CTDAT& oper2)
 */
TFIMG& TFIMG::operator +=(const CTDAT &oper2) {
	int i, j;
	TFIMG temp;
	int main;
	int sub;

	if (MainPixel >= oper2.MainPixel && SubPixel >= oper2.SubPixel) {
		for (i = 0; i < oper2.MainPixel; ++i) {
			for (j = 0; j < oper2.SubPixel; ++j) {
				Dt[i][j] += float(oper2.Dt[j][i]);
			}
		}
		return (*this);
	}

	temp.NewDt(MainPixel, SubPixel);
	for (i = 0; i < MainPixel; ++i) {
		for (j = 0; j < SubPixel; ++j) {
			temp.Dt[i][j] = Dt[i][j];
		}
	}

	main = MainPixel;
	sub = SubPixel;
	if (main < oper2.MainPixel)
		main = oper2.MainPixel;
	if (sub < oper2.SubPixel)
		sub = oper2.SubPixel;
	NewDt(main, sub);

	for (i = 0; i < main; ++i) {
		for (j = 0; j < sub; ++j) {
			Dt[i][j] = 0.0;
		}
	}
	for (i = 0; i < temp.MainPixel; ++i) {
		for (j = 0; j < temp.SubPixel; ++j) {
			Dt[i][j] = temp.Dt[i][j];
		}
	}
	for (i = 0; i < oper2.MainPixel; ++i) {
		for (j = 0; j < oper2.SubPixel; ++j) {
			Dt[i][j] += oper2.Dt[j][i];
		}
	}
	return (*this);
}
//---------------------------------------------------------------------------
/*
 operator -=(const CTDAT& oper2)
 */
TFIMG& TFIMG::operator -=(const CTDAT &oper2) {
	int i, j;
	TFIMG temp;
	int main;
	int sub;

	if (MainPixel >= oper2.MainPixel && SubPixel >= oper2.SubPixel) {
		for (i = 0; i < oper2.MainPixel; ++i) {
			for (j = 0; j < oper2.SubPixel; ++j) {
				Dt[i][j] -= oper2.Dt[j][i];
			}
		}
		return (*this);
	}

	temp.NewDt(MainPixel, SubPixel);
	for (i = 0; i < MainPixel; ++i) {
		for (j = 0; j < SubPixel; ++j) {
			temp.Dt[i][j] = Dt[i][j];
		}
	}

	main = MainPixel;
	sub = SubPixel;
	if (main < oper2.MainPixel)
		main = oper2.MainPixel;
	if (sub < oper2.SubPixel)
		sub = oper2.SubPixel;
	NewDt(main, sub);

	for (i = 0; i < main; ++i) {
		for (j = 0; j < sub; ++j) {
			Dt[i][j] = 0.0;
		}
	}
	for (i = 0; i < temp.MainPixel; ++i) {
		for (j = 0; j < temp.SubPixel; ++j) {
			Dt[i][j] = temp.Dt[i][j];
		}
	}
	for (i = 0; i < oper2.MainPixel; ++i) {
		for (j = 0; j < oper2.SubPixel; ++j) {
			Dt[i][j] -= oper2.Dt[j][i];
		}
	}
	return (*this);
}
//---------------------------------------------------------------------------
/*
 operator -=(const TFIMG& oper2)
 */
TFIMG& TFIMG::operator -=(const TFIMG &oper2) {
	int i, j;
	TFIMG temp;
	int main;
	int sub;

	if (MainPixel >= oper2.MainPixel && SubPixel >= oper2.SubPixel) {
		for (i = 0; i < oper2.MainPixel; ++i) {
			for (j = 0; j < oper2.SubPixel; ++j) {
				Dt[i][j] -= oper2.Dt[i][j];
			}
		}
		return (*this);
	}

	temp.NewDt(MainPixel, SubPixel);
	for (i = 0; i < MainPixel; ++i) {
		for (j = 0; j < SubPixel; ++j) {
			temp.Dt[i][j] = Dt[i][j];
		}
	}

	main = MainPixel;
	sub = SubPixel;
	if (main < oper2.MainPixel)
		main = oper2.MainPixel;
	if (sub < oper2.SubPixel)
		sub = oper2.SubPixel;
	NewDt(main, sub);

	for (i = 0; i < main; ++i) {
		for (j = 0; j < sub; ++j) {
			Dt[i][j] = 0.0;
		}
	}
	for (i = 0; i < temp.MainPixel; ++i) {
		for (j = 0; j < temp.SubPixel; ++j) {
			Dt[i][j] = temp.Dt[i][j];
		}
	}
	for (i = 0; i < oper2.MainPixel; ++i) {
		for (j = 0; j < oper2.SubPixel; ++j) {
			Dt[i][j] -= oper2.Dt[i][j];
		}
	}
	return (*this);
}
//---------------------------------------------------------------------------
/*
 operator /=(const CTDAT& oper)
 */
TFIMG& TFIMG::operator /=(const CTDAT &oper) {
	int iMCnt, iSCnt;
	int main;
	int sub;

	main = MainPixel;
	sub = SubPixel;
	if (main > oper.MainPixel)
		main = oper.MainPixel;
	if (sub > oper.SubPixel)
		sub = oper.SubPixel;

	for (iSCnt = 0; iSCnt < sub; ++iSCnt) {
		for (iMCnt = 0; iMCnt < main; ++iMCnt) {
			if (oper.Dt[iSCnt][iMCnt] == 0) {
				Dt[iMCnt][iSCnt] = oper.DtMax;
			} else {
				Dt[iMCnt][iSCnt] /= oper.Dt[iSCnt][iMCnt];
			}
		}
	}
	return (*this);
}
//---------------------------------------------------------------------------
//ttsuji追加

TFIMG TFIMG::operator /(const TFIMG FImg) {
	TFIMG FImgOut;
	FImgOut.NewDt(MainPixel, SubPixel);
	int iM, iS;
	for (iS = 0; iS < SubPixel; iS++) {
		for (iM = 0; iM < MainPixel; iM++) {
			if (0 == FImg.Dt[iM][iS]) {
				FImgOut.Dt[iM][iS] = 0.0;
			} else {
				FImgOut.Dt[iM][iS] = Dt[iM][iS] / FImg.Dt[iM][iS];
			}
		}
	}

	return FImgOut;
}
//---------------------------------------------------------------------------
/*
 deconstructor
 */
TFIMG::~TFIMG() {
	DeleteDt();
}
//---------------------------------------------------------------------------
/*
 Dt[][]の平均値
 */
double TFIMG::AverageD(int iMainSrt, int iMainEnd, int iSubSrt, int iSubEnd) {
	double dSum = 0.0;
	int iMCnt, iSCnt;

	if (iSubSrt < 0)
		iSubSrt = 0;
	if (iSubEnd > SubPixel - 1)
		iSubEnd = SubPixel - 1;
	if (iMainSrt < 0)
		iMainSrt = 0;
	if (iMainEnd > MainPixel - 1)
		iMainEnd = MainPixel - 1;

	for (iSCnt = iSubSrt; iSCnt <= iSubEnd; ++iSCnt)
		for (iMCnt = iMainSrt; iMCnt <= iMainEnd; ++iMCnt)
			dSum += Dt[iMCnt][iSCnt];

	return (dSum / ((iMainEnd - iMainSrt + 1) * (iSubEnd - iSubSrt + 1)));
}
//---------------------------------------------------------------------------
double TFIMG::SummationD(int iMainSrt, int iMainEnd, int iSubSrt, int iSubEnd) {
	double dSum = 0.0;
	int iMCnt, iSCnt;

	if (iSubSrt < 0)
		iSubSrt = 0;
	if (iSubEnd > SubPixel - 1)
		iSubEnd = SubPixel - 1;
	if (iMainSrt < 0)
		iMainSrt = 0;
	if (iMainEnd > MainPixel - 1)
		iMainEnd = MainPixel - 1;

	for (iSCnt = iSubSrt; iSCnt <= iSubEnd; ++iSCnt)
		for (iMCnt = iMainSrt; iMCnt <= iMainEnd; ++iMCnt)
			dSum += Dt[iMCnt][iSCnt];

	return dSum;
}
//---------------------------------------------------------------------------
/*

 */
void TFIMG::Abs() {
	for (int iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
		for (int iSCnt = 0; iSCnt < SubPixel; ++iSCnt) {
			Dt[iMCnt][iSCnt] = abs(Dt[iMCnt][iSCnt]);
		}
	}
}
//---------------------------------------------------------------------------
/*
 引数が画像範囲内であれば    true
 */
bool TFIMG::bCheckPos(const int iM, const int iS) const {
	if (iM < 0 || iS < 0 || iM >= MainPixel || iS >= SubPixel) {
		return (false);
	} else {
		return (true);
	}
}
//---------------------------------------------------------------------------
/*
 Text Fileの読込
 形式

 主画素数 \t 副画素数
 Dt[0][0] \t Dt[1][0] ,,,
 Dt[0][1] \t Dt[1][1] ,,,
 ,,,
 */
bool TFIMG::bReadTxt(string fileName) {
	fstream file;
	int iMCnt, iSCnt;

	file.open(fileName.c_str(), ios::in);
	if (file.fail()) {
		file.close();
		return (false);
	}

	file >> iMCnt;
	file >> iSCnt;

	NewDt(iMCnt, iSCnt);
	for (iSCnt = 0; iSCnt < SubPixel; ++iSCnt) {
		for (iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
			file >> Dt[iMCnt][iSCnt];
		}
	}

	if (file.fail()) {
		file.close();
		return (false);
	}

	file.close();
	return (true);
}
//---------------------------------------------------------------------------
/*
 textでfile出力
 */
bool TFIMG::bWriteTxt(const string fileName, string header) {
	fstream file(fileName.c_str(), ios::out | ios::trunc);
	if (file.fail()) {
		return (false);
	}

	file << MainPixel << "\t" << SubPixel << "\t" << header.c_str();
	for (int iMainCnt = 0; iMainCnt < MainPixel - 3; ++iMainCnt) {
		file << "\t";
	}
	file << endl;

	for (int iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
		file << Dt[0][iSubCnt];
		for (int iMainCnt = 1; iMainCnt < MainPixel; ++iMainCnt) {
			file << "\t" << Dt[iMainCnt][iSubCnt];
		}
		file << endl;
	}

	if (file.fail()) {
		return (false);
	}
	return (true);
}
//---------------------------------------------------------------------------
/*
 Raw Fileの読込 //ttsuji追加(180402)
 読み込むRAW画像の縦、横pix数と１画素あたりのByte数、読み飛ばすヘッダByteを指定する
 */

bool TFIMG::bReadRaw(string fileName, int iMainPixel, int iSubPixel,
		int iPixelByte, int iHeaderByte) {
#if 0
	fstream file;
    int iMCnt, iSCnt;

	file.open(fileName.c_str(), ios::in | ios::binary);
	if(file.fail()){
		Application->MessageBox((fileName + " openできませんでした").c_str(), "Error", MB_OK);
		file.close();
		return false;
	}


	if(iHeaderByte < 0){
		Application->MessageBox("ヘッダサイズが不正です", "Error", MB_OK);
		file.close();
		return false;
	}


	// ファイルサイズ計算
	long file_size;
	file.seekg(0, ios::end);
	file_size = file.tellg();
	file.seekg(0);

	if (file_size != iHeaderByte + iMainPixel * iSubPixel * iPixelByte){
		Application->MessageBox("縦横画素数と１画素あたりのByte数、読み飛ばすヘッダByteが、ファイルサイズと合っていません", "Error", MB_OK);
		file.close();
		return false;
	}

	int i, iS, iM;
	char tmp;//1byte
	unsigned short int data;//2byte
	for(i = 0; i < iHeaderByte; i++){
		file.read((unsigned char*)&tmp, sizeof(char));//読み飛ばし
	}
/*	for(i = 0 ; i < iSubPixel ; i++){
		file.read((float*)Dt[i], iPixelByte * iMainPixel);
	}
*/
/*
	//fread関数を使用
	fp=fopen("data.bin", "rb");
	fread( &reading, sizeof(double), 1, fp );
*/
	fstream OutPSFText;
	OutPSFText.open((ExtractFilePath(fileName) + "OutPSF.txt").c_str(), ios::out|ios::trunc);
	if (OutPSFText.fail()){
		Application->MessageBox("OutPSF.txtのオープンに失敗しました", "Error", MB_OK);
		return false;
	}
	OutPSFText << iMainPixel << "\t" << iSubPixel << endl;


	NewDt(iMainPixel, iSubPixel);

	for (iS = 0 ; iS < iSubPixel ; iS++){
		for (iM = 0 ; iM < iMainPixel ; iM++){
			file.read((unsigned char*)&Dt[iM][iS], iPixelByte);
			OutPSFText << Dt[iM][iS] << "\t";
		}
		OutPSFText << endl;
	}
	OutPSFText << endl;

	file.close();

/*
	Dt[iSCnt][iMCnt] = (unsigned short)((Dt[iSCnt][iMCnt] << 8) + (Dt[iSCnt][iMCnt] >> 8));
*/
#endif
	return true;
}
//---------------------------------------------------------------------------
/*
 textでfile出力
 */
bool TFIMG::bSaveAsText(const string fileName) {
	fstream file(fileName.c_str(), ios::out | ios::trunc);
	if (file.fail()) {
		return (false);
	}

	for (int iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
		file << Dt[0][iSubCnt];
		for (int iMainCnt = 1; iMainCnt < MainPixel; ++iMainCnt) {
			file << '\t' << Dt[iMainCnt][iSubCnt];
		}
		file << '\n';
	}

	if (file.fail()) {
		return (false);
	}
	return (true);
}
//---------------------------------------------------------------------------
/*
 textでfile出力(タブか改行かを指定する(ttsuji追加))
 */
bool TFIMG::bSaveAsText(const string fileName, bool bLineFeed) {
	fstream file(fileName.c_str(), ios::out | ios::trunc);
	if (file.fail()) {
		return (false);
	}

	char cInterval;
	if (bLineFeed) {
		cInterval = '\n';
	} else {
		cInterval = '\t';
	}

	for (int iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
		file << Dt[0][iSubCnt];
		for (int iMainCnt = 1; iMainCnt < MainPixel; ++iMainCnt) {
			file << cInterval << Dt[iMainCnt][iSubCnt];
		}
		file << '\n';
	}

	if (file.fail()) {
		return (false);
	}
	return (true);
}
//---------------------------------------------------------------------------
/*
 textでfile出力
 */
bool TFIMG::bSaveAsTextWithIndex(const string fileName) {
	fstream file(fileName.c_str(), ios::out | ios::trunc);
	if (file.fail()) {
		return (false);
	}

	for (int iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
		file << '\t' << iMCnt;
	}
	file << '\n';

	for (int iSCnt = 0; iSCnt < SubPixel; ++iSCnt) {
		file << iSCnt;
		for (int iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
			file << '\t' << Dt[iMCnt][iSCnt];
		}
		file << '\n';
	}

	if (file.fail()) {
		return (false);
	}
	return (true);
}
//---------------------------------------------------------------------------
/*
 主iMainStart～iMainEnd（含む)までを平均化した副走査方向のプロフィルを
 TSINGLE_DATAに上書きする
 引数    TSINGLE_DATA& line  副走査方向のプロフィルの結果
 const int iMainStart    主平均開始画素
 const int iMainEnd      主平均終了画素（含む）
 */
bool TFIMG::bSubLine(TSINGLE_DATA &line, const int iMainStart,
		const int iMainEnd) {
	int iMainCnt, iSubCnt;
	int iNum = iMainEnd - iMainStart + 1;
	double dSum;

	if (iMainStart < 0 || iMainEnd > MainPixel - 1) {
		return (false);
	}

	line.NewDt(SubPixel);
	for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
		dSum = 0.0;
		for (iMainCnt = iMainStart; iMainCnt <= iMainEnd; ++iMainCnt) {
			dSum += double(Dt[iMainCnt][iSubCnt]);
		}
		line.dDt[iSubCnt] = dSum / iNum;
	}
	return (true);
}
//---------------------------------------------------------------------------
/*
 Formatしてcopy
 */
void TFIMG::CopyFormat(const int iOrgMain, const int iOrgSub, CTDAT &CopyImg,
		const int iCopySubPixel, const int iCopyMainPixel,
		const int iCopyOrgSub, const int iCopyOrgMain) {
	int iMainCnt, iSubCnt;

	for (iMainCnt = 0; iMainCnt < iCopyMainPixel; ++iMainCnt) {
		for (iSubCnt = 0; iSubCnt < iCopySubPixel; ++iSubCnt) {
			Dt[iOrgMain + iMainCnt][iOrgSub + iSubCnt] = CopyImg.Dt[iCopyOrgSub
					+ iSubCnt][iCopyOrgMain + iMainCnt];
		}
	}
}
//---------------------------------------------------------------------------
/*
 Formatしてcopy
 */
void TFIMG::CopyFormat(const int iOrgMain, const int iOrgSub, TFIMG &CopyImg,
		const int iCopyMainPixel, const int iCopySubPixel,
		const int iCopyOrgMain, const int iCopyOrgSub) {
	int iMainCnt, iSubCnt;

	for (iMainCnt = 0; iMainCnt < iCopyMainPixel; ++iMainCnt) {
		for (iSubCnt = 0; iSubCnt < iCopySubPixel; ++iSubCnt) {
			Dt[iOrgMain + iMainCnt][iOrgSub + iSubCnt] = CopyImg.Dt[iCopyOrgMain
					+ iMainCnt][iCopyOrgSub + iSubCnt];
		}
	}
}
//---------------------------------------------------------------------------
/*
 引数領域の平均値を返す。
 */
double TFIMG::dAve(int iMain1, int iSub1, int iMain2, int iSub2) {
	if (iMain1 < 0) {
		iMain1 = 0;
	}
	if (iSub1 < 0) {
		iSub1 = 0;
	}
	if (MainPixel <= iMain2) {
		iMain2 = MainPixel - 1;
	}
	if (SubPixel <= iSub2) {
		iSub2 = SubPixel - 1;
	}
	if (iMain2 < iMain1) {
		if (MainPixel <= iMain1) {
			iMain1 = MainPixel - 1;
		}
		iMain2 = iMain1;
	}
	if (iSub2 < iSub1) {
		if (SubPixel <= iSub1) {
			iSub1 = SubPixel - 1;
		}
		iSub2 = iSub1;
	}

	double dSum = 0.0;
	for (int iMCnt = iMain1; iMCnt <= iMain2; ++iMCnt) {
		for (int iSCnt = iSub1; iSCnt <= iSub2; ++iSCnt) {
			dSum += Dt[iMCnt][iSCnt];
		}
	}
	return (dSum / ((iMain2 - iMain1 + 1) * (iSub2 - iSub1 + 1)));
}
//---------------------------------------------------------------------------
/*
 Dt[][]のdelete
 */
void TFIMG::DeleteDt() {
	int i;

	if (DtExist) {
		for (i = 0; i < MainPixel; ++i) {
			delete[] Dt[i];
		}
		delete[] Dt;

		DtExist = false;
	}
}
//---------------------------------------------------------------------------
/*
 ２値化
 */
void TFIMG::Bin(float thres, float xmin, float xmax) 	//副走査画素数
		{
	for (int iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {		//副走査平均
		for (int iSCnt = 1; iSCnt < SubPixel; ++iSCnt) {
			if (Dt[iMCnt][iSCnt] >= thres)
				Dt[iMCnt][iSCnt] = xmax;
			else
				Dt[iMCnt][iSCnt] = xmin;
		}
	}
}
//---------------------------------------------------------------------------
/*
 最大最小
 bReturnMax = trueの場合は最大を返し、falseの場合は最小を返す
 */
float TFIMG::Maxmin(int x1, int x2, int y1, int y2, bool bReturnMax) {
	float max = -1e+7, min = 1e+7;

//	for(int iMCnt = x1; iMCnt < x2-1; ++iMCnt){		//副走査平均
//		for(int iSCnt = y1; iSCnt < y2-1; ++iSCnt){
	for (int iMCnt = x1; iMCnt <= x2; iMCnt++) {		//副走査平均
		for (int iSCnt = y1; iSCnt <= y2; iSCnt++) {
			if (Dt[iMCnt][iSCnt] > max)
				max = Dt[iMCnt][iSCnt];
			if (Dt[iMCnt][iSCnt] < min)
				min = Dt[iMCnt][iSCnt];

		}
	}

	if (bReturnMax == true)
		return max;
	else
		return min;
}
//---------------------------------------------------------------------------
/*
 画像縮小処理
 画素数の平均値で画像縮小
 */
void TFIMG::DifS1_0() 	//副走査画素数
{
	for (int iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {		//副走査平均
		for (int iSCnt = 1; iSCnt < SubPixel; ++iSCnt) {
			Dt[iMCnt][iSCnt] = Dt[iMCnt][iSCnt] - Dt[iMCnt][iSCnt - 1];
		}
	}
}
//---------------------------------------------------------------------------
/*
 画像縮小処理
 画素数の平均値で画像縮小
 */
void TFIMG::Div(const int iMainDiv,	    //主走査画素数
		const int iSubDiv) 	//副走査画素数
		{
	int iMCnt, iSCnt, iAveCnt;
	float fSum;
	TFIMG temp;
	float fSubDiv = iSubDiv;
	float fMainDiv = iMainDiv;

	temp.NewDt(MainPixel, SubPixel / iSubDiv);

	for (iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {		//副走査平均
		for (iSCnt = 0; iSCnt < temp.SubPixel; ++iSCnt) {
			fSum = 0.0;
			for (iAveCnt = 0; iAveCnt < iSubDiv; ++iAveCnt) {
				fSum += Dt[iMCnt][iSCnt * iSubDiv + iAveCnt];
			}
			temp.Dt[iMCnt][iSCnt] = fSum / fSubDiv;
		}
	}

	NewDt(MainPixel / iMainDiv, temp.SubPixel);

	for (iSCnt = 0; iSCnt < temp.SubPixel; ++iSCnt) { 		//主走査平均
		for (iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
			fSum = 0.0;
			for (iAveCnt = 0; iAveCnt < iMainDiv; ++iAveCnt) {
				fSum += temp.Dt[iMCnt * iMainDiv + iAveCnt][iSCnt];
			}
			Dt[iMCnt][iSCnt] = fSum / fMainDiv;
		}
	}
}
//---------------------------------------------------------------------------
//平均縮小は切り捨て(ttsuji追加)

void TFIMG::DivTrancation(const int iMainDiv, const int iSubDiv) {
	int iMCnt, iSCnt, iAveCnt;
	float fSum;
	TFIMG temp;
//	float fSubDiv = iSubDiv;
//	float fMainDiv = iMainDiv;

	temp.NewDt(MainPixel, SubPixel / iSubDiv);

	for (iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {		//副走査平均
		for (iSCnt = 0; iSCnt < temp.SubPixel; ++iSCnt) {
			fSum = 0.0;
			for (iAveCnt = 0; iAveCnt < iSubDiv; ++iAveCnt) {
				fSum += Dt[iMCnt][iSCnt * iSubDiv + iAveCnt];
			}
			temp.Dt[iMCnt][iSCnt] = (int) fSum / iSubDiv;		//切り捨て
		}
	}

	NewDt(MainPixel / iMainDiv, temp.SubPixel);

	for (iSCnt = 0; iSCnt < temp.SubPixel; ++iSCnt) { 		//主走査平均
		for (iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
			fSum = 0.0;
			for (iAveCnt = 0; iAveCnt < iMainDiv; ++iAveCnt) {
				fSum += temp.Dt[iMCnt * iMainDiv + iAveCnt][iSCnt];
			}
			Dt[iMCnt][iSCnt] = (int) fSum / iMainDiv; 		//切り捨て
		}
	}
}
//---------------------------------------------------------------------------
/*
 画像縮小処理
 引数	const int sDiv	副走査画素数
 const int mDiv	主走査画素数

 画素数の平均値で画像縮小
 */
void TFIMG::DivWithRest(const int iMainDiv, const int iSubDiv) {
	int iMCnt, iSCnt, iAveCnt;
	int iRes;
	float fSum;
	TFIMG temp;

	//副走査平均
	iRes = SubPixel % iSubDiv;
	if (iRes == 0) {
		temp.NewDt(MainPixel, SubPixel / iSubDiv);

		for (iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
			for (iSCnt = 0; iSCnt < temp.SubPixel; ++iSCnt) {
				fSum = 0.0;
				for (iAveCnt = 0; iAveCnt < iSubDiv; ++iAveCnt) {
					fSum += Dt[iMCnt][iSCnt * iSubDiv + iAveCnt];
				}
				temp.Dt[iMCnt][iSCnt] = fSum / float(iSubDiv);
			}
		}
	} else {
		temp.NewDt(MainPixel, SubPixel / iSubDiv + 1);

		for (iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
			for (iSCnt = 0; iSCnt < temp.SubPixel - 1; ++iSCnt) {
				fSum = 0.0;
				for (iAveCnt = 0; iAveCnt < iSubDiv; ++iAveCnt) {
					fSum += Dt[iMCnt][iSCnt * iSubDiv + iAveCnt];
				}
				temp.Dt[iMCnt][iSCnt] = fSum / float(iSubDiv);
			}
			fSum = 0.0;
			for (iSCnt = (temp.SubPixel - 1) * iSubDiv; iSCnt < SubPixel;
					++iSCnt) {
				fSum += Dt[iMCnt][iSCnt];
			}
			temp.Dt[iMCnt][temp.SubPixel - 1] = fSum / float(iRes);
		}
	}

//主走査平均
	iRes = MainPixel % iMainDiv;
	if (iRes == 0) {
		NewDt(MainPixel / iMainDiv, temp.SubPixel);

		for (iSCnt = 0; iSCnt < SubPixel; ++iSCnt) {
			for (iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
				fSum = 0.0;
				for (iAveCnt = 0; iAveCnt < iMainDiv; ++iAveCnt) {
					fSum += temp.Dt[iMCnt * iMainDiv + iAveCnt][iSCnt];
				}
				Dt[iMCnt][iSCnt] = fSum / float(iMainDiv);
			}
		}
	} else {
		NewDt(MainPixel / iMainDiv + 1, temp.SubPixel);

		for (iSCnt = 0; iSCnt < SubPixel; ++iSCnt) {
			for (iMCnt = 0; iMCnt < MainPixel - 1; ++iMCnt) {
				fSum = 0.0;
				for (iAveCnt = 0; iAveCnt < iMainDiv; ++iAveCnt) {
					fSum += temp.Dt[iMCnt * iMainDiv + iAveCnt][iSCnt];
				}
				Dt[iMCnt][iSCnt] = fSum / float(iMainDiv);
			}
			fSum = 0.0;
			for (iMCnt = (MainPixel - 1) * iMainDiv; iMCnt < temp.MainPixel;
					++iMCnt) {
				fSum += temp.Dt[iMCnt][iSCnt];
			}
			Dt[MainPixel - 1][iSCnt] = fSum / float(iRes);
		}
	}
}
//---------------------------------------------------------------------------
//縦横が奇数画素数の間引き（画像中央が縮小後の画像中央に配置されるようにする）
//　（二次元PSFの間引きに使用）
//	ttsuji 180624

bool TFIMG::DivMabikiForOddSizeImage(const int iDiv) {
	if (1 == iDiv) {
		return true;
	}

	if (!MainPixel % 2) { 		//偶数サイズの場合はエラー
		return false;
	}
	if (!SubPixel % 2) { 		//偶数サイズの場合はエラー
		return false;
	}

	int iMainPixDiv = MainPixel / iDiv + 1; //MainPixel=177の場合は、iDiv=2ならiMainPixDiv=89、iDiv=4ならiMainPixDiv=45
	int iSubPixDiv = SubPixel / iDiv + 1; //SubPixel=177の場合は、iDiv=2ならiSubPixDiv=89、iDiv=4ならiSubPixDiv=45
	int iMCnt, iSCnt, iMDivCnt, iSDivCnt;
	TFIMG FImgTmp;
	FImgTmp = *this;

	NewDt(iMainPixDiv, iSubPixDiv);

	//0番目が0番目に配置されるようにする
	for (iSCnt = 0; iSCnt < FImgTmp.SubPixel; iSCnt += iDiv) {
		for (iMCnt = 0; iMCnt < FImgTmp.MainPixel; iMCnt += iDiv) {
			Dt[iMCnt / iDiv][iSCnt / iDiv] = FImgTmp.Dt[iMCnt][iSCnt];
		}
	}

	/*
	 //画像中央が縮小後の画像中央に配置されるようにする（0起点でMainPixel/2(=88)番目をiMainPixDiv/2(=44)番目とする）
	 for (iSCnt = SubPixel/2; iSCnt >= 0; iSCnt -= iDiv){
	 for (iMCnt = MainPixel/2; iMCnt >= 0; iMCnt -= iDiv){
	 Dt[iMCnt / iDiv][iSCnt / iDiv] = FImgTmp.Dt[iMCnt][iSCnt];
	 }
	 }
	 */

	return true;
}
//---------------------------------------------------------------------------
/*
 引数領域の絶対値最大値を返す。領域checkは行わない。
 */
float TFIMG::fAbsMax(const int iMain1, const int iSub1, const int iMain2,
		const int iSub2) {
	int iMCnt, iSCnt;
	float fMax = 0.0;

	for (iMCnt = iMain1; iMCnt <= iMain2; ++iMCnt) {
		for (iSCnt = iSub1; iSCnt <= iSub2; ++iSCnt) {
			if (fMax < fabs(Dt[iMCnt][iSCnt])) {
				fMax = fabs(Dt[iMCnt][iSCnt]);
			}
		}
	}
	return (fMax);
}
//---------------------------------------------------------------------------
/*
 引数領域の平均値を返す。領域checkは行わない。
 */
float TFIMG::fAve(const int iMain1, const int iSub1, const int iMain2,
		const int iSub2) {
	float fSum = 0.0;

	for (int iMCnt = iMain1; iMCnt <= iMain2; ++iMCnt) {
		for (int iSCnt = iSub1; iSCnt <= iSub2; ++iSCnt) {
			fSum += Dt[iMCnt][iSCnt];
		}
	}
	return (fSum / ((iMain2 - iMain1 + 1) * (iSub2 - iSub1 + 1)));
}
//---------------------------------------------------------------------------
/*
 2D-FFT
 2D-FFTのpowerに書き換える
 const int iFFTNumは2^nである必要がある。checkしない。
 また画像sizeも2^nである必要がある。

 */
void TFIMG::FFT2D(const int iFFTNum) {
	int iMainCnt, iSubCnt;
	int *ip, n1, n2, n;
	double **a, *w;

	n1 = iFFTNum;
	n2 = iFFTNum * 2;
	a = alloc_2d_double(n1, n2);
	n = MAX(n1, n2 / 2);
	ip = alloc_1d_int(2 + (int) sqrt(n + 0.5));
	n = MAX(n1, n2) * 3 / 2;
	w = alloc_1d_double(n);
	ip[0] = 0;

	for (iSubCnt = 0; iSubCnt < iFFTNum; ++iSubCnt) {
		for (iMainCnt = 0; iMainCnt < iFFTNum; ++iMainCnt) {
			a[iMainCnt][iSubCnt * 2] = Dt[iMainCnt][iSubCnt];
			a[iMainCnt][iSubCnt * 2 + 1] = 0.0;
		}
	}
	cdft2d(n1, n2, 1, a, NULL, ip, w);
	for (iSubCnt = 0; iSubCnt < iFFTNum; ++iSubCnt) {
		for (iMainCnt = 0; iMainCnt < iFFTNum; ++iMainCnt) {
			Dt[iMainCnt][iSubCnt] = a[iMainCnt][iSubCnt * 2]
					* a[iMainCnt][iSubCnt * 2]
					+ a[iMainCnt][iSubCnt * 2 + 1]
							* a[iMainCnt][iSubCnt * 2 + 1];
		}
	}
	free_1d_double(w);
	free_1d_int(ip);
	free_2d_double(a);
	;
}
//---------------------------------------------------------------------------
/*
 2D-FFT 指定領域を計算幅/2ずらしながら計算したpower平均値を求める
 const int iFFTNumは2^nである必要がある。checkしない。
 */
void TFIMG::FFT2DRepeat(const int iFFTNum,      //計算幅2^n
		const int iMain1,        //計算領域主開始座標
		const int iSub1,        //計算領域副開始座標
		const int iMain2,        //計算領域主終了座標
		const int iSub2,        //計算領域副終了座標
		TFIMG &result)        //2D-FFTpower平均結果
		{
	int iMainCnt, iSubCnt;
	int iMainSrt, iSubSrt;
	int numFFT = 0;
	int step = iFFTNum / 2;

	int *ip, n1, n2, n;
	double **a, *w;

	n1 = iFFTNum;
	n2 = iFFTNum * 2;
	a = alloc_2d_double(n1, n2);
	n = MAX(n1, n2 / 2);
	ip = alloc_1d_int(2 + (int) sqrt(n + 0.5));
	n = MAX(n1, n2) * 3 / 2;
	w = alloc_1d_double(n);
	ip[0] = 0;

	result.NewDt(iFFTNum, iFFTNum);
	for (iMainCnt = 0; iMainCnt < iFFTNum; ++iMainCnt)
		for (iSubCnt = 0; iSubCnt < iFFTNum; ++iSubCnt)
			result.Dt[iMainCnt][iSubCnt] = 0.0;

	for (iMainSrt = iMain1; iMainSrt + iFFTNum - 1 <= iMain2; iMainSrt +=
			step) {
		for (iSubSrt = iSub1; iSubSrt + iFFTNum - 1 <= iSub2; iSubSrt += step) {
			for (iSubCnt = 0; iSubCnt < iFFTNum; ++iSubCnt) {
				for (iMainCnt = 0; iMainCnt < iFFTNum; ++iMainCnt) {
					a[iMainCnt][iSubCnt * 2] = Dt[iSubSrt + iSubCnt][iMainSrt
							+ iMainCnt];
					a[iMainCnt][iSubCnt * 2 + 1] = 0.0;
				}
			}
			cdft2d(n1, n2, 1, a, NULL, ip, w);
			++numFFT;
			for (iSubCnt = 0; iSubCnt < iFFTNum; ++iSubCnt) {
				for (iMainCnt = 0; iMainCnt < iFFTNum; ++iMainCnt) {
					result.Dt[iMainCnt][iSubCnt] += a[iMainCnt][iSubCnt * 2]
							* a[iMainCnt][iSubCnt * 2]
							+ a[iMainCnt][iSubCnt * 2 + 1]
									* a[iMainCnt][iSubCnt * 2 + 1];
				}
			}
		}
	}
	free_1d_double(w);
	free_1d_int(ip);
	free_2d_double(a);

	for (iSubCnt = 0; iSubCnt < iFFTNum; ++iSubCnt) {
		for (iMainCnt = 0; iMainCnt < iFFTNum; ++iMainCnt) {
			result.Dt[iMainCnt][iSubCnt] /= numFFT;
		}
	}
}
//---------------------------------------------------------------------------
/*
 2D-FFT 指定領域を計算幅/2ずらしながら、各領域の平均値を引いてから計算した
 power平均値を求める

 const int iFFTNumは2^nである必要がある。checkしない。
 */
void TFIMG::FFT2DRepeatRemoveAve(const int iFFTNum,      //計算幅2^n
		const int iMain1,        //計算領域主開始座標
		const int iSub1,        //計算領域副開始座標
		const int iMain2,        //計算領域主終了座標
		const int iSub2,        //計算領域副終了座標
		TFIMG &result)        //2D-FFTpower平均結果
		{
	int iMainCnt, iSubCnt;
	int iMainSrt, iSubSrt;
	int numFFT = 0;
	int step = iFFTNum / 2;
	double dSum, dAve;

	int *ip, n1, n2, n;
	double **a, *w;

	n1 = iFFTNum;
	n2 = iFFTNum * 2;
	a = alloc_2d_double(n1, n2);
	n = MAX(n1, n2 / 2);
	ip = alloc_1d_int(2 + (int) sqrt(n + 0.5));
	n = MAX(n1, n2) * 3 / 2;
	w = alloc_1d_double(n);
	ip[0] = 0;

	result.NewDt(iFFTNum, iFFTNum);
	for (iMainCnt = 0; iMainCnt < iFFTNum; ++iMainCnt)
		for (iSubCnt = 0; iSubCnt < iFFTNum; ++iSubCnt)
			result.Dt[iMainCnt][iSubCnt] = 0.0;

	for (iMainSrt = iMain1; iMainSrt + iFFTNum - 1 <= iMain2; iMainSrt +=
			step) {
		for (iSubSrt = iSub1; iSubSrt + iFFTNum - 1 <= iSub2; iSubSrt += step) {
			dSum = 0.0;
			for (iSubCnt = 0; iSubCnt < iFFTNum; ++iSubCnt) {
				for (iMainCnt = 0; iMainCnt < iFFTNum; ++iMainCnt) {
					dSum += Dt[iMainSrt + iMainCnt][iSubSrt + iSubCnt];
				}
			}
			dAve = dSum / iFFTNum / iFFTNum;
			for (iSubCnt = 0; iSubCnt < iFFTNum; ++iSubCnt) {
				for (iMainCnt = 0; iMainCnt < iFFTNum; ++iMainCnt) {
					a[iMainCnt][iSubCnt * 2] = Dt[iMainSrt + iMainCnt][iSubSrt
							+ iSubCnt] - dAve;
					a[iMainCnt][iSubCnt * 2 + 1] = 0.0;
				}
			}
			cdft2d(n1, n2, 1, a, NULL, ip, w);
			++numFFT;
			for (iSubCnt = 0; iSubCnt < iFFTNum; ++iSubCnt) {
				for (iMainCnt = 0; iMainCnt < iFFTNum; ++iMainCnt) {
					result.Dt[iMainCnt][iSubCnt] += a[iMainCnt][iSubCnt * 2]
							* a[iMainCnt][iSubCnt * 2]
							+ a[iMainCnt][iSubCnt * 2 + 1]
									* a[iMainCnt][iSubCnt * 2 + 1];
				}
			}
		}
	}
	free_1d_double(w);
	free_1d_int(ip);
	free_2d_double(a);

	for (iSubCnt = 0; iSubCnt < iFFTNum; ++iSubCnt) {
		for (iMainCnt = 0; iMainCnt < iFFTNum; ++iMainCnt) {
			result.Dt[iMainCnt][iSubCnt] /= numFFT;
		}
	}
}
//---------------------------------------------------------------------------
/*
 全dataをfValにする
 */
void TFIMG::Fill(const float fVal) {
	int iMCnt, iSCnt;

	for (iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
		for (iSCnt = 0; iSCnt < SubPixel; ++iSCnt) {
			Dt[iMCnt][iSCnt] = fVal;
		}
	}
}
//---------------------------------------------------------------------------
/*
 filter処理

 引数	fFilter -> filterの配列
 iLength  -> filter長
 isMain   -> 処理方向　true：主　false:副
 */
void TFIMG::Filter(const float fFilter[], const int iLength,
		const bool isMain) {
	int iHalfNum = iLength / 2;
	int iNormalEndPos;
	int iMainCnt, iSubCnt;
	int iDtCnt, iDtStartPos, iDtEndPos;
	int iFilterCnt, iFilterStartPos;
	float fFilterSum;
	float fDtSum;
	float *pfOrgDt;

	if (isMain) {
		iNormalEndPos = MainPixel - iHalfNum;
		pfOrgDt = new float[MainPixel];
		for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
			for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
				pfOrgDt[iMainCnt] = Dt[iMainCnt][iSubCnt];
			}
			iDtEndPos = 3;
			iFilterStartPos = iHalfNum - 1;
			for (iMainCnt = 1; iMainCnt < iHalfNum; ++iMainCnt) {
				fDtSum = fFilter[iFilterStartPos] * pfOrgDt[0];
				fFilterSum = fFilter[iFilterStartPos];
				for (iDtCnt = 1, iFilterCnt = iFilterStartPos + 1;
						iDtCnt < iDtEndPos; ++iDtCnt, ++iFilterCnt) {
					fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
					fFilterSum += fFilter[iFilterCnt];
				}
				Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
				iDtEndPos += 2;
				--iFilterStartPos;
			}
			iDtStartPos = 0;
			for (iMainCnt = iHalfNum; iMainCnt < iNormalEndPos; ++iMainCnt) {
				fDtSum = fFilter[0] * pfOrgDt[iDtStartPos];
				for (iDtCnt = iDtStartPos + 1, iFilterCnt = 1;
						iFilterCnt < iLength; ++iDtCnt, ++iFilterCnt) {
					fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
				}
				Dt[iMainCnt][iSubCnt] = fDtSum;
				++iDtStartPos;
			}
			iDtStartPos = MainPixel - iLength + 2;
			iFilterStartPos = 1;
			for (iMainCnt = iNormalEndPos; iMainCnt < MainPixel; ++iMainCnt) {
				fDtSum = fFilter[iFilterStartPos] * pfOrgDt[iDtStartPos];
				fFilterSum = fFilter[iFilterStartPos];
				for (iDtCnt = iDtStartPos + 1, iFilterCnt = iFilterStartPos + 1;
						iDtCnt < MainPixel; ++iDtCnt, ++iFilterCnt) {
					fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
					fFilterSum += fFilter[iFilterCnt];
				}
				Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
				iDtStartPos += 2;
				++iFilterStartPos;
			}
		}
	} else {
		iNormalEndPos = SubPixel - iHalfNum;
		pfOrgDt = new float[SubPixel];
		for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
			for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
				pfOrgDt[iSubCnt] = Dt[iMainCnt][iSubCnt];
			}
			iDtEndPos = 3;
			iFilterStartPos = iHalfNum - 1;
			for (iSubCnt = 1; iSubCnt < iHalfNum; ++iSubCnt) {
				fDtSum = fFilter[iFilterStartPos] * pfOrgDt[0];
				fFilterSum = fFilter[iFilterStartPos];
				for (iDtCnt = 1, iFilterCnt = iFilterStartPos + 1;
						iDtCnt < iDtEndPos; ++iDtCnt, ++iFilterCnt) {
					fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
					fFilterSum += fFilter[iFilterCnt];
				}
				Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
				iDtEndPos += 2;
				--iFilterStartPos;
			}
			iDtStartPos = 0;
			for (iSubCnt = iHalfNum; iSubCnt < iNormalEndPos; ++iSubCnt) {
				fDtSum = fFilter[0] * pfOrgDt[iDtStartPos];
				for (iDtCnt = iDtStartPos + 1, iFilterCnt = 1;
						iFilterCnt < iLength; ++iDtCnt, ++iFilterCnt) {
					fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
				}
				Dt[iMainCnt][iSubCnt] = fDtSum;
				++iDtStartPos;
			}
			iDtStartPos = SubPixel - iLength + 2;
			iFilterStartPos = 1;
			for (iSubCnt = iNormalEndPos; iSubCnt < SubPixel; ++iSubCnt) {
				fDtSum = fFilter[iFilterStartPos] * pfOrgDt[iDtStartPos];
				fFilterSum = fFilter[iFilterStartPos];
				for (iDtCnt = iDtStartPos + 1, iFilterCnt = iFilterStartPos + 1;
						iDtCnt < SubPixel; ++iDtCnt, ++iFilterCnt) {
					fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
					fFilterSum += fFilter[iFilterCnt];
				}
				Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
				iDtStartPos += 2;
				++iFilterStartPos;
			}
		}
	}

	delete[] pfOrgDt;
}
//---------------------------------------------------------------------------
float TFIMG::fMedian(int iM0, int iS0, int iM1, int iS1, const float fTh) {
	TrimArea(iM0, iS0, iM1, iS1);
	int iNum = (iM1 - iM0 + 1) * (iS1 - iS0 + 1);
	float *fDt = new float[iNum];
	int iCnt = 0;
	if (fTh == 0.0f) {
		for (int iMCnt = iM0; iMCnt <= iM1; ++iMCnt) {
			for (int iSCnt = iS0; iSCnt <= iS1; ++iSCnt) {
				fDt[iCnt] = Dt[iMCnt][iSCnt];
				++iCnt;
			}
		}
	} else {
		for (int iMCnt = iM0; iMCnt <= iM1; ++iMCnt) {
			for (int iSCnt = iS0; iSCnt <= iS1; ++iSCnt) {
				if (fabs(Dt[iMCnt][iSCnt]) < fTh) {
					fDt[iCnt] = Dt[iMCnt][iSCnt];
					++iCnt;
				}
			}
		}
	}
	qsort(fDt, iCnt, sizeof(float), FloatCmp);
	float median = fDt[iCnt / 2];
	delete[] fDt;
	return (median);
}
//---------------------------------------------------------------------------
/*
 Format処理
 引数	const int sub	副走査画素数
 const int main	主走査画素数
 const int sSt	副走査原点位置
 const int mSt	主走査原点位置
 const int back	背景色

 原点位置が負のとき、切り出し、正のときshift
 */
void TFIMG::Format(int main, int sub, const int mSt, const int sSt,
		const float back) {
	if (sub < 0) {
		sub = SubPixel;
	}
	if (main < 0) {
		main = MainPixel;
	}
	int i, j;
	int endMain, endSub;        //元画像データの取り出し終了画素位置
	TFIMG temp;

	temp.NewDt(main, sub);

	temp.Fill(back);		//全画素背景色にする

	if (mSt >= 0 && sSt >= 0) { 		//主・副shift
		if (mSt + MainPixel > main) {
			endMain = main - mSt;
		} else {
			endMain = MainPixel;
		}
		if (sSt + SubPixel > sub) {
			endSub = sub - sSt;
		} else {
			endSub = SubPixel;
		}
		for (i = 0; i < endSub; ++i)
			for (j = 0; j < endMain; ++j)
				temp.Dt[j + mSt][i + sSt] = Dt[j][i];

	} else if (mSt >= 0 && sSt < 0) {		//主shift・副切り出し
		if (mSt + MainPixel > main) {
			endMain = main - mSt;
		} else {
			endMain = MainPixel;
		}
		if (SubPixel + sSt < sub) {
			endSub = SubPixel + sSt;
		} else {
			endSub = sub;
		}
		for (i = 0; i < endSub; ++i)
			for (j = 0; j < endMain; ++j)
				temp.Dt[j + mSt][i] = Dt[j][i - sSt];

	} else if (mSt < 0 && sSt >= 0) { // 主切り出し・副shift  													  // 主・副切り出し
		if (MainPixel + mSt < main) {
			endMain = MainPixel + mSt;
		} else {
			endMain = main;
		}
		if (sSt + SubPixel > sub) {
			endSub = sub - sSt;
		} else {
			endSub = SubPixel;
		}
		for (i = 0; i < endSub; ++i)
			for (j = 0; j < endMain; ++j)
				temp.Dt[j][i + sSt] = Dt[j - mSt][i];

	} else { 													  // 主・副切り出し
		if (MainPixel + mSt < main) {
			endMain = MainPixel + mSt;
		} else {
			endMain = main;
		}
		if (SubPixel + sSt < sub) {
			endSub = SubPixel + sSt;
		} else {
			endSub = sub;
		}
		for (i = 0; i < endSub; ++i)
			for (j = 0; j < endMain; ++j)
				temp.Dt[j][i] = Dt[j - mSt][i - sSt];
	}

	NewDt(main, sub);
	for (i = 0; i < main; ++i)
		for (j = 0; j < sub; ++j)
			Dt[i][j] = temp.Dt[i][j];
}
//---------------------------------------------------------------------------
/*
 141202 SD1桑原健です。

 桑原Ｍ殿から、ご連絡があったので共有化します。

 FIMageクラスの、fRMS関数でバグがあったそうです。
 内容
 ・この関数は floatで計算しているため、
 画像の内容によっては、途中計算で精度が足りず、正しくない結果を返すことがある　⇒　今回マンモで発見
 ・他のRMS系の関数は、doubleで計算しているので、問題となっていない。

 ざっと見た感じ、一般系の開発では該当部分を使っていないようでしたが、気をつけてください(使う場合はdoubleにする)。

 以上よろしくお願いします。

 */

float TFIMG::fRMS(const int iMain1,        //計算領域主開始座標
		const int iSub1,        //計算領域副開始座標
		const int iMain2,        //計算領域主終了座標
		const int iSub2)        //計算領域副終了座標
		{
	int iMCnt, iSCnt;
	float fSS = 0.0;		//2乗和
	float fS = 0.0;		//和

	if (iMain1 < 0 || iSub1 < 0 || iMain2 >= MainPixel || iSub2 >= SubPixel) {
		return (0.0);
	}

	for (iMCnt = iMain1; iMCnt <= iMain2; ++iMCnt) {
		for (iSCnt = iSub1; iSCnt <= iSub2; ++iSCnt) {
			fSS += Dt[iMCnt][iSCnt] * Dt[iMCnt][iSCnt];
			fS += Dt[iMCnt][iSCnt];
		}
	}

	float MPixel = iMain2 - iMain1 + 1;
	float SPixel = iSub2 - iSub1 + 1;
	return (sqrt((fSS - fS * fS / SPixel / MPixel) / SPixel / MPixel));
}
//---------------------------------------------------------------------------
float TFIMG::fYKMedian(int iM0, int iS0, int iM1, int iS1, const TFIMG &img,
		const float fTh) {
	TrimArea(iM0, iS0, iM1, iS1);
	int iNum = (iM1 - iM0 + 1) * (iS1 - iS0 + 1);
	float *fDt = new float[iNum];
	int iCnt = 0;
	for (int iMCnt = iM0; iMCnt <= iM1; ++iMCnt) {
		for (int iSCnt = iS0; iSCnt <= iS1; ++iSCnt) {
			if (img.Dt[iMCnt][iSCnt] < fTh) {
				fDt[iCnt] = Dt[iMCnt][iSCnt];
				++iCnt;
			}
		}
	}
	float median;
	if (iCnt == 0) {
		median = 0.0f;
	} else {
		qsort(fDt, iCnt, sizeof(float), FloatCmp);
		median = fDt[iCnt / 2];
	}
	delete[] fDt;
	return (median);
}
//---------------------------------------------------------------------------
/*

 */
int TFIMG::iFindPos(const float fVal, float fDt[], int iS, int iE) {
	int iHalf = (iS + iE) / 2;

	if (fDt[iHalf] < fVal) {
		return (iFindPos(fVal, fDt, iHalf + 1, iE));
	} else if (fDt[iHalf] > fVal) {
		return (iFindPos(fVal, fDt, iS, iHalf - 1));
	} else {
		return (iHalf);
	}
}
//---------------------------------------------------------------------------
/*
 直線補間で拡大
 */
void TFIMG::LinearExpand(const int iMainExpand, const int iSubExpand) {
	int iMCnt, iSCnt;
	int iCnt;
	int iPos;
	TFIMG orgImg;
	float *pfSmall, *pfBig;

	if (iSubExpand <= 1 && iMainExpand <= 1)
		return;

	orgImg.NewDt(MainPixel, SubPixel);
	for (iSCnt = 0; iSCnt < SubPixel; ++iSCnt) {
		for (iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
			orgImg.Dt[iMCnt][iSCnt] = Dt[iMCnt][iSCnt];
		}
	}

	NewDt(MainPixel * iMainExpand, SubPixel * iSubExpand);
	//副へ拡大
	if (iSubExpand > 1) {
		pfSmall = new float[iSubExpand];
		pfBig = new float[iSubExpand];

		if (iSubExpand % 2 == 0) {
			//make filter
			for (iCnt = 0; iCnt < iSubExpand; ++iCnt) {
				pfBig[iCnt] = float(2 * iCnt + 1) / float(iSubExpand * 2);
				pfSmall[iCnt] = 1.0 - pfBig[iCnt];
			}

			for (iMCnt = 0; iMCnt < orgImg.MainPixel; ++iMCnt) {
				iPos = 0;
				//端部処理
				for (iCnt = iSubExpand / 2 - 1; iCnt >= 0; --iCnt) {
					Dt[iMCnt][iPos] = (1.0 + pfBig[iCnt]) * orgImg.Dt[iMCnt][0]
							- pfBig[iCnt] * orgImg.Dt[iMCnt][1];
					++iPos;
				}

				for (iSCnt = 1; iSCnt < orgImg.SubPixel; ++iSCnt) {
					for (iCnt = 0; iCnt < iSubExpand; ++iCnt) {
						Dt[iMCnt][iPos] = pfSmall[iCnt]
								* orgImg.Dt[iMCnt][iSCnt - 1]
								+ pfBig[iCnt] * orgImg.Dt[iMCnt][iSCnt];
						++iPos;
					}
				}
				//端部処理
				for (iCnt = 0; iCnt < iSubExpand / 2; ++iCnt) {
					Dt[iMCnt][iPos] = -pfBig[iCnt]
							* orgImg.Dt[iMCnt][orgImg.SubPixel - 2]
							+ (1.0 + pfBig[iCnt])
									* orgImg.Dt[iMCnt][orgImg.SubPixel - 1];
					++iPos;
				}
			}
		} else {
			//make filter
			for (iCnt = 0; iCnt < iSubExpand - 1; ++iCnt) {
				pfBig[iCnt] = float(iCnt + 1) / float(iSubExpand);
				pfSmall[iCnt] = 1.0 - pfBig[iCnt];
			}

			for (iMCnt = 0; iMCnt < orgImg.MainPixel; ++iMCnt) {
				iPos = 0;
				//端部処理
				for (iCnt = iSubExpand / 2 - 1; iCnt >= 0; --iCnt) {
					Dt[iMCnt][iPos] = (1.0 + pfBig[iCnt]) * orgImg.Dt[iMCnt][0]
							- pfBig[iCnt] * orgImg.Dt[iMCnt][1];
					++iPos;
				}

				for (iSCnt = 1; iSCnt < orgImg.SubPixel; ++iSCnt) {
					Dt[iMCnt][iPos] = orgImg.Dt[iMCnt][iSCnt - 1];
					++iPos;
					for (iCnt = 0; iCnt < iSubExpand - 1; ++iCnt) {
						Dt[iMCnt][iPos] = pfSmall[iCnt]
								* orgImg.Dt[iMCnt][iSCnt - 1]
								+ pfBig[iCnt] * orgImg.Dt[iMCnt][iSCnt];
						++iPos;
					}
				}
				//端部処理
				Dt[iMCnt][iPos] = orgImg.Dt[iMCnt][iSCnt - 1];
				++iPos;
				for (iCnt = 0; iCnt < iSubExpand / 2; ++iCnt) {
					Dt[iMCnt][iPos] = -pfBig[iCnt]
							* orgImg.Dt[iMCnt][orgImg.SubPixel - 2]
							+ (1.0 + pfBig[iCnt])
									* orgImg.Dt[iMCnt][orgImg.SubPixel - 1];
					++iPos;
				}
			}
		}
		delete[] pfSmall;
		delete[] pfBig;
	}
	//主へ拡大
	if (iMainExpand > 1) {
		if (iSubExpand > 1) {
			orgImg.NewDt(MainPixel, SubPixel * iSubExpand);
			for (iMCnt = 0; iMCnt < orgImg.MainPixel; ++iMCnt) {
				for (iSCnt = 0; iSCnt < orgImg.SubPixel; ++iSCnt) {
					orgImg.Dt[iMCnt][iSCnt] = Dt[iMCnt][iSCnt];
				}
			}
		}
		pfSmall = new float[iMainExpand];
		pfBig = new float[iMainExpand];

		if (iMainExpand % 2 == 0) {
			//make filter
			for (iCnt = 0; iCnt < iMainExpand; ++iCnt) {
				pfBig[iCnt] = float(2 * iCnt + 1) / float(iMainExpand * 2);
				pfSmall[iCnt] = 1.0 - pfBig[iCnt];
			}

			for (iSCnt = 0; iSCnt < orgImg.SubPixel; ++iSCnt) {
				iPos = 0;
				//端部処理
				for (iCnt = iMainExpand / 2 - 1; iCnt >= 0; --iCnt) {
					Dt[iPos][iSCnt] = (1.0 + pfBig[iCnt]) * orgImg.Dt[0][iSCnt]
							- pfBig[iCnt] * orgImg.Dt[1][iSCnt];
					++iPos;
				}

				for (iMCnt = 1; iMCnt < orgImg.MainPixel; ++iMCnt) {
					for (iCnt = 0; iCnt < iMainExpand; ++iCnt) {
						Dt[iPos][iSCnt] = pfSmall[iCnt]
								* orgImg.Dt[iMCnt - 1][iSCnt]
								+ pfBig[iCnt] * orgImg.Dt[iMCnt][iSCnt];
						++iPos;
					}
				}
				//端部処理
				for (iCnt = 0; iCnt < iMainExpand / 2; ++iCnt) {
					Dt[iPos][iSCnt] = -pfBig[iCnt]
							* orgImg.Dt[orgImg.MainPixel - 2][iSCnt]
							+ (1.0 + pfBig[iCnt])
									* orgImg.Dt[orgImg.MainPixel - 1][iSCnt];
					++iPos;
				}
			}
		} else {
			//make filter
			for (iCnt = 0; iCnt < iMainExpand - 1; ++iCnt) {
				pfBig[iCnt] = float(iCnt + 1) / float(iMainExpand);
				pfSmall[iCnt] = 1.0 - pfBig[iCnt];
			}

			for (iSCnt = 0; iSCnt < orgImg.SubPixel; ++iSCnt) {
				iPos = 0;
				//端部処理
				for (iCnt = iMainExpand / 2 - 1; iCnt >= 0; --iCnt) {
					Dt[iPos][iSCnt] = (1.0 + pfBig[iCnt]) * orgImg.Dt[0][iSCnt]
							- pfBig[iCnt] * orgImg.Dt[1][iSCnt];
					++iPos;
				}

				for (iMCnt = 1; iMCnt < orgImg.MainPixel; ++iMCnt) {
					Dt[iPos][iSCnt] = orgImg.Dt[iMCnt - 1][iSCnt];
					++iPos;
					for (iCnt = 0; iCnt < iMainExpand - 1; ++iCnt) {
						Dt[iPos][iSCnt] = pfSmall[iCnt]
								* orgImg.Dt[iMCnt - 1][iSCnt]
								+ pfBig[iCnt] * orgImg.Dt[iMCnt][iSCnt];
						++iPos;
					}
				}
				//端部処理
				Dt[iPos][iSCnt] = orgImg.Dt[iMCnt - 1][iSCnt];
				++iPos;
				for (iCnt = 0; iCnt < iMainExpand / 2; ++iCnt) {
					Dt[iPos][iSCnt] = -pfBig[iCnt]
							* orgImg.Dt[orgImg.MainPixel - 2][iSCnt]
							+ (1.0 + pfBig[iCnt])
									* orgImg.Dt[orgImg.MainPixel - 1][iSCnt];
					++iPos;
				}
			}
		}
		delete[] pfSmall;
		delete[] pfBig;
	}
}
//---------------------------------------------------------------------------
/*
 最小2乗法 y = dA * x + dB
 Dt[0][] x
 Dt[1][] y

 引数	double& dA
 double& dB

 ※X = constの時はdA = dB = 0.0を返す
 */
void TFIMG::LMS(double &dA, double &dB) {
	int iSCnt;
	double dSx, dSy, dSxx, dSxy;

	dSx = dSy = dSxx = dSxy = 0.0;
	for (iSCnt = 0; iSCnt < SubPixel; ++iSCnt) {
		dSx += Dt[0][iSCnt];
		dSy += Dt[1][iSCnt];
		dSxx += Dt[0][iSCnt] * Dt[0][iSCnt];
		dSxy += Dt[0][iSCnt] * Dt[1][iSCnt];
	}
	if ((SubPixel * dSxx - dSx * dSx) == 0.0) {
		dA = dB = 0.0;
	} else {
		dA = (SubPixel * dSxy - dSx * dSy) / (SubPixel * dSxx - dSx * dSx);
		dB = (dSxx * dSy - dSxy * dSx) / (SubPixel * dSxx - dSx * dSx);
	}
}
//---------------------------------------------------------------------------
/*
 filter処理

 引数	fFilter -> filterの配列
 iLength  -> filter長
 */
void TFIMG::MainFilter(const float fFilter[], const int iLength) {
	/*
	 //  y
	 const int   iNUM = 6;      //移動加算の段数
	 const float   fREG = 102.0;     //規格化値
	 int iSumLen[] = {29, 23, 19, 15, 11,        //移動加算の長さ
	 5};
	 float   fFactor[] = {1.0,  1.0,  1.0,  1.0,  1.0,
	 1.0};
	 */

//  z
	const int iNUM = 5;      //移動加算の段数
	const float fREG = 103.0;     //規格化値
	int iSumLen[] = { 39, 25, 19, 13, 7 };
	float fFactor[] = { 1.0, 1.0, 1.0, 1.0, 1.0 };

	/*
	 //  fcut = 0.25c/mm
	 const int   iNUM = 7;      //移動加算の段数
	 const float   fREG = 61.0;     //規格化値
	 int iSumLen[] = {41, 33, 23, 19, 15,        //移動加算の長さ
	 9,  3};
	 float   fFactor[] = {-1.0,  1.0,  1.0,  1.0,  1.0,
	 1.0,  1.0};
	 */
	/*
	 //  fcut = 0.25c/mm
	 const int   iNUM = 10;      //移動加算の段数
	 const float   fREG = 106.0;     //規格化値
	 int iSumLen[] = {45, 29, 25, 21, 19,        //移動加算の長さ
	 17, 15, 11,  9,  5};
	 float   fFactor[] = {-1.0,  1.0,  1.0,  1.0,  1.0,
	 1.0,  1.0,  1.0,  1.0,  1.0};
	 */
	/*
	 //  fcut = 0.1c/mm
	 const int   iNUM = 10;      //移動加算の段数
	 const float   fREG = 329.0;     //規格化値
	 int iSumLen[] = {119, 105, 91, 81, 71,        //移動加算の長さ
	 63,  53, 45, 35, 23};
	 float   fFactor[] = {-2.0,  1.0,  1.0,  1.0,  1.0,
	 1.0,  1.0,  1.0,  1.0,  1.0};
	 */
	/*
	 //  fcut = 0.25c/mm
	 const int   iNUM = 16;      //移動加算の段数
	 const float   fREG = 204.0;     //規格化値
	 int iSumLen[] = {45, 43, 31, 27, 25,        //移動加算の長さ
	 23, 21, 19, 17, 15,
	 13, 11,  9,  7,  5,
	 3};
	 float   fFactor[] = {-1.0, -1.0,  1.0,  1.0,  1.0,
	 2.0,  1.0,  1.0,  2.0,  2.0,
	 1.0,  2.0,  1.0,  1.0,  1.0,
	 1.0};
	 */
	float fSum;     //移動加算の合計
	int iSubPos[iNUM];      //引く位置
	int iAddPos[iNUM];      //加算位置
	int iHalfNum = iLength / 2;
	int iNormalEndPos;
	int iMainCnt, iSubCnt;
	int iDtCnt, iDtStartPos, iDtEndPos;
	int iFilterCnt, iFilterStartPos;
	int iCnt, iCnt2;
	float fFilterSum;
	float fDtSum;
	float *pfOrgDt;

	iNormalEndPos = MainPixel - iHalfNum;
	pfOrgDt = new float[MainPixel];
	for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
		for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
			pfOrgDt[iMainCnt] = Dt[iMainCnt][iSubCnt];
		}
		iDtEndPos = 3;
		iFilterStartPos = iHalfNum - 1;
		for (iMainCnt = 1; iMainCnt < iHalfNum; ++iMainCnt) {
			fDtSum = fFilter[iFilterStartPos] * pfOrgDt[0];
			fFilterSum = fFilter[iFilterStartPos];
			for (iDtCnt = 1, iFilterCnt = iFilterStartPos + 1;
					iDtCnt < iDtEndPos; ++iDtCnt, ++iFilterCnt) {
				fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
				fFilterSum += fFilter[iFilterCnt];
			}
			Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
			iDtEndPos += 2;
			--iFilterStartPos;
		}

//移動加算
		fSum = 0.0;
		for (iCnt = 0; iCnt < iNUM; ++iCnt) {     //移動加算の初期化
			iSubPos[iCnt] = iHalfNum - iSumLen[iCnt] / 2;
			iAddPos[iCnt] = iHalfNum + iSumLen[iCnt] / 2 + 1;
			fSum += fFactor[iCnt] * pfOrgDt[iSubPos[iCnt]];
		}
		for (iCnt = 0; iCnt < iNUM; ++iCnt) {
			for (iCnt2 = iSubPos[iCnt] + 1; iCnt2 < iAddPos[iCnt]; ++iCnt2) {
				fSum += fFactor[iCnt] * pfOrgDt[iCnt2];
			}
		}
		Dt[iHalfNum][iSubCnt] = fSum / fREG;
		for (iMainCnt = iHalfNum + 1; iMainCnt < iNormalEndPos; ++iMainCnt) {
			for (iCnt = 0; iCnt < iNUM; ++iCnt) {
				fSum += fFactor[iCnt]
						* (pfOrgDt[iAddPos[iCnt]] - pfOrgDt[iSubPos[iCnt]]);
				++iSubPos[iCnt];
				++iAddPos[iCnt];
			}
			Dt[iMainCnt][iSubCnt] = fSum / fREG;
		}

		iDtStartPos = MainPixel - iLength + 2;
		iFilterStartPos = 1;
		for (iMainCnt = iNormalEndPos; iMainCnt < MainPixel; ++iMainCnt) {
			fDtSum = fFilter[iFilterStartPos] * pfOrgDt[iDtStartPos];
			fFilterSum = fFilter[iFilterStartPos];
			for (iDtCnt = iDtStartPos + 1, iFilterCnt = iFilterStartPos + 1;
					iDtCnt < MainPixel; ++iDtCnt, ++iFilterCnt) {
				fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
				fFilterSum += fFilter[iFilterCnt];
			}
			Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
			iDtStartPos += 2;
			++iFilterStartPos;
		}
	}
	delete[] pfOrgDt;
}
//---------------------------------------------------------------------------
/*
 filter処理

 引数	fFilter -> filterの配列
 iLength  -> filter長
 */
bool TFIMG::MainMultiAve(const string fileName) {
	int iCnt;
	int iFCnt;
	int iMainCnt, iSubCnt;
	double sum;     //移動平均値を出すための合計
	float *pfOrgDt;
	fstream file;
	int iFNum;      //移動平均段数
	int *piLen;     //移動平均長さ
	float *pfFactor;      //移動平均重み
	float fReg;       //移動平均規格化値
	int *piAddPos, *piSubPos;       //移動平均加算・減算位置
	int iHalfNum;        //移動平均個数の半分
	int iNormalEndPos;
	float *pfFilter;
	int iDtCnt, iDtStartPos, iDtEndPos;
	int iFilterCnt, iFilterStartPos;
	float fFilterSum;
	float fDtSum;

	file.open(fileName.c_str(), ios::in);
	if (file.fail())
		return (false);
	file >> iFNum;
	piLen = new int[iFNum];
	pfFactor = new float[iFNum];
	piAddPos = new int[iFNum];
	piSubPos = new int[iFNum];

	fReg = 0.0;
	for (iCnt = 0; iCnt < iFNum; ++iCnt) {
		file >> piLen[iCnt];
		file >> pfFactor[iCnt];
		fReg += pfFactor[iCnt] * piLen[iCnt];
	}
	file.close();

//make filter
	iHalfNum = piLen[0] / 2;
	pfFilter = new float[piLen[0]];
	piAddPos[0] = piLen[0];
	piSubPos[0] = 0;
	for (iCnt = 0; iCnt < piAddPos[0]; ++iCnt) {
		pfFilter[iCnt] = pfFactor[0];
	}
	for (iFCnt = 1; iFCnt < iFNum; ++iFCnt) {        //加減算位置計算
		piAddPos[iFCnt] = iHalfNum + piLen[iFCnt] / 2 + 1;
		piSubPos[iFCnt] = iHalfNum - piLen[iFCnt] / 2;
		for (iCnt = piSubPos[iFCnt]; iCnt < piAddPos[iFCnt]; ++iCnt) {
			pfFilter[iCnt] += pfFactor[iFCnt];
		}
	}
	for (iCnt = 0; iCnt < piAddPos[0]; ++iCnt) {
		pfFilter[iCnt] /= fReg;
	}

	iNormalEndPos = MainPixel - iHalfNum;
	pfOrgDt = new float[MainPixel];
	for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
		for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
			pfOrgDt[iMainCnt] = Dt[iMainCnt][iSubCnt];
		}
		iDtEndPos = 3;
		iFilterStartPos = iHalfNum - 1;
		for (iMainCnt = 1; iMainCnt < iHalfNum; ++iMainCnt) {
			fDtSum = pfFilter[iFilterStartPos] * pfOrgDt[0];
			fFilterSum = pfFilter[iFilterStartPos];
			for (iDtCnt = 1, iFilterCnt = iFilterStartPos + 1;
					iDtCnt < iDtEndPos; ++iDtCnt, ++iFilterCnt) {
				fDtSum += pfFilter[iFilterCnt] * pfOrgDt[iDtCnt];
				fFilterSum += pfFilter[iFilterCnt];
			}
			Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
			iDtEndPos += 2;
			--iFilterStartPos;
		}

//移動加算
		piAddPos[0] = piLen[0];
		piSubPos[0] = 0;
		for (iFCnt = 1; iFCnt < iFNum; ++iFCnt) {        //加減算位置計算
			piAddPos[iFCnt] = iHalfNum + piLen[iFCnt] / 2 + 1;
			piSubPos[iFCnt] = iHalfNum - piLen[iFCnt] / 2;
		}
		sum = 0.0;
		for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {
			for (iCnt = piSubPos[iFCnt]; iCnt < piAddPos[iFCnt]; ++iCnt) {
				sum += pfFactor[iFCnt] * pfOrgDt[iCnt];
			}
		}
		Dt[iHalfNum][iSubCnt] = sum / fReg;
		for (iMainCnt = iHalfNum + 1; iMainCnt < iNormalEndPos; ++iMainCnt) {
			for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {
				sum += pfFactor[iFCnt]
						* (pfOrgDt[piAddPos[iFCnt]] - pfOrgDt[piSubPos[iFCnt]]);
				++piAddPos[iFCnt];
				++piSubPos[iFCnt];
			}
			Dt[iMainCnt][iSubCnt] = sum / fReg;
		}

//filter
		iDtStartPos = MainPixel - piLen[0] + 2;
		iFilterStartPos = 1;
		for (iMainCnt = iNormalEndPos; iMainCnt < MainPixel; ++iMainCnt) {
			fDtSum = pfFilter[iFilterStartPos] * pfOrgDt[iDtStartPos];
			fFilterSum = pfFilter[iFilterStartPos];
			for (iDtCnt = iDtStartPos + 1, iFilterCnt = iFilterStartPos + 1;
					iDtCnt < MainPixel; ++iDtCnt, ++iFilterCnt) {
				fDtSum += pfFilter[iFilterCnt] * pfOrgDt[iDtCnt];
				fFilterSum += pfFilter[iFilterCnt];
			}
			Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
			iDtStartPos += 2;
			++iFilterStartPos;
		}
	}
	delete[] pfFilter;
	delete[] pfOrgDt;
	delete[] piLen;
	delete[] pfFactor;
	delete[] piAddPos;
	delete[] piSubPos;

	return (true);
}
//---------------------------------------------------------------------------
/*
 filter処理

 引数	fFilter -> filterの配列
 iLength  -> filter長
 */

bool TFIMG::MainExpandMultiAve(const string fileName) {
	int iCnt;
	int iFCnt;
	int iMainCnt, iSubCnt;
	int halfNum;        //移動平均個数の半分
	double sum;     //移動平均値を出すための合計
	double edgeSum;     //移動平均するとき、端部データの平均値を用いる。平均値計算用合計
	float edge;     //移動平均するとき、端部データの平均値を用いる。その平均値
	float *pfOrgDt;
	int iPos;
	fstream file;
	int iFNum;      //移動平均段数
	int *piLen;     //移動平均長さ
	float *pfFactor;      //移動平均重み
	float fReg;       //移動平均規格化値
	int *piAddPos, *piSubPos;       //移動平均加算・減算位置

	file.open(fileName.c_str(), ios::in);
	if (file.fail())
		return (false);
	file >> iFNum;
	piLen = new int[iFNum];
	pfFactor = new float[iFNum];
	piAddPos = new int[iFNum];
	piSubPos = new int[iFNum];

	fReg = 0.0;
	for (iCnt = 0; iCnt < iFNum; ++iCnt) {
		file >> piLen[iCnt];
		file >> pfFactor[iCnt];
		fReg += pfFactor[iCnt] * piLen[iCnt];
	}
	file.close();

	//多段フィルターの最も長いフィルター長が1（フィルターかけないことと同義）ならば何もせず終了（ttsuji追加）
	if (1 == piLen[0]) {
		return true;
	}

	halfNum = piLen[0] / 2;
	if (halfNum > MainPixel) {
		return (false);
	}
	pfOrgDt = new float[MainPixel + piLen[0]];

	for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {

//Trend除去する画像のcopy  端部データの平均値を左右に付け加える
		edgeSum = Dt[0][iSubCnt];     //端部データの平均値計算
		for (iMainCnt = 1; iMainCnt < halfNum; ++iMainCnt) {
			edgeSum += Dt[iMainCnt][iSubCnt];
		}
		edge = edgeSum / halfNum;
		for (iPos = 0; iPos < halfNum; ++iPos) {
			pfOrgDt[iPos] = edge;
		}

		for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
			pfOrgDt[iPos] = Dt[iMainCnt][iSubCnt];
			++iPos;
		}

		edgeSum = Dt[iMainCnt - halfNum][iSubCnt];     //端部データの平均値計算
		for (iMainCnt = MainPixel - halfNum + 1; iMainCnt < MainPixel;
				++iMainCnt) {
			edgeSum += Dt[iMainCnt][iSubCnt];
		}
		edge = edgeSum / halfNum;
		for (iMainCnt = 0; iMainCnt < halfNum; ++iMainCnt) {
			pfOrgDt[iPos] = edge;
			++iPos;
		}
//end of data copy
//移動平均
		for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {        //加減算位置計算
			piAddPos[iFCnt] = halfNum + piLen[iFCnt] / 2 + 1;
			piSubPos[iFCnt] = halfNum - piLen[iFCnt] / 2;
		}
		sum = 0.0;
		for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {
			for (iCnt = piSubPos[iFCnt]; iCnt < piAddPos[iFCnt]; ++iCnt) {
				sum += pfFactor[iFCnt] * pfOrgDt[iCnt];
			}
		}
		Dt[0][iSubCnt] = sum / fReg;
		for (iMainCnt = 1; iMainCnt < MainPixel; ++iMainCnt) {
			for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {
				sum += pfFactor[iFCnt]
						* (pfOrgDt[piAddPos[iFCnt]] - pfOrgDt[piSubPos[iFCnt]]);
				++piAddPos[iFCnt];
				++piSubPos[iFCnt];
			}
			Dt[iMainCnt][iSubCnt] = sum / fReg;
		}
	}
	delete[] pfOrgDt;
	delete[] piLen;
	delete[] pfFactor;
	delete[] piAddPos;
	delete[] piSubPos;

	return (true);
}
//---------------------------------------------------------------------------
//	130109 ttsuji
//	フィルター処理対象の画像とは別の画像で指定された主方向の区分ごとに主フィルター処理する
//	（HONEYの1ライン横スジ補正対策として実装）
//
bool TFIMG::MainExpandMultiAveForEachDetectedLine(const string fileName,
		CTDAT &DetectedLineImg) {
	if (MainPixel != DetectedLineImg.MainPixel
			|| SubPixel != DetectedLineImg.SubPixel) {
		return false;
	}

	int iCnt;
	int iFCnt;
	int iMainCnt, iSubCnt;
	int halfNum;        //移動平均個数の半分
	double sum;     //移動平均値を出すための合計
	double edgeSum;     //移動平均するとき、端部データの平均値を用いる。平均値計算用合計
	float edge;     //移動平均するとき、端部データの平均値を用いる。その平均値
	float *pfOrgDt;
	int iPos;
	fstream file;
	int iFNum;      //移動平均段数
	int *piLen;     //移動平均長さ
	float *pfFactor;      //移動平均重み
	float fReg;       //移動平均規格化値
	int *piAddPos, *piSubPos;       //移動平均加算・減算位置

	file.open(fileName.c_str(), ios::in);
	if (file.fail()) {
		return false;
	}
	file >> iFNum;
	piLen = new int[iFNum];
	pfFactor = new float[iFNum];
	piAddPos = new int[iFNum];
	piSubPos = new int[iFNum];

	fReg = 0.0;
	for (iCnt = 0; iCnt < iFNum; ++iCnt) {
		file >> piLen[iCnt];
		file >> pfFactor[iCnt];
		fReg += pfFactor[iCnt] * piLen[iCnt];
	}
	file.close();

	//多段フィルターの最も長いフィルター長が1（フィルターかけないことと同義）ならば何もせず終了（ttsuji追加）
	if (1 == piLen[0]) {
		delete[] piLen;
		delete[] pfFactor;
		delete[] piAddPos;
		delete[] piSubPos;
		return true;
	}

	halfNum = piLen[0] / 2;
	if (halfNum > MainPixel) {
		delete[] piLen;
		delete[] pfFactor;
		delete[] piAddPos;
		delete[] piSubPos;
		return false;
	}
//	pfOrgDt = new float[MainPixel + piLen[0]];

	for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
		//主方向の線分の数をカウント
		int iNumOfLine = 0;       //線分の数（濃度境界がなければ1）
		int iMStart[1024] = { 0 };       //各線分の主スタート座標
		int iMEnd[1024] = { 0 };       //各線分の主end座標
		for (int iMCnt = 0; iMCnt < DetectedLineImg.MainPixel; iMCnt++) {
			if (!DetectedLineImg.Dt[iSubCnt][iMCnt]) {
				iNumOfLine++;       //画素値zero領域を検出するたびにカウント
				iMStart[iNumOfLine - 1] = iMCnt;
				iMCnt++;
				while (!DetectedLineImg.Dt[iSubCnt][iMCnt]
						&& iMCnt < DetectedLineImg.MainPixel - 1) { //画素値zero領域をスキャン
					iMCnt++;
				}

				if (DetectedLineImg.MainPixel - 1 == iMCnt) {
					iMEnd[iNumOfLine - 1] = DetectedLineImg.MainPixel - 1;
				} else {
					iMEnd[iNumOfLine - 1] = iMCnt - 1;
				}

				while (DetectedLineImg.Dt[iSubCnt][iMCnt]
						&& iMCnt < DetectedLineImg.MainPixel - 1) { //画素値non-zero領域をスキャン
					iMCnt++;
				}
				if (iMCnt < DetectedLineImg.MainPixel - 1) {
					iMCnt--;
				}
			}

		}

		//線分検出ができなければエラー
		if (!iNumOfLine) {
			delete[] piLen;
			delete[] pfFactor;
			delete[] piAddPos;
			delete[] piSubPos;
			return false;
		}

		//検出した線分ごとに主フィルター処理
		for (int iLineCnt = 0; iLineCnt < iNumOfLine; iLineCnt++) {
			int iMWidth = iMEnd[iLineCnt] - iMStart[iLineCnt] + 1;       //線分長さ
			pfOrgDt = new float[iMWidth + piLen[0]];

//Trend除去する画像のcopy  端部データの平均値を左右に付け加える
			edgeSum = Dt[iMStart[iLineCnt]][iSubCnt];     //端部データの平均値計算
			int iCntAve = 0;
//			for(iMainCnt = iMStart[iLineCnt] + 1 ; iMainCnt < iMStart[iLineCnt] + halfNum ; ++iMainCnt){
			for (iMainCnt = iMStart[iLineCnt] + 1;
					(iMainCnt < iMStart[iLineCnt] + halfNum)
							&& (iMainCnt < MainPixel); ++iMainCnt) {
				edgeSum += Dt[iMainCnt][iSubCnt];
				iCntAve++;
			}
			edge = edgeSum / halfNum;
			edge = edgeSum / iCntAve;
			for (iPos = 0; iPos < halfNum; ++iPos) {
				pfOrgDt[iPos] = edge;
			}

			for (iMainCnt = iMStart[iLineCnt]; iMainCnt <= iMEnd[iLineCnt];
					++iMainCnt) {
				pfOrgDt[iPos] = Dt[iMainCnt][iSubCnt];
				++iPos;
			}

			edgeSum =
					Dt[CLIP(iMEnd[iLineCnt] + 1 - halfNum, 0, MainPixel - 1)][iSubCnt]; //端部データの平均値計算
			for (iMainCnt = CLIP(iMEnd[iLineCnt] + 2 - halfNum, 0,
					MainPixel - 1); iMainCnt <= iMEnd[iLineCnt]; ++iMainCnt) {
				edgeSum += Dt[iMainCnt][iSubCnt];
			}
			edge = edgeSum / halfNum;
			for (iMainCnt = 0; iMainCnt < halfNum; ++iMainCnt) {
				pfOrgDt[iPos] = edge;
				++iPos;
			}
//end of data copy
//移動平均
			for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {        //加減算位置計算
				piAddPos[iFCnt] = halfNum + piLen[iFCnt] / 2 + 1;
				piSubPos[iFCnt] = halfNum - piLen[iFCnt] / 2;
			}
			sum = 0.0;
			for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {
				for (iCnt = piSubPos[iFCnt]; iCnt < piAddPos[iFCnt]; ++iCnt) {
					sum += pfFactor[iFCnt] * pfOrgDt[iCnt];
				}
			}
			Dt[iMStart[iLineCnt]][iSubCnt] = sum / fReg;
			for (iMainCnt = iMStart[iLineCnt] + 1; iMainCnt <= iMEnd[iLineCnt];
					++iMainCnt) {
				for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {
					sum += pfFactor[iFCnt]
							* (pfOrgDt[piAddPos[iFCnt]]
									- pfOrgDt[piSubPos[iFCnt]]);
					++piAddPos[iFCnt];
					++piSubPos[iFCnt];
				}
				Dt[iMainCnt][iSubCnt] = sum / fReg;
			}

		}        //endif iLineCnt

				 //濃度境界画素を周囲の画素を用いて補間(境界は(iNumOfLine - 1)箇所)
		if (iNumOfLine > 1) {
			for (int iBorderCnt = 0; iBorderCnt < iNumOfLine - 1;
					iBorderCnt++) {
				for (iMainCnt = iMEnd[iBorderCnt] + 1;
						iMainCnt < iMStart[iBorderCnt + 1]; iMainCnt++) {
					if (iMStart[iBorderCnt + 1] - iMEnd[iBorderCnt]) {
						float fA = Dt[iMEnd[iBorderCnt]][iSubCnt];
						float fB = Dt[iMStart[iBorderCnt + 1]][iSubCnt];
						Dt[iMainCnt][iSubCnt] = (fA
								* (iMStart[iBorderCnt + 1] - iMainCnt)
								+ fB * (iMainCnt - iMEnd[iBorderCnt]))
								/ (iMStart[iBorderCnt + 1] - iMEnd[iBorderCnt]);
					}
				}
			}
		}

	}        //endif iSubCnt
	delete[] pfOrgDt;
	delete[] piLen;
	delete[] pfFactor;
	delete[] piAddPos;
	delete[] piSubPos;

	return true;
}
//---------------------------------------------------------------------------
/*
 福与さん作成(DEVO3 FPGA G面補正における横傘コンペアのため)

 filter処理

 引数	fFilter -> filterの配列
 iLength  -> filter長
 */

bool TFIMG::MainExpandMultiAveD3(const string fileName) {
	int iCnt;
	int iFCnt;
	int iMainCnt, iSubCnt;
	int halfNum;        //移動平均個数の半分
	double sum;     //移動平均値を出すための合計
	double edgeSum;     //移動平均するとき、端部データの平均値を用いる。平均値計算用合計
	float edge;     //移動平均するとき、端部データの平均値を用いる。その平均値
	float *pfOrgDt;
	int iPos;
	fstream file;
	int iFNum;      //移動平均段数
	int *piLen;     //移動平均長さ
	float *pfFactor;      //移動平均重み
	float fReg;       //移動平均規格化値
	int *piAddPos, *piSubPos;       //移動平均加算・減算位置

	file.open(fileName.c_str(), ios::in);
	if (file.fail())
		return (false);
	file >> iFNum;
	piLen = new int[iFNum];
	pfFactor = new float[iFNum];
	piAddPos = new int[iFNum];
	piSubPos = new int[iFNum];

	fReg = 0.0;
	for (iCnt = 0; iCnt < iFNum; ++iCnt) {
		file >> piLen[iCnt];
		file >> pfFactor[iCnt];
		fReg += pfFactor[iCnt] * piLen[iCnt];
	}
	file.close();

	//多段フィルターの最も長いフィルター長が1（フィルターかけないことと同義）ならば何もせず終了（ttsuji追加）
	if (1 == piLen[0]) {
		return true;
	}

	halfNum = piLen[0] / 2;
	if (halfNum > MainPixel) {
		return (false);
	}
	pfOrgDt = new float[MainPixel + piLen[0]];

	for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {

//Trend除去する画像のcopy  端部データの平均値を左右に付け加える
		edgeSum = Dt[0][iSubCnt];     //端部データの平均値計算
		for (iMainCnt = 1; iMainCnt < halfNum; ++iMainCnt) {
			edgeSum += Dt[iMainCnt][iSubCnt];
		}
		edge = int((edgeSum + (halfNum / 2) + 32768.0f * halfNum) / halfNum)
				- 32768.0f;
		//edge = edgeSum / halfNum;
		for (iPos = 0; iPos < halfNum; ++iPos) {
			pfOrgDt[iPos] = edge;
		}

		for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
			pfOrgDt[iPos] = Dt[iMainCnt][iSubCnt];
			++iPos;
		}

		edgeSum = Dt[iMainCnt - halfNum][iSubCnt];     //端部データの平均値計算
		for (iMainCnt = MainPixel - halfNum + 1; iMainCnt < MainPixel;
				++iMainCnt) {
			edgeSum += Dt[iMainCnt][iSubCnt];
		}
		edge = int((edgeSum + (halfNum / 2) + 32768.0f * halfNum) / halfNum)
				- 32768.0f;
		//edge = edgeSum / halfNum;
		for (iMainCnt = 0; iMainCnt < halfNum; ++iMainCnt) {
			pfOrgDt[iPos] = edge;
			++iPos;
		}
//end of data copy
//移動平均
		for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {        //加減算位置計算
			piAddPos[iFCnt] = halfNum + piLen[iFCnt] / 2 + 1;
			piSubPos[iFCnt] = halfNum - piLen[iFCnt] / 2;
		}
		sum = 0.0;
		for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {
			for (iCnt = piSubPos[iFCnt]; iCnt < piAddPos[iFCnt]; ++iCnt) {
				sum += pfFactor[iFCnt] * pfOrgDt[iCnt];
			}
		}
		Dt[0][iSubCnt] = sum / fReg;
		for (iMainCnt = 1; iMainCnt < MainPixel; ++iMainCnt) {
			for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {
				sum += pfFactor[iFCnt]
						* (pfOrgDt[piAddPos[iFCnt]] - pfOrgDt[piSubPos[iFCnt]]);
				++piAddPos[iFCnt];
				++piSubPos[iFCnt];
			}
			Dt[iMainCnt][iSubCnt] = sum / fReg;
		}
	}
	delete[] pfOrgDt;
	delete[] piLen;
	delete[] pfFactor;
	delete[] piAddPos;
	delete[] piSubPos;

	return (true);
}
//---------------------------------------------------------------------------
/*
 filter処理

 引数	fFilter -> filterの配列
 iLength  -> filter長
 */
void TFIMG::MainLowFilter(const float fFilter[], const int iLength) {
	/*
	 //  fcut = 0.25c/mm
	 const int   iNUM = 7;      //移動加算の段数
	 const float   fREG = 61.0;     //規格化値
	 int iSumLen[] = {41, 33, 23, 19, 15,        //移動加算の長さ
	 9,  3};
	 float   fFactor[] = {-1.0,  1.0,  1.0,  1.0,  1.0,
	 1.0,  1.0};
	 */
	/*
	 //  fcut = 0.25c/mm
	 const int   iNUM = 10;      //移動加算の段数
	 const float   fREG = 106.0;     //規格化値
	 int iSumLen[] = {45, 29, 25, 21, 19,        //移動加算の長さ
	 17, 15, 11,  9,  5};
	 float   fFactor[] = {-1.0,  1.0,  1.0,  1.0,  1.0,
	 1.0,  1.0,  1.0,  1.0,  1.0};
	 */

//  fcut = 0.1c/mm "filter0,1.txt"
	const int iNUM = 10;      //移動加算の段数
	const float fREG = 329.0;     //規格化値
	int iSumLen[] = { 119, 105, 91, 81, 71,        //移動加算の長さ
			63, 53, 45, 35, 23 };
	float fFactor[] = { -2.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };

	/*
	 //  fcut = 0.25c/mm
	 const int   iNUM = 16;      //移動加算の段数
	 const float   fREG = 204.0;     //規格化値
	 int iSumLen[] = {45, 43, 31, 27, 25,        //移動加算の長さ
	 23, 21, 19, 17, 15,
	 13, 11,  9,  7,  5,
	 3};
	 float   fFactor[] = {-1.0, -1.0,  1.0,  1.0,  1.0,
	 2.0,  1.0,  1.0,  2.0,  2.0,
	 1.0,  2.0,  1.0,  1.0,  1.0,
	 1.0};
	 */
	float fSum;     //移動加算の合計
	int iSubPos[iNUM];      //引く位置
	int iAddPos[iNUM];      //加算位置
	int iHalfNum = iLength / 2;
	int iNormalEndPos;
	int iMainCnt, iSubCnt;
	int iDtCnt, iDtStartPos, iDtEndPos;
	int iFilterCnt, iFilterStartPos;
	int iCnt, iCnt2;
	float fFilterSum;
	float fDtSum;
	float *pfOrgDt;

	iNormalEndPos = MainPixel - iHalfNum;
	pfOrgDt = new float[MainPixel];
	for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
		for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
			pfOrgDt[iMainCnt] = Dt[iMainCnt][iSubCnt];
		}
		iDtEndPos = 3;
		iFilterStartPos = iHalfNum - 1;
		for (iMainCnt = 1; iMainCnt < iHalfNum; ++iMainCnt) {
			fDtSum = fFilter[iFilterStartPos] * pfOrgDt[0];
			fFilterSum = fFilter[iFilterStartPos];
			for (iDtCnt = 1, iFilterCnt = iFilterStartPos + 1;
					iDtCnt < iDtEndPos; ++iDtCnt, ++iFilterCnt) {
				fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
				fFilterSum += fFilter[iFilterCnt];
			}
			Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
			iDtEndPos += 2;
			--iFilterStartPos;
		}

//移動加算
		fSum = 0.0;
		for (iCnt = 0; iCnt < iNUM; ++iCnt) {     //移動加算の初期化
			iSubPos[iCnt] = iHalfNum - iSumLen[iCnt] / 2;
			iAddPos[iCnt] = iHalfNum + iSumLen[iCnt] / 2 + 1;
			fSum += fFactor[iCnt] * pfOrgDt[iSubPos[iCnt]];
		}
		for (iCnt = 0; iCnt < iNUM; ++iCnt) {
			for (iCnt2 = iSubPos[iCnt] + 1; iCnt2 < iAddPos[iCnt]; ++iCnt2) {
				fSum += fFactor[iCnt] * pfOrgDt[iCnt2];
			}
		}
		Dt[iHalfNum][iSubCnt] = fSum / fREG;
		for (iMainCnt = iHalfNum + 1; iMainCnt < iNormalEndPos; ++iMainCnt) {
			for (iCnt = 0; iCnt < iNUM; ++iCnt) {
				fSum += fFactor[iCnt]
						* (pfOrgDt[iAddPos[iCnt]] - pfOrgDt[iSubPos[iCnt]]);
				++iSubPos[iCnt];
				++iAddPos[iCnt];
			}
			Dt[iMainCnt][iSubCnt] = fSum / fREG;
		}

		iDtStartPos = MainPixel - iLength + 2;
		iFilterStartPos = 1;
		for (iMainCnt = iNormalEndPos; iMainCnt < MainPixel; ++iMainCnt) {
			fDtSum = fFilter[iFilterStartPos] * pfOrgDt[iDtStartPos];
			fFilterSum = fFilter[iFilterStartPos];
			for (iDtCnt = iDtStartPos + 1, iFilterCnt = iFilterStartPos + 1;
					iDtCnt < MainPixel; ++iDtCnt, ++iFilterCnt) {
				fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
				fFilterSum += fFilter[iFilterCnt];
			}
			Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
			iDtStartPos += 2;
			++iFilterStartPos;
		}
	}
	delete[] pfOrgDt;
}
//---------------------------------------------------------------------------
/*
 filter処理

 引数	fFilter -> filterの配列
 iLength  -> filter長
 isMain   -> 処理方向　true：主　false:副
 */
void TFIMG::SubFilter(const float fFilter[], const int iLength) {

//  fcut = 0.03c/mm  "filter_f_psf0,03d319.txt"
	const int iNUM = 4;      //移動加算の段数
	const float fREG = 978.0;     //規格化値（移動加算の画素数）
	int iSumLen[] = { 319, 299, 223, 137 };
	float fFactor[] = { 1.0, 1.0, 1.0, 1.0 };

	/*
	 //  fcut = 0.03c/mm  "filter0,03.txt"
	 const int   iNUM = 6;      //移動加算の段数
	 const float fREG = 1223.0;     //規格化値（移動加算の画素数）
	 int iSumLen[] = {599, 517, 395, 323, 199,
	 127};
	 float   fFactor[] = {-1.0,  1.0,  1.0,  1.0,  1.0,
	 1.0};
	 */
	/*
	 //  fcut = 1.1c/mm  "filter1,1.txt"
	 const int   iNUM = 5;      //移動加算の段数
	 const float fREG = 40.0;     //規格化値（移動加算の画素数）
	 int iSumLen[] = {13, 9, 7, 5, 3};
	 float   fFactor[] = {-1.0,  2.0,  2.0,  3.0,  2.0};
	 */
	/*
	 //  fcut = 0.025c/mm
	 const int   iNUM = 7;      //移動加算の段数
	 const float fREG = 1099.0;     //規格化値（移動加算の画素数）
	 int iSumLen[] = {245,211,183,157,131,
	 103, 69};
	 */
	/*
	 //  fcut = 0.025c/mm
	 const int   iNUM = 4;      //移動加算の段数
	 const float fREG = 648.0;     //規格化値（移動加算の画素数）
	 int iSumLen[] = {233,183,139,93};
	 */
	/*
	 //  fcut = 0.025c/mm
	 const int   iNUM = 2;      //移動加算の段数
	 const float fREG = 324.0;     //規格化値（移動加算の画素数）
	 int iSumLen[] = {207, 117};
	 */
	/*
	 //  fcut = 0.025c/mm
	 const int   iNUM = 15;      //移動加算の段数
	 const float fREG = 2225.0;     //規格化値（移動加算の画素数）
	 int iSumLen[] = {255, 235, 219, 203, 189,        //移動加算の長さ
	 177, 163, 151, 137, 123,
	 111,  95,  79,  59,  29};
	 */
	float fSum;     //移動加算の合計
	int iSubPos[iNUM];      //引く位置
	int iAddPos[iNUM];      //加算位置
	int iHalfNum = iLength / 2;
	int iNormalEndPos;
	int iMainCnt, iSubCnt;
	int iDtCnt, iDtStartPos, iDtEndPos;
	int iFilterCnt, iFilterStartPos;
	int iCnt, iCnt2;
	float fFilterSum;
	float fDtSum;
	float *pfOrgDt;

	iNormalEndPos = SubPixel - iHalfNum;
	pfOrgDt = new float[SubPixel];
	for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
		for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
			pfOrgDt[iSubCnt] = Dt[iMainCnt][iSubCnt];
		}
		iDtEndPos = 3;
		iFilterStartPos = iHalfNum - 1;
		for (iSubCnt = 1; iSubCnt < iHalfNum; ++iSubCnt) {
			fDtSum = fFilter[iFilterStartPos] * pfOrgDt[0];
			fFilterSum = fFilter[iFilterStartPos];
			for (iDtCnt = 1, iFilterCnt = iFilterStartPos + 1;
					iDtCnt < iDtEndPos; ++iDtCnt, ++iFilterCnt) {
				fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
				fFilterSum += fFilter[iFilterCnt];
			}
			Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
			iDtEndPos += 2;
			--iFilterStartPos;
		}

//移動加算
		fSum = 0, 0;
		for (iCnt = 0; iCnt < iNUM; ++iCnt) {     //移動加算の初期化
			iSubPos[iCnt] = iHalfNum - iSumLen[iCnt] / 2;
			iAddPos[iCnt] = iHalfNum + iSumLen[iCnt] / 2 + 1;
			fSum += fFactor[iCnt] * pfOrgDt[iSubPos[iCnt]];
		}
		for (iCnt = 0; iCnt < iNUM; ++iCnt) {
			for (iCnt2 = iSubPos[iCnt] + 1; iCnt2 < iAddPos[iCnt]; ++iCnt2) {
				fSum += fFactor[iCnt] * pfOrgDt[iCnt2];
			}
		}
		Dt[iMainCnt][iHalfNum] = fSum / fREG;
		for (iSubCnt = iHalfNum + 1; iSubCnt < iNormalEndPos; ++iSubCnt) {
			for (iCnt = 0; iCnt < iNUM; ++iCnt) {
				fSum += fFactor[iCnt]
						* (pfOrgDt[iAddPos[iCnt]] - pfOrgDt[iSubPos[iCnt]]);
				++iSubPos[iCnt];
				++iAddPos[iCnt];
			}
			Dt[iMainCnt][iSubCnt] = fSum / fREG;
		}

		iDtStartPos = SubPixel - iLength + 2;
		iFilterStartPos = 1;
		for (iSubCnt = iNormalEndPos; iSubCnt < SubPixel; ++iSubCnt) {
			fDtSum = fFilter[iFilterStartPos] * pfOrgDt[iDtStartPos];
			fFilterSum = fFilter[iFilterStartPos];
			for (iDtCnt = iDtStartPos + 1, iFilterCnt = iFilterStartPos + 1;
					iDtCnt < SubPixel; ++iDtCnt, ++iFilterCnt) {
				fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
				fFilterSum += fFilter[iFilterCnt];
			}
			Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
			iDtStartPos += 2;
			++iFilterStartPos;
		}
	}

	delete[] pfOrgDt;
}

//---------------------------------------------------------------------------
/*
 filter処理

 引数	fFilter -> filterの配列
 iLength  -> filter長
 */
/*
 void TFIMG::MainFilter(const float fFilter[], const int iLength)
 {
 const int   iNUM = 16;      //移動加算の段数
 const int   iREG = 204;     //規格化値（移動加算の画素数）

 int iSum[iNUM];     //移動加算の合計
 int iTotalSum;
 int iSubPos[iNUM];      //引く位置
 int iAddPos[iNUM];      //加算位置
 int iSumLen[] = {45, 43, 31, 27, 25,        //移動加算の長さ
 23, 21, 19, 17, 15,
 13, 11,  9,  7,  5,
 3};
 int iFactor[] = {-1, -1,  1,  1,  1,
 2,  1,  1,  2,  2,
 1,  2,  1,  1,  1,
 1};
 int iHalfNum = iLength / 2;
 int iNormalEndPos;
 int iMainCnt, iSubCnt;
 int iDtCnt, iDtStartPos, iDtEndPos;
 int iFilterCnt, iFilterStartPos;
 int iCnt, iCnt2;
 float   fFilterSum;
 float   fDtSum;
 int *piOrgDt;

 iNormalEndPos = MainPixel - iHalfNum;
 piOrgDt = new int[MainPixel];
 for(iSubCnt = 0 ; iSubCnt < SubPixel ; ++iSubCnt){
 for(iMainCnt = 0 ; iMainCnt < MainPixel ; ++iMainCnt){
 piOrgDt[iMainCnt] = Dt[iMainCnt][iSubCnt];
 }
 iDtEndPos = 3;
 iFilterStartPos = iHalfNum - 1;
 for(iMainCnt = 1 ; iMainCnt < iHalfNum ; ++iMainCnt){
 fDtSum = fFilter[iFilterStartPos] * piOrgDt[0];
 fFilterSum = fFilter[iFilterStartPos];
 for(iDtCnt = 1, iFilterCnt = iFilterStartPos + 1; iDtCnt < iDtEndPos; ++iDtCnt, ++iFilterCnt){
 fDtSum += fFilter[iFilterCnt] * piOrgDt[iDtCnt];
 fFilterSum += fFilter[iFilterCnt];
 }
 Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
 iDtEndPos += 2;
 --iFilterStartPos;
 }

 //移動加算
 for(iCnt = 0; iCnt < iNUM; ++iCnt){     //移動加算の初期化
 iSubPos[iCnt] = iHalfNum - iSumLen[iCnt] / 2;
 iAddPos[iCnt] = iHalfNum + iSumLen[iCnt] / 2;
 iSum[iCnt] = piOrgDt[iSubPos[iCnt]];
 }
 iTotalSum = 0;
 for(iCnt = 1; iCnt < iNUM; ++iCnt){
 for(iCnt2 = iSubPos[iCnt] + 1; iCnt2 < iAddPos[iCnt]; ++iCnt2){
 iSum[iCnt] += piOrgDt[iCnt2];
 }
 iTotalSum += iFactor[iCnt] * iSum[iCnt];
 }
 Dt[iMainCnt][iSubCnt] = iTotalSum / iREG;
 for(iMainCnt = iHalfNum + 1; iMainCnt < iNormalEndPos; ++iMainCnt){
 iTotalSum = 0;
 for(iCnt = 0; iCnt < iNUM; ++iCnt){
 ++iSubPos[iCnt];
 ++iAddPos[iCnt];
 iSum[iCnt] = iSum[iCnt] - piOrgDt[iSubPos[iCnt]] + piOrgDt[iAddPos[iCnt]];
 iTotalSum += iFactor[iCnt] * iSum[iCnt];
 }
 Dt[iMainCnt][iSubCnt] = iTotalSum / iREG;
 }

 iDtStartPos = MainPixel - iLength + 2;
 iFilterStartPos = 1;
 for(iMainCnt = iNormalEndPos; iMainCnt < MainPixel; ++iMainCnt){
 fDtSum = fFilter[iFilterStartPos] * piOrgDt[iDtStartPos];
 fFilterSum = fFilter[iFilterStartPos];
 for(iDtCnt = iDtStartPos + 1, iFilterCnt = iFilterStartPos + 1; iDtCnt < MainPixel ; ++iDtCnt, ++iFilterCnt){
 fDtSum += fFilter[iFilterCnt] * piOrgDt[iDtCnt];
 fFilterSum += fFilter[iFilterCnt];
 }
 Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
 iDtStartPos += 2;
 ++iFilterStartPos;
 }
 }
 delete[] piOrgDt;
 }
 */
//---------------------------------------------------------------------------
/*
 filter処理

 引数	fFilter -> filterの配列
 iLength  -> filter長
 isMain   -> 処理方向　true：主　false:副
 */
/*
 void TFIMG::SubFilter(const float fFilter[], const int iLength)
 {
 const int   iNUM = 15;      //移動加算の段数
 const int   iREG = 2225;     //規格化値（移動加算の画素数）

 int iSum[iNUM];     //移動加算の合計
 int iTotalSum;
 int iSubPos[iNUM];      //引く位置
 int iAddPos[iNUM];      //加算位置
 int iSumLen[] = {255, 235, 219, 203, 189,        //移動加算の長さ
 177, 163, 151, 137, 123,
 111,  95,  79,  59,  29};
 int iHalfNum = iLength / 2;
 int iNormalEndPos;
 int iMainCnt, iSubCnt;
 int iDtCnt, iDtStartPos, iDtEndPos;
 int iFilterCnt, iFilterStartPos;
 int iCnt, iCnt2;
 float   fFilterSum;
 float   fDtSum;
 int *piOrgDt;


 iNormalEndPos = SubPixel - iHalfNum;
 piOrgDt = new int[SubPixel];
 for(iMainCnt = 0 ; iMainCnt < MainPixel ; ++iMainCnt){
 for(iSubCnt = 0 ; iSubCnt < SubPixel ; ++iSubCnt){
 piOrgDt[iSubCnt] = Dt[iMainCnt][iSubCnt];
 }
 iDtEndPos = 3;
 iFilterStartPos = iHalfNum - 1;
 for(iSubCnt = 1 ; iSubCnt < iHalfNum ; ++iSubCnt){
 fDtSum = fFilter[iFilterStartPos] * piOrgDt[0];
 fFilterSum = fFilter[iFilterStartPos];
 for(iDtCnt = 1, iFilterCnt = iFilterStartPos + 1; iDtCnt < iDtEndPos; ++iDtCnt, ++iFilterCnt){
 fDtSum += fFilter[iFilterCnt] * piOrgDt[iDtCnt];
 fFilterSum += fFilter[iFilterCnt];
 }
 Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
 iDtEndPos += 2;
 --iFilterStartPos;
 }

 //移動加算
 for(iCnt = 0; iCnt < iNUM; ++iCnt){     //移動加算の初期化
 iSubPos[iCnt] = iHalfNum - iSumLen[iCnt] / 2;
 iAddPos[iCnt] = iHalfNum + iSumLen[iCnt] / 2;
 iSum[iCnt] = piOrgDt[iSubPos[iCnt]];
 }
 iTotalSum = 0;
 for(iCnt = 1; iCnt < iNUM; ++iCnt){
 for(iCnt2 = iSubPos[iCnt] + 1; iCnt2 < iAddPos[iCnt]; ++iCnt2){
 iSum[iCnt] += piOrgDt[iCnt2];
 }
 iTotalSum += iSum[iCnt];
 }
 Dt[iMainCnt][iSubCnt] = iTotalSum / iREG;
 for(iSubCnt = iHalfNum + 1; iSubCnt < iNormalEndPos; ++iSubCnt){
 iTotalSum = 0;
 for(iCnt = 0; iCnt < iNUM; ++iCnt){
 ++iSubPos[iCnt];
 ++iAddPos[iCnt];
 iSum[iCnt] = iSum[iCnt] - piOrgDt[iSubPos[iCnt]] + piOrgDt[iAddPos[iCnt]];
 iTotalSum += iSum[iCnt];
 }
 Dt[iMainCnt][iSubCnt] = iTotalSum / iREG;
 }

 iDtStartPos = SubPixel - iLength + 2;
 iFilterStartPos = 1;
 for(iSubCnt = iNormalEndPos; iSubCnt < SubPixel; ++iSubCnt){
 fDtSum = fFilter[iFilterStartPos] * piOrgDt[iDtStartPos];
 fFilterSum = fFilter[iFilterStartPos];
 for(iDtCnt = iDtStartPos + 1, iFilterCnt = iFilterStartPos + 1; iDtCnt < SubPixel ; ++iDtCnt, ++iFilterCnt){
 fDtSum += fFilter[iFilterCnt] * piOrgDt[iDtCnt];
 fFilterSum += fFilter[iFilterCnt];
 }
 Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
 iDtStartPos += 2;
 ++iFilterStartPos;
 }
 }

 delete[] piOrgDt;
 }
 */
//---------------------------------------------------------------------------
/*
 filter処理
 両端filter長1/2のdataは処理しない。
 filter長と画素数のcheckはしていない。

 引数	filter -> filterの配列
 f_num  -> filter長
 main   -> 処理方向　true：主　false:副
 */
/*  old version
 void TFIMG::Filter(const float filter[], const int f_num, const bool main)
 {
 int i, j, k;
 int halfNum;        //filter長の半分
 int ct;
 int iend1, iend2;
 float *porgDt, *pdt;

 //filter処理
 halfNum = f_num / 2;
 if(main == true){
 porgDt = new float[MainPixel];
 for(j = 0 ; j < SubPixel ; ++j){
 for(i = 0 ; i < MainPixel ; ++i){
 porgDt[i] = Dt[i][j];
 }
 iend1 = MainPixel - halfNum;
 for(i = halfNum ; i < iend1 ; ++i){
 Dt[i][j] = 0.0;
 ct = 0;
 iend2 = i + halfNum;
 for(k = i - halfNum ; k <= iend2 ; ++k){
 Dt[i][j] += filter[ct] * porgDt[k];
 ++ct;
 }
 }
 }
 }else{
 porgDt = new float[SubPixel];
 for(i = 0 ; i < MainPixel ; ++i){
 pdt = Dt[i];
 for(j = 0 ; j < SubPixel ; ++j){
 porgDt[j] = *pdt;
 ++pdt;
 }
 iend1 = SubPixel - halfNum;
 for(j = halfNum ; j < iend1 ; ++j){
 Dt[i][j] = 0.0;
 ct = 0;
 iend2 = j + halfNum;
 for(k = j - halfNum ; k <= iend2 ; ++k){
 Dt[i][j] += filter[ct] * porgDt[k];
 ++ct;
 }
 }
 }
 }

 delete[] porgDt;
 }
 //---------------------------------------------------------------------------
 /*
 主方向Median filter処理
 両端filter長1/2のdataは処理しない。
 filter長>画素数の時何もしない。

 引数	ilength -> filter長
 */
/*
 void TFIMG::MainMedian(const int ilength)
 {
 int iXCnt, iYCnt, iCnt;
 int *piOrder;       //順番
 float *pfOriginal;        //original data
 int iOrder;     //追加dataの順位を示すcounter
 int iIncludePos, iRemovePos;        //data追加位置と除去位置
 int iCrtPos;        //補正位置
 int iEndOrder = ilength - 1;
 int iMedian;

 if(ilength > MainPixel) return;

 piOrder = new int[ilength];
 pfOriginal = new float[MainPixel];

 for(iYCnt = 0 ; iYCnt < SubPixel ; ++iYCnt){
 for(iXCnt = 0 ; iXCnt < MainPixel ; ++iXCnt){
 pfOriginal[iXCnt] = Dt[iXCnt][iYCnt];
 }

 iMedian = 0;
 iCrtPos = 0;
 piOrder[0] = 0;
 iIncludePos = 1;
 while(iIncludePos < ilength){
 iOrder = iIncludePos;
 for(iCnt = 0; iCnt < iIncludePos; ++iCnt){
 if(pfOriginal[iCnt] > pfOriginal[iIncludePos]){
 ++piOrder[iCnt];
 --iOrder;
 }
 }
 piOrder[iIncludePos] = iOrder;
 ++iIncludePos;
 iOrder = iIncludePos;
 for(iCnt = 0; iCnt < iIncludePos; ++iCnt){
 if(pfOriginal[iCnt] > pfOriginal[iIncludePos]){
 ++piOrder[iCnt];
 --iOrder;
 }
 }
 ++iMedian;
 ++iCrtPos;
 piOrder[iIncludePos] = iOrder;
 for(iCnt = 0; iCnt <= iIncludePos; ++iCnt){
 if(piOrder[iCnt] == iMedian){
 Dt[iCrtPos][iYCnt] = pfOriginal[iCnt];
 break;
 }
 }
 ++iIncludePos;
 }

 iRemovePos = 0;
 for(; iIncludePos < MainPixel ; ++iIncludePos){
 iOrder = iEndOrder;
 if(piOrder[0] == iMedian) Dt[iCrtPos][iYCnt] = pfOriginal[iRemovePos];
 for(iCnt = 1 ; iCnt < ilength ; ++iCnt){
 if(piOrder[iCnt] == iMedian) Dt[iCrtPos][iYCnt] = pfOriginal[iRemovePos + iCnt];
 if(pfOriginal[iRemovePos + iCnt] >= pfOriginal[iRemovePos]){
 piOrder[iCnt - 1] = piOrder[iCnt] - 1;
 }else{
 piOrder[iCnt - 1] = piOrder[iCnt];
 }
 if(pfOriginal[iRemovePos + iCnt] > pfOriginal[iIncludePos]){
 ++piOrder[iCnt - 1];
 --iOrder;
 }
 }
 piOrder[iEndOrder] = iOrder;
 ++iRemovePos;
 ++iCrtPos;
 }
 while(iRemovePos < MainPixel - 1){
 if(piOrder[0] == iMedian) Dt[iCrtPos][iYCnt] = pfOriginal[iRemovePos];
 for(iCnt = iRemovePos + 1; iCnt < MainPixel; ++iCnt){
 if(piOrder[iCnt - iRemovePos] == iMedian) Dt[iCrtPos][iYCnt] =
 pfOriginal[iCnt];
 if(pfOriginal[iCnt] >= pfOriginal[iRemovePos]){
 piOrder[iCnt - iRemovePos - 1] = piOrder[iCnt - iRemovePos] - 1;
 }else{
 piOrder[iCnt - iRemovePos - 1] = piOrder[iCnt - iRemovePos];
 }
 }
 ++iRemovePos;
 for(iCnt = iRemovePos + 1; iCnt < MainPixel; ++iCnt){
 if(pfOriginal[iCnt] >= pfOriginal[iRemovePos]){
 piOrder[iCnt - iRemovePos - 1] = piOrder[iCnt - iRemovePos] - 1;
 }else{
 piOrder[iCnt - iRemovePos - 1] = piOrder[iCnt - iRemovePos];
 }
 }
 ++iRemovePos;
 ++iCrtPos;
 --iMedian;
 }
 }
 delete[] piOrder;
 delete[] pfOriginal;
 }
 */
//---------------------------------------------------------------------------
/*
 主方向Median filter処理
 両端は1, 3,,,iNumまで広げながら処理する。
 filter長>画素数の時何もしない。

 引数	iNum -> filter長
 */
void TFIMG::MainMedian(const int iNum) {
	if (iNum > MainPixel || iNum % 2 == 0) {
		return;
	}

	int iMCnt, iSCnt;
	int iCnt, iOrdCnt;
	int iOrder;     //除くdataのpfOrder配列の位置
	int iPos;
	int iEnd;		//pfOrder配列の有効数
	int iRemPos;
	int iAddPos;
	int iMed;
	float *pfOrg, *pfOrder;

	pfOrg = new float[MainPixel];
	pfOrder = new float[iNum];

	for (iSCnt = 0; iSCnt < SubPixel; ++iSCnt) {
		iPos = 1;
		iEnd = 0;
		iRemPos = 0;
		iAddPos = 1;
		iMed = 0;
		for (iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
			pfOrg[iMCnt] = Dt[iMCnt][iSCnt];
		}

		pfOrder[0] = pfOrg[0];
		while (iAddPos < iNum) {
			iOrdCnt = iEnd;
			while (iOrdCnt >= 0 && pfOrder[iOrdCnt] > pfOrg[iAddPos]) {
				pfOrder[iOrdCnt + 1] = pfOrder[iOrdCnt];
				--iOrdCnt;
			}
			//pfOrder[iOrdCnt] <= pfOrg[iAddPos]
			pfOrder[iOrdCnt + 1] = pfOrg[iAddPos];
			++iAddPos;
			++iEnd;

			iOrdCnt = iEnd;
			while (iOrdCnt >= 0 && pfOrder[iOrdCnt] > pfOrg[iAddPos]) {
				pfOrder[iOrdCnt + 1] = pfOrder[iOrdCnt];
				--iOrdCnt;
			}
			//pfOrder[iOrdCnt] <= pfOrg[iAddPos]
			pfOrder[iOrdCnt + 1] = pfOrg[iAddPos];
			++iAddPos;
			++iEnd;

			++iMed;
			Dt[iPos][iSCnt] = pfOrder[iMed];
			++iPos;
		}

		while (iAddPos < MainPixel) {
			iOrder = iFindPos(pfOrg[iRemPos], pfOrder, 0, iEnd);
			/*
			 iOrder = iEnd;
			 while(pfOrder[iOrder] != pfOrg[iRemPos]){
			 --iOrder;
			 }
			 */
			++iRemPos;

			if (pfOrg[iAddPos] >= pfOrder[iOrder]) {
				++iOrder;
				while (iOrder <= iEnd && pfOrder[iOrder] < pfOrg[iAddPos]) {
					pfOrder[iOrder - 1] = pfOrder[iOrder];
					++iOrder;
				}
				pfOrder[iOrder - 1] = pfOrg[iAddPos];
			} else {
				--iOrder;
				while (iOrder >= 0 && pfOrder[iOrder] > pfOrg[iAddPos]) {
					pfOrder[iOrder + 1] = pfOrder[iOrder];
					--iOrder;
				}
				pfOrder[iOrder + 1] = pfOrg[iAddPos];
			}
			++iAddPos;

			Dt[iPos][iSCnt] = pfOrder[iMed];
			++iPos;
		}

		while (iRemPos < MainPixel - 2) {
			iOrder = iFindPos(pfOrg[iRemPos], pfOrder, 0, iEnd);
			for (iCnt = iOrder; iCnt < iEnd; ++iCnt) {
				pfOrder[iCnt] = pfOrder[iCnt + 1];
			}
			++iRemPos;
			--iEnd;

			iOrder = iFindPos(pfOrg[iRemPos], pfOrder, 0, iEnd);
			for (iCnt = iOrder; iCnt < iEnd; ++iCnt) {
				pfOrder[iCnt] = pfOrder[iCnt + 1];
			}
			++iRemPos;
			--iEnd;

			--iMed;
			Dt[iPos][iSCnt] = pfOrder[iMed];
			++iPos;
		}
	}
	delete[] pfOrg;
	delete[] pfOrder;
}

//---------------------------------------------------------------------------
/*
 主方向の移動平均
 Averaging個数が偶数の時前半dataを多くする。

 引数	iLength -> Averaging個数
 */
void TFIMG::MainAve(const int iStart, const int iEnd) {
	int iMainCnt, iSubCnt;
	int iSubPos, iAddPos;
	float fLength;
	float fDtSum;
	float *pfOrgDt;

	pfOrgDt = new float[MainPixel];
	for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
		for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
			pfOrgDt[iMainCnt] = Dt[iMainCnt][iSubCnt];
		}

		iAddPos = 1;
		fDtSum = pfOrgDt[0];
		for (iMainCnt = 1; iMainCnt <= iEnd; ++iMainCnt) {
			fDtSum += pfOrgDt[iAddPos];
			++iAddPos;
			fDtSum += pfOrgDt[iAddPos];
			++iAddPos;
			Dt[iMainCnt][iSubCnt] = fDtSum / iAddPos;
		}

		fLength = iStart + iEnd + 1;
		iSubPos = 0;
		for (iMainCnt = iEnd + 1; iMainCnt < MainPixel - iStart; ++iMainCnt) {
			fDtSum += (pfOrgDt[iAddPos] - pfOrgDt[iSubPos]);
			Dt[iMainCnt][iSubCnt] = fDtSum / fLength;
			++iSubPos;
			++iAddPos;
		}
		for (iMainCnt = MainPixel - iStart; iMainCnt < MainPixel; ++iMainCnt) {
			fLength -= 2.0;
			fDtSum -= pfOrgDt[iSubPos];
			++iSubPos;
			fDtSum -= pfOrgDt[iSubPos];
			++iSubPos;
			Dt[iMainCnt][iSubCnt] = fDtSum / fLength;
		}
	}
	delete[] pfOrgDt;
}
//---------------------------------------------------------------------------
/*
 主方向の移動平均
 Averaging個数が偶数の時前半dataを多くする。

 引数	iLength -> Averaging個数
 */
void TFIMG::MainMoveAve(const int iLength) {
	int iMainCnt, iSubCnt;
	int iSubPos, iAddPos;
	int iHalfNum = iLength / 2;        //移動平均個数の半分
	float fLength = iLength;
	float fDtSum;
	int iNum;
	int iNormalEndPos = MainPixel - iHalfNum;
	float *pfOrgDt;

	pfOrgDt = new float[MainPixel];
	for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
		for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
			pfOrgDt[iMainCnt] = Dt[iMainCnt][iSubCnt];
		}
		iAddPos = 1;
		fDtSum = Dt[0][iSubCnt];
		for (iMainCnt = 1; iMainCnt <= iHalfNum; ++iMainCnt) {
			fDtSum = fDtSum + pfOrgDt[iAddPos] + pfOrgDt[iAddPos + 1];
			iAddPos += 2;
			Dt[iMainCnt][iSubCnt] = fDtSum / iAddPos;
		}
		iSubPos = 0;
		for (iMainCnt = iHalfNum + 1; iMainCnt < iNormalEndPos; ++iMainCnt) {
			fDtSum = fDtSum - pfOrgDt[iSubPos] + pfOrgDt[iAddPos];
			Dt[iMainCnt][iSubCnt] = fDtSum / fLength;
			++iSubPos;
			++iAddPos;
		}
		iNum = iLength - 2;
		for (iMainCnt = iNormalEndPos; iMainCnt < MainPixel; ++iMainCnt) {
			fDtSum = fDtSum - pfOrgDt[iSubPos] - pfOrgDt[iSubPos + 1];
			Dt[iMainCnt][iSubCnt] = fDtSum / iNum;
			iSubPos += 2;
			iNum -= 2;
		}
	}
	delete[] pfOrgDt;
}
//---------------------------------------------------------------------------
/*
 主方向のFilter処理
 端部は平均値で拡大して処理
 */
void TFIMG::MainExpandFilter(const float fFilter[], const int iLength) {
	int iHalfNum = iLength / 2;
	int iMainCnt, iSubCnt;
	float fDtSum;
	float fAve;
	float *pfOrgDt;
	int iPos;
	int iDtCnt;

	pfOrgDt = new float[MainPixel + 2 * iHalfNum];
	for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
		//平均値で拡大
		fDtSum = 0.0;
		for (iDtCnt = 0; iDtCnt <= iHalfNum; ++iDtCnt) {
			fDtSum += Dt[iDtCnt][iSubCnt];
		}
		fAve = fDtSum / (iHalfNum + 1);

		iPos = 0;
		for (iDtCnt = 0; iDtCnt < iHalfNum; ++iDtCnt) {
			pfOrgDt[iPos] = fAve;
			++iPos;
		}

		//そのまま
		for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
			pfOrgDt[iPos] = Dt[iMainCnt][iSubCnt];
			++iPos;
		}

		//平均値で拡大
		fDtSum = 0.0;
		for (iDtCnt = 0; iDtCnt <= iHalfNum; ++iDtCnt) {
			fDtSum += Dt[MainPixel - 1 - iDtCnt][iSubCnt];
		}
		fAve = fDtSum / (iHalfNum + 1);

		for (iDtCnt = 0; iDtCnt < iHalfNum; ++iDtCnt) {
			pfOrgDt[iPos] = fAve;
			++iPos;
		}
		//filter処理
		iPos = 0;
		for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
			fDtSum = 0.0;
			for (iDtCnt = 0; iDtCnt < iLength; ++iDtCnt) {
				fDtSum += fFilter[iDtCnt] * pfOrgDt[iPos + iDtCnt];
			}
			Dt[iMainCnt][iSubCnt] = fDtSum;
			++iPos;
		}
	}
	delete[] pfOrgDt;
}
//---------------------------------------------------------------------------
/*
 主方向の移動平均
 Averaging個数が偶数の時前半dataを多くする。

 引数	aveNum -> Averaging個数
 */
void TFIMG::MainExpandMoveAve(const int aveNum) {
	int iMainCnt, iSubCnt;
	int halfNum;        //移動平均個数の半分
	double sum;     //移動平均値を出すための合計
	double edgeSum;     //移動平均するとき、端部データの平均値を用いる。平均値計算用合計
	float edge;     //移動平均するとき、端部データの平均値を用いる。その平均値
	float *pfOrgDt;
	int iPos, iAddPos, iSubPos;

	pfOrgDt = new float[MainPixel + aveNum];
	halfNum = aveNum / 2;

	for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {

//Trend除去する画像のcopy  端部データの平均値を上下に付け加える
		edgeSum = Dt[0][iSubCnt];     //端部データの平均値計算
		for (iMainCnt = 1; iMainCnt <= halfNum; ++iMainCnt) {
			edgeSum += Dt[iMainCnt][iSubCnt];
		}
		edge = edgeSum / (halfNum + 1);
		for (iPos = 0; iPos < halfNum; ++iPos) {
			pfOrgDt[iPos] = edge;
		}

		for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
			pfOrgDt[iPos] = Dt[iMainCnt][iSubCnt];
			++iPos;
		}

		edgeSum = Dt[MainPixel - halfNum - 1][iSubCnt];     //端部データの平均値計算
		for (iMainCnt = MainPixel - halfNum; iMainCnt < MainPixel; ++iMainCnt) {
			edgeSum += Dt[iMainCnt][iSubCnt];
		}
		edge = edgeSum / (halfNum + 1);
		for (iMainCnt = 0; iMainCnt < halfNum; ++iMainCnt) {
			pfOrgDt[iPos] = edge;
			++iPos;
		}
//end of data copy
//移動平均
		sum = pfOrgDt[0];
		for (iMainCnt = 1; iMainCnt < aveNum; ++iMainCnt) {
			sum += pfOrgDt[iMainCnt];
		}
		iSubPos = 0;
		iAddPos = aveNum;
		Dt[0][iSubCnt] = sum / aveNum;
		for (iMainCnt = 1; iMainCnt < MainPixel; ++iMainCnt) {
			sum += pfOrgDt[iAddPos] - pfOrgDt[iSubPos];
			Dt[iMainCnt][iSubCnt] = sum / aveNum;
			++iAddPos;
			++iSubPos;
		}
	}
	delete[] pfOrgDt;
}
//---------------------------------------------------------------------------
/*
 副方向Median filter処理
 両端filter長1/2のdataは処理しない。
 filter長>画素数の時何もしない。

 引数	ilength -> filter長
 */
/*
 void TFIMG::SubMedian(const int ilength)
 {
 int iXCnt, iYCnt, iCnt;
 int *piOrder;       //順番
 float *pfOriginal;        //original data
 int iOrder;     //追加dataの順位を示すcounter
 int iIncludePos, iRemovePos;        //data追加位置と除去位置
 int iCrtPos;        //補正位置
 int iEndOrder = ilength - 1;
 int iMedian;

 if(ilength > SubPixel) return;

 piOrder = new int[ilength];
 pfOriginal = new float[SubPixel];

 for(iXCnt = 0 ; iXCnt < MainPixel ; ++iXCnt){
 for(iYCnt = 0 ; iYCnt < SubPixel ; ++iYCnt){
 pfOriginal[iYCnt] = Dt[iXCnt][iYCnt];
 }

 iMedian = 0;
 iCrtPos = 0;
 piOrder[0] = 0;
 iIncludePos = 1;
 while(iIncludePos < ilength){
 iOrder = iIncludePos;
 for(iCnt = 0; iCnt < iIncludePos; ++iCnt){
 if(pfOriginal[iCnt] > pfOriginal[iIncludePos]){
 ++piOrder[iCnt];
 --iOrder;
 }
 }
 piOrder[iIncludePos] = iOrder;
 ++iIncludePos;
 iOrder = iIncludePos;
 for(iCnt = 0; iCnt < iIncludePos; ++iCnt){
 if(pfOriginal[iCnt] > pfOriginal[iIncludePos]){
 ++piOrder[iCnt];
 --iOrder;
 }
 }
 ++iMedian;
 ++iCrtPos;
 piOrder[iIncludePos] = iOrder;
 for(iCnt = 0; iCnt <= iIncludePos; ++iCnt){
 if(piOrder[iCnt] == iMedian){
 Dt[iXCnt][iCrtPos] = pfOriginal[iCnt];
 break;
 }
 }
 ++iIncludePos;
 }

 iRemovePos = 0;
 for(; iIncludePos < SubPixel ; ++iIncludePos){
 iOrder = iEndOrder;
 if(piOrder[0] == iMedian) Dt[iXCnt][iCrtPos] = pfOriginal[iRemovePos];
 for(iCnt = 1 ; iCnt < ilength ; ++iCnt){
 if(piOrder[iCnt] == iMedian) Dt[iXCnt][iCrtPos] = pfOriginal[iRemovePos + iCnt];
 if(pfOriginal[iRemovePos + iCnt] >= pfOriginal[iRemovePos]){
 piOrder[iCnt - 1] = piOrder[iCnt] - 1;
 }else{
 piOrder[iCnt - 1] = piOrder[iCnt];
 }
 if(pfOriginal[iRemovePos + iCnt] > pfOriginal[iIncludePos]){
 ++piOrder[iCnt - 1];
 --iOrder;
 }
 }
 piOrder[iEndOrder] = iOrder;
 ++iRemovePos;
 ++iCrtPos;
 }
 while(iRemovePos < SubPixel - 1){
 if(piOrder[0] == iMedian) Dt[iXCnt][iCrtPos] = pfOriginal[iRemovePos];
 for(iCnt = iRemovePos + 1; iCnt < SubPixel; ++iCnt){
 if(piOrder[iCnt - iRemovePos] == iMedian) Dt[iXCnt][iCrtPos] =
 pfOriginal[iCnt];
 if(pfOriginal[iCnt] >= pfOriginal[iRemovePos]){
 piOrder[iCnt - iRemovePos - 1] = piOrder[iCnt - iRemovePos] - 1;
 }else{
 piOrder[iCnt - iRemovePos - 1] = piOrder[iCnt - iRemovePos];
 }
 }
 ++iRemovePos;
 for(iCnt = iRemovePos + 1; iCnt < SubPixel; ++iCnt){
 if(pfOriginal[iCnt] >= pfOriginal[iRemovePos]){
 piOrder[iCnt - iRemovePos - 1] = piOrder[iCnt - iRemovePos] - 1;
 }else{
 piOrder[iCnt - iRemovePos - 1] = piOrder[iCnt - iRemovePos];
 }
 }
 ++iRemovePos;
 ++iCrtPos;
 --iMedian;
 }
 }
 delete[] piOrder;
 delete[] pfOriginal;
 }
 */
//---------------------------------------------------------------------------
/*
 副方向Median filter処理
 両端は1, 3,,,iNumまで広げながら処理する。
 filter長>画素数の時何もしない。

 引数	iNum -> filter長
 */
void TFIMG::SubMedian(const int iNum) {
	if (iNum > SubPixel || iNum % 2 == 0) {
		return;
	}

	int iMCnt, iSCnt;
	int iCnt, iOrdCnt;
	int iOrder;     //除くdataのpnOrder配列の位置
	int iPos;
	int iEnd;		//pnOrder配列の有効数
	int iRemPos;
	int iAddPos;
	int iMed;
	float *pfOrg, *pfOrder;

	pfOrg = new float[SubPixel];
	pfOrder = new float[iNum];

	for (iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
		iPos = 1;
		iEnd = 0;
		iRemPos = 0;
		iAddPos = 1;
		iMed = 0;
		for (iSCnt = 0; iSCnt < SubPixel; ++iSCnt) {
			pfOrg[iSCnt] = Dt[iMCnt][iSCnt];
		}

		pfOrder[0] = pfOrg[0];
		while (iAddPos < iNum) {
			iOrdCnt = iEnd;
			while (iOrdCnt >= 0 && pfOrder[iOrdCnt] > pfOrg[iAddPos]) {
				pfOrder[iOrdCnt + 1] = pfOrder[iOrdCnt];
				--iOrdCnt;
			}
			//pfOrder[iOrdCnt] <= pfOrg[iAddPos]
			pfOrder[iOrdCnt + 1] = pfOrg[iAddPos];
			++iAddPos;
			++iEnd;

			iOrdCnt = iEnd;
			while (iOrdCnt >= 0 && pfOrder[iOrdCnt] > pfOrg[iAddPos]) {
				pfOrder[iOrdCnt + 1] = pfOrder[iOrdCnt];
				--iOrdCnt;
			}
			//pfOrder[iOrdCnt] <= pfOrg[iAddPos]
			pfOrder[iOrdCnt + 1] = pfOrg[iAddPos];
			++iAddPos;
			++iEnd;

			++iMed;
			Dt[iMCnt][iPos] = pfOrder[iMed];
			++iPos;
		}

		while (iAddPos < SubPixel) {
			iOrder = iFindPos(pfOrg[iRemPos], pfOrder, 0, iEnd);
			/*
			 iOrder = iEnd;
			 while(pfOrder[iOrder] != pfOrg[iRemPos]){
			 --iOrder;
			 }
			 */
			++iRemPos;

			if (pfOrg[iAddPos] >= pfOrder[iOrder]) {
				++iOrder;
				while (iOrder <= iEnd && pfOrder[iOrder] < pfOrg[iAddPos]) {
					pfOrder[iOrder - 1] = pfOrder[iOrder];
					++iOrder;
				}
				pfOrder[iOrder - 1] = pfOrg[iAddPos];
			} else {
				--iOrder;
				while (iOrder >= 0 && pfOrder[iOrder] > pfOrg[iAddPos]) {
					pfOrder[iOrder + 1] = pfOrder[iOrder];
					--iOrder;
				}
				pfOrder[iOrder + 1] = pfOrg[iAddPos];
			}
			++iAddPos;

			Dt[iMCnt][iPos] = pfOrder[iMed];
			++iPos;
		}

		while (iRemPos < SubPixel - 2) {
			iOrder = iFindPos(pfOrg[iRemPos], pfOrder, 0, iEnd);
			for (iCnt = iOrder; iCnt < iEnd; ++iCnt) {
				pfOrder[iCnt] = pfOrder[iCnt + 1];
			}
			++iRemPos;
			--iEnd;

			iOrder = iFindPos(pfOrg[iRemPos], pfOrder, 0, iEnd);
			for (iCnt = iOrder; iCnt < iEnd; ++iCnt) {
				pfOrder[iCnt] = pfOrder[iCnt + 1];
			}
			++iRemPos;
			--iEnd;

			--iMed;
			Dt[iMCnt][iPos] = pfOrder[iMed];
			++iPos;
		}
	}
	delete[] pfOrg;
	delete[] pfOrder;
}

//---------------------------------------------------------------------------
/*
 副方向の移動平均
 Averaging個数が偶数の時前半dataを多くする。

 引数	aveNum -> Averaging個数
 */
void TFIMG::SubMoveAve(const int iLength) {
	int iMainCnt, iSubCnt;
	int iSubPos, iAddPos;
	int iHalfNum = iLength / 2;        //移動平均個数の半分
	float fLength = iLength;
	float fDtSum;
	int iNum;
	int iNormalEndPos = SubPixel - iHalfNum;
	float *pfOrgDt;

	pfOrgDt = new float[SubPixel];
	for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
		for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
			pfOrgDt[iSubCnt] = Dt[iMainCnt][iSubCnt];
		}
		iAddPos = 1;
		fDtSum = Dt[iMainCnt][0];
		for (iSubCnt = 1; iSubCnt <= iHalfNum; ++iSubCnt) {
			fDtSum = fDtSum + pfOrgDt[iAddPos] + pfOrgDt[iAddPos + 1];
			iAddPos += 2;
			Dt[iMainCnt][iSubCnt] = fDtSum / iAddPos;
		}
		iSubPos = 0;
		for (iSubCnt = iHalfNum + 1; iSubCnt < iNormalEndPos; ++iSubCnt) {
			fDtSum = fDtSum - pfOrgDt[iSubPos] + pfOrgDt[iAddPos];
			Dt[iMainCnt][iSubCnt] = fDtSum / fLength;
			++iSubPos;
			++iAddPos;
		}
		iNum = iLength - 2;
		for (iSubCnt = iNormalEndPos; iSubCnt < SubPixel; ++iSubCnt) {
			fDtSum = fDtSum - pfOrgDt[iSubPos] - pfOrgDt[iSubPos + 1];
			Dt[iMainCnt][iSubCnt] = fDtSum / iNum;
			iSubPos += 2;
			iNum -= 2;
		}
	}
	delete[] pfOrgDt;
}
//---------------------------------------------------------------------------
/*
 副方向の移動平均
 Averaging個数が偶数の時前半dataを多くする。

 引数	aveNum -> Averaging個数
 */
bool TFIMG::SubExpandMultiAve(const string fileName) {
	int iCnt;
	int iFCnt;
	int iMainCnt, iSubCnt;
	int halfNum;        //移動平均個数の半分
	double sum;     //移動平均値を出すための合計
	double edgeSum;     //移動平均するとき、端部データの平均値を用いる。平均値計算用合計
	float edge;     //移動平均するとき、端部データの平均値を用いる。その平均値
	float *pfOrgDt;
	int iPos;
	fstream file;
	int iFNum;      //移動平均段数
	int *piLen;     //移動平均長さ
	float *pfFactor;      //移動平均重み
	float fReg;       //移動平均規格化値
	int *piAddPos, *piSubPos;       //移動平均加算・減算位置

	file.open(fileName.c_str(), ios::in);
	if (file.fail())
		return (false);
	file >> iFNum;
	piLen = new int[iFNum];
	pfFactor = new float[iFNum];
	piAddPos = new int[iFNum];
	piSubPos = new int[iFNum];

	fReg = 0.0;
	for (iCnt = 0; iCnt < iFNum; ++iCnt) {
		file >> piLen[iCnt];
		file >> pfFactor[iCnt];
		fReg += pfFactor[iCnt] * piLen[iCnt];
	}
	file.close();

	//多段フィルターの最も長いフィルター長が1（フィルターかけないことと同義）ならば何もせず終了（ttsuji追加）
	if (1 == piLen[0]) {
		return true;
	}

	halfNum = piLen[0] / 2;
	if (halfNum > SubPixel) {
		return (false);
	}
	pfOrgDt = new float[SubPixel + piLen[0]];

	for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {

//Trend除去する画像のcopy  端部データの平均値を左右に付け加える
		edgeSum = Dt[iMainCnt][0];     //端部データの平均値計算
		for (iSubCnt = 1; iSubCnt < halfNum; ++iSubCnt) {
			edgeSum += Dt[iMainCnt][iSubCnt];
		}
		edge = edgeSum / halfNum;
		for (iPos = 0; iPos < halfNum; ++iPos) {
			pfOrgDt[iPos] = edge;
		}

		for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
			pfOrgDt[iPos] = Dt[iMainCnt][iSubCnt];
			++iPos;
		}

		edgeSum = Dt[iMainCnt][SubPixel - halfNum];     //端部データの平均値計算
		for (iSubCnt = SubPixel - halfNum + 1; iSubCnt < SubPixel; ++iSubCnt) {
			edgeSum += Dt[iMainCnt][iSubCnt];
		}
		edge = edgeSum / halfNum;
		for (iSubCnt = 0; iSubCnt < halfNum; ++iSubCnt) {
			pfOrgDt[iPos] = edge;
			++iPos;
		}
//end of data copy
//移動平均
		for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {        //加減算位置計算
			piAddPos[iFCnt] = halfNum + piLen[iFCnt] / 2 + 1;
			piSubPos[iFCnt] = halfNum - piLen[iFCnt] / 2;
		}
		sum = 0.0;
		for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {
			for (iCnt = piSubPos[iFCnt]; iCnt < piAddPos[iFCnt]; ++iCnt) {
				sum += pfFactor[iFCnt] * pfOrgDt[iCnt];
			}
		}
		Dt[iMainCnt][0] = sum / fReg;
		for (iSubCnt = 1; iSubCnt < SubPixel; ++iSubCnt) {
			for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {
				sum += pfFactor[iFCnt]
						* (pfOrgDt[piAddPos[iFCnt]] - pfOrgDt[piSubPos[iFCnt]]);
				++piAddPos[iFCnt];
				++piSubPos[iFCnt];
			}
			Dt[iMainCnt][iSubCnt] = sum / fReg;
		}
	}
	delete[] pfOrgDt;
	delete[] piLen;
	delete[] pfFactor;
	delete[] piAddPos;
	delete[] piSubPos;

	return (true);
}
//---------------------------------------------------------------------------
/*
 福与さん作成(DEVO3 FPGA G面補正における横傘コンペアのため)

 副方向の移動平均
 Averaging個数が偶数の時前半dataを多くする。

 引数	aveNum -> Averaging個数
 */
bool TFIMG::SubExpandMultiAveD3(const string fileName) {
	int iCnt;
	int iFCnt;
	int iMainCnt, iSubCnt;
	int halfNum;        //移動平均個数の半分
	double sum;     //移動平均値を出すための合計
	double edgeSum;     //移動平均するとき、端部データの平均値を用いる。平均値計算用合計
	float edge;     //移動平均するとき、端部データの平均値を用いる。その平均値
	float *pfOrgDt;
	int iPos;
	fstream file;
	int iFNum;      //移動平均段数
	int *piLen;     //移動平均長さ
	float *pfFactor;      //移動平均重み
	float fReg;       //移動平均規格化値
	int *piAddPos, *piSubPos;       //移動平均加算・減算位置

	file.open(fileName.c_str(), ios::in);
	if (file.fail())
		return (false);
	file >> iFNum;
	piLen = new int[iFNum];
	pfFactor = new float[iFNum];
	piAddPos = new int[iFNum];
	piSubPos = new int[iFNum];

	fReg = 0.0;
	for (iCnt = 0; iCnt < iFNum; ++iCnt) {
		file >> piLen[iCnt];
		file >> pfFactor[iCnt];
		fReg += pfFactor[iCnt] * piLen[iCnt];
	}
	file.close();

	//多段フィルターの最も長いフィルター長が1（フィルターかけないことと同義）ならば何もせず終了（ttsuji追加）
	if (1 == piLen[0]) {
		return true;
	}

	halfNum = piLen[0] / 2;
	if (halfNum > SubPixel) {
		return (false);
	}
	pfOrgDt = new float[SubPixel + piLen[0]];

	for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {

//Trend除去する画像のcopy  端部データの平均値を左右に付け加える
		edgeSum = Dt[iMainCnt][0];     //端部データの平均値計算
		for (iSubCnt = 1; iSubCnt < halfNum; ++iSubCnt) {
			edgeSum += Dt[iMainCnt][iSubCnt];
		}
		if (halfNum == 19) {
			edge = int(
					(edgeSum + 9 + 32768.0f * 19) * 13797.0f / (65536.0f * 4))
					- 32768.0f;
		} else {
			edge = int((edgeSum + (halfNum / 2)) / halfNum);
		}
		for (iPos = 0; iPos < halfNum; ++iPos) {
			pfOrgDt[iPos] = edge;
		}

		for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
			pfOrgDt[iPos] = Dt[iMainCnt][iSubCnt];
			++iPos;
		}

		edgeSum = Dt[iMainCnt][SubPixel - halfNum];     //端部データの平均値計算
		for (iSubCnt = SubPixel - halfNum + 1; iSubCnt < SubPixel; ++iSubCnt) {
			edgeSum += Dt[iMainCnt][iSubCnt];
		}
		if (halfNum == 19) {
			edge = int(
					(edgeSum + 9 + 32768.0f * 19) * 13797.0f / (65536.0f * 4))
					- 32768.0f;
		} else {
			edge = int((edgeSum + (halfNum / 2)) / halfNum);
		}
		for (iSubCnt = 0; iSubCnt < halfNum; ++iSubCnt) {
			pfOrgDt[iPos] = edge;
			++iPos;
		}
//end of data copy
//移動平均
		for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {        //加減算位置計算
			piAddPos[iFCnt] = halfNum + piLen[iFCnt] / 2 + 1;
			piSubPos[iFCnt] = halfNum - piLen[iFCnt] / 2;
		}
		sum = 0.0;
		for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {
			for (iCnt = piSubPos[iFCnt]; iCnt < piAddPos[iFCnt]; ++iCnt) {
				sum += pfFactor[iFCnt] * pfOrgDt[iCnt];
			}
		}
		Dt[iMainCnt][0] = sum / fReg;
		for (iSubCnt = 1; iSubCnt < SubPixel; ++iSubCnt) {
			for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {
				sum += pfFactor[iFCnt]
						* (pfOrgDt[piAddPos[iFCnt]] - pfOrgDt[piSubPos[iFCnt]]);
				++piAddPos[iFCnt];
				++piSubPos[iFCnt];
			}
			Dt[iMainCnt][iSubCnt] = sum / fReg;
		}
	}
	delete[] pfOrgDt;
	delete[] piLen;
	delete[] pfFactor;
	delete[] piAddPos;
	delete[] piSubPos;

	return (true);
}
//---------------------------------------------------------------------------
/*
 void TFIMG::SubMoveAve(const int aveNum)
 {
 int iMainCnt, iSubCnt;
 int halfNum;        //移動平均個数の半分
 double sum;     //移動平均値を出すための合計
 double edgeSum;     //移動平均するとき、端部データの平均値を用いる。平均値計算用合計
 float edge;     //移動平均するとき、端部データの平均値を用いる。その平均値
 TFIMG copy;      //画像のcopy
 //    float residual;

 halfNum = aveNum / 2;

 //Trend除去する画像のcopy  端部データの平均値を上下に付け加える
 copy.NewDt(MainPixel, SubPixel + aveNum);
 for(iMainCnt = 0 ; iMainCnt < MainPixel ; ++iMainCnt){
 edgeSum = 0;     //端部データの平均値計算
 for(iSubCnt = 0 ; iSubCnt < halfNum ; ++iSubCnt){
 edgeSum += Dt[iMainCnt][iSubCnt];
 }
 edge = edgeSum / halfNum;
 for(iSubCnt = 0 ; iSubCnt < halfNum ; ++iSubCnt){
 copy.Dt[iMainCnt][iSubCnt] = edge;
 }

 for(iSubCnt = 0 ; iSubCnt < SubPixel ; ++iSubCnt){
 copy.Dt[iMainCnt][iSubCnt + halfNum] = Dt[iMainCnt][iSubCnt];
 }

 edgeSum = 0;     //端部データの平均値計算
 for(iSubCnt = SubPixel - halfNum ; iSubCnt < SubPixel ; ++iSubCnt){
 edgeSum += Dt[iMainCnt][iSubCnt];
 }
 edge = edgeSum / halfNum;
 for(iSubCnt = 0 ; iSubCnt < halfNum ; ++iSubCnt){
 copy.Dt[iMainCnt][SubPixel + halfNum + iSubCnt] = edge;
 }
 }

 //移動平均
 for(iMainCnt = 0 ; iMainCnt < MainPixel ; ++iMainCnt){
 sum = 0.0;
 for(iSubCnt = 0 ; iSubCnt <  aveNum ; ++iSubCnt){
 sum += copy.Dt[iMainCnt][iSubCnt];
 }
 Dt[iMainCnt][0] = sum / aveNum;
 //        residual = sum / aveNum - Dt[0][i];
 //        if((residual > float(rand()) / float(RAND_MAX)) && (Dt[0][i] < DtMax)) ++Dt[0][i];
 for(iSubCnt = 1 ; iSubCnt < SubPixel ; ++iSubCnt){
 sum = sum - copy.Dt[iMainCnt][iSubCnt - 1] +  copy.Dt[iMainCnt][iSubCnt + aveNum - 1];
 Dt[iMainCnt][iSubCnt] = sum / aveNum;
 //            residual = sum / aveNum - Dt[j][i];
 //            if((residual > float(rand()) / float(RAND_MAX)) && (Dt[j][i] < DtMax)) ++Dt[j][i];
 }
 }
 }
 */
//---------------------------------------------------------------------------
/*
 Read Data
 引数	File Name

 戻り値	成功->true
 失敗->false
 kuwa
 */
bool TFIMG::ReadDt(const string fileName, int mainp, int subp) {
	int handle;		//file handler
	int i;

	MainPixel = mainp;
	SubPixel = subp;

	NewDt(MainPixel, SubPixel);

	FILE *fp = fopen(fileName.c_str(), "rb");
	if (!fp) {
//		Application->MessageBox((fileName + " openできませんでした").c_str(), "Error", MB_OK);
		return false;
	}

	//例外処理とする
	try {
		for (i = 0; i < MainPixel; ++i) {
			size_t size = fread(Dt[i], sizeof(float), SubPixel, fp);
			if ((int) size != SubPixel) {
				throw i;
			}
		}
	} catch (int) {
//			Application->MessageBox((fileName + "\n" + IntToStr(i) + "行目をreadできませんでした").c_str(), "例外発生", MB_OK);
		fclose(fp);
		return false;
	}

	fclose(fp);

	return (true);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*
 Read Data 8byte読み込み
 引数	File Name

 戻り値	成功->true
 失敗->false
 kuwa
 */
bool TFIMG::ReadDoubleDt(const string fileName, int mainp, int subp) {
	int handle;		//file handler
	int i;

	MainPixel = mainp;
	SubPixel = subp;

	NewDt(MainPixel, SubPixel);

	// DtとdDtはMain,Sub逆に定義
	double **dDt = new double*[SubPixel];
	for (int i = 0; i < SubPixel; i++)
		dDt[i] = new double[MainPixel];

	FILE *fp = fopen(fileName.c_str(), "rb");
	if (!fp) {
//		Application->MessageBox((fileName + " openできませんでした").c_str(), "Error", MB_OK);
		return false;
	}

	//例外処理とする
	try {
		for (i = 0; i < MainPixel; ++i) {
			size_t size = fread(dDt[i], sizeof(double), SubPixel, fp);
			if ((int) size != SubPixel) {
				throw i;
			}
		}
	} catch (int) {
//			Application->MessageBox((fileName + "\n" + IntToStr(i) + "行目をreadできませんでした").c_str(), "例外発生", MB_OK);
		fclose(fp);
		return false;
	}

	fclose(fp);

	for (int iM = 0; iM < MainPixel; iM++) {
		for (int iS = 0; iS < SubPixel; iS++) {
			Dt[iM][iS] = dDt[iS][iM];

		}
	}

	for (int i = 0; i < MainPixel; ++i) {
		delete[] dDt[i];
	}
	delete[] dDt;

	return (true);

}
//---------------------------------------------------------------------------
bool TFIMG::Save(const string fileName) {
#if 0				//TODO とりあえずコメントアウト
    const int iHEADDER_SIZE = 10;
    int handle;
    char cHead[iHEADDER_SIZE + 1];
    char    cSSize[6];
    string fn;		//fileName
    TCommon Common;

    fn = ChangeFileExt(fileName, ".fhd");
	if(Common.OverWrite(fn) == false) return(false);

//Hedder File出力
	sprintf(cHead, "%05d", MainPixel);
    sprintf(cSSize, "%05d", SubPixel);
    strcat(cHead, cSSize);

    if ((handle = open(fn.c_str(), O_RDWR|O_CREAT|O_BINARY|O_TRUNC, S_IWRITE)) == -1){
//    	Application->MessageBox((fn + " openできませんでした").c_str(), "", MB_OK);
    	return(false);
    }
    if ((write(handle, cHead, iHEADDER_SIZE)) == -1){
//    	Application->MessageBox((fn + " writeできませんでした").c_str(), "", MB_OK);
    	close(handle);
        return(false);
    }
    close(handle);


//画像data File出力
    fn = ChangeFileExt(fileName, ".fdt");
	if(Common.OverWrite(fn) == false) return(false);
    if(SaveDt(fn) == false) return(false);
#endif
	return (true);
}
//---------------------------------------------------------------------------
bool TFIMG::SaveDt(const string fileName) {
#if 0				//TODO とりあえずコメントアウト
    int handle;
    int iMCnt;

//画像data File出力
    if ((handle = open(fileName.c_str(),
            O_RDWR|O_CREAT|O_BINARY|O_TRUNC, S_IWRITE)) == -1){
//    	Application->MessageBox((fileName + " openできませんでした").c_str(), "",
//                MB_OK);
    	return(false);
    }
	for(iMCnt = 0 ; iMCnt < MainPixel ; ++iMCnt){
    	if ((write(handle, Dt[iMCnt], MainPixel * 4)) == -1){
//    		Application->MessageBox(
//                    (fileName + " writeできませんでした").c_str(), "", MB_OK);
    		close(handle);
        	return(false);
        }
    }
    close(handle);
#endif

	return (true);
}
//---------------------------------------------------------------------------
bool TFIMG::SaveDoubleDt(const string fileName) {
#if 0						//TODO とりあえずコメントアウト
	int handle;
	int iMCnt;

	// dDtとDtは主副逆
	double **dDt = new double*[SubPixel];
	for (int i = 0 ; i < SubPixel ; i++)
		dDt[i] = new double[MainPixel];

	for(int iM=0; iM<MainPixel; iM++){
	  for(int iS=0; iS<SubPixel; iS++){
		dDt[iS][iM] = Dt[iM][iS];

	  }
	}

	//画像data File出力
	if ((handle = open(fileName.c_str(),
			O_RDWR|O_CREAT|O_BINARY|O_TRUNC, S_IWRITE)) == -1){
		Application->MessageBox((fileName + " openできませんでした").c_str(), "",
				MB_OK);
		return(false);
	}
	for(iMCnt = 0 ; iMCnt < MainPixel ; ++iMCnt){
		if ((write(handle, dDt[iMCnt], SubPixel * sizeof(double))) == -1){  // kuwa
			Application->MessageBox(
					(fileName + " writeできませんでした").c_str(), "", MB_OK);
			close(handle);
			return(false);
		}
	}
	close(handle);

	for(int i = 0 ; i < MainPixel ; ++i){
	  delete[] dDt[i];
	}
	delete[] dDt;

#endif

	return (true);

}
//---------------------------------------------------------------------------
//Takeshi Kuwabara

void TFIMG::WriteTma(string str, const double dFct, const unsigned short unOff,
		bool b = true) {
#if 0				//TODO
	TTMA tmp(SubPixel, MainPixel, 16);
	for (int i = 0; i < SubPixel; i++){
		for (int j = 0; j < MainPixel; j++){
			tmp.SetVal(i, j, round(Dt[j][i] * dFct + unOff));
		}
	}
	tmp.WriteTma(str, b); // kuwa
#endif
}
//---------------------------------------------------------------------------
//
void TFIMG::SaveTmaTdt(string str, const double dFct,
		const unsigned short unOff) {
#if 0				//TODO
	TTMA tmp(SubPixel, MainPixel, 16);
	for (int i = 0; i < SubPixel; i++){
		for (int j = 0; j < MainPixel; j++){
			tmp.SetVal(i, j, round(Dt[j][i] * dFct + unOff));
		}
	}
	tmp.SaveTmaTdt(str);
#endif
}
//---------------------------------------------------------------------------
/*
 副方向の移動平均
 両端はそれぞれの引数の平均値を延長する。

 引数	iLength -> Averaging個数
 */
void TFIMG::SubAve(const int iStart, const int iEnd) {
	int iMainCnt, iSubCnt;
	int iSubPos, iAddPos;
	float fLength = iStart + iEnd + 1;
	float fDtSum;
	float *pfOrgDt;
	float fAve;

	pfOrgDt = new float[SubPixel + iEnd];
	for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
		for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
			pfOrgDt[iSubCnt] = Dt[iMainCnt][iSubCnt];
		}

		fDtSum = pfOrgDt[SubPixel - 1];
		for (iSubCnt = 1; iSubCnt < iStart; ++iSubCnt) {
			fDtSum += pfOrgDt[SubPixel - 1 - iSubCnt];
		}
		fAve = fDtSum / iEnd;
		for (iSubCnt = 0; iSubCnt < iStart; ++iSubCnt) {
			pfOrgDt[iSubCnt + SubPixel] = fAve;
		}

		fDtSum = pfOrgDt[0];
		for (iSubCnt = 1; iSubCnt < iEnd; ++iSubCnt) {
			fDtSum += pfOrgDt[iSubCnt];
		}
		fAve = fDtSum / iEnd;
		for (iSubCnt = 0; iSubCnt < iStart - 1; ++iSubCnt) {
			fDtSum += pfOrgDt[iSubCnt];
		}
		fDtSum += fAve;
		iAddPos = iStart;
		for (iSubCnt = 0; iSubCnt <= iEnd; ++iSubCnt) {
			fDtSum += (pfOrgDt[iAddPos] - fAve);
			Dt[iMainCnt][iSubCnt] = fDtSum / fLength;
			++iAddPos;
		}

		iSubPos = 0;
		for (iSubCnt = iEnd + 1; iSubCnt < SubPixel; ++iSubCnt) {
			fDtSum += (pfOrgDt[iAddPos] - pfOrgDt[iSubPos]);
			Dt[iMainCnt][iSubCnt] = fDtSum / fLength;
			++iSubPos;
			++iAddPos;
		}
	}
	delete[] pfOrgDt;
}
//---------------------------------------------------------------------------
/*
 副方向のFilter処理
 端部は平均値で拡大して処理
 */
void TFIMG::SubExpandFilter(const float fFilter[], const int iLength) {
	int iHalfNum = iLength / 2;
	int iMCnt, iSCnt;
	float fDtSum;
	float fAve;
	float *pfOrgDt;
	int iPos;
	int iDtCnt;

	pfOrgDt = new float[SubPixel + 2 * iHalfNum];
	for (iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
		//平均値で拡大
		fDtSum = 0.0;
		for (iDtCnt = 0; iDtCnt <= iHalfNum; ++iDtCnt) {
			fDtSum += Dt[iMCnt][iDtCnt];
		}
		fAve = fDtSum / (iHalfNum + 1);

		iPos = 0;
		for (iDtCnt = 0; iDtCnt < iHalfNum; ++iDtCnt) {
			pfOrgDt[iPos] = fAve;
			++iPos;
		}

		//そのまま
		for (iSCnt = 0; iSCnt < SubPixel; ++iSCnt) {
			pfOrgDt[iPos] = Dt[iMCnt][iSCnt];
			++iPos;
		}

		//平均値で拡大
		fDtSum = 0.0;
		for (iDtCnt = 0; iDtCnt <= iHalfNum; ++iDtCnt) {
			fDtSum += Dt[iMCnt][SubPixel - 1 - iDtCnt];
		}
		fAve = fDtSum / (iHalfNum + 1);

		for (iDtCnt = 0; iDtCnt < iHalfNum; ++iDtCnt) {
			pfOrgDt[iPos] = fAve;
			++iPos;
		}
		//filter処理
		iPos = 0;
		for (iSCnt = 0; iSCnt < SubPixel; ++iSCnt) {
			fDtSum = 0.0;
			for (iDtCnt = 0; iDtCnt < iLength; ++iDtCnt) {
				fDtSum += fFilter[iDtCnt] * pfOrgDt[iPos + iDtCnt];
			}
			Dt[iMCnt][iSCnt] = fDtSum;
			++iPos;
		}
#if 0
		if (2 == iMCnt) {
			int a = 1;
		}
/*
		if (1200 == iMCnt) {
			int a = 1;
		}
		if (1400 == iMCnt) {
			int a = 1;
		}
		if (1600 == iMCnt) {
			int a = 1;
		}
		if (2000 == iMCnt) {
			int a = 1;
		}
*/
#endif
	}
	delete[] pfOrgDt;
}
//---------------------------------------------------------------------------
/*
 副方向の移動平均
 Averaging個数が偶数の時前半dataを多くする。

 引数	aveNum -> Averaging個数
 */
void TFIMG::SubExpandMoveAve(const int aveNum) {
	int iMainCnt, iSubCnt;
	int halfNum;        //移動平均個数の半分
	double sum;     //移動平均値を出すための合計
	double edgeSum;     //移動平均するとき、端部データの平均値を用いる。平均値計算用合計
	float edge;     //移動平均するとき、端部データの平均値を用いる。その平均値
	float *pfOrgDt;
	int iPos, iAddPos, iSubPos;

	pfOrgDt = new float[SubPixel + aveNum];
	halfNum = aveNum / 2;

	for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {

//Trend除去する画像のcopy  端部データの平均値を左右に付け加える
		edgeSum = Dt[iMainCnt][0];     //端部データの平均値計算
		for (iSubCnt = 1; iSubCnt <= halfNum; ++iSubCnt) {
			edgeSum += Dt[iMainCnt][iSubCnt];
		}
		edge = edgeSum / (halfNum + 1);
		for (iPos = 0; iPos < halfNum; ++iPos) {
			pfOrgDt[iPos] = edge;
		}

		for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
			pfOrgDt[iPos] = Dt[iMainCnt][iSubCnt];
			++iPos;
		}

		edgeSum = Dt[iMainCnt][SubPixel - halfNum - 1];     //端部データの平均値計算
		for (iSubCnt = SubPixel - halfNum; iSubCnt < SubPixel; ++iSubCnt) {
			edgeSum += Dt[iMainCnt][iSubCnt];
		}
		edge = edgeSum / (halfNum + 1);
		for (iSubCnt = 0; iSubCnt < halfNum; ++iSubCnt) {
			pfOrgDt[iPos] = edge;
			++iPos;
		}
//end of data copy
//移動平均
		sum = pfOrgDt[0];
		for (iSubCnt = 1; iSubCnt < aveNum; ++iSubCnt) {
			sum += pfOrgDt[iSubCnt];
		}
		iSubPos = 0;
		iAddPos = aveNum;
		Dt[iMainCnt][0] = sum / aveNum;
		for (iSubCnt = 1; iSubCnt < SubPixel; ++iSubCnt) {
			sum += pfOrgDt[iAddPos] - pfOrgDt[iSubPos];
			Dt[iMainCnt][iSubCnt] = sum / aveNum;
			++iAddPos;
			++iSubPos;
		}
	}
	delete[] pfOrgDt;
}
//---------------------------------------------------------------------------
/*
 副方向の移動平均
 Averaging個数が偶数の時前半dataを多くする。

 引数	aveNum -> Averaging個数
 */
void TFIMG::SubMoveAveAveExpand(const int aveNum) {
	int halfNum = aveNum / 2;        //移動平均個数の半分
	TFIMG copy(MainPixel, SubPixel + aveNum);      //画像のcopy
//    float residual;

//Trend除去する画像のcopy  端部データの平均値を上下に付け加える
	for (int iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
		float edgeSum = 0.0f;     //端部データの平均値計算
		for (int iSCnt = 0; iSCnt <= halfNum; ++iSCnt) {
			edgeSum += Dt[iMCnt][iSCnt];
		}
		float edge = edgeSum / (halfNum + 1);
		for (int iSCnt = 0; iSCnt < halfNum; ++iSCnt) {
			copy.Dt[iMCnt][iSCnt] = edge;
		}

		for (int iSCnt = 0, iSCnt2 = halfNum; iSCnt < SubPixel;
				++iSCnt, ++iSCnt2) {
			copy.Dt[iMCnt][iSCnt2] = Dt[iMCnt][iSCnt];
		}

		edgeSum = 0.0f;     //端部データの平均値計算
		for (int iSCnt = SubPixel - halfNum - 1; iSCnt < SubPixel; ++iSCnt) {
			edgeSum += Dt[iMCnt][iSCnt];
		}
		edge = edgeSum / (halfNum + 1);
		for (int iSCnt = 0, iSCnt2 = SubPixel + halfNum; iSCnt < halfNum;
				++iSCnt, ++iSCnt2) {
			copy.Dt[iMCnt][iSCnt2] = edge;
		}
	}

//移動平均
	for (int iMCnt = 0; iMCnt < MainPixel; ++iMCnt) {
		float sum = copy.Dt[iMCnt][0];
		for (int iSCnt = 1; iSCnt < aveNum; ++iSCnt) {
			sum += copy.Dt[iMCnt][iSCnt];
		}
		Dt[iMCnt][0] = sum / aveNum;
		for (int iSCnt = 1, iSCntM = 0, iSCntP = aveNum; iSCnt < SubPixel;
				++iSCnt, ++iSCntM, ++iSCntP) {
			sum = sum - copy.Dt[iMCnt][iSCntM] + copy.Dt[iMCnt][iSCntP];
			Dt[iMCnt][iSCnt] = sum / aveNum;
		}
	}
}
//---------------------------------------------------------------------------
/*
 filter処理

 引数	fFilter -> filterの配列
 iLength  -> filter長
 */
bool TFIMG::SubMultiAve(const string fileName) {
	int iCnt;
	int iFCnt;
	int iMainCnt, iSubCnt;
	double sum;     //移動平均値を出すための合計
	float *pfOrgDt;
	fstream file;
	int iFNum;      //移動平均段数
	int *piLen;     //移動平均長さ
	float *pfFactor;      //移動平均重み
	float fReg;       //移動平均規格化値
	int *piAddPos, *piSubPos;       //移動平均加算・減算位置
	int iHalfNum;        //移動平均個数の半分
	int iNormalEndPos;
	float *pfFilter;
	int iDtCnt, iDtStartPos, iDtEndPos;
	int iFilterCnt, iFilterStartPos;
	float fFilterSum;
	float fDtSum;

	file.open(fileName.c_str(), ios::in);
	if (file.fail())
		return (false);
	file >> iFNum;
	piLen = new int[iFNum];
	pfFactor = new float[iFNum];
	piAddPos = new int[iFNum];
	piSubPos = new int[iFNum];

	fReg = 0.0;
	for (iCnt = 0; iCnt < iFNum; ++iCnt) {
		file >> piLen[iCnt];
		file >> pfFactor[iCnt];
		fReg += pfFactor[iCnt] * piLen[iCnt];
	}
	file.close();

//make filter
	iHalfNum = piLen[0] / 2;
	pfFilter = new float[piLen[0]];
	piAddPos[0] = piLen[0];
	piSubPos[0] = 0;
	for (iCnt = 0; iCnt < piAddPos[0]; ++iCnt) {
		pfFilter[iCnt] = pfFactor[0];
	}
	for (iFCnt = 1; iFCnt < iFNum; ++iFCnt) {        //加減算位置計算
		piAddPos[iFCnt] = iHalfNum + piLen[iFCnt] / 2 + 1;
		piSubPos[iFCnt] = iHalfNum - piLen[iFCnt] / 2;
		for (iCnt = piSubPos[iFCnt]; iCnt < piAddPos[iFCnt]; ++iCnt) {
			pfFilter[iCnt] += pfFactor[iFCnt];
		}
	}
	for (iCnt = 0; iCnt < piAddPos[0]; ++iCnt) {
		pfFilter[iCnt] /= fReg;
	}

	iNormalEndPos = SubPixel - iHalfNum;
	pfOrgDt = new float[SubPixel];
	for (iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt) {
		for (iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt) {
			pfOrgDt[iSubCnt] = Dt[iMainCnt][iSubCnt];
		}
		iDtEndPos = 3;
		iFilterStartPos = iHalfNum - 1;
		for (iSubCnt = 1; iSubCnt < iHalfNum; ++iSubCnt) {
			fDtSum = pfFilter[iFilterStartPos] * pfOrgDt[0];
			fFilterSum = pfFilter[iFilterStartPos];
			for (iDtCnt = 1, iFilterCnt = iFilterStartPos + 1;
					iDtCnt < iDtEndPos; ++iDtCnt, ++iFilterCnt) {
				fDtSum += pfFilter[iFilterCnt] * pfOrgDt[iDtCnt];
				fFilterSum += pfFilter[iFilterCnt];
			}
			Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
			iDtEndPos += 2;
			--iFilterStartPos;
		}

//移動加算
		piAddPos[0] = piLen[0];
		piSubPos[0] = 0;
		for (iFCnt = 1; iFCnt < iFNum; ++iFCnt) {        //加減算位置計算
			piAddPos[iFCnt] = iHalfNum + piLen[iFCnt] / 2 + 1;
			piSubPos[iFCnt] = iHalfNum - piLen[iFCnt] / 2;
		}
		sum = 0.0;
		for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {
			for (iCnt = piSubPos[iFCnt]; iCnt < piAddPos[iFCnt]; ++iCnt) {
				sum += pfFactor[iFCnt] * pfOrgDt[iCnt];
			}
		}

		Dt[iMainCnt][iHalfNum] = sum / fReg;
		for (iSubCnt = iHalfNum + 1; iSubCnt < iNormalEndPos; ++iSubCnt) {
			for (iFCnt = 0; iFCnt < iFNum; ++iFCnt) {
				sum += pfFactor[iFCnt]
						* (pfOrgDt[piAddPos[iFCnt]] - pfOrgDt[piSubPos[iFCnt]]);
				++piAddPos[iFCnt];
				++piSubPos[iFCnt];
			}
			Dt[iMainCnt][iSubCnt] = sum / fReg;
		}

//filter
		iDtStartPos = SubPixel - piLen[0] + 2;
		iFilterStartPos = 1;
		for (iSubCnt = iNormalEndPos; iSubCnt < SubPixel; ++iSubCnt) {
			fDtSum = pfFilter[iFilterStartPos] * pfOrgDt[iDtStartPos];
			fFilterSum = pfFilter[iFilterStartPos];
			for (iDtCnt = iDtStartPos + 1, iFilterCnt = iFilterStartPos + 1;
					iDtCnt < SubPixel; ++iDtCnt, ++iFilterCnt) {
				fDtSum += pfFilter[iFilterCnt] * pfOrgDt[iDtCnt];
				fFilterSum += pfFilter[iFilterCnt];
			}
			Dt[iMainCnt][iSubCnt] = fDtSum / fFilterSum;
			iDtStartPos += 2;
			++iFilterStartPos;
		}

	}
	delete[] pfFilter;
	delete[] pfOrgDt;
	delete[] piLen;
	delete[] pfFactor;
	delete[] piAddPos;
	delete[] piSubPos;

	return (true);
}
//---------------------------------------------------------------------------
/*

 */
void TFIMG::TrimArea(int &iM0, int &iS0, int &iM1, int &iS1) {
	if (iM0 < 0) {
		iM0 = 0;
	}
	if (iS0 < 0) {
		iS0 = 0;
	}
	if (MainPixel <= iM1) {
		iM1 = MainPixel - 1;
	}
	if (SubPixel <= iS1) {
		iS1 = SubPixel - 1;
	}
	if (iM1 < iM0) {
		if (MainPixel <= iM0) {
			iM0 = MainPixel - 1;
		}
		iM1 = iM0;
	}
	if (iS1 < iS0) {
		if (SubPixel <= iS0) {
			iS0 = SubPixel - 1;
		}
		iS1 = iS0;
	}
}
//---------------------------------------------------------------------------
//ttsuji追加 140523
//　主副反転
void TFIMG::MainSub() {
	int i, j;
	TFIMG temp;

	temp.NewDt(MainPixel, SubPixel);

	for (i = 0; i < MainPixel; ++i)
		for (j = 0; j < SubPixel; ++j)
			temp.Dt[i][j] = Dt[i][j];

	NewDt(SubPixel, MainPixel);

//    for(i = 0 ; i < SubPixel ; ++i)
//		for(j = 0 ; j < MainPixel ; ++j)
	for (i = 0; i < MainPixel; ++i)
		for (j = 0; j < SubPixel; ++j)
			Dt[i][j] = temp.Dt[j][i];
}
//---------------------------------------------------------------------------
//ttsuji追加 170912
//　float型画像の副方向微分処理（微分の定義を指定可能）
//  iPos - iPosRef　という定義式で微分を計算（ただし、iPos,iPosRefのいずれかはゼロである必要あり）

bool TFIMG::DifSub(int iPos, int iPosRef) {
	if (iPos * iPosRef) {
		return false;
	}
	int iDif = iPos - iPosRef;
	if (iDif < 0) {
		iDif *= -1;        //iDifは絶対値とする
	}

	TFIMG copy;
	copy = *this;
	for (int iMCnt = 0; iMCnt < MainPixel; iMCnt++) {
		for (int iSCnt = 0; iSCnt < SubPixel - iDif; iSCnt++) {
			Dt[iMCnt][iSCnt] = copy.Dt[iMCnt][iPos + iSCnt]
					- copy.Dt[iMCnt][iPosRef + iSCnt];
		}
	}

	return true;
}
//---------------------------------------------------------------------------
/*
 Log->linear
 9.48 * 10^(QLlog / 4096)

 ttsuji追加（LOG変換時の変数で指定）
 */

void TFIMG::LogToLinear2(const int iBit, const double dCenter,
		const int iKeta) {
	int iMCnt, iSCnt;
	double dQLlogPerKeta = pow(2.0, iBit) / iKeta;        //4096.
	double dCoef = dCenter / pow(10.0, iKeta / 2.0);        //9.48

	for (iSCnt = 0; iSCnt < SubPixel; iSCnt++) {
		for (iMCnt = 0; iMCnt < MainPixel; iMCnt++) {
			Dt[iMCnt][iSCnt] = dCoef
					* pow(10.0, (Dt[iMCnt][iSCnt] / dQLlogPerKeta)); //9.48 * 10^(QLlog / 4096)
		}
	}
}
