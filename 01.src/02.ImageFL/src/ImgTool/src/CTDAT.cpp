/***********************************************************
 * @file    CTDAT.cpp
 * @brief	画像基本クラス
 * 			データはunsigned shortの形で保持することを基本とする
 * @author  FF	T.Taneichi
 * @date    2021/03/30	ImageToolのソースコードを流用して整理
 * Copyright(c) 2021 FUJIFILM Corporation.
 ***********************************************************/
#include "CTDAT.h"
#include "CTDATProc.h"
#include "TRECTANGLE.h"
#include "utility.h"
#include "CApplication.h"
#include <math.h>
#include <vector>
#include <stack>
#include <algorithm>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>

using namespace std;

/* -------------------------------------------------------------------
* @brief	コンストラクタ
* @param	なし
* @retval   なし
* @date     2021/03/30	T.Taneichi	流用
 --------------------------------------------------------------------- */
CTDAT::CTDAT() :DtExist(false), HistExist(false), DtMax(0), MainPixel(0), SubPixel(0)  {
	Dt = NULL;
	Bit = 0;
	Hist = NULL;
	HistRuisekiSum = 0;
}

CTDAT::CTDAT(const int iS, const int iM, const int iBit)
        :MainPixel(iM), SubPixel(iS), DtExist(true), HistExist(false)
{
    SetBit(iBit);

    Dt = new unsigned short int*[iS];
    for (int iSCnt = 0; iSCnt < iS; ++iSCnt){
        Dt[iSCnt] = new unsigned short int[iM];
    }
}
//---------------------------------------------------------------------------
/*
	copy constructor
*/
CTDAT::CTDAT(const CTDAT &copy)
{
	int i, j;
    DtExist = false;//DtExistをあらわに規定(ttsuji090210)
	NewDt(copy.SubPixel, copy.MainPixel, copy.Bit);
	for(i = 0 ; i < SubPixel ; ++i)
		for(j = 0 ; j < MainPixel ; ++j)
			Dt[i][j] = copy.Dt[i][j];
}
/* -------------------------------------------------------------------
* @brief	ディストラクタ
* @param	なし
* @retval   なし
* @date     2021/03/30	T.Taneichi	流用
 --------------------------------------------------------------------- */
CTDAT::~CTDAT() {
	DeleteDt();

	if(HistExist == true) delete[] Hist;
}
/* -------------------------------------------------------------------
* @brief	Dt[][]の領域確保
* @param	sub			副画素数
* @param	main		主画素数
* @param	bit			ビット数
* @retval   なし
* @date     2021/03/30	T.Taneichi	流用
 --------------------------------------------------------------------- */
void CTDAT::NewDt(const int sub, const int main, const int bit)
{
	int i;

	if(DtExist){
#if 1   //NewDt関数の引数main,subにメンバ変数MainPixel,SubPixelを指定した場合、既に確保しているメモリがmain*sub個でなくてもそれを検知できず、Deleteもnewもせずreturnする不具合あり→メモリアクセスバイオレーションの実行エラーにつながる（ttsuji）
		if(MainPixel == main && SubPixel == sub){
			if(Bit != bit){
				SetBit(bit);
			}
			return;
		}else{
			DeleteDt();
		}
#else	//必ず一旦デリートするよう修正（ttsuji）
		DeleteDt();
#endif
    }

    SetBit(bit);

    Dt = new unsigned short int*[sub];
	for (i = 0 ; i < sub ; i++)
        Dt[i] = new unsigned short int[main];
    DtExist = true;

    MainPixel = main;
    SubPixel = sub;
}


/* -------------------------------------------------------------------
* @brief	Dt[][]のメモリ開放
* @param	なし
* @retval   なし
* @date     2021/03/30	T.Taneichi	流用
 --------------------------------------------------------------------- */
void CTDAT::DeleteDt()
{
	if(DtExist == true){
    	for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        	delete[] Dt[iSCnt];
        }
        delete[] Dt;
        DtExist = false;
	    if(HistExist == true){
            delete[] Hist;
            HistExist = false;
        }
        DtMax = 0;		//画像data最大値
        MainPixel = 0;		//主画素数
        SubPixel = 0;		//副画素数
        Bit = 0;		//画像data bit数
    }
}
//---------------------------------------------------------------------------
/*
	Bitの設定(DtMaxの設定も行う)
*/
void CTDAT::SetBit(const int in_bit)
{
    if(in_bit < 1 || in_bit > 16) return;
    DtMax =(unsigned short int)((1 << in_bit) - 1);		//画像data最大値
    Bit = in_bit;		//画像data bit数
}

//---------------------------------------------------------------------------
/*
	if(副差分絶対値 > iSDifOffset &&	主差分絶対値 < iMDifOffset)
		result.Dt[iSCnt][iMCnt] = result.DtMax;
	else
		result.Dt[iSCnt][iMCnt] = 0;

	※元画像は変化しない。
*/

//---------------------------------------------------------------------------
/*
	operator =
*/
CTDAT& CTDAT::operator =(const CTDAT &copy)
{
	int i,j;

    NewDt(copy.SubPixel, copy.MainPixel, copy.Bit);
    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	Dt[i][j] = copy.Dt[i][j];

    return(*this);
}

//---------------------------------------------------------------------------
//ttsuji追加
/*
	operator +(const int oper)
*/
CTDAT CTDAT::operator +(const int oper)
{
	CTDAT result;
	int main = MainPixel;
	int sub = SubPixel;
	int bit = Bit;
	result.NewDt(sub, main, bit);

	int i, j;
	for(i = 0 ; i < sub ; i++){
		for(j = 0 ; j < main ; j++){
			result.Dt[i][j] = CLIP((Dt[i][j] + oper), 0, result.DtMax);
		}
	}
	return result;
}

//---------------------------------------------------------------------------
//ttsuji追加
/*
	operator -(const int oper)
*/
CTDAT CTDAT::operator -(const int oper)
{
	CTDAT result;
	int main = MainPixel;
	int sub = SubPixel;
	int bit = Bit;
	result.NewDt(sub, main, bit);

	int i, j;
	for(i = 0 ; i < sub ; i++){
		for(j = 0 ; j < main ; j++){
			result.Dt[i][j] = CLIP((Dt[i][j] - oper), 0, result.DtMax);
		}
	}
	return result;
}

//---------------------------------------------------------------------------
//ttsuji追加
/*
	operator *(const float fVal)
*/
CTDAT CTDAT::operator *(const float fVal)
{
	CTDAT result;
	int main = MainPixel;
	int sub = SubPixel;
	int bit = Bit;
	result.NewDt(sub, main, bit);

	int i, j;
	for(i = 0 ; i < sub ; i++){
		for(j = 0 ; j < main ; j++){
			float fdata = (float)Dt[i][j];
			result.Dt[i][j] = CLIP((long)(fdata * fVal + 0.5), 0, result.DtMax);
		}
	}
	return result;
}

CTDAT& CTDAT::operator +=(const int oper2)
{
	int i, j;

    for(i = 0 ; i < SubPixel ; ++i){
    	for(j = 0 ; j < MainPixel ; ++j){
            SetVal(i, j, Dt[i][j] + oper2);
        }
    }

    return(*this);
}
CTDAT& CTDAT::operator -=(const int oper)
{
	int i, j;

    for(i = 0 ; i < SubPixel ; ++i){
    	for(j = 0 ; j < MainPixel ; ++j){
            SetVal(i, j ,int(Dt[i][j]) - oper);
        }
    }

    return(*this);
}

CTDAT& CTDAT::operator -=(const double dVal)
{
    for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
    	for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
            SetVal(iSCnt, iMCnt, double(Dt[iSCnt][iMCnt]) - dVal);
        }
    }
    return(*this);
}

//---------------------------------------------------------------------------
/*
	operator *= float
*/
CTDAT& CTDAT::operator *=(const float oper2)
{
	int i, j;
    float result;

    for(i = 0 ; i < SubPixel ; ++i){
    	for(j = 0 ; j < MainPixel ; ++j){
            result = Dt[i][j] * oper2;
            if(result > DtMax){
                Dt[i][j] = DtMax;
            }else if(result < 0){
                Dt[i][j] = 0;
            }else{
        	    Dt[i][j] = result;
            }
        }
    }

    return(*this);
}
//---------------------------------------------------------------------------
/*
	operator *= int
*/
CTDAT& CTDAT::operator *=(const int oper2)
{
	int i, j;

    for(i = 0 ; i < SubPixel ; ++i){
    	for(j = 0 ; j < MainPixel ; ++j){
            SetVal(i, j, Dt[i][j] * oper2);
        }
    }

    return(*this);
}

CTDAT& CTDAT::operator /=(const int oper2)
{
	int i, j;

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	SetVal(i, j, Dt[i][j] / oper2);

    return(*this);
}
//---------------------------------------------------------------------------
/*
	operator /= float
*/
CTDAT& CTDAT::operator /=(const float fOper2)
{
	int i, j;

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	SetVal(i, j, float(Dt[i][j]) / fOper2);

    return(*this);
}

CTDAT& CTDAT::operator <<=(const int oper2)
{
	int i, j;

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	Dt[i][j] <<= oper2;

    return(*this);
}
//---------------------------------------------------------------------------
/*
	operator >>= int
*/
CTDAT& CTDAT::operator >>=(const int oper2)
{
	int i, j;

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	Dt[i][j] >>= oper2;

    return(*this);
}

CTDAT& CTDAT::operator / (const float oper2)
{
    int i, j;
    float result;

    for(i = 0 ; i < MainPixel ; ++i){
        for(j = 0 ; j < SubPixel ; ++j){
            result = Dt[j][i] /= oper2;
            if(result > DtMax){
                Dt[j][i] = DtMax;
            }else if(result < 0){
                Dt[j][i] = 0;
            }else{
        	    Dt[j][i] = result;
            }
        }
    }

    return(*this);
}
//---------------------------------------------------------------------------
//ttsuji追加
/*
	operator +(const TDAT& copy)
*/
CTDAT CTDAT::operator +(const CTDAT& copy)
{
	CTDAT result;
	result.NewDt(copy.SubPixel, copy.MainPixel, copy.Bit);

	int i, j;
	for(i = 0 ; i < copy.SubPixel ; i++){
		for(j = 0 ; j < copy.MainPixel ; j++){
			result.Dt[i][j] = CLIP((Dt[i][j] + copy.Dt[i][j]), 0, result.DtMax);
		}
	}
	return result;
}
CTDAT& CTDAT::operator +=(const CTDAT& oper2)
{
	int i, j;
    CTDAT temp;
    int main;
    int sub;
    int iMCnt,  iSCnt;

    if(MainPixel >= oper2.MainPixel && SubPixel >= oper2.SubPixel){
        for(iSCnt = 0; iSCnt < oper2.SubPixel; ++iSCnt){
            for(iMCnt = 0; iMCnt < oper2.MainPixel; ++iMCnt){
                SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + oper2.Dt[iSCnt][iMCnt]);
            }
        }
    }else{
            //大きいほうの画素数にする
        if(MainPixel < oper2.MainPixel){
            main = oper2.MainPixel;
        }else{
            main = MainPixel;
        }
        if(SubPixel < oper2.SubPixel){
            sub = oper2.SubPixel;
        }else{
            sub = SubPixel;
        }
        temp = oper2;
		temp.Format(sub, main, 0, 0, 0);
        for(i = 0 ; i < SubPixel ; ++i){
    	    for(j = 0 ; j < MainPixel ; ++j){
        	    temp.SetVal(i, j, temp.Dt[i][j] + Dt[i][j]);
            }
        }

        NewDt(sub, main, Bit);
        for(i = 0 ; i < sub ; ++i){
    	    for(j = 0 ; j < main ; ++j){
                Dt[i][j] = temp.Dt[i][j];
            }
        }
    }
    return(*this);
}

CTDAT& CTDAT::operator -=(const CTDAT& oper2)
{
	int i, j;
    CTDAT temp;
    int main;
    int sub;
    int iMCnt, iSCnt;

	if(MainPixel >= oper2.MainPixel && SubPixel >= oper2.SubPixel){
    	for(i = 0 ; i < oper2.SubPixel ; ++i){
        	for(j = 0 ; j < oper2.MainPixel ; ++j){
                if(Dt[i][j] >= oper2.Dt[i][j]){
            	    Dt[i][j] -= oper2.Dt[i][j];
                }else{
                    Dt[i][j] = 0;
                }
            }
        }
    }else{
        temp.NewDt(SubPixel, MainPixel, Bit);
        for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
            for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
                temp.Dt[iSCnt][iMCnt] = Dt[iSCnt][iMCnt];
            }
        }
            //大きいほうの画素数にする
        if(MainPixel < oper2.MainPixel){
            main = oper2.MainPixel;
        }else{
            main = MainPixel;
        }
        if(SubPixel < oper2.SubPixel){
            sub = oper2.SubPixel;
        }else{
            sub = SubPixel;
        }
        NewDt(sub, main, Bit);
        FillValue(0);
        for(i = 0 ; i < temp.SubPixel ; ++i){
    	    for(j = 0 ; j < temp.MainPixel ; ++j){
        	    Dt[i][j] = temp.Dt[i][j];
            }
        }

        for(i = 0 ; i < oper2.SubPixel ; ++i){
    	    for(j = 0 ; j < oper2.MainPixel; ++j){
                SetVal(i, j, Dt[i][j] - oper2.Dt[i][j]);
            }
        }
    }
    return(*this);
}

CTDAT& CTDAT::operator *=(const CTDAT& oper2)
{
	int i, j;
    int main;
    int sub;

    if(MainPixel <= oper2.MainPixel)
        main = MainPixel;
    else
        main = oper2.MainPixel;

    if(SubPixel <= oper2.SubPixel)
        sub = SubPixel;
    else
        sub = oper2.SubPixel;

    for(i = 0 ; i < sub ; ++i){
        for(j = 0 ; j < main ; ++j){
            SetVal(i, j, int(Dt[i][j]) * int(oper2.Dt[i][j]));
        }
    }
    return(*this);
}

void CTDAT::Grad(const int iSDifOffset, const int iMDifOffset, CTDAT& result)
{
	result.NewDt(SubPixel, MainPixel, Bit);
    result.FillValue(0);
	for(int iSCnt = 1; iSCnt < SubPixel - 1; ++iSCnt){
		for(int iMCnt = 1; iMCnt < MainPixel - 1; ++iMCnt){
			if(abs(Dt[iSCnt + 1][iMCnt] - Dt[iSCnt - 1][iMCnt]) > iSDifOffset
			   && abs(Dt[iSCnt][iMCnt + 1] - Dt[iSCnt][iMCnt - 1]) < iMDifOffset
					){
//            if(abs(img.Dt[iSCnt + 1][iMCnt] - img.Dt[iSCnt - 1][iMCnt]) - iDifOffset
//                    > abs(img.Dt[iSCnt][iMCnt + 1] - img.Dt[iSCnt][iMCnt - 1])){
				result.Dt[iSCnt][iMCnt] = result.DtMax;
            }
        }
    }

#ifdef DEBUG
	TTMA debug(result);
	debug.Mul(1, 2);
	debug.WriteTma("c:\Grad", false);
#endif
}

//---------------------------------------------------------------------------
/*
	if(副差分絶対値 - iDifOffset > 主差分絶対値)
		result.Dt[iSCnt][iMCnt] = result.DtMax;
	else
		result.Dt[iSCnt][iMCnt] = 0;

	※元画像は変化しない。
*/
void CTDAT::Grad(const int iDifOffset, CTDAT& result)
{
	result.NewDt(SubPixel, MainPixel, Bit);
    result.FillValue(0);
	for(int iSCnt = 1; iSCnt < SubPixel - 1; ++iSCnt){
		for(int iMCnt = 1; iMCnt < MainPixel - 1; ++iMCnt){
//            if(abs(img.Dt[iSCnt + 1][iMCnt] - img.Dt[iSCnt - 1][iMCnt]) > iDifOffset){
			if(abs(Dt[iSCnt + 1][iMCnt] - Dt[iSCnt - 1][iMCnt]) - iDifOffset
					> abs(Dt[iSCnt][iMCnt + 1] - Dt[iSCnt][iMCnt - 1])){
                result.Dt[iSCnt][iMCnt] = result.DtMax;
			}
        }
	}
#ifdef DEBUG
	static int iCnt = 0;
    TTMA debug(result);
    debug.Mul(1, 2);
    String dir = "C:\\";
    String name = "Grad";
    debug.WriteTma(dir + IntToStr(iCnt) + name, false);

	debug = *this;
    debug.Mul(1, 2);
    name = "img";
    debug.WriteTma(dir + IntToStr(iCnt) + name, false);
    ++iCnt;
#endif
}

//---------------------------------------------------------------------------
/*
	平均値を求める
*/
double CTDAT::dAve()
{
    int iMCnt, iSCnt;
    double dSum = 0.0;

    for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
            dSum += Dt[iSCnt][iMCnt];
        }
    }
    return(dSum / SubPixel / MainPixel);
}

//---------------------------------------------------------------------------
/*
	指定領域の平均値を求める
        領域指定が異常なときは- 1.0を返す
*/
double CTDAT::dAve(const int iS1, const int iM1, const int iS2, const int iM2)
{
    if(bCheckArea(iS1, iM1, iS2, iM2) == false){
        return(- 1.0);
    }

	int iMCnt, iSCnt;
	double dSum = 0.0;

	for(iSCnt = iS1; iSCnt <= iS2; ++iSCnt){
		for(iMCnt = iM1; iMCnt <= iM2; ++iMCnt){
			dSum += Dt[iSCnt][iMCnt];
		}
	}
	return(dSum / (iS2 - iS1 + 1) / (iM2 - iM1 + 1));
}

//---------------------------------------------------------------------------
/*
	画像の値加算
*/
void CTDAT::Add(CTDAT& addImg)
{
/*
    int iMainBig, iMainSmall, iSubBig, iSubSmall;
    int iMCnt, iSCnt;

            //MainPixel >= addImg.MainPixel && SubPixel >= addImg.SubPixel
    if(MainPixel < addImg.MainPixel){
        iMainBig = addImg.MainPixel;
        iMainSmall
    if(MainPixel < addImg.MainPixel || SubPixel < addImg.SubPixel)

            //MainPixel >= addImg.MainPixel && SubPixel < addImg.SubPixel
            //MainPixel < addImg.MainPixel && SubPixel >= addImg.SubPixel
            //MainPixel < addImg.MainPixel && SubPixel < addImg.SubPixel
*/
}

//---------------------------------------------------------------------------
/*
	Bit変換
		dataも変換
*/
void CTDAT::BitChange(const int iNewBit)
{
	int iMainCnt, iSubCnt;
	int iShift;

	if(iNewBit < 1 || iNewBit > 16) return;

	if(Bit > iNewBit){
		iShift = Bit - iNewBit;
		for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
			for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
				Dt[iSubCnt][iMainCnt] >>= iShift;
			}
		}
	}else{
        iShift = iNewBit - Bit;
		for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
            for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
				Dt[iSubCnt][iMainCnt] <<= iShift;
			}
        }
	}

	SetBit(iNewBit);
}

//---------------------------------------------------------------------------
/*
	Bad Pixel補間
    	周囲8画素の正常画素の平均値とする
*/
void CTDAT::BadCor()
{
    const float TH = 0.01;		//欠陥判定の最大histに対する割合
    float fFilter[3][3] = {{0.707107, 1, 0.707107},
                         {1,        1,        1},
                         {0.707107, 1, 0.707107}};

	int i, j, ks, km;
    int ct;
    int iBadMCnt, iBadSCnt;
    float sum;
    float fFilterSum;
    bool **bad;     //欠陥flag

    bad = new bool*[SubPixel];
    for (i = 0 ; i < SubPixel ; ++i)
        bad[i] = new bool[MainPixel];

    BadFind(bad, TH);

    for(i = 1 ; i < SubPixel - 1 ; ++i){
    	for(j = 1 ; j < MainPixel - 1 ; ++j){
        	if(bad[i][j] == true){
        		ct = 0;
            	sum = 0;
                fFilterSum = 0.0;
            	for(ks = i - 1, iBadSCnt = 0; iBadSCnt < 3; ++ks, ++iBadSCnt){
            		for(km = j - 1, iBadMCnt = 0; iBadMCnt < 3; ++km, ++iBadMCnt){
            			if(bad[ks][km] == false){
                    		sum += fFilter[iBadSCnt][iBadMCnt] * Dt[ks][km];
                            fFilterSum += fFilter[iBadSCnt][iBadMCnt];
                            ++ct;
                        }
                    }
                }
                if(ct != 0) Dt[i][j] = sum / fFilterSum;
            }
        }
    }

    for(i = 0 ; i < SubPixel ; ++i)
        delete[] bad[i];
    delete[] bad;
}
//---------------------------------------------------------------------------
/*
	Bad Pixel補間
    	周囲8画素の正常画素の平均値とする
*/
void CTDAT::BadCor(bool **bad, int iCorNum)
{
//    float fFilter[3][3] = {{0.707107, 1, 0.707107},
//                         {1,        1,        1},
//                         {0.707107, 1, 0.707107}};
    float fFilter[3][3] = {{0.0, 1, 0.0},
                         {1,        1,        1},
                         {0.0, 1, 0.0}};
    float fSum;
    float fFilterSum;
    bool **cor;     //欠陥flag
    int iMCnt,  iSCnt;
    int iMCnt2, iSCnt2;
    int iBadMCnt, iBadSCnt;
    bool    IsBadExist = false;
    int iCorCnt = 0;

    cor = new bool*[SubPixel];
    for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        cor[iSCnt] = new bool[MainPixel];
    }

    for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
            cor[iSCnt][iMCnt] = bad[iSCnt][iMCnt];
            if(cor[iSCnt][iMCnt]){
                IsBadExist = true;
            }
        }
    }
    while(IsBadExist && iCorCnt < iCorNum){
        for(iSCnt = 1; iSCnt < SubPixel - 1; ++iSCnt){
    	    for(iMCnt = 1; iMCnt < MainPixel - 1; ++iMCnt){
        	    if(bad[iSCnt][iMCnt] == true){
            	    fSum = 0.0;
                    fFilterSum = 0.0;
            	    for(iSCnt2 = iSCnt - 1, iBadSCnt = 0; iBadSCnt < 3; ++iSCnt2, ++iBadSCnt){
            		    for(iMCnt2 = iMCnt - 1, iBadMCnt = 0; iBadMCnt < 3; ++iMCnt2, ++iBadMCnt){
            			    if(bad[iSCnt2][iMCnt2] == false){
                    		    fSum += fFilter[iBadSCnt][iBadMCnt] * Dt[iSCnt2][iMCnt2];
                                fFilterSum += fFilter[iBadSCnt][iBadMCnt];
                            }
                        }
                    }
                    if(fFilterSum > 0.0){
                        SetVal(iSCnt, iMCnt, fSum / fFilterSum);
                        cor[iSCnt][iMCnt] = false;
                    }
                }
            }
        }
        ++iCorCnt;
        IsBadExist = false;
        for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
            for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
                bad[iSCnt][iMCnt] = cor[iSCnt][iMCnt];
                if(bad[iSCnt][iMCnt]){
                    IsBadExist = true;
                }
            }
        }
    }

    for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        delete[] cor[iSCnt];
    }
    delete[] cor;
}
//---------------------------------------------------------------------------
/*
	Bad Pixel補間
*/
void CTDAT::BadCor2(bool **bad)
{

// original
    float fSum;
    float fWeight, fWeightSum;
    float fLength;
    int iMCnt,  iSCnt;
    int iMCnt2, iSCnt2;
    int iBadMCnt, iBadSCnt;
    int iSearchCnt;

    for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
            if(bad[iSCnt][iMCnt] == true){
                iSearchCnt = 1;
                for(;;){
                    fSum = 0.0;
                    fWeightSum = 0.0;
                    for(iSCnt2 = - iSearchCnt; iSCnt2 <= iSearchCnt; ++iSCnt2){
                        for(iMCnt2 = - iSearchCnt; iMCnt2 <= iSearchCnt; ++iMCnt2){
                            iBadMCnt = iMCnt2 + iMCnt;
                            iBadSCnt = iSCnt2 + iSCnt;
                            fLength = sqrt(iMCnt2 * iMCnt2 + iSCnt2 * iSCnt2);
                            if(fLength <= iSearchCnt
                                    && iBadSCnt >= 0 && iBadMCnt >= 0
                                    && iBadSCnt < SubPixel && iBadMCnt < MainPixel
                                    && bad[iBadSCnt][iBadMCnt] == false){
                                fWeight = 1.0 / fLength / fLength / fLength;
                                fSum += fWeight * Dt[iBadSCnt][iBadMCnt];
                                fWeightSum += fWeight;
                            }
                        }
                    }
                    if(fLength * fWeightSum > 1.0f){       //(fLength * fLength * fWeightSum > fLength){
                        SetVal(iSCnt, iMCnt, fSum / fWeightSum);
                        break;
                    }else{
                        ++iSearchCnt;
                    }
                }
            }
        }
    }
//
}


//---------------------------------------------------------------------------
/*
	Bad Pixel補間
        欠陥周辺画素の距離で重みづけ
*/
bool CTDAT::bBadCorAround(CTDAT& bad)         //DtMaxを欠陥と認識
{
    const int iSIZE_MAX = 100;      //重み付け補正する欠陥の最大の大きさ

    int iSearchCnt;
    int iCnt, iSum;
    int iMCnt,  iSCnt;
    int iMCnt2, iSCnt2;
    int iMCnt3, iSCnt3;
    int iMBad1, iMBad2, iSBad1, iSBad2;
    bool bBadExist;
    float fDistance;
    float fSum;
    float fSumWeight;
    float fWeight[iSIZE_MAX + 2][iSIZE_MAX + 2];
    unsigned short int nPer = (unsigned short int)(bad.DtMax / 2);

    if(MainPixel != bad.MainPixel || SubPixel != bad.SubPixel){
        return(false);
    }

//      L <  2 の時　距離の3乗
//      L >= 2 の時　距離の2乗
//      距離の3乗とcase by case
    for(iSCnt = 0; iSCnt <= iSIZE_MAX; ++iSCnt){
        for(iMCnt = 0; iMCnt <= iSIZE_MAX; ++iMCnt){
            fDistance = sqrt(iSCnt * iSCnt + iMCnt * iMCnt);
            if(fDistance == 0.0){
                fWeight[iSCnt][iMCnt] = 0.0;
            }else if(fDistance < 2.0){
                fDistance = fDistance * fDistance * fDistance;
                fWeight[iSCnt][iMCnt] = 1.0 / fDistance;
            }else{
                fWeight[iSCnt][iMCnt] = 1.0 / 2.0 / fDistance / fDistance;
            }
        }
    }

    for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
            if(bad.Dt[iSCnt][iMCnt] == bad.DtMax){
                    //欠陥大きさ決定
                if(iMCnt > 0){
                    iMBad1 = iMCnt - 1;
                }else{
                    iMBad1 = 0;
                }
                if(iSCnt > 0){
                    iSBad1 = iSCnt - 1;
                }else{
                    iSBad1 = 0;
                }
                iMBad2 = iMCnt;
                while(iMBad2 < MainPixel - 1 && bad.Dt[iSCnt][iMBad2] == bad.DtMax){
                    ++iMBad2;
                }
                iSBad2 = iSCnt;
                while(iSBad2 < SubPixel - 1 && bad.Dt[iSBad2][iMCnt] == bad.DtMax){
                    ++iSBad2;
                }
                do{
                    bBadExist = false;
                    for(iMCnt2 = iMBad1; iMCnt2 <= iMBad2; ++iMCnt2){
                        if(bad.Dt[iSBad1][iMCnt2] == bad.DtMax && iSBad1 > 0){
                            bBadExist = true;
                            --iSBad1;
                            iMCnt2 = iMBad1 - 1;    //resetして最初からcheck
                        }
                    }
                    for(iMCnt2 = iMBad1; iMCnt2 <= iMBad2; ++iMCnt2){
                        if(bad.Dt[iSBad2][iMCnt2] == bad.DtMax && iSBad2 < SubPixel - 1){
                            bBadExist = true;
                            ++iSBad2;
                            iMCnt2 = iMBad1 - 1;    //resetして最初からcheck
                        }
                    }
                    for(iSCnt2 = iSBad1; iSCnt2 <= iSBad2; ++iSCnt2){
                        if(bad.Dt[iSCnt2][iMBad1] == bad.DtMax && iMBad1 > 0){
                            bBadExist = true;
                            --iMBad1;
                            iSCnt2 = iSBad1 - 1;    //resetして最初からcheck
                        }
                    }
                    for(iSCnt2 = iSBad1; iSCnt2 <= iSBad2; ++iSCnt2){
                        if(bad.Dt[iSCnt2][iMBad2] == bad.DtMax && iMBad2 < MainPixel - 1){
                            bBadExist = true;
                            ++iMBad2;
                            iSCnt2 = iSBad1 - 1;    //resetして最初からcheck
                        }
                    }
                }while(bBadExist);

                    //欠陥補正
                if(bad.Dt[iSBad1][iMBad1] != bad.DtMax
                        && ((iMBad1 + 1 < bad.MainPixel && bad.Dt[iSBad1][iMBad1 + 1] == bad.DtMax)
                            || (iSBad1 + 1 < bad.SubPixel && bad.Dt[iSBad1 + 1][iMBad1] == bad.DtMax)
                            || (iMBad1 + 1 < bad.MainPixel && iSBad1 + 1 < bad.SubPixel && bad.Dt[iSBad1 + 1][iMBad1 + 1] == bad.DtMax))){
                    bad.Dt[iSBad1][iMBad1] = nPer;
                }
                if(bad.Dt[iSBad1][iMBad2] != bad.DtMax
                        && ((iMBad2 - 1 >= 0 && bad.Dt[iSBad1][iMBad2 - 1] == bad.DtMax)
                            || (iSBad1 + 1 < bad.SubPixel && bad.Dt[iSBad1 + 1][iMBad2] == bad.DtMax)
                            || (iMBad2 - 1 >= 0 && iSBad1 + 1 < bad.SubPixel && bad.Dt[iSBad1 + 1][iMBad2 - 1] == bad.DtMax))){
                    bad.Dt[iSBad1][iMBad2] = nPer;
                }
                if(bad.Dt[iSBad2][iMBad1] != bad.DtMax
                        && ((iMBad1 + 1 < bad.MainPixel && bad.Dt[iSBad2][iMBad1 + 1] == bad.DtMax)
                            || (iSBad2 - 1 >= 0 && bad.Dt[iSBad2 - 1][iMBad1] == bad.DtMax)
                            || (iSBad2 - 1 >= 0 && iMBad1 + 1 < bad.MainPixel && bad.Dt[iSBad2 - 1][iMBad1 + 1] == bad.DtMax))){
                    bad.Dt[iSBad2][iMBad1] = nPer;
                }
                if(bad.Dt[iSBad2][iMBad2] != bad.DtMax
                        && ((iMBad2 - 1 >= 0 && bad.Dt[iSBad2][iMBad2 - 1] == bad.DtMax)
                            || (iSBad2 - 1 >= 0 &&bad.Dt[iSBad2 - 1][iMBad2] == bad.DtMax)
                            || (iSBad2 - 1 >= 0 && iMBad2 - 1 >= 0 &&bad.Dt[iSBad2 - 1][iMBad2 - 1] == bad.DtMax))){
                    bad.Dt[iSBad2][iMBad2] = nPer;
                }
                for(iSCnt2 = iSBad1 + 1; iSCnt2 < iSBad2; ++iSCnt2){
                    if(bad.Dt[iSCnt2][iMBad1] != bad.DtMax
                            && (bad.Dt[iSCnt2 - 1][iMBad1] == bad.DtMax || (iMBad1 + 1 < bad.MainPixel && bad.Dt[iSCnt2 - 1][iMBad1 + 1] == bad.DtMax)
                            || (iMBad1 + 1 < bad.MainPixel && bad.Dt[iSCnt2][iMBad1 + 1] == bad.DtMax)
                            || bad.Dt[iSCnt2 + 1][iMBad1] == bad.DtMax || (iMBad1 + 1 < bad.MainPixel && bad.Dt[iSCnt2 + 1][iMBad1 + 1] == bad.DtMax))){
                        bad.Dt[iSCnt2][iMBad1] = nPer;
                    }
                    if(bad.Dt[iSCnt2][iMBad2] != bad.DtMax
                            && ((iMBad2 - 1 >= 0 && bad.Dt[iSCnt2 - 1][iMBad2 - 1] == bad.DtMax) || bad.Dt[iSCnt2 - 1][iMBad2] == bad.DtMax
                            || (iMBad2 - 1 >= 0 && bad.Dt[iSCnt2][iMBad2 - 1] == bad.DtMax)
                            || (iMBad2 - 1 >= 0 && bad.Dt[iSCnt2 + 1][iMBad2 - 1] == bad.DtMax) || bad.Dt[iSCnt2 + 1][iMBad2] == bad.DtMax)){
                        bad.Dt[iSCnt2][iMBad2] = nPer;
                    }
                }
                for(iMCnt2 = iMBad1 + 1; iMCnt2 < iMBad2; ++iMCnt2){
                    if(bad.Dt[iSBad1][iMCnt2] != bad.DtMax
                            && (bad.Dt[iSBad1][iMCnt2 - 1] == bad.DtMax || (iSBad1 - 1 >= 0 && bad.Dt[iSBad1 - 1][iMCnt2 + 1] == bad.DtMax)
                            || (iSBad1 + 1 < bad.SubPixel && (bad.Dt[iSBad1 + 1][iMCnt2 - 1] == bad.DtMax || bad.Dt[iSBad1 + 1][iMCnt2] == bad.DtMax || bad.Dt[iSBad1 + 1][iMCnt2 + 1] == bad.DtMax)))){
                        bad.Dt[iSBad1][iMCnt2] = nPer;
                    }
                    if(bad.Dt[iSBad2][iMCnt2] != bad.DtMax
                            && ((iSBad2 - 1 >= 0 && (bad.Dt[iSBad2 - 1][iMCnt2 - 1] == bad.DtMax || bad.Dt[iSBad2 - 1][iMCnt2] == bad.DtMax || bad.Dt[iSBad2 - 1][iMCnt2 + 1] == bad.DtMax))
                            || bad.Dt[iSBad2][iMCnt2 - 1] == bad.DtMax || bad.Dt[iSBad2][iMCnt2 + 1] == bad.DtMax)){
                        bad.Dt[iSBad2][iMCnt2] = nPer;
                    }
                }
                for(iSCnt2 = iSBad1 + 1; iSCnt2 < iSBad2; ++iSCnt2){
                    for(iMCnt2 = iMBad1 + 1; iMCnt2 < iMBad2; ++iMCnt2){
                        if(bad.Dt[iSCnt2][iMCnt2] != bad.DtMax
                                && (bad.Dt[iSCnt2 - 1][iMCnt2 - 1] == bad.DtMax || bad.Dt[iSCnt2 - 1][iMCnt2] == bad.DtMax || bad.Dt[iSCnt2 - 1][iMCnt2 + 1] == bad.DtMax
                                || bad.Dt[iSCnt2][iMCnt2 - 1] == bad.DtMax || bad.Dt[iSCnt2][iMCnt2 + 1] == bad.DtMax
                                || bad.Dt[iSCnt2 + 1][iMCnt2 - 1] == bad.DtMax || bad.Dt[iSCnt2 + 1][iMCnt2] == bad.DtMax || bad.Dt[iSCnt2 + 1][iMCnt2 + 1] == bad.DtMax)){
                            bad.Dt[iSCnt2][iMCnt2] = nPer;
                        }
                    }
                }

                for(iSCnt2 = iSBad1; iSCnt2 <= iSBad2; ++iSCnt2){
                    for(iMCnt2 = iMBad1; iMCnt2 <= iMBad2; ++iMCnt2){
                        if(bad.Dt[iSCnt2][iMCnt2] == bad.DtMax){
                            if(iMBad2 - iMBad1 < iSIZE_MAX && iSBad2 - iSBad1 < iSIZE_MAX){
                                fSum = 0.0;
                                fSumWeight = 0.0;
                                for(iSCnt3 = iSBad1; iSCnt3 <= iSBad2; ++iSCnt3){
                                    for(iMCnt3 = iMBad1; iMCnt3 <= iMBad2; ++iMCnt3){
                                        if(bad.Dt[iSCnt3][iMCnt3] == nPer){
//                                            if((iSCnt3 == iSCnt2) || (iMCnt3 == iMCnt2)){
                                                fSum += Dt[iSCnt3][iMCnt3] * fWeight[abs(iSCnt3 - iSCnt2)][abs(iMCnt3 - iMCnt2)];
                                                fSumWeight += fWeight[abs(iSCnt3 - iSCnt2)][abs(iMCnt3 - iMCnt2)];
//                                            }
                                        }
                                    }
                                }
                                SetVal(iSCnt2, iMCnt2, fSum / fSumWeight);
                            }else{
                                iSearchCnt = 1;
                                iCnt = 0;
                                iSum = 0;
                                for(;;){
                                    for(iSCnt3 = - iSearchCnt; iSCnt3 <= iSearchCnt; ++iSCnt3){
                                        for(iMCnt3 = - iSearchCnt; iMCnt3 <= iSearchCnt; ++iMCnt3){
                                            if(iSCnt2 + iSCnt3 >= 0 && iSCnt2 + iSCnt3 < SubPixel
                                                    && iMCnt2 + iMCnt3 >= 0 && iMCnt2 + iMCnt3 < MainPixel
                                                    && bad.Dt[iSCnt2 + iSCnt3][iMCnt2 + iMCnt3] != bad.DtMax){
                                                iSum += Dt[iSCnt2 + iSCnt3][iMCnt2 + iMCnt3];
                                                ++iCnt;
                                            }
                                        }
                                    }
                                    if(iCnt != 0){
                                        SetVal(iSCnt2, iMCnt2, float(iSum) / iCnt);
                                        break;
                                    }else{
                                        ++iSearchCnt;
                                    }
                                }
                            }
                        }
                    }
                }
                        //欠陥のreset
                for(iSCnt2 = iSBad1; iSCnt2 <= iSBad2; ++iSCnt2){
                    for(iMCnt2 = iMBad1; iMCnt2 <= iMBad2; ++iMCnt2){
                        bad.Dt[iSCnt2][iMCnt2] = 0;
                    }
                }
            }
        }
    }
    return(true);
}

/* -------------------------------------------------------------------
* @brief	引数領域が画像範囲内かの確認
* @param	iS1		副走査開始点
* @param	iM1		主走査開始点
* @param	iS2		副走査終了点
* @param	iM2		主走査終了点
* @retval   true:範囲内／false:範囲外
* @date     2021/03/30	T.Taneichi	流用
 --------------------------------------------------------------------- */
bool CTDAT::bCheckArea(const int iS1, const int iM1, const int iS2, const int iM2)
{
    if(iS1 < 0 || iM1 < 0 || iS2 >= SubPixel || iM2 >= MainPixel ||
            iS2 < iS1 || iM2 < iM1){
        return(false);
    }else{
        return(true);
    }
}
/* -------------------------------------------------------------------
* @brief	引数領域が画像範囲内かの確認
* @param	area
* @retval   true:範囲内／false:範囲外
* @date     2021/03/30	T.Taneichi	流用
 --------------------------------------------------------------------- */
bool CTDAT::bCheckArea(TRECTANGLE area)
{
    if(area.GetS0() < 0 || area.GetM0() < 0 || area.GetS1() >= SubPixel || area.GetM1() >= MainPixel ||
            area.GetS1() < area.GetS0() || area.GetM1() < area.GetM0()){
        return(false);
    }else{
        return(true);
    }
}
/* -------------------------------------------------------------------
* @brief	引数領域が画像範囲内かの確認
* @param	area
* @retval   true:範囲内／false:範囲外
* @date     2021/03/30	T.Taneichi	流用
 --------------------------------------------------------------------- */
bool CTDAT::bCheckPos(const int iS, const int iM) const
{
    if(iS < 0 || iM < 0 || iS >= SubPixel || iM >= MainPixel){
        return(false);
    }else{
        return(true);
    }
}
/* -------------------------------------------------------------------
* @brief	画像データをコピーする
* 			コピー先のデータサイズが異なっていてもOK
* 			異なっていた場合は改めて領域を確保する
* @param	destination	コピー先
* @retval   なし
* @date     2021/03/30	T.Taneichi	流用
 --------------------------------------------------------------------- */
void CTDAT::Copy(CTDAT& destination)
{
    int iMainCnt, iSubCnt;

    destination.NewDt(SubPixel, MainPixel, Bit);
    for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
        for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
            destination.Dt[iSubCnt][iMainCnt] = Dt[iSubCnt][iMainCnt];
        }
    }

    // DtMax,Histは更新不要？
}
//---------------------------------------------------------------------------
/*
	copy
*/
void CTDAT::Copy(TFIMG& fimg, const int iBit)
{
    int iMainCnt, iSubCnt;

    int mainPix = fimg.MainPixel;
    int subPix = fimg.SubPixel;

    NewDt(subPix, mainPix, iBit);
    for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
        for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
            SetVal(iSubCnt, iMainCnt, fimg.Dt[iMainCnt][iSubCnt]);
        }
    }
}
/* -------------------------------------------------------------------
* @brief	floatの画像を画像サイズと基準値を指定してコピーする
* @param	fDt				画像データ
* @param	iNormalize		基準データ (この値と画像データを掛けた値を書き込む)
* @param	iMainPixel		主画素サイズ
* @param	iSubPixel		副画素サイズ
* @param	iBit			画像データbit数
* @retval   なし
* @date		???			ttuji		新規
* @date     2021/03/30	T.Taneichi	流用
 --------------------------------------------------------------------- */
void CTDAT::Copy(float* fDt, unsigned short int iNormalize, int iMainPixel, int iSubPixel, int iBit)
{
	int iMainCnt, iSubCnt;

	NewDt(iSubPixel, iMainPixel, iBit);
	for(iMainCnt = 0; iMainCnt < iMainPixel; iMainCnt++){
		for(iSubCnt = 0; iSubCnt < iSubPixel; iSubCnt++){
			SetVal(iSubCnt, iMainCnt, fDt[iMainPixel * iSubCnt + iMainCnt] * iNormalize);
        }
    }
}
//---------------------------------------------------------------------------
/*
	copy
*/
void CTDAT::Copy(TFIMG& destination)
{
    int iMainCnt, iSubCnt;

    destination.NewDt(MainPixel, SubPixel);
    for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
        for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
            destination.Dt[iMainCnt][iSubCnt] = Dt[iSubCnt][iMainCnt];
        }
    }
}
//---------------------------------------------------------------------------
/*
    0でないbadの連結領域をcopyへnValでcopyする
*/
void CTDAT::Copy(CTDAT& bad, const int iM0, const int iS0, CTDAT& copy, const unsigned short int nVal)
{
    if(bad.Dt[iS0][iM0] != 0){
        stack<int, vector<int> > defect;
        stack<int, vector<int> > checked;
        stack<unsigned short int, vector<unsigned short int> > val;

        defect.push(iM0);
        defect.push(iS0);

        checked.push(iM0);
        checked.push(iS0);
        val.push(bad.Dt[iS0][iM0]);

        bad.Dt[iS0][iM0] = 0;

        do{
            int iS = defect.top();
            defect.pop();
            int iM = defect.top();
            defect.pop();

            for(int iSCnt = iS - 1; iSCnt <= iS + 1; ++iSCnt){
                for(int iMCnt = iM - 1; iMCnt <= iM + 1; ++iMCnt){
                    if(bad.bCheckPos(iSCnt, iMCnt) && bad.Dt[iSCnt][iMCnt] != 0){
                        defect.push(iMCnt);
                        defect.push(iSCnt);

                        checked.push(iMCnt);
                        checked.push(iSCnt);
                        val.push(bad.Dt[iSCnt][iMCnt]);

                        bad.Dt[iSCnt][iMCnt] = 0;
                    }
                }
            }
        }while(!defect.empty());

        do{
            int iS = checked.top();
            checked.pop();
            int iM = checked.top();
            checked.pop();
            bad.Dt[iS][iM] = val.top();
            val.pop();
            copy.Dt[iS][iM] = nVal;
        }while(!checked.empty());
    }
}
/* -------------------------------------------------------------------
* @brief	領域を指定してデータをコピーする
* 			書き換わるのは自身のデータで、パラメータで指定した側ではない！
* 			データ
* @param	iOrgSub			書き換え対象の書き換え開始Y座標
* @param	iOrgMain		書き換え対象の書き換え開始X座標
* @param	CopyImg			コピー元データ
* @param	iCopySubPixel	書き換えサイズ(副方向)
* @param	iCopyMainPixel	書き換えサイズ(主方向)
* @param	iCopyOrgSub		コピー元のデータ開始Y座標
* @param	iCopyOrgMain	コピー元のデータ開始X座標
* @retval   なし
* @date     2021/03/30	T.Taneichi	流用
 --------------------------------------------------------------------- */
void CTDAT::CopyFormat(const int iOrgSub, const int iOrgMain,
            CTDAT& CopyImg, const int iCopySubPixel, const int iCopyMainPixel,
                            const int iCopyOrgSub, const int iCopyOrgMain)
{
    int iMainCnt, iSubCnt;

    for(iMainCnt = 0; iMainCnt < iCopyMainPixel; ++iMainCnt){
        for(iSubCnt = 0; iSubCnt < iCopySubPixel; ++iSubCnt){
        	if (iOrgSub+iSubCnt < this->SubPixel && iOrgMain + iMainCnt < this->MainPixel &&
        		iCopyOrgSub + iSubCnt < CopyImg.SubPixel && iCopyOrgMain + iMainCnt < CopyImg.MainPixel){
        		Dt[iOrgSub + iSubCnt][iOrgMain + iMainCnt] =
        				CopyImg.Dt[iCopyOrgSub + iSubCnt][iCopyOrgMain + iMainCnt];
        	}
        }
    }
}

//---------------------------------------------------------------------------
/*
	欠陥補正
    　主方向正常画素範囲で置き換え
*/
bool CTDAT::bCorBadForNoise(CTDAT &DefectImage)
{
    if(MainPixel != DefectImage.MainPixel || SubPixel != DefectImage.SubPixel){
        return(false);
    }

    int iMCnt,  iSCnt;
    int iMNormalPos;

    for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
            if(DefectImage.Dt[iSCnt][iMCnt] == DefectImage.DtMax){
                iMNormalPos = iMCnt - 1;
                do{
                    if(iMNormalPos > 0){
                        while(iMNormalPos > 0 && DefectImage.Dt[iSCnt][iMNormalPos] == DefectImage.DtMax){
                            --iMNormalPos;
                        }
                    }else{
                        iMNormalPos = 0;
                    }
                    Dt[iSCnt][iMCnt] =  Dt[iSCnt][iMNormalPos];
                    --iMNormalPos;
                    ++iMCnt;
                }while(iMCnt < DefectImage.MainPixel && DefectImage.Dt[iSCnt][iMCnt] == DefectImage.DtMax);
            }
        }
    }
    return(true);
}
/* -------------------------------------------------------------------
* @brief	欠陥検出　欠陥画像として上書き
* 			正常：1 / 欠陥：DtMax
* @param	iMedianLength		メディアンフィルタ長
* @param	dDetectTh			DefectImgの判定用閾値
* @param	dHemTh				candidateImgの判定用閾値
* @retval   なし
* @date     2021/03/30	T.Taneichi	流用
 --------------------------------------------------------------------- */
void CTDAT::DetectDefect(const int iMedianLength, double dDetectTh, double dHemTh)
{
	CTDAT MedianImg;
    int iMCnt, iSCnt;
    double  dSS, dS, dN, dSigma;
    double  dDif;

    Copy(MedianImg);
    MedianImg.SubMedian(3);
    dSS = dS = 0.0;
    for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
            dDif = Dt[iSCnt][iMCnt] - MedianImg.Dt[iSCnt][iMCnt];
            dSS += dDif * dDif;
            dS += dDif;
        }
    }
    dN = MainPixel * SubPixel;
    dSigma = sqrt((dSS - dS * dS / dN) / dN);
    dDetectTh *= dSigma;
    dHemTh *= dSigma;

    CTDAT    DefectImg, candidateImg;
    int iXCnt,  iYCnt;
    double  dAbsDif;

    DefectImg.NewDt(SubPixel, MainPixel, Bit);
    candidateImg.NewDt(SubPixel, MainPixel, Bit);

    Copy(MedianImg);
    MedianImg.MainMedian(iMedianLength);
    for(iYCnt = 0; iYCnt < SubPixel; ++iYCnt){
        for(iXCnt = 0; iXCnt < MainPixel; ++iXCnt){
            dAbsDif = fabs(Dt[iYCnt][iXCnt] - MedianImg.Dt[iYCnt][iXCnt]);
            if(dAbsDif > dDetectTh){
                DefectImg.Dt[iYCnt][iXCnt] = 1;
            }else{
                DefectImg.Dt[iYCnt][iXCnt] = 0;
            }
            if(dAbsDif > dHemTh){
                candidateImg.Dt[iYCnt][iXCnt] = 1;
            }else{
                candidateImg.Dt[iYCnt][iXCnt] = 0;
            }
        }
    }
    MedianImg.DeleteDt();

    int iXSearch,   iYSearch;

    FillValue(0);
    for(iYCnt = 0; iYCnt < SubPixel; ++iYCnt){
        for(iXCnt = 0; iXCnt < MainPixel; ++iXCnt){
            if(DefectImg.Dt[iYCnt][iXCnt] == 1){
                iXSearch = iXCnt;
                while(iXSearch >= 0 && candidateImg.Dt[iYCnt][iXSearch] == 1){
                    Dt[iYCnt][iXSearch] = DtMax;
                    --iXSearch;
                }
                ++iXCnt;
                while(iXCnt < candidateImg.MainPixel && candidateImg.Dt[iYCnt][iXCnt] == 1){
                    Dt[iYCnt][iXCnt] = DtMax;
                    ++iXCnt;
                }
            }
        }
    }
    for(iXCnt = 0; iXCnt < MainPixel; ++iXCnt){
        for(iYCnt = 0; iYCnt < SubPixel; ++iYCnt){
            if(Dt[iYCnt][iXCnt] == DtMax){
//            if(DefectImg.Dt[iYCnt][iXCnt] == 1){
                iYSearch = iYCnt;
                while(iYSearch >= 0 && candidateImg.Dt[iYSearch][iXCnt] == 1){
                    Dt[iYSearch][iXCnt] = DtMax;
                    --iYSearch;
                }
                ++iYCnt;
                while(iYCnt < candidateImg.SubPixel && candidateImg.Dt[iYCnt][iXCnt] == 1){
                    Dt[iYCnt][iXCnt] = DtMax;
                    ++iYCnt;
                }
            }
        }
    }
}
/* -------------------------------------------------------------------
* @brief	主方向差分のデータとする
* 			X=0の画素値はそのままで、X>0の画素値はDt[X-1] - Dt[X]となる
* 			値が範囲内に収まるように、DMax/2でかさ上げする
* @param	なし
* @retval   なし
* @date     2021/03/30	T.Taneichi	流用
 --------------------------------------------------------------------- */
void CTDAT::DifMainDown()
{
    CTDAT copy;
    Copy(copy);
    int iH = DtMax / 2 + 1;
    for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        for(int iMCnt = 1; iMCnt < MainPixel; ++iMCnt){
            SetVal(iSCnt, iMCnt, copy.Dt[iSCnt][iMCnt - 1] - copy.Dt[iSCnt][iMCnt] + iH);
        }
    }
}

//---------------------------------------------------------------------------
/*
	画像主方向差分
        Dt[0]はそのまま
        Dt[1]は Dt[1] - Dt[0]
*/
void CTDAT::DifMainUp()
{
    CTDAT copy;
    Copy(copy);
    int iH = DtMax / 2 + 1;
    for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        for(int iMCnt = 1; iMCnt < MainPixel; ++iMCnt){
            SetVal(iSCnt, iMCnt, copy.Dt[iSCnt][iMCnt] - copy.Dt[iSCnt][iMCnt - 1] + iH);
        }
    }
}
//---------------------------------------------------------------------------
/*
	画像副方向差分
        Dt[0]はそのまま
        Dt[1]は Dt[0] - Dt[1]
*/
void CTDAT::DifSubDown()
{
    CTDAT copy;
    Copy(copy);
    int iH = DtMax / 2 + 1;
    for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
        for(int iSCnt = 1; iSCnt < SubPixel; ++iSCnt){
            SetVal(iSCnt, iMCnt, copy.Dt[iSCnt - 1][iMCnt] - copy.Dt[iSCnt][iMCnt] + iH);
        }
    }
}
//---------------------------------------------------------------------------
/*
	画像副方向差分
        Dt[0]はそのまま
        Dt[1]は Dt[1] - Dt[0]
*/
void CTDAT::DifSubUp()
{
   CTDAT copy;
    Copy(copy);
    int iH = DtMax / 2 + 1;
    for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
        for(int iSCnt = 1; iSCnt < SubPixel; ++iSCnt){
            SetVal(iSCnt, iMCnt, copy.Dt[iSCnt][iMCnt] - copy.Dt[iSCnt - 1][iMCnt] + iH);
        }
    }
}

//---------------------------------------------------------------------------
/*
   座標指定して、値を取得
    指定領域が画像範囲外の時は端の値を返す
*/
unsigned short int CTDAT::nGetVal(int iS, int iM)
{
    if(iS < 0){
        iS = 0;
    }else if(iS >= SubPixel){
        iS =  SubPixel - 1;
    }
    if(iM < 0){
        iM = 0;
    }else if(iM >= MainPixel){
        iM =  MainPixel - 1;
    }
    return(Dt[iS][iM]);
}
/* -------------------------------------------------------------------
* @brief	降順にソートされている配列を渡したときに、該当画素値のデータが配列の何番目に入っているかを返す
* 			再帰呼び出し
* @param	nVal		比較値
* @param	nDt[]		対象配列		iEで指定する要素数が確保されていること
* @param	iS			検索先頭番号
* @param	iE			検索末尾番号
* @retval   先頭からの要素番号
* @date     2021/04/01	T.Taneichi	流用
 --------------------------------------------------------------------- */
int CTDAT::iFindPos(const unsigned short int nVal,
            unsigned short int nDt[],
            int iS,
            int iE)
{
    int iHalf = (iS + iE) / 2;

    if(nDt[iHalf] < nVal){
        return(iFindPos(nVal, nDt, iHalf + 1, iE));
    }else if(nDt[iHalf] > nVal){
        return(iFindPos(nVal, nDt, iS, iHalf - 1));
    }else{
        return(iHalf);
    }
}
//---------------------------------------------------------------------------
//	上下byte入れ替え
void CTDAT::LittleBig()
{
    int iMCnt, iSCnt;

    for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
			Dt[iSCnt][iMCnt] =
				(unsigned short)((Dt[iSCnt][iMCnt] << 8) + (Dt[iSCnt][iMCnt] >> 8));
		}
    }
}
/* -------------------------------------------------------------------
* @brief	Log変換
* @param	dMid	中心値
* @param	dL		Latitude
* @retval   true:範囲内／false:範囲外
* @date     2021/03/30	T.Taneichi	流用
 --------------------------------------------------------------------- */
void CTDAT::Log(const double dMid, const double dL)
{
    CTDAT    table;				//LUT
	double dOffset, dTemp;
    int iQLCnt;
	int iMCnt, iSCnt;

	table.NewDt(1, DtMax + 1, Bit);
	//計算簡略化(ttsuji)
	dOffset = (DtMax + 1) / 2;//16ビットLin中心値948を14ビットLOG8192に変換 (171025)
	dTemp = double(DtMax + 1) / dL;
	table.Dt[0][0] = 0;
	for (iQLCnt = 1; iQLCnt <= DtMax; iQLCnt++){
		table.SetVal(0, iQLCnt, dTemp * log10(iQLCnt / dMid) + dOffset);
	}
	for (iSCnt = 0; iSCnt < SubPixel; iSCnt++){
		for (iMCnt = 0; iMCnt < MainPixel; iMCnt++){
			Dt[iSCnt][iMCnt] = table.Dt[0][Dt[iSCnt][iMCnt]];
		}
	}

}
/* -------------------------------------------------------------------
* @brief	閾値処理付きLog変換
* @param	dMid	中心値
* @param	dL		Latitude
* @retval   true:範囲内／false:範囲外
* @date     2021/03/30	T.Taneichi	流用
 --------------------------------------------------------------------- */
void CTDAT::LogThreshold(const double dMid, const double dL, const int iTh)
{
	CTDAT    table;
	double dOffset, dTemp;
	int iQLCnt;
	int iMCnt, iSCnt;

	if (iTh > DtMax || iTh < 0) {
		return;
	}

	table.NewDt(1, DtMax + 1, Bit);
//	dOffset = DtMax / 2;      //16ビットLin中心値948を14ビットLOG8191に変換
	dOffset = (DtMax + 1) / 2;//16ビットLin中心値948を14ビットLOG8192に変換 (171025)
	dTemp = double(DtMax + 1) / dL;
	table.Dt[0][0] = 0;
	for (iQLCnt = 1; iQLCnt <= DtMax; iQLCnt++){
//		table.SetVal(0, iQLCnt, dTemp * log10(iQLCnt / dMid) + dOffset);//四捨五入
		table.SetValWithTruncation(0, iQLCnt, dTemp * log10(iQLCnt / dMid) + dOffset);//切り捨て
	}

	int iLogTh = table.Dt[0][iTh - 1];

	for (iSCnt = 0; iSCnt < SubPixel; iSCnt++){
		for (iMCnt = 0; iMCnt < MainPixel; iMCnt++){
			if (Dt[iSCnt][iMCnt] < iTh) {
				Dt[iSCnt][iMCnt] = table.Dt[0][Dt[iSCnt][iMCnt]];
			}
        	//閾値以上のリニアQLに対しては一定の上限値でクリップ
			else{
				Dt[iSCnt][iMCnt] = iLogTh;
			}
		}
	}

#if 0
	//table text output
	FILE* fp1 = fopen("C:\\LogTableThreshold16.txt", "wt");
	FILE* fp2 = fopen("C:\\LogTableThreshold14.txt", "wt");
//	for (iQLCnt = 0; iQLCnt <= DtMax; iQLCnt+=10)
	for (iQLCnt = 0; iQLCnt <= DtMax; iQLCnt++)
	{
		if (iQLCnt < iTh) {
			fprintf(fp1, "%d %d\n", iQLCnt, table.Dt[0][iQLCnt]);
			fprintf(fp2, "%d %d\n", iQLCnt, (table.Dt[0][iQLCnt]>> 2));
		}
		else{
			fprintf(fp1, "%d %d\n", iQLCnt, iLogTh);
			fprintf(fp2, "%d %d\n", iQLCnt, (iLogTh >> 2));
		}
	}
	fclose(fp1);
	fclose(fp2);
#endif
}
/* -------------------------------------------------------------------
* @brief	LOG変換後2ビットダウン
* @param	dMid		中心値
* @param	dL			Latitude
* @retval   なし
* @date     2021/04/01	T.Taneichi	流用
 --------------------------------------------------------------------- */
void CTDAT::Log2BitDown(const double dMid,       const double dL)
{
    CTDAT    table;
    double dOffset, dTemp;
    int iQLCnt;
    int iMCnt, iSCnt;

    table.NewDt(1, DtMax + 1, Bit - 2);//14bitデータを65536個用意

    //16bitLinear→14bitLog変換テーブルを作成
	dOffset = (DtMax+1) / 2 - (DtMax + 1) / dL * log10(dMid);//16ビットLin中心値948を14ビットLOG8192に変換 (171025 )
	dTemp = double(DtMax + 1) / dL;
    table.Dt[0][0] = 0;
	for(iQLCnt = 1; iQLCnt <= DtMax; iQLCnt++){
		table.SetVal(0, iQLCnt, ((int)(dTemp * log10(iQLCnt) + dOffset + 0.5) >> 2));//LOGテーブルを2bitシフト
    }

    //14bit画像に変更（画素値Dtは16bitのまま）
	SetBit(Bit - 2);

    for(iSCnt = 0; iSCnt < SubPixel; iSCnt++){
        for(iMCnt = 0; iMCnt < MainPixel; iMCnt++){
            Dt[iSCnt][iMCnt] = table.Dt[0][Dt[iSCnt][iMCnt]];
        }
    }
}
//---------------------------------------------------------------------------
//低線量でリニアなLOG変換（2ビットダウン）      ttsuji 141017
void CTDAT::Lin16ToLog14_LowDoseLinear(const double dMid,       //中心値
               const double dL)         //Latitude
{
    CTDAT    table;
	double dOffset, dTemp;
    int iQLCnt;
    int iMCnt, iSCnt;

	table.NewDt(1, DtMax + 1, Bit - 2);//14bitデータを65536個用意

    //16bitLinear→14bitLog変換テーブルを作成
	dOffset = DtMax / 2 - (DtMax + 1) / dL * log10(dMid);
    dTemp = double(DtMax + 1) / dL;
    table.Dt[0][0] = 0;
	for(iQLCnt = 1; iQLCnt <= DtMax; iQLCnt++){
		table.SetVal(0, iQLCnt, ((int)(dTemp * log10(iQLCnt) + dOffset + 0.5) >> 2));//LOGテーブルを2bitシフト
	}

	//低線量域のリニアテーブル
	const int iLin16 = 30;
	const int iLog14 = 2049;
	double dCoef = (double)iLog14 / iLin16;
	for(iQLCnt = 0; iQLCnt <= iLin16; iQLCnt++){
		table.SetVal(0, iQLCnt, round(dCoef * iQLCnt));//線形テーブル
	}

  //test
	FILE* fp1 = fopen("Lin16ToLog14_LowDoseLinear.txt", "wt");
//	for(iQLCnt = 0; iQLCnt <= DtMax; iQLCnt+=10)
	for(iQLCnt = 0; iQLCnt <= DtMax; iQLCnt++)
    {
//        fprintf(fp1, "%d %d\n", iQLCnt, table.Dt[0][iQLCnt]);
		fprintf(fp1, "%d,\n", table.Dt[0][iQLCnt]);//MC形式
	}
	fclose(fp1);

    //14bit画像に変更（画素値Dtは16bitのまま）
	SetBit(Bit - 2);

    for(iSCnt = 0; iSCnt < SubPixel; iSCnt++){
        for(iMCnt = 0; iMCnt < MainPixel; iMCnt++){
            Dt[iSCnt][iMCnt] = table.Dt[0][Dt[iSCnt][iMCnt]];
        }
    }
}
//---------------------------------------------------------------------------
/*
	Log->linear

		引数	ilength -> filter長
		dC: 9.48
		dK: 4096
		dOffset: 0
		iNewBit: 16
*/
void CTDAT::LogToLinear(const double dC, const double dK, const double dOffset,
        const int iNewBit)
{
    int iCnt;
    int iMCnt, iSCnt;
    CTDAT table;     //変換tableとして使用

// anti log変換table作成
    table.NewDt(1, DtMax + 1, iNewBit);
    for(iCnt = 0; iCnt <= DtMax; ++iCnt){
        table.SetVal(0, iCnt, dC * pow(10.0, iCnt / dK) + dOffset);
    }

    SetBit(iNewBit);
	for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt)
    	for(iMCnt = 0; iMCnt < MainPixel ; ++iMCnt)
            Dt[iSCnt][iMCnt] = table.Dt[0][Dt[iSCnt][iMCnt]];
}
//---------------------------------------------------------------------------
//	Log->linear	   ttsuji追加（LOG変換時の変数で指定）
void CTDAT::LogToLinear2(const int iBit, const double dCenter, const int iKeta)
{
	int iCnt;
	int iMCnt, iSCnt;
    CTDAT table;     //変換tableとして使用

    double tmp  = iKeta / pow(2.0, iBit);
    double tmp2 = -pow(2.0, iBit - 1);

// anti log変換table作成
#ifndef _14LOG_TABLE
	table.NewDt(1, DtMax + 1, iBit);
#else
	table.NewDt(1, DtMax + 1, iBit + 2);//14bitLog->16bitLinearテーブルを直接作成する
	SetBit(iBit + 2);
#endif
	for (iCnt = 0; iCnt <= DtMax; iCnt++){
		table.SetVal(0, iCnt, dCenter * pow(10.0, tmp * (iCnt + tmp2)));
	}

	SetBit(iBit);
    for (iSCnt = 0; iSCnt < SubPixel; iSCnt++){
    	for (iMCnt = 0; iMCnt < MainPixel ; iMCnt++){
            Dt[iSCnt][iMCnt] = table.Dt[0][Dt[iSCnt][iMCnt]];
        }
	}

}
//---------------------------------------------------------------------------
//低線量でリニアな逆LOG変換     ttsuji 141017

void CTDAT::Log14ToLin16_LowDoseLinear(const int iBit, const double dCenter, const int iKeta)
{
	int iCnt;
    int iMCnt, iSCnt;
    CTDAT table;     //変換tableとして使用

    double tmp  = iKeta / pow(2.0, iBit);
    double tmp2 = -pow(2.0, iBit - 1);

// anti log変換table作成
	table.NewDt(1, DtMax + 1, iBit + 2);//14bitLog->16bitLinearテーブルを直接作成する
	int DtMax14 = DtMax;
	for (iCnt = 0; iCnt <= DtMax; iCnt++){
		table.SetVal(0, iCnt, dCenter * pow(10.0, tmp * (iCnt + tmp2)));
	}

	//低線量域のリニアテーブル
	const int iLin16 = 30;
	const int iLog14 = 2049;
	double dCoef = (double)iLin16 / iLog14;
	for(iCnt = 0; iCnt <= iLog14; iCnt++){
		table.SetVal(0, iCnt, round(dCoef * iCnt));//線形テーブル
	}


	SetBit(iBit + 2);
// 	SetBit(iBit);
	for (iSCnt = 0; iSCnt < SubPixel; iSCnt++){
    	for (iMCnt = 0; iMCnt < MainPixel ; iMCnt++){
            Dt[iSCnt][iMCnt] = table.Dt[0][Dt[iSCnt][iMCnt]];
        }
	}

}
//---------------------------------------------------------------------------
/*
	主方向Median filter処理
		両端は1, 3,,,ilengthまで広げながら処理する。
        filter長>画素数の時何もしない。

    	引数	ilength -> filter長
*/
/*
void TDAT::MainMedian(const int ilength)
{

    int iXCnt, iYCnt, iCnt;
    int *piOrder;       //順番
    unsigned short int *pnOriginal;        //original data
    int iOrder;     //追加dataの順位を示すcounter
    int iIncludePos, iRemovePos;        //data追加位置と除去位置
    int iCrtPos;        //補正位置
    int iEndOrder = ilength - 1;
    int iMedian;

    if(ilength > MainPixel) return;

    piOrder = new int[ilength];
    pnOriginal = new unsigned short int[MainPixel];

    for(iYCnt = 0 ; iYCnt < SubPixel; ++iYCnt){
        for(iXCnt = 0 ; iXCnt < MainPixel ; ++iXCnt){
            pnOriginal[iXCnt] = Dt[iYCnt][iXCnt];
        }

        iMedian = 0;
        iCrtPos = 0;
        piOrder[0] = 0;
        iIncludePos = 1;
        while(iIncludePos < ilength){
            iOrder = iIncludePos;
            for(iCnt = 0; iCnt < iIncludePos; ++iCnt){
                if(pnOriginal[iCnt] > pnOriginal[iIncludePos]){
                    ++piOrder[iCnt];
                    --iOrder;
                }
            }
            piOrder[iIncludePos] = iOrder;
            ++iIncludePos;
            iOrder = iIncludePos;
            for(iCnt = 0; iCnt < iIncludePos; ++iCnt){
                if(pnOriginal[iCnt] > pnOriginal[iIncludePos]){
                    ++piOrder[iCnt];
                    --iOrder;
                }
            }
            ++iMedian;
            ++iCrtPos;
            piOrder[iIncludePos] = iOrder;
            for(iCnt = 0; iCnt <= iIncludePos; ++iCnt){
                if(piOrder[iCnt] == iMedian){
                    Dt[iYCnt][iCrtPos] = pnOriginal[iCnt];
                    break;
                }
            }
            ++iIncludePos;
        }

        iRemovePos = 0;
        for(; iIncludePos < MainPixel ; ++iIncludePos){
            iOrder = iEndOrder;
            if(piOrder[0] == iMedian) Dt[iYCnt][iCrtPos] = pnOriginal[iRemovePos];
            for(iCnt = 1 ; iCnt < ilength ; ++iCnt){
                if(piOrder[iCnt] == iMedian) Dt[iYCnt][iCrtPos] = pnOriginal[iRemovePos + iCnt];
                if(pnOriginal[iRemovePos + iCnt] >= pnOriginal[iRemovePos]){
                    piOrder[iCnt - 1] = piOrder[iCnt] - 1;
                }else{
                    piOrder[iCnt - 1] = piOrder[iCnt];
                }
                if(pnOriginal[iRemovePos + iCnt] > pnOriginal[iIncludePos]){
                    ++piOrder[iCnt - 1];
                    --iOrder;
                }
            }
            piOrder[iEndOrder] = iOrder;
            ++iRemovePos;
            ++iCrtPos;
        }
        while(iRemovePos < MainPixel - 1){
            if(piOrder[0] == iMedian) Dt[iYCnt][iCrtPos] = pnOriginal[iRemovePos];
            for(iCnt = iRemovePos + 1; iCnt < MainPixel; ++iCnt){
                if(piOrder[iCnt - iRemovePos] == iMedian) Dt[iYCnt][iCrtPos] =
                        pnOriginal[iCnt];
                if(pnOriginal[iCnt] >= pnOriginal[iRemovePos]){
                    piOrder[iCnt - iRemovePos - 1] = piOrder[iCnt - iRemovePos] - 1;
                }else{
                    piOrder[iCnt - iRemovePos - 1] = piOrder[iCnt - iRemovePos];
                }
            }
            ++iRemovePos;
            for(iCnt = iRemovePos + 1; iCnt < MainPixel; ++iCnt){
                if(pnOriginal[iCnt] >= pnOriginal[iRemovePos]){
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
    delete[] pnOriginal;
}
*/
/* -------------------------------------------------------------------
* @brief	主方向線欠陥検出
* 			欠陥画像に対して主走査方向にiWThだけ欠陥が続いている場合のみ欠陥を残し、それ以外は0にする。
* @param	iM0			主検出領域開始点
* @param	iM1			主検出領域終了点
* @param	iWTh		欠陥長閾値
* @param	result
* @retval   なし
* @date     2021/04/01	T.Taneichi	流用
 --------------------------------------------------------------------- */
void CTDAT::MainBinary(const int iM0, const int iM1 ,const int iWTh, CTDAT& result)
{
	//TODO パラメータチェック

	if(result.MainPixel != MainPixel && result.SubPixel != SubPixel){
		result.NewDt(SubPixel, MainPixel, Bit);
	}

	for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
		for(int iMCnt = iM0; iMCnt < iM1; ++iMCnt){
			if(Dt[iSCnt][iMCnt]){
				int iMSrt = iMCnt;
				do{
					++iMCnt;
				}while(iMCnt < iM1 && Dt[iSCnt][iMCnt]);
				if(iMCnt - iMSrt > iWTh){ 	//DtがiWTh以上に続いた場合にはDtMaxで埋める
					for(int i = iMSrt; i < iMCnt; ++i){
						result.Dt[iSCnt][i] = result.DtMax;
					}
				}else{       				//DtがiWTh以上に続かない場合には0に戻す
					for(int i = iMSrt; i < iMCnt; ++i){
						result.Dt[iSCnt][i] = 0;
					}
				}
				--iMCnt;
			}else{
				result.Dt[iSCnt][iMCnt] = 0;
			}
		}
	}
}

void CTDAT::Median( const int iSNum, const int iMNum)
{
     const int iMSNum = iMNum*iSNum;  // filter面積

     // filter長が長いか偶数のとき、何もしない。
     if(iMNum > MainPixel || iSNum > SubPixel || (iMSNum % 2 == 0)){
		return;
     }

     // 端を伸ばした画像を用意／ついでに元画像の保存にもなっている。
     CTDAT ImageHashi;
     ImageHashi.NewDt(SubPixel+iSNum-1, MainPixel+iMNum-1, Bit);
       // bulkはそのまま (端じゃないのでコピー)
     for(int iS=0; iS<SubPixel; iS++){
       for(int iM=0; iM<MainPixel; iM++){
         ImageHashi.Dt[iS+iSNum/2][iM+iMNum/2] = Dt[iS][iM];
       }
     }
       // 斜め4ブロックは角データのコピー
     for(int iS=0; iS<iSNum/2; iS++){
       for(int iM=0; iM<iMNum/2; iM++){
         ImageHashi.Dt[iS][iM] = Dt[0][0];
         ImageHashi.Dt[iS][MainPixel+iMNum-iM-2] = Dt[0][MainPixel-1];
         ImageHashi.Dt[SubPixel+iSNum-iS-2][MainPixel+iMNum-iM-2] = Dt[SubPixel-1][MainPixel-1];
         ImageHashi.Dt[SubPixel+iSNum-iS-2][iM] = Dt[SubPixel-1][0];
       }
     }
       // 上下2ブロックは辺のコピー
     for(int iS=0; iS<iSNum/2; iS++){
       for(int iM=0; iM<MainPixel; iM++){
         ImageHashi.Dt[iS][iM+iMNum/2] = Dt[0][iM];
         ImageHashi.Dt[SubPixel+iSNum-iS-2][iM+iMNum/2] = Dt[SubPixel-1][iM];
        }
     }
	   // 左右2ブロックも辺のコピー
     for(int iS=0; iS<SubPixel; iS++){
       for(int iM=0; iM<iMNum/2; iM++){
         ImageHashi.Dt[iS+iSNum/2][iM] = Dt[iS][0];
         ImageHashi.Dt[iS+iSNum/2][MainPixel+iMNum-iM-2] = Dt[iS][MainPixel-1];
       }
     }


	 unsigned short *Buff;                // 1次元バッファ。sorted
     Buff = new unsigned short [iMSNum];
     unsigned short int iRem;       // バッファから除く要素
     unsigned short int iAdd;       // バッファに加える要素
     int iMed = iMSNum/2;
     int iiM;    // 逆向き主走査用 iMの補数
     int iR, iA; // 逆向き主走査用 iRem,iAddの補正値

     // Buffを初期化する。
     int iBuffNum = 0;   // ループ中の一時変数
     for(int iScell=0; iScell<iSNum; iScell++){
       for(int iMcell=0; iMcell<iMNum; iMcell++){
         Buff[iBuffNum++] = ImageHashi.Dt[iScell][iMcell];
       }
     }
	 qsort( Buff, iMSNum, sizeof(unsigned short), Cmp); // 最初はqsortで。

	 for(int iS=0; iS<SubPixel; iS++){   // セルを画像上で走査し、中身を順次更新する。
       for(int iM=0; iM<MainPixel; iM++){
		 if( (iS & 1) == 1 ){
		   iiM = MainPixel-iM-1;        //  奇数行目は逆に主走査する
           iR = iMNum-1;                //
           iA = -iMNum-1;               //
         } else {
           iiM = iM;                    //  偶数行目は順方向に主走査する。
           iR = 0;                      //
           iA = 0;                      //
         }

         Dt[iS][iiM] = Buff[iMed];      //  ここでmedian取り出し

         if( (iS+1)>=SubPixel && (iM+1)>= MainPixel ){
           break;
         }
         // ↑上記breakは、2重ループの最後で↓下記の配列アクセスで範囲外を指すため

         // 主走査が端まで達したら、セル上部の要素を捨てて、一段下がる
         if( (iM+1)==MainPixel ){
           for(int iMcell=0; iMcell<iMNum; iMcell++){
             iRem = ImageHashi.Dt[iS][iiM+iMcell];
             iAdd = ImageHashi.Dt[iS+iSNum][iiM+iMcell];
             RemoveAddDt( Buff, iMSNum, iRem, iAdd);

           }
         // 主走査の途中なら右(or左)のデータを読み込み古いのを捨てる
         } else {
           for(int iScell=0; iScell<iSNum; iScell++){
             iRem = ImageHashi.Dt[iS+iScell][iiM+iR];
             iAdd = ImageHashi.Dt[iS+iScell][iiM+iMNum+iA];
			 RemoveAddDt( Buff, iMSNum, iRem, iAdd);

           }
         }
       }
     }

     delete[] Buff;
}


//---------------------------------------------------------------------------
/*
      2D-median filter処理
        端は端画素をコピーして伸ばし、普通にmedian filterする。
        filter長>画素数 or filter長=偶数の時 何もしない。
      引数    iMNum, iSNum  → 主副filter長
      速度のネックはRemoveAddDtであるので、ループ中の変数の使い方は効率より見やすさを優先。
*/
// ソートされた配列BuffからiRemを除いてiAddを足す
void CTDAT::RemoveAddDt( unsigned short *Buff, const int num, const unsigned short iRem, const unsigned short iAdd)
{
   int iOrder = iFindPos(iRem, Buff, 0, num-1);   // 取り除く位置をfind
   //  取り除く位置より後ろに追加する場合は、他要素を前に少し詰める
   if( iAdd >= iRem){
     ++iOrder;
     while(iOrder < num && Buff[iOrder] < iAdd){
       Buff[iOrder - 1] = Buff[iOrder];
       ++iOrder;
     }
     Buff[iOrder - 1] = iAdd;   // 要素追加
     //  取り除く位置より前に追加する場合は、他要素を後ろに少しずらす
   } else {
     --iOrder;
     while(iOrder >= 0 && Buff[iOrder] > iAdd){
       Buff[iOrder + 1] = Buff[iOrder];
       --iOrder;
     }
     Buff[iOrder + 1] = iAdd;   // 要素追加
   }
}


/* -------------------------------------------------------------------
* @brief	Median処理をかける
* 			欠陥画像に対して主走査方向にiWThだけ欠陥が続いている場合のみ欠陥を残し、それ以外は0にする。
* @param	iM0			主検出領域開始点
* @param	iM1			主検出領域終了点
* @param	iWTh		欠陥長閾値
* @param	result
* @retval   なし
* @date     2021/04/01	T.Taneichi	流用
 --------------------------------------------------------------------- */
unsigned short int CTDAT::nMedian(int& iS0, int& iM0, int& iS1, int& iM1)
{
    TrimArea(iS0, iM0, iS1, iM1);
    int iNum = (iM1 - iM0 + 1) * (iS1 - iS0 + 1);
    unsigned short int* pnD = new unsigned short int[iNum];

    int iCnt = 0;
    for(int iSCnt = iS0; iSCnt <= iS1; ++iSCnt){
        for(int iMCnt = iM0; iMCnt <= iM1; ++iMCnt){
            pnD[iCnt] = Dt[iSCnt][iMCnt];
            ++iCnt;
        }
    }
    qsort(pnD, iNum, 2, Cmp);
    unsigned short int nResult = pnD[iNum / 2];
    delete pnD;
    return(nResult);
}
//---------------------------------------------------------------------------
//	Read 1Byte Data
//    	引数	File Name
//        戻り値	成功->true		失敗->false
bool CTDAT::Read1Byte(const string fileName)
{
#if 0
    int handle;		//file handler

    if((handle = open(fileName.c_str(), O_RDONLY|O_BINARY)) == -1){
    	return(false);
	}

    int fileLen = filelength(handle);
	if(fileLen != SubPixel * MainPixel){
    	return(false);
    }

	unsigned char* buffer = new unsigned char[fileLen];		//Bufferを確保
    if((read(handle, buffer, fileLen)) == -1){
        close(handle);
        delete[] buffer;
		return(false);
    }

    close(handle);
    NewDt(SubPixel, MainPixel, Bit);
    int iCt = 0;
    for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
            Dt[iSCnt][iMCnt] = buffer[iCt];
            ++iCt;
        }
    }
    delete[] buffer;
#endif
    return(true);
}
//---------------------------------------------------------------------------
/*
	Read Data
    	引数	File Name

        戻り値	成功->true
        		失敗->false
*/
bool CTDAT::ReadDt(const string fileName)
{
    int i;

	NewDt(SubPixel, MainPixel, Bit);

	FILE* fp = fopen(fileName.c_str(), "rb");
	if (!fp) {
		CApplication::GetInstance()->MessageBox((fileName + " openできませんでした").c_str(), "Error", MB_OK);
		return false;
	}

	for(i = 0 ; i < SubPixel ; ++i){
		size_t size = fread(Dt[i], sizeof(unsigned short int), MainPixel, fp);
		if (size != MainPixel) {
			CApplication::GetInstance()->MessageBox((fileName + "\n" + to_string(i) + "行目をreadできませんでした").c_str(), "Error", MB_OK);
			fclose(fp);
			return false;
		}
	}

	fclose(fp);

	return true;
}
//---------------------------------------------------------------------------
/*
	ヘッダ付画像のヘッダをとばして画像データのみを読み込む(ttsuji追加)
	（DICOMデータやfcrデータを扱う際に用いる）
*/
bool CTDAT::ReadDtSkipHead(const string fileName)
{
	if (!MainPixel || !SubPixel) {
		CApplication::GetInstance()->MessageBox("主副画素数の設定が不正です", "Error", MB_OK);
		return false;
	}
	NewDt(SubPixel, MainPixel, Bit);

	ifstream inFile(fileName.c_str(), ios::in | ios::binary);
	if(!inFile){
		CApplication::GetInstance()->MessageBox((fileName + " openできませんでした").c_str(), "Error", MB_OK);
		return false;
	}
	// ファイルサイズ計算
	long file_size, head_size;
	inFile.seekg(0, ios::end);
	file_size = inFile.tellg();
	inFile.seekg(0);
	head_size = file_size - MainPixel * SubPixel * 2;
	if(head_size < 0){
		CApplication::GetInstance()->MessageBox("不正なファイルサイズの画像です", "Error", MB_OK);
		inFile.close();
		return false;
	}

	int i;
	char tmp;//1byte
	unsigned short int data;//2byte
	for(i = 0; i < head_size; i++){
		inFile.read((char*)&tmp, sizeof(char));//読み飛ばし
	}
	for(i = 0 ; i < SubPixel ; i++){
		inFile.read((char*)Dt[i], sizeof(unsigned short int) * MainPixel);
	}
	inFile.close();

	return true;
}
//---------------------------------------------------------------------------
/*
	ヘッダ付の動画データ（複数フレーム画像データ）(*.sif)を、
	ヘッダなしの複数の画像データに分解する(ttsuji追加)
*/
bool CTDAT::ReadFramesSkipHead(const string fileName, const int NumOfFrame)
{
	if (!MainPixel || !SubPixel) {
		CApplication::GetInstance()->MessageBox("主副画素数の設定が不正です", "Error", MB_OK);
		return false;
	}
	if (1 > NumOfFrame) {
		CApplication::GetInstance()->MessageBox("分解フレーム数の設定が不正です", "Error", MB_OK);
		return false;
	}
	NewDt(SubPixel, MainPixel, Bit);

	ifstream inFile(fileName.c_str(), ios::in | ios::binary);
	if(!inFile){
		CApplication::GetInstance()->MessageBox((fileName + " openできませんでした").c_str(), "Error", MB_OK);
		return false;
	}
	// ファイルサイズ計算
	long file_size, head_size;
	inFile.seekg(0, ios::end);
	file_size = inFile.tellg();
	inFile.seekg(0);
	head_size = file_size - MainPixel * SubPixel * 2 * NumOfFrame;
	if(head_size < 0){
		CApplication::GetInstance()->MessageBox("不正なファイルサイズの画像です", "Error", MB_OK);
		inFile.close();
		return false;
	}

	int i, iFrame;
	char tmp;//1byte
	unsigned short int data;//2byte
	for (i = 0; i < head_size; i++){
		inFile.read((char*)&tmp, sizeof(char));//読み飛ばし
	}
	for (iFrame = 0 ; iFrame < NumOfFrame ; iFrame++){
		for (i = 0 ; i < SubPixel ; i++){
			inFile.read((char*)Dt[i], sizeof(unsigned short int) * MainPixel);
		}

	}
	inFile.close();

	return true;
}
//---------------------------------------------------------------------------
/*
	フッタ付画像のフッタを切り捨てて画像を読む(ttsuji追加)
	（tomoimageフォルダ内のSpcImage*.raw、Image*.rawを扱うときに用いる）
*/
bool CTDAT::ReadDtSkipFoot(const string fileName)
{
	if (!MainPixel || !SubPixel) {
		CApplication::GetInstance()->MessageBox("主副画素数の設定が不正です", "Error", MB_OK);
		return false;
	}

	ifstream inFile(fileName.c_str(), ios::in | ios::binary);
	if(!inFile){
		CApplication::GetInstance()->MessageBox((fileName + " openできませんでした").c_str(), "Error", MB_OK);
		return false;
	}
	// ファイルサイズ計算
	long file_size, footer_size;
	inFile.seekg(0, ios::end);
	file_size = inFile.tellg();
	inFile.seekg(0);
	footer_size = file_size - MainPixel * SubPixel * 2;
	if(footer_size < 0){
		CApplication::GetInstance()->MessageBox("主副画素数の設定、もしくは画像サイズが不正です", "Error", MB_OK);
		inFile.close();
		return false;
	}

	NewDt(SubPixel, MainPixel, Bit);

	int i;
	char tmp;//1byte
	unsigned short int data;//2byte
	for(i = 0 ; i < SubPixel ; i++){
		inFile.read((char*)Dt[i], sizeof(unsigned short int) * MainPixel);
	}
	//フッタは読まない
	inFile.close();

	return true;
}
//---------------------------------------------------------------------------
/*
	Read Data 新圧縮
		引数	File Name

		戻り値	成功->true
				失敗->false
*/
bool CTDAT::ReadDtD(const string fileName)
{
#if 0
	int i, j;
	int dwInput_cnt = 0;
	unsigned char bDt, bDt_sv;
	short int sOut_dt, sDiff_dt = 0, sWk_dt, sLen_cnt;
    unsigned char *buffer;		//data読み込みBuffer
    short int *dt;		//data一時buffer
    short int *lpwDat;
	int fileLen;		//file length(byte)
    unsigned char *lpBuffDist;

    NewDt(SubPixel, MainPixel, Bit);

//Read Data File
    FILE *fp;
    if ((fp = fopen(fileName.c_str(),"rb")) == NULL)
    	CApplication::GetInstance()->MessageBox((fileName + " openできませんでした").c_str(), "Error", MB_OK);
    	return(false);
    }

	struct stat statBuf;
//	stat("aaa", &statBuf);

	fileLen = statBuf.st_size;
    buffer = new unsigned char[fileLen];		//Bufferを確保
    int readSize = fread(buffer, 1, fileLen, fp);
    if(readSize != fileLen){
    	CApplication::GetInstance()->MessageBox((fileName + " readできませんでした").c_str(), "Error", MB_OK);
    	fclose(fp);
    	delete[] buffer;
        return(false);
    }
    fclose(fp);


//新圧縮
	dt = new short int[SubPixel * MainPixel];
    lpwDat = dt;
	lpBuffDist = buffer;
	while(fileLen > dwInput_cnt){
    	bDt_sv = bDt = *lpBuffDist;		//１バイト読込み
        ++lpBuffDist;
        ++dwInput_cnt;
        if ((bDt & 0x03) == 0) {		//１バイト２画素（３ビットデータ）　：　データステータス＝０
//        	sOut_dt = short(bDt) << (3 + 8);
            sOut_dt = (short)(bDt << (3 + 8));
            sOut_dt >>= (5 + 8);
            sOut_dt += sDiff_dt;
            *lpwDat = sOut_dt;
            ++lpwDat;

//            sWk_dt = short(bDt_sv) << 8;
            sWk_dt = (short)(bDt_sv << 8);
            sWk_dt >>= (5 + 8);
            sOut_dt += sWk_dt;
            *lpwDat = sOut_dt;
            ++lpwDat;
        }else if((bDt & 0x03) == 2) {	//１バイト１画素（３ビットデータ）　：　データステータス＝２
//        	sOut_dt = short(bDt) << (3 + 8);
            sOut_dt = (short)(bDt << (3 + 8));
            sOut_dt >>= (5 + 8);
            sOut_dt += sDiff_dt;
            *lpwDat = sOut_dt;
            ++lpwDat;
        }else if((bDt & 0x03) == 1) {	//１バイト１画素（６ビットデータ）　：　データステータス＝１
//        	sOut_dt = short(bDt_sv) << 8;
            sOut_dt = (short)(bDt_sv << 8);
            sOut_dt >>= (2 + 8);
            sOut_dt += sDiff_dt;
            *lpwDat = sOut_dt;
            ++lpwDat;
        }else{
        	bDt = *lpBuffDist;		//上位１バイト読込み
            ++lpBuffDist;
            ++dwInput_cnt;

            if((bDt & 0x80) == 0) {	//２バイト１画素（元データ）
            	sOut_dt = short(bDt_sv >> 2);
                sOut_dt |= short(bDt << 6);
                *lpwDat = sOut_dt;
                ++lpwDat;
            }else{			//ランレングス差分データ
//            	sWk_dt = short(bDt_sv & 0x1c) << (3 + 8);	//差分データの取り出し
            	sWk_dt = (short)((bDt_sv & 0x1c) << (3 + 8));	//差分データの取り出し
//                sOut_dt = sWk_dt >> (5 + 8);
                sOut_dt = (short)(sWk_dt >> (5 + 8));

                sLen_cnt = short(bDt_sv >> 5);		//ランレングスカウンタ値の取り出し
                sLen_cnt |= short((bDt & 0x7f) << 3);
                for(i = 0; i < sLen_cnt; ++i){
                	sDiff_dt += sOut_dt;
                    *lpwDat = sDiff_dt;
                    ++lpwDat;
                }
                sOut_dt = sDiff_dt;
            }
        }
        sDiff_dt = sOut_dt;
    }

    lpwDat = dt;
    for(i = 0 ; i < SubPixel ; ++i){
    	for(j = 0 ; j < MainPixel ; ++j){
        	Dt[i][j] = *lpwDat;
            ++lpwDat;
        }
    }
    delete[] dt;
    delete[] buffer;
#endif
    return(true);
}



//---------------------------------------------------------------------------
/*
	指定領域のminを返す
        領域が画像範囲外の時は画像範囲に修正して検出
*/
unsigned short int CTDAT::nMin(int iS1, int iM1, int iS2, int iM2)
{
	CTDATProc procObj(SubPixel,MainPixel,Bit,Dt);

	return procObj.GetMin(iS1,iM1,iS2,iM2);
}

//---------------------------------------------------------------------------
//	Dtの値代入

void CTDAT::SetVal(const int iSub, const int iMain, const double dVal)
{
	if(dVal <= 0.0){
		Dt[iSub][iMain] = 0;
	}else if(dVal >= double(DtMax)){
		Dt[iSub][iMain] = DtMax;
	}else{
		Dt[iSub][iMain] = (unsigned short int)(dVal + 0.5);
	}
}
//---------------------------------------------------------------------------
/*
	Dtの値代入
*/
void CTDAT::SetVal(const int iSub, const int iMain, const float fVal)
{
    if(fVal <= 0.0){
        Dt[iSub][iMain] = 0;
    }else if(fVal >= float(DtMax)){
        Dt[iSub][iMain] = DtMax;
    }else{
        Dt[iSub][iMain] = (unsigned short int)(fVal + 0.5f);
    }
}
//---------------------------------------------------------------------------
/*
	Dtの値代入
*/
void CTDAT::SetVal(const int iSub, const int iMain, const int iVal)
{
    if(iVal <= 0){
        Dt[iSub][iMain] = 0;
    }else if(iVal >= int(DtMax)){
        Dt[iSub][iMain] = DtMax;
    }else{
        Dt[iSub][iMain] = (unsigned short int)iVal;
    }
}
//---------------------------------------------------------------------------
/*
	Dtの値代入
*/
void CTDAT::SetVal(const int iSub, const int iMain, const unsigned int iVal)
{
	if(iVal >= int(DtMax)){
        Dt[iSub][iMain] = DtMax;
    }else{
		Dt[iSub][iMain] = (unsigned short int)iVal;
    }
}
//---------------------------------------------------------------------------
/*
	Dtの値代入
*/
void CTDAT::SetVal(const int iSub, const int iMain, const unsigned long iVal)
{
	if(iVal >= int(DtMax)){
		Dt[iSub][iMain] = DtMax;
	}else{
		Dt[iSub][iMain] = (unsigned short int)iVal;
	}
}
//---------------------------------------------------------------------------
/*
	Dtの値代入（切り捨て）　ttsuji追加
*/
void CTDAT::SetValWithTruncation(const int iSub, const int iMain, const double dVal)
{
	if(dVal <= 0.0){
		Dt[iSub][iMain] = 0;
	}else if(dVal >= double(DtMax)){
		Dt[iSub][iMain] = DtMax;
	}else{
		Dt[iSub][iMain] = (unsigned short int)dVal;//切り捨て
	}
}


//---------------------------------------------------------------------------
/*
	Dtの値代入
*/
void CTDAT::SetValWithAreaCheck(int iSub, int iMain, const unsigned short int nVal)
{
    if(iSub < 0 || iSub >= SubPixel || iMain < 0 ||iMain >= MainPixel){
        return;
    }
    Dt[iSub][iMain] = nVal;

}
//---------------------------------------------------------------------------
/*
	Dtの値代入
*/
void CTDAT::SetValWithAreaCheck(int iSub, int iMain, const double dVal)
{
    if(iSub < 0 || iSub >= SubPixel || iMain < 0 ||iMain >= MainPixel){
        return;
    }

    if(dVal <= 0.0){
        Dt[iSub][iMain] = 0;
    }else if(dVal >= double(DtMax)){
        Dt[iSub][iMain] = DtMax;
    }else{
        Dt[iSub][iMain] = (unsigned short int)(dVal + 0.5);
    }

}
//---------------------------------------------------------------------------
/*
	Dtの値代入
*/
void CTDAT::SetValWithAreaCheck(int iSub, int iMain, const int iVal)
{
    if(iSub < 0 || iSub >= SubPixel || iMain < 0 ||iMain >= MainPixel){
        return;
    }
    if(iVal <= 0){
        Dt[iSub][iMain] = 0;
    }else if(iVal >= int(DtMax)){
        Dt[iSub][iMain] = DtMax;
    }else{
        Dt[iSub][iMain] = (unsigned short int)iVal;
    }

}


//---------------------------------------------------------------------------
/*
	Dt[][]の平均値
*/
unsigned short int CTDAT::Average()
{
//	float sum;
	double sum;//float型だと平均QLにズレが生じるためdoubleに変更する（ttsuji）
	int i, j;

    sum = 0;
    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
    		sum += Dt[i][j];

    return((unsigned short int)(sum / MainPixel / SubPixel + 0.5));
}
//---------------------------------------------------------------------------
/*
	Dt[][]の平均値
*/
unsigned short int CTDAT::Average(int iSubSrt, int iSubEnd, int iMainSrt, int iMainEnd)
{

	return ((unsigned short int)(AverageD(iSubSrt, iSubEnd, iMainSrt, iMainEnd) + 0.5));
}
//---------------------------------------------------------------------------
/*
	Dt[][]の平均値
*/
double CTDAT::AverageD(int iSubSrt, int iSubEnd, int iMainSrt, int iMainEnd)
{
	double dSum = 0;
	int iMCnt, iSCnt;

	if(iSubSrt < 0) iSubSrt = 0;
	if(iSubEnd > SubPixel - 1) iSubEnd = SubPixel - 1;
	if(iMainSrt < 0) iMainSrt = 0;
	if(iMainEnd > MainPixel - 1) iMainEnd = MainPixel - 1;

	for(iSCnt = iSubSrt ; iSCnt <= iSubEnd ; ++iSCnt)
		for(iMCnt = iMainSrt; iMCnt <= iMainEnd; ++iMCnt)
			dSum += Dt[iSCnt][iMCnt];

    return (dSum / ((iMainEnd - iMainSrt + 1) * (iSubEnd - iSubSrt + 1)));
}
/* -------------------------------------------------------------------
* @brief	ヒストグラムデータの更新(矩形領域指定)
* @param	s1		開始y座標
* @param	s2		終了y座標
* @param	m1		開始x座標
* @param	m2		終了y座標
* @retval   なし
* @date     2021/04/01	T.Taneichi	流用
 --------------------------------------------------------------------- */
void CTDAT::Histogram(const int s1, const int s2, const int m1, const int m2)
{
	int i, j;

	//TODO 範囲チェック


	if (HistExist == true) {
		delete[] Hist;
	}
	Hist = new int[DtMax + 1];
	HistExist = true;
	HistRuisekiSum = (m2 - m1 + 1) * (s2 - s1 + 1);

	for(i = 0 ; i <= DtMax ; ++i)
		Hist[i] = 0;

	for(i = s1 ; i <= s2 ; ++i){
		for(j = m1 ; j <= m2 ; ++j){
			if(Dt[i][j] <= DtMax) ++Hist[Dt[i][j]];
		}
	}
}
/* -------------------------------------------------------------------
* @brief	ヒストグラムの最頻QL値を返す
* 			ヒストグラムの計算をしていない場合は、呼ばれたタイミングで全体に対して計算する
* @param	なし
* @retval   最頻QL値
* @date     2021/04/01	T.Taneichi	流用
 --------------------------------------------------------------------- */
int CTDAT::HistMaxQL()
{
	int i;
	int maxQL;		//度数最大のQL

	if(HistExist == false){
		Histogram(0, SubPixel - 1, 0, MainPixel - 1);
	}

	maxQL = 0;
	for(i = 1 ; i <= DtMax ; ++i){
		if(Hist[maxQL] < Hist[i]) {
			maxQL = i;
		}
	}

	return(maxQL);
}
//---------------------------------------------------------------------------
/*
	ttsuji追加（140517）
	dataのHistogram のうち、度数がゼロでない最大のQL
		戻り値	->	度数がゼロでない最大のQL
			-> これは単純に画像の最大QLと同じだと思うが、Histから取得する必要があるのか?処理時間短縮？
*/
int CTDAT::HistExistMaxQL()
{
	int i;
    int maxQL;		//度数最大のQL

	if(HistExist == false){
		Histogram(0, SubPixel - 1, 0, MainPixel - 1);
    }

	i = DtMax;
	while (Hist[i] == 0){
		i--;
	}
	maxQL = i;

	return maxQL;
}
//---------------------------------------------------------------------------
/*
	ttsuji追加（140517）
	dataのHistogram のうち、度数累積和の指定％相当のQLを返す
		戻り値	->	度数累積和の指定％相当のQL
		bFromMax -> trueなら最大QLから数えて指定％のQL
					falseなら最小QLから数えて指定％のQL


*/
int CTDAT::HistRuisekiDosuuPercent(float fPercentage, bool bFromMax)
{
	int i;
	int iCnt, iSum;
	int QL;//度数累積和の指定％相当のQL

	// TODO 範囲チェック

	if(HistExist == false) {
		Histogram(0, SubPixel - 1, 0, MainPixel - 1);
	}
	int iAccum = int((1. - fPercentage) * HistRuisekiSum + 0.5);		//TODO 1から引く必要がある？


	if (bFromMax){
		iCnt = DtMax;
		iSum = Hist[iCnt];
		while (iSum <= iAccum){
			iCnt--;
			iSum += Hist[iCnt];
		}
	}
	else{
		iCnt = 0;
		iSum = Hist[iCnt];
		while (iSum <= iAccum){
			iCnt++;
			iSum += Hist[iCnt];
		}
	}
	QL = iCnt;

	return QL;
}
//---------------------------------------------------------------------------
/*
	ttsuji追加（191029）

	dataのHistogram のうち、度数累積和の指定％で指定された範囲のピクセル数のQL値の平均値を返す
		戻り値	->	度数累積和の指定された範囲のピクセル数のQL値の平均値
*/

double CTDAT::HistRuisekiDosuuPercentRange(float fPercentMin, float fPercentMax)
{
	int i;
	int iQLCnt, iSum;
	double dQL;//度数累積和の指定％相当のQL

/*	if(HistExist == false) {
		Histogram(0, SubPixel - 1, 0, MainPixel - 1);
	}
*/
	std::vector<int> v_ql;
	int iS, iM;
	for (iS = 0; iS < SubPixel; iS++) {
		for (iM = 0; iM < MainPixel; iM++) {
			v_ql.push_back(Dt[iS][iM]);
		}
	}
	std::sort(v_ql.begin(), v_ql.end());
	dQL = (double)vecpercentile(v_ql, fPercentMin ,fPercentMax);

	return dQL;


}
//---------------------------------------------------------------------------
//	画像データをソートした後に、所定の順位のQL値を返す
//		戻り値	->	成功か失敗かのフラグ
bool CTDAT::bCalcQLOfGivenOrderPixel(int iOrder,  int& iRetQL)
{
	iRetQL = -1;
	std::vector<int> v_ql;
	int iS, iM;
	for (iS = 0; iS < SubPixel; iS++) {
		for (iM = 0; iM < MainPixel; iM++) {
			v_ql.push_back(Dt[iS][iM]);
		}
	}
	std::sort(v_ql.begin(), v_ql.end());

	if (iOrder < 0 || iOrder > v_ql.size()) {
		return false;
	}
	iRetQL = v_ql[iOrder];

	return true;
}

//---------------------------------------------------------------------------
/*
	RMS計算

*/
double CTDAT::RMS()
{
	const double a = 1;//(2.64 - 1.2) / 512;		//QL -> D 変換関数 y = a * x + b
    const double b = 0;//2.64 - a * 1023;

    int i, j;
    double ss;		//2乗和
    double s;		//和
    double d;		//濃度

    ss = s = 0;
    for(i = 0 ; i < SubPixel ; ++i){
    	for(j = 0 ; j < MainPixel ; ++j){
        	d = a * Dt[i][j] + b;
        	ss += d * d;
            s += d;
        }
    }

    return(sqrt((ss - s * s / SubPixel / MainPixel) / SubPixel / MainPixel));
}

//---------------------------------------------------------------------------
/*
	分散計算

*/
double CTDAT::Variance()
{
    int i, j;
    double ss;		//2乗和
    double s;		//和
    double d;		//double変換濃度

    ss = s = 0;
    for(i = 0 ; i < SubPixel ; ++i){
    	for(j = 0 ; j < MainPixel ; ++j){
        	d = double(Dt[i][j]);
        	ss += d * d;
            s += d;
        }
    }

    return(sqrt((ss - s * s / SubPixel / MainPixel) / (SubPixel * MainPixel - 1)));
}
//---------------------------------------------------------------------------
/*
	濃度変換後の分散計算

*/
double CTDAT::VarianceD()
{
	const double a = (2.64 - 1.2) / 512;		//QL -> D 変換関数

    return(Variance() * a);
}
//---------------------------------------------------------------------------
/*
	Histogram 0 となった両端を省く
    	注	Histogramが既に求められていることを前提にしている

*/
void CTDAT::Hist0Cut()
{
	const int AVENUM = 5;		//Histogram平均化個数

	int min;		//Histogram 0 となる最小値
    int max;		//Histogram 0 となる最大値
    int i;
    int *hist;		//平均Histogram平均
    int sum;		//平均化用

    hist = new int[DtMax + 1];

//histogramの平均化
    for(i = 0 ; i < AVENUM / 2 ; ++i)
    	hist[i] = Hist[i];

    sum = 0;
    for(i = 0 ; i < AVENUM ; ++i)
    	sum += Hist[i];
    hist[AVENUM / 2] = sum / AVENUM;
    for(i = AVENUM / 2 + 1 ; i <= DtMax - AVENUM / 2 ; ++i){
    	sum = sum - Hist[i - AVENUM / 2 - 1] + Hist[i + AVENUM / 2];
        hist[i] = sum / AVENUM;
    }

    for(i = DtMax - AVENUM / 2  + 1; i <= DtMax ; ++i)
    	hist[i] = Hist[i];

//Histogram 0 となった両端を省く
    min = max = HistMaxQL();

    while(hist[min] != 0){
    	if(min == 0) break;
    	--min;
    }

    while(hist[max] != 0){
    	if(max == DtMax) break;
        ++max;
    }

    for(i = 0 ; i < min ; ++i){
    	Hist[i] = 0;
    }

    for(i = max ; i <= DtMax ; ++i){
    	Hist[i] = 0;
    }

    delete[] hist;
}
//---------------------------------------------------------------------------
/*
	Histogram からの分散計算
    	注	Histogramが既に求められていることを前提にしている

*/
double CTDAT::HistVariance()
{
    int i;
    double ss;		//2乗和
    double s;		//和
    int num;		//分散計算個数

	num = 0;
	ss = s = 0;
    for(i = 0 ; i <= DtMax ; ++i){
    	num += Hist[i];
    	ss += double(Hist[i]) * i * i;
        s += double(Hist[i]) * i;
    }

    return(sqrt((ss - s * s / num) / (num - 1)));
}
//---------------------------------------------------------------------------
/*
	Histogram度数割合がσとなる点を求めて分散とする
    	注	Histogramが既に求められていることを前提にしている

*/
double CTDAT::HistVarRatio()
{
	const int AVENUM = 9;		//Histogram平均化個数
    const double SIGMA = 0.68268;		//σまでの積分

	double min;		//σとなる最小値
    double max;		//σとなる最大値
    int i;
    double s;		//和
    int *hist;		//平均Histogram平均
    int sum;
    double total;		//全度数
    int histMax;		//度数最大QL
    int ct;

//histogramの平均化
    hist = new int[DtMax + 1];
    for(i = 0 ; i < AVENUM / 2 ; ++i)
    	hist[i] = Hist[i];

    sum = 0;
    for(i = 0 ; i < AVENUM ; ++i)
    	sum += Hist[i];
    hist[AVENUM / 2] = sum / AVENUM;
    for(i = AVENUM / 2 + 1 ; i <= DtMax - AVENUM / 2 ; ++i){
    	sum = sum - Hist[i - AVENUM / 2 - 1] + Hist[i + AVENUM / 2];
        hist[i] = sum / AVENUM;
    }
    for(i = DtMax - AVENUM / 2  + 1; i <= DtMax ; ++i)
    	hist[i] = Hist[i];

//度数最大QL検索
	histMax = 0;
    for(i = 1 ; i <= DtMax ; ++i)
    	if(hist[histMax] < hist[i]) histMax = i;

//σになるQL探索
	total = double(hist[histMax]) / 2;		//histMax以降の全度数を求める
	for(i = histMax + 1 ; i <= DtMax ; ++i)
    	total += hist[i];

    ct = histMax;
    s = double(hist[histMax]) / 2;
    while(s < total * SIGMA){
    	++ct;
    	s += hist[ct];
    }
    max = ct - (s - total * SIGMA) / (hist[ct] - hist[ct - 1]);

	total = double(hist[histMax]) / 2;		//histMax以前の全度数を求める
	for(i = 0 ; i <histMax ; ++i)
    	total += hist[i];

    ct = histMax;
    s = double(hist[histMax]) / 2;
    while(s < total * SIGMA){
    	--ct;
    	s += hist[ct];
    }
    min = (s - total * SIGMA) / (hist[ct + 1] - hist[ct]) + ct;

    delete[] hist;

    return((max - min) / 2);
}
//---------------------------------------------------------------------------
/*
	data反転処理
*/
void CTDAT::Reverse()
{
    int i, j;

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	Dt[i][j] = (unsigned short int)(DtMax - Dt[i][j]);
}

/* -------------------------------------------------------------------
* @brief	画像サイズの変更（切り出し含む）
* 			TODO マイナス座標指定したときに怪しい
* @param	sub		副走査画素数    < 0 の時元の画素数
* @param	main	主走査画素数    < 0 の時元の画素数
* @param	sSt		副走査原点位置	元画像の位置を指定すると、そこが新しい画像の(0,0)の位置になる。
* @param	mSt		主走査原点位置　　　なので、画像を小さくする場合はマイナスを指定することとなる。
* @param	back	背景色
* @retval   true:範囲内／false:範囲外
* @date     2021/03/30	T.Taneichi	流用
 --------------------------------------------------------------------- */
void CTDAT::Format(int sub, int main, const int sSt, const int mSt, const unsigned short int back)
{
    if(sub < 0){
        sub = SubPixel;
    }
    if(main < 0){
        main = MainPixel;
    }
    int i, j;
    int endMain, endSub;        //元画像データの取り出し終了画素位置
    CTDAT temp;

	temp.NewDt(sub, main, Bit);

    //ttsuji追加（背景色がオーバーフローしていた場合の対処）
    unsigned short int back2 = (back > DtMax) ? DtMax : back;

    for(i = 0 ; i < sub ; ++i)		//全画素背景色にする
    	for(j = 0 ; j < main ; ++j)
        	temp.Dt[i][j] = back2;

    if(mSt >= 0 && sSt >= 0){ 		//主・副shift
        if(mSt + MainPixel > main){
            endMain = main - mSt;
        }else{
            endMain = MainPixel;
        }
        if(sSt + SubPixel > sub){
            endSub = sub - sSt;
        }else{
            endSub = SubPixel;
        }
        for(i = 0 ; i < endSub ; ++i)
            for(j = 0 ; j < endMain ; ++j)
                temp.Dt[i + sSt][j + mSt] = Dt[i][j];

    }else if(mSt >= 0 && sSt < 0){		//主shift・副切り出し
        if(mSt + MainPixel > main){
            endMain = main - mSt;
        }else{
            endMain = MainPixel;
        }
        if(SubPixel + sSt < sub){
            endSub = SubPixel + sSt;
        }else{
            endSub = sub;
        }
        for(i = 0 ; i < endSub ; ++i)
            for(j = 0 ; j < endMain ; ++j)
            	temp.Dt[i][j + mSt] = Dt[i - sSt][j];

    }else if(mSt < 0 && sSt >= 0){ 		// 主切り出し・副shift  													  // 主・副切り出し
        if(MainPixel + mSt < main){
            endMain = MainPixel + mSt;
        }else{
            endMain = main;
        }
        if(sSt + SubPixel > sub){
            endSub = sub - sSt;
        }else{
            endSub = SubPixel;
        }
    	for(i = 0 ; i < endSub ; ++i)
        	for(j = 0 ; j < endMain ; ++j)
            	temp.Dt[i + sSt][j] = Dt[i][j - mSt];

    }else{ 													  // 主・副切り出し
        if(MainPixel + mSt < main){
            endMain = MainPixel + mSt;
        }else{
            endMain = main;
        }
        if(SubPixel + sSt < sub){
            endSub = SubPixel + sSt;
        }else{
            endSub = sub;
        }
    	for(i = 0 ; i < endSub ; ++i)
        	for(j = 0 ; j < endMain ; ++j)
            	temp.Dt[i][j] = Dt[i - sSt][j - mSt];
    }

    NewDt(sub, main, Bit);
    for(i = 0 ; i < sub ; ++i)
    	for(j = 0 ; j < main ; ++j)
        	Dt[i][j] = temp.Dt[i][j];

}
//---------------------------------------------------------------------------
/*
	画像縮小処理
		引数	const int sDiv	副走査縮小率
				const int mDiv	主走査縮小率

		画素数の平均値で画像縮小
*/
void CTDAT::Div(const int sDiv, const int mDiv)
{
	if (1 == sDiv && 1 == mDiv) {
		return;
	}

	int i, j, k;
	int sum;
	CTDAT temp;
	float fSubDiv = sDiv;
	float fMainDiv = mDiv;

	temp.NewDt(SubPixel / sDiv, MainPixel, Bit);

	for(k = 0 ; k < MainPixel ; ++k){		//副走査平均
		for(i = 0 ; i < temp.SubPixel ; ++i){
			sum = 0;
			for(j = 0 ; j < sDiv ; ++j){
				sum += Dt[i * sDiv + j][k];
			}
			temp.SetVal(i, k, float(sum) / fSubDiv);
		}
	}

	NewDt(temp.SubPixel, MainPixel / mDiv, Bit);

	for(k = 0 ; k < temp.SubPixel ; ++k){ 		//主走査平均
		for(i = 0 ; i < MainPixel ; ++i){
			sum = 0;
			for(j = 0 ; j < mDiv ; ++j){
				sum += temp.Dt[k][i * mDiv + j];
			}
			SetVal(k, i, float(sum) / fMainDiv);
		}
	}
}


//---------------------------------------------------------------------------
/*
	画像縮小処理
    	引数	const int sDiv	副走査画素数
        		const int mDiv	主走査画素数

		画素数の平均値で画像縮小

		b45: true(四捨五入), false(切り捨て)
		b2D: true(2D縮小), false(1D副方向縮小→1D主方向縮小のシーケンシャル処理)
*/
void CTDAT::Div(const int sDiv, const int mDiv, bool b45, bool b2D, bool bByLinear)
{
//  バイリニアは、1/16、四捨五入ON、二次元ONのみ
  int i, j, k, l;
  int sum;
  CTDAT temp;
  float fSubDiv = sDiv;
  float fMainDiv = mDiv;

  if( bByLinear==false ){
	if( b2D==false ){
	  temp.NewDt(SubPixel / sDiv, MainPixel, Bit);

	  for(k = 0 ; k < MainPixel ; ++k){		//副走査平均
		  for(i = 0 ; i < temp.SubPixel ; ++i){
			  sum = 0;
			  for(j = 0 ; j < sDiv ; ++j){
				sum += Dt[i * sDiv + j][k];
			  }
			  if( b45==true )
			  temp.SetVal(i, k, float(sum) / fSubDiv);
			  else
			  temp.SetVal(i, k, int(float(sum) / fSubDiv));
		  }
	  }


	  NewDt(temp.SubPixel, MainPixel / mDiv, Bit);

	  for(k = 0 ; k < temp.SubPixel ; ++k){ 		//主走査平均
		for(i = 0 ; i < MainPixel ; ++i){
			sum = 0;
			for(j = 0 ; j < mDiv ; ++j){
				sum += temp.Dt[k][i * mDiv + j];
			}
			if( b45==true )
			SetVal(k, i, float(sum) / fMainDiv);
			else
			SetVal(k, i, int(float(sum) / fMainDiv));
		}
	  }
	} else {
	  temp.NewDt(SubPixel / sDiv, MainPixel / mDiv, Bit);

	  for(k = 0 ; k < temp.MainPixel ; ++k){
		  for(i = 0 ; i < temp.SubPixel ; ++i){
			  sum = 0;
			  for(j = 0 ; j < sDiv ; ++j){
				for( l = 0 ; l < mDiv; ++l){
				  sum += Dt[i * sDiv + j][k * mDiv + l];
				}
			  }
			  if( b45==true )
			  temp.SetVal(i, k, float(sum) / fSubDiv / fMainDiv);
			  else
			  temp.SetVal(i, k, int(float(sum) / fSubDiv / fMainDiv));
		  }
	  }

	  NewDt(SubPixel / sDiv, MainPixel / mDiv, Bit);
	  for(k = 0 ; k < temp.MainPixel ; ++k){
		  for(i = 0 ; i < temp.SubPixel ; ++i){
			SetVal(i, k, temp.Dt[i][k] );
		  }
	  }

	}
  } else {
	int mMain = MainPixel/mDiv;
	int mSub  = SubPixel /sDiv;
	int mcen = mDiv/2;
	int scen = sDiv/2;
	CTDAT temp;
	temp.NewDt(mSub, mMain, Bit);
	int sum;
	for(int iScell=0; iScell<mSub; iScell++){
	  for(int iMcell=0; iMcell<mMain; iMcell++){
		sum = Dt[iScell*sDiv + scen - 1][iMcell*mDiv + mcen - 1]
			 +Dt[iScell*sDiv + scen    ][iMcell*mDiv + mcen - 1]
			 +Dt[iScell*sDiv + scen - 1][iMcell*mDiv + mcen    ]
			 +Dt[iScell*sDiv + scen    ][iMcell*mDiv + mcen    ];
		temp.SetVal( iScell, iMcell, (sum+2)>>2 );

	  }

	}

	  NewDt(SubPixel / sDiv, MainPixel / mDiv, Bit);
	  for(k = 0 ; k < temp.MainPixel ; ++k){
		  for(i = 0 ; i < temp.SubPixel ; ++i){
			SetVal(i, k, temp.Dt[i][k] );
		  }
	  }

  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*
	画像縮小処理
		引数	const int sDiv	副走査画素数
				const int mDiv	主走査画素数

		画素数の加算値で画像縮小
*/
void CTDAT::DivAdd(const int sDiv, const int mDiv)
{
	int i, j, k;
	int sum;
	CTDAT temp;
	temp.NewDt(SubPixel / sDiv, MainPixel, Bit);

	for (k = 0 ; k < MainPixel ; k++){		//副走査平均
		for (i = 0 ; i < temp.SubPixel ; i++){
			sum = 0;
			for (j = 0 ; j < sDiv ; j++){
				sum += Dt[i * sDiv + j][k];
			}
			temp.SetVal(i, k, sum);//オーバーフロー分はクリッピング
		}
	}

	NewDt(temp.SubPixel, MainPixel / mDiv, Bit);

	for (k = 0 ; k < temp.SubPixel ; k++){ 		//主走査平均
		for (i = 0 ; i < MainPixel ; i++){
			sum = 0;
			for (j = 0 ; j < mDiv ; j++){
				sum += temp.Dt[k][i * mDiv + j];
			}
			SetVal(k, i, sum);
		}
	}
}
//---------------------------------------------------------------------------
/*
	画像縮小処理
		引数	const int sDiv	副走査画素数
				const int mDiv	主走査画素数

		最近接画素の画素値で画像縮小
*/
void CTDAT::DivNearestNeighbor(const int sDiv, const int mDiv)
{
	if (1 == sDiv && 1 == mDiv) {
		return;
	}

	int i, j, k;
	int sum;
	CTDAT temp;
	temp.NewDt(SubPixel / sDiv, MainPixel, Bit);
//	float fLatticeSub = (float)SubPixel / temp.SubPixel;
	float fLatticeSub = (int)((float)SubPixel / temp.SubPixel);//切り捨て

	for(k = 0 ; k < MainPixel ; k++){		//副走査平均
		for(i = 0 ; i < temp.SubPixel ; i++){
			float fLattice = fLatticeSub * (i + 0.5);
			int iLatticeNeighbor = (int)fLattice;
			temp.SetVal(i, k, Dt[iLatticeNeighbor][k]);
		}
	}

	NewDt(temp.SubPixel, MainPixel / mDiv, Bit);
//	float fLatticeMain = (float)temp.MainPixel / MainPixel;
	float fLatticeMain = (int)((float)temp.MainPixel / MainPixel);//切り捨て

	for(k = 0 ; k < temp.SubPixel ; k++){ 		//主走査平均
		for(i = 0 ; i < MainPixel ; i++){
			float fLattice = fLatticeMain * (i + 0.5);
			int iLatticeNeighbor = (int)fLattice;
			SetVal(k, i, temp.Dt[k][iLatticeNeighbor]);
		}
	}
}
//---------------------------------------------------------------------------
/*
	画像縮小処理
		引数	const int sDiv	副走査画素数
				const int mDiv	主走査画素数

		画素数の平均値で画像縮小する。ただし、すべて0の場合は原則0QLとし、それ以外は切り上げる
*/
void CTDAT::DivAll0To0(const int sDiv, const int mDiv)
{
	if (1 == sDiv && 1 == mDiv) {
		return;
	}

	int i, j, k;
	int sum;
	CTDAT temp;
	float fSubDiv = sDiv;
	float fMainDiv = mDiv;

	temp.NewDt(SubPixel / sDiv, MainPixel, Bit);

	for(k = 0 ; k < MainPixel ; ++k){		//副走査平均
		for(i = 0 ; i < temp.SubPixel ; ++i){
			sum = 0;
			for(j = 0 ; j < sDiv ; ++j){
				sum += Dt[i * sDiv + j][k];
			}
			if (!sum) {
				temp.SetVal(i, k, 0);
			}
			else if (float(sum) / fSubDiv < 1.0){
				temp.SetVal(i, k, 1);
			}
			else{
				temp.SetVal(i, k, float(sum) / fSubDiv);
			}
		}
	}

	NewDt(temp.SubPixel, MainPixel / mDiv, Bit);

	for(k = 0 ; k < temp.SubPixel ; ++k){ 		//主走査平均
    	for(i = 0 ; i < MainPixel ; ++i){
        	sum = 0;
            for(j = 0 ; j < mDiv ; ++j){
            	sum += temp.Dt[k][i * mDiv + j];
            }
			if (!sum) {
				SetVal(k, i, 0);
			}
			else if (float(sum) / fMainDiv < 1.0){
				SetVal(k, i, 1);
			}
			else{
				SetVal(k, i, float(sum) / fMainDiv);
			}
		}
	}

}
//---------------------------------------------------------------------------
/*
	画像縮小処理（SPCコンペア用）
		引数　なし
		主２、副２の塊単位の平均値で画像縮小
*/
void CTDAT::DivSPC2x2()
{
	int i, j, k;
	int sum;
	CTDAT temp;

	temp.NewDt(SubPixel/2, MainPixel/2, Bit);

	for( i = 0; i < SubPixel/2; i++ ) {
		for( j = 0; j < MainPixel/2  ; j++ ){
			sum = Dt[2*i]    [2*j]
				+ Dt[2*i + 1][2*j]
				+ Dt[2*i]    [2*j + 1]
				+ Dt[2*i + 1][2*j + 1];

			temp.Dt[i][j] = sum / 4;//切捨て
		}
	}

	*this = temp;
}

//---------------------------------------------------------------------------
//平均縮小は切り捨て(ttsuji追加)

void CTDAT::DivTrancation(const int sDiv, const int mDiv)
{
	if (1 == sDiv && 1 == mDiv) {
		return;
	}

	int i, j, k;
	int sum;
	CTDAT temp;
//	float fSubDiv = sDiv;
//	float fMainDiv = mDiv;

	temp.NewDt(SubPixel / sDiv, MainPixel, Bit);

	for(k = 0 ; k < MainPixel ; ++k){		//副走査平均
		for(i = 0 ; i < temp.SubPixel ; ++i){
			sum = 0;
			for(j = 0 ; j < sDiv ; ++j){
				sum += Dt[i * sDiv + j][k];
			}
//			temp.SetVal(i, k, float(sum) / fSubDiv);
			temp.Dt[i][k] = sum / sDiv;//切り捨て
		}
	}

	NewDt(temp.SubPixel, MainPixel / mDiv, Bit);

	for(k = 0 ; k < temp.SubPixel ; ++k){ 		//主走査平均
		for(i = 0 ; i < MainPixel ; ++i){
			sum = 0;
			for(j = 0 ; j < mDiv ; ++j){
				sum += temp.Dt[k][i * mDiv + j];
			}
//			SetVal(k, i, float(sum) / fMainDiv);
			Dt[k][i] = sum / mDiv;//切り捨て
		}
	}
}

//---------------------------------------------------------------------------
/*
	画像による割り算
*/
void CTDAT::DivByImg(const CTDAT& img, const double dReg)
{
    int iMCnt,  iSCnt;

	if(MainPixel >= img.MainPixel && SubPixel >= img.SubPixel){
    	for(iSCnt = 0; iSCnt < img.SubPixel; ++iSCnt){
        	for(iMCnt = 0; iMCnt < img.MainPixel ; ++iMCnt){
                if(img.Dt[iSCnt][iMCnt] == 0){
//                    Dt[iSCnt][iMCnt] = DtMax;
					Dt[iSCnt][iMCnt] = 0;//ゲイン補正用は0が好ましい（オフセット後0QLはゲイン補正後も0QLであるべきなので）（ttsuji）
				}else{
                    SetVal(iSCnt, iMCnt,
                        double(Dt[iSCnt][iMCnt]) / double(img.Dt[iSCnt][iMCnt]) * dReg);
                }
            }
        }
    }else{
        CTDAT    temp;
        int iBigMainPix, iBigSubPix;

        temp.NewDt(SubPixel, MainPixel, Bit);
    	for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        	for(iMCnt = 0; iMCnt < MainPixel ; ++iMCnt){
        	    temp.Dt[iSCnt][iMCnt] = Dt[iSCnt][iMCnt];
            }
        }

        if(MainPixel > img.MainPixel){
            iBigMainPix = MainPixel;
        }else{
            iBigMainPix = img.MainPixel;
        }
        if(SubPixel > img.SubPixel){
            iBigSubPix = SubPixel;
        }else{
            iBigSubPix = img.SubPixel;
        }

        NewDt(iBigSubPix, iBigMainPix, Bit);
        FillValue(0);
    	for(iSCnt = 0; iSCnt < temp.SubPixel; ++iSCnt){
        	for(iMCnt = 0; iMCnt < temp.MainPixel ; ++iMCnt){
        	    Dt[iSCnt][iMCnt] = temp.Dt[iSCnt][iMCnt];
            }
        }

    	for(iSCnt = 0; iSCnt < img.SubPixel; ++iSCnt){
        	for(iMCnt = 0; iMCnt < img.MainPixel ; ++iMCnt){
                if(img.Dt[iSCnt][iMCnt] == 0){
//                    Dt[iSCnt][iMCnt] = DtMax;
					Dt[iSCnt][iMCnt] = 0;//ゲイン補正用は0が好ましい（オフセット後0QLはゲイン補正後も0QLであるべきなので）（ttsuji）
				}else{
                    SetVal(iSCnt, iMCnt,
                        double(Dt[iSCnt][iMCnt]) / double(img.Dt[iSCnt][iMCnt]) * dReg);
                }
            }
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
void CTDAT::DivWithRest(const int iSubDiv, const int iMainDiv)
{
    int iMCnt, iSCnt, iAveCnt;
    int iRes;
    int iSum;
    CTDAT temp;

		//副走査平均
    iRes = SubPixel % iSubDiv;
    if(iRes == 0){
        temp.NewDt(SubPixel / iSubDiv, MainPixel, Bit);

        for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
    	    for(iSCnt = 0; iSCnt < temp.SubPixel; ++iSCnt){
        	    iSum = 0;
        	    for(iAveCnt = 0; iAveCnt < iSubDiv; ++iAveCnt){
            	    iSum += Dt[iSCnt * iSubDiv + iAveCnt][iMCnt];
                }
                temp.SetVal(iSCnt, iMCnt, iSum / float(iSubDiv));
            }
        }
    }else{
        temp.NewDt(SubPixel / iSubDiv + 1, MainPixel, Bit);

        for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
    	    for(iSCnt = 0; iSCnt < temp.SubPixel - 1; ++iSCnt){
        	    iSum = 0;
        	    for(iAveCnt = 0; iAveCnt < iSubDiv; ++iAveCnt){
            	    iSum += Dt[iSCnt * iSubDiv + iAveCnt][iMCnt];
                }
                temp.SetVal(iSCnt, iMCnt, iSum / float(iSubDiv));
            }
            iSum = 0;
    	    for(iSCnt = (temp.SubPixel - 1) * iSubDiv; iSCnt < SubPixel; ++iSCnt){
                iSum += Dt[iSCnt][iMCnt];
            }
            temp.SetVal(temp.SubPixel - 1, iMCnt, iSum / float(iRes));
        }
    }

//主走査平均
    iRes = MainPixel % iMainDiv;
    if(iRes == 0){
        NewDt(temp.SubPixel, MainPixel / iMainDiv, Bit);

        for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
    	    for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
        	    iSum = 0;
        	    for(iAveCnt = 0; iAveCnt < iMainDiv; ++iAveCnt){
            	    iSum += temp.Dt[iSCnt][iMCnt * iMainDiv + iAveCnt];
                }
                SetVal(iSCnt, iMCnt, iSum / float(iMainDiv));
            }
        }
    }else{
        NewDt(temp.SubPixel, MainPixel / iMainDiv + 1, Bit);

        for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
    	    for(iMCnt = 0; iMCnt < MainPixel - 1; ++iMCnt){
        	    iSum = 0;
        	    for(iAveCnt = 0; iAveCnt < iMainDiv; ++iAveCnt){
            	    iSum += temp.Dt[iSCnt][iMCnt * iMainDiv + iAveCnt];
                }
                SetVal(iSCnt, iMCnt, iSum / float(iSubDiv));
            }
            iSum = 0;
    	    for(iMCnt = (MainPixel - 1) * iMainDiv; iMCnt < temp.MainPixel; ++iMCnt){
                iSum += temp.Dt[iSCnt][iMCnt];
            }
            SetVal(iSCnt, MainPixel - 1, iSum / float(iRes));
        }
    }
}



//---------------------------------------------------------------------------
/*
	下へ画像連結
    	bit数は連結する画像の大きい方とする

        引数	TTMA down	下に連結する画像
        		const int background	連結する画像サイズが異なる場合の隙間を
                						埋める値。引数無しの時は最大値。
*/
void CTDAT::DownConnect(const CTDAT& down, int background)
{
	int i,j;
    int bit;
    CTDAT temp;

    temp.NewDt(SubPixel, MainPixel, Bit);		//元画像の退避
    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	temp.Dt[i][j] = Dt[i][j];

    if(temp.Bit > down.Bit){		//bit数は連結する画像の大きい方とする
    	bit = temp.Bit;
    }else{
    	bit = down.Bit;
    }

    if(background == -1) background = (1 << bit) - 1;		//background 引数無しの時は最大値

    if(temp.MainPixel > down.MainPixel){		//連結画像の設定
    	NewDt(temp.SubPixel + down.SubPixel, temp.MainPixel, bit);
        for(i = temp.SubPixel ; i < SubPixel ; ++i)		//backgroundで埋める
        	for(j = down.MainPixel ; j < MainPixel ; ++j)
            	Dt[i][j] = (unsigned short int)background;
    }else{
    	NewDt(temp.SubPixel + down.SubPixel, down.MainPixel, bit);
        for(i = 0 ; i < temp.SubPixel ; ++i)		//backgroundで埋める
        	for(j = temp.MainPixel ; j < MainPixel ; ++j)
            	Dt[i][j] = (unsigned short int)background;
    }

    for(i = 0 ; i < temp.SubPixel ; ++i)		//元画像を上にcopy
    	for(j = 0 ; j < temp.MainPixel ; ++j)
        	Dt[i][j] = temp.Dt[i][j];

    for(i = temp.SubPixel ; i < SubPixel ; ++i)		//連結画像を下にcopy
    	for(j = 0 ; j < down.MainPixel ; ++j)
        	Dt[i][j] = down.Dt[i - temp.SubPixel][j];
}


//---------------------------------------------------------------------------
/*
	画像拡大処理
    	引数	const int sDiv	副走査画素数
        		const int mDiv	主走査画素数

        画素数そのまま画像拡大
*/
void CTDAT::Mul(const int sMul, const int mMul)
{
	if (1 == sMul & 1 == mMul) {
		return;//何もしない
	}

    int i, j, k;
    CTDAT temp;

    temp.NewDt(SubPixel * sMul, MainPixel, Bit);

    for(k = 0 ; k < MainPixel ; ++k)		//副走査
    	for(i = 0 ; i < SubPixel ; ++i)
        	for(j = 0 ; j < sMul ; ++j)
            	temp.Dt[i * sMul + j][k] = Dt[i][k];

    NewDt(SubPixel * sMul, MainPixel * mMul, Bit);

    for(k = 0 ; k < temp.SubPixel ; ++k) 		//主走査
    	for(i = 0 ; i < temp.MainPixel ; ++i)
            for(j = 0 ; j < mMul ; ++j)
            	Dt[k][i * mMul + j] = temp.Dt[k][i];
}
//---------------------------------------------------------------------------
/*
	画像拡大処理
		引数	const int sDiv	副走査画素数
        		const int mDiv	主走査画素数

		0QL挿入した上での拡大
*/
void CTDAT::Mul0Insert(const int sMul, const int mMul)
{
    int i, j, k;
    CTDAT temp;

    temp.NewDt(SubPixel * sMul, MainPixel, Bit);

	for(k = 0 ; k < MainPixel ; ++k){		//副走査
		for(i = 0 ; i < SubPixel ; ++i){
			temp.Dt[i * sMul][k] = Dt[i][k];
			for(j = 1 ; j < sMul ; ++j){
				temp.Dt[i * sMul + j][k] = 0;//0QL挿入
			}
		}
	}

	NewDt(SubPixel * sMul, MainPixel * mMul, Bit);

	for(k = 0 ; k < temp.SubPixel ; ++k){ 		//主走査
		for(i = 0 ; i < temp.MainPixel ; ++i){
			Dt[k][i * mMul] = temp.Dt[k][i];
			for(j = 1 ; j < mMul ; ++j){
				Dt[k][i * mMul + j] = 0;//0QL挿入
			}
		}
	}
}
//---------------------------------------------------------------------------
/*
	画像拡大処理
		引数	const int sDiv	副走査画素数
        		const int mDiv	主走査画素数

		QL挿入した上での拡大
*/
void CTDAT::MulQLInsert(const int sMul, const int mMul, const int iQL)
{
    int i, j, k;
    CTDAT temp;

    temp.NewDt(SubPixel * sMul, MainPixel, Bit);

	for(k = 0 ; k < MainPixel ; ++k){		//副走査
		for(i = 0 ; i < SubPixel ; ++i){
			temp.Dt[i * sMul][k] = Dt[i][k];
			for(j = 1 ; j < sMul ; ++j){
				temp.Dt[i * sMul + j][k] = iQL;//QL挿入
			}
		}
	}

	NewDt(SubPixel * sMul, MainPixel * mMul, Bit);

	for(k = 0 ; k < temp.SubPixel ; ++k){ 		//主走査
		for(i = 0 ; i < temp.MainPixel ; ++i){
			Dt[k][i * mMul] = temp.Dt[k][i];
			for(j = 1 ; j < mMul ; ++j){
				Dt[k][i * mMul + j] = iQL;//QL挿入
			}
		}
	}
}

//---------------------------------------------------------------------------
/*
	主 <--> 副　入れ替え
*/
void CTDAT::MainSub()
{
    int i, j;
    CTDAT temp;

    temp.NewDt(SubPixel, MainPixel, Bit);

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	temp.Dt[i][j] = Dt[i][j];

    NewDt(MainPixel, SubPixel, Bit);

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
            Dt[i][j] = temp.Dt[j][i];
}


//---------------------------------------------------------------------------
/*
	Shift
        num画素shift、空いた画素はbackgroundで埋める
            引数    num         shiftする画素数
                    background  空いた画素を埋めるdata値
*/
void CTDAT::Shift(const int num, const unsigned short int background)
{
    int i, j;
    unsigned short int *dummy;
    unsigned short int *pt;

    if(num >= 0){
        dummy = new unsigned short int[MainPixel * SubPixel + num];
        pt = dummy;

        for(i = 0 ; i < num ; ++i){
            *pt = background;
            ++pt;
        }

        for(i = 0 ; i < SubPixel ; ++i){
            for(j = 0 ; j < MainPixel ; ++j){
                *pt = Dt[i][j];
                ++pt;
            }
        }
    }else{
        dummy = new unsigned short int[MainPixel * SubPixel];
        pt = dummy;

        for(i = - num % MainPixel ; i < MainPixel ; ++i){
            *pt = Dt[- num / MainPixel][i];
            ++pt;
        }

        for(i = - num / MainPixel + 1 ; i < SubPixel ; ++i){
            for(j = 0 ; j < MainPixel ; ++j){
                *pt = Dt[i][j];
                ++pt;
            }
        }

        for(i = num ; i < 0 ; ++i){
            *pt = background;
            ++pt;
        }
    }

    pt = dummy;
    for(i = 0 ; i < SubPixel ; ++i){
        for(j = 0 ; j < MainPixel ; ++j){
            Dt[i][j] = *pt;
            ++pt;
        }
    }

    delete[] dummy;
}


//---------------------------------------------------------------------------
/*
	左右入れ換え
*/
void CTDAT::RtoL()
{
	int i, j;
    CTDAT temp;

    temp.NewDt(SubPixel, MainPixel, Bit);

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	temp.Dt[i][j] = Dt[i][j];

//    NewDt(SubPixel, MainPixel, Bit);

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
            Dt[i][j] = temp.Dt[i][MainPixel - 1 - j];
}


//---------------------------------------------------------------------------
/*
	主走査方向shading除去
        副走査startからlastまでを平均してshadingデータを作成し、

        元画像 / shading data * shading最大値

        に変換する

*/
bool CTDAT::RemoveMainShading(const int start, const int last)
{
    int i, j;
    float *shading;      //shadingデータ
    int temp;
    int max;        //shading 最大値

    if(start < 0) return(false);        //引数が画像内であることのcheck
    if(last > SubPixel) return(false);

    shading = new float[MainPixel];

    for(i = 0 ; i < MainPixel ; ++i)        //startからlastまでの加算
        shading[i] = Dt[start][i];
    for(i = start + 1 ; i <= last ; ++i)
        for(j = 0 ; j < MainPixel ; ++j)
            shading[j] += Dt[i][j];

    shading[0] /= (last - start + 1);      //平均
    max = shading[0];
    for(i = 1 ; i < MainPixel ; ++i){
        shading[i] /= (last - start + 1);      //平均
        if(shading[i] > max) max = shading[i];
    }

    for(i = 0 ; i < SubPixel ; ++i){        //補正
        for(j = 0 ; j < MainPixel ; ++j){
            temp = float(Dt[i][j]) / shading[j] * max;
            if(temp < 0) temp = 0;
            if(temp > DtMax) temp = DtMax;
            Dt[i][j] = (unsigned short int)temp;
        }
    }

    delete[] shading;

    return(true);
}

//---------------------------------------------------------------------------
/*
	縦スジ除去
        副走査startからlastまでを平均して縦スジデータを作成し、画像からひく
*/
bool CTDAT::Suji(const int start, const int last, const int addConst, const int iMMedian, const int iSMedian)
{
    int i, j;
    int *suji;      //縦スジデータ

    if(start < 0 || last > SubPixel || start > last){
        return(false);
    }

    CTDAT Median;

    Copy(Median);
    Median.MainMedian(iMMedian);
    Median.SubMedian(iSMedian);

    suji = new int[MainPixel];

    for(i = 0 ; i < MainPixel ; ++i)        //startからlastまでの加算
        suji[i] = Median.Dt[start][i];
    for(i = start + 1 ; i <= last ; ++i)
        for(j = 0 ; j < MainPixel ; ++j)
            suji[j] += Median.Dt[i][j];

    for(i = 0 ; i < MainPixel ; ++i)
        suji[i] /= (last - start + 1);      //平均


    for(i = 0 ; i < SubPixel ; ++i){        //縦スジデータを画像からひく
        for(j = 0 ; j < MainPixel ; ++j){
            SetVal(i, j, Dt[i][j] - suji[j] + addConst);
        }
    }

    delete[] suji;

    return(true);
}

//---------------------------------------------------------------------------
/*
	直線補間で画像拡大
        端部は直線で延長
*/
void CTDAT::LinearExpand(const int iSExp, const int iMExp)
{
	int iMCnt, iSCnt;
    int iCnt;
    float fPos;     //元dataが表す座標
    int iOffset;
    CTDAT orgImg;
    float a, b;

    if(iSExp <= 1 && iMExp <= 1) return;

    orgImg.NewDt(SubPixel * iSExp, MainPixel, Bit);
    for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
            orgImg.Dt[iSCnt][iMCnt] = Dt[iSCnt][iMCnt];
        }
    }

    NewDt(SubPixel * iSExp, MainPixel * iMExp, Bit);
        //副へ拡大
    if(iSExp > 1){
        iOffset = iSExp / 2;
        for(iMCnt = 0; iMCnt < orgImg.MainPixel; ++iMCnt){
            fPos = (iSExp - 1) / 2.0;
            a = float(orgImg.Dt[1][iMCnt] - orgImg.Dt[0][iMCnt]) / float(iSExp);
            b = orgImg.Dt[0][iMCnt] - a * fPos;
            for(iCnt = 0; iCnt < iOffset; ++iCnt){
                SetVal(iCnt, iMCnt, a * iCnt + b);
            }
            fPos += iSExp;

            for(iSCnt = 1; iSCnt < orgImg.SubPixel / iSExp; ++iSCnt){
                a = float(orgImg.Dt[iSCnt][iMCnt] - orgImg.Dt[iSCnt - 1][iMCnt])
                        / float(iSExp);
                b = orgImg.Dt[iSCnt][iMCnt] - a * fPos;
                for(iCnt = (iSCnt - 1)* iSExp + iOffset;
                        iCnt < iSCnt * iSExp + iOffset; ++iCnt){
                    SetVal(iCnt, iMCnt, a * iCnt + b);
                }
                fPos += iSExp;
            }
            for(; iCnt < SubPixel; ++iCnt){
                SetVal(iCnt, iMCnt, a * iCnt + b);
            }
        }
    }
        //主へ拡大
    if(iMExp > 1){
        if(iSExp > 1){
            for(iMCnt = 0; iMCnt < orgImg.MainPixel; ++iMCnt){
                for(iSCnt = 0; iSCnt < orgImg.SubPixel; ++iSCnt){
                    orgImg.Dt[iSCnt][iMCnt] = Dt[iSCnt][iMCnt];
                }
            }
        }
        iOffset = iMExp / 2;
        for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
            fPos = (iMExp - 1) / 2.0;
            a = float(orgImg.Dt[iSCnt][1] - orgImg.Dt[iSCnt][0]) / float(iMExp);
            b = orgImg.Dt[iSCnt][0] - a * fPos;
            for(iCnt = 0; iCnt < iOffset; ++iCnt){
                SetVal(iSCnt, iCnt, a * iCnt + b);
            }
            fPos += iMExp;

            for(iMCnt = 1; iMCnt < orgImg.MainPixel; ++iMCnt){
                a = float(orgImg.Dt[iSCnt][iMCnt] - orgImg.Dt[iSCnt][iMCnt - 1])
                        / float(iMExp);
                b = orgImg.Dt[iSCnt][iMCnt] - a * fPos;
                for(iCnt = (iMCnt - 1)* iMExp + iOffset;
                        iCnt < iMCnt * iMExp + iOffset; ++iCnt){
                    SetVal(iSCnt, iCnt, a * iCnt + b);
                }
                fPos += iMExp;
            }
            for(; iCnt < MainPixel; ++iCnt){
                SetVal(iSCnt, iCnt, a * iCnt + b);
            }
        }
    }
}
//---------------------------------------------------------------------------
/*
	Line Filter処理
    	主走査方向startとlastまでの平均値からそれをAVE個移動平均した低周波を引く
        ことにより横スジデータを作り、ひいて補正
*/
bool CTDAT::LineFilter(const int start, const int last, const int ave)
{
	int i, j;
    int *line;      //横スジdata
    int *lineAve;
    int temp;

    if(start < 0) return(false);        //引数が画像内であることのcheck
    if(last > MainPixel) return(false);

    line = new int[SubPixel];
    lineAve = new int[SubPixel];

    for(i = 0 ; i < SubPixel ; ++i)        //startからlastまでの加算
        line[i] = Dt[i][start];
    for(i = start + 1 ; i <= last ; ++i)
        for(j = 0 ; j < SubPixel ; ++j)
            line[j] += Dt[j][i];

    for(i = 0 ; i < SubPixel ; ++i){
        line[i] /= (last - start + 1);      //平均
        lineAve[i] = line[i];
    }

    Averaging(lineAve, 0, SubPixel - 1, ave);

    for(i = 0 ; i < SubPixel ; ++i){
        for(j = 0 ; j < MainPixel ; ++j){
            temp = Dt[i][j] - line[i] + lineAve[i];         //縦スジデータを画像からひく
            if(temp < 0) temp = 0;
            if(temp > DtMax) temp = DtMax;
            Dt[i][j] = (unsigned short int)temp;
        }
    }

    delete[] line;
    delete[] lineAve;

    return(true);

}
//---------------------------------------------------------------------------
/*
	DataのAveraging
		Averaging個数が偶数の時前半dataを多くする。

    	引数	s->  Averaging開始点
        		e->  Averaging終了点
                aveNum -> Averaging個数
*/
void CTDAT::Averaging(int dt[], const int s, int e, const int aveNum)
{
	int side1;		//Averaging前半data
	int side2;		//Averaging後半data
    float sum;
    int i;

//    if(e >= SUB) e = SUB - 1;

    if(aveNum % 2 == 0){
    	side1 = aveNum / 2;		//Averaging個数が偶数の時
        side2 = aveNum / 2 - 1;
    }else{
    	side1 = side2 = aveNum / 2;		//Averaging個数が奇数の時
    }

    sum = 0.0;
    for(i = s - side1 ; i <= s + side2 ; ++i){
    	if(i < 0){
        	sum += dt[0];
        }else{
        	sum += dt[i];
        }
    }
    dt[s] = sum / aveNum;

    for(i = s + 1 ; i <= e ; ++i){
    	if(i - side1 - 1 < 0){
        	sum -= dt[0];
        }else{
        	sum -= dt[i - side1 - 1];
        }
    	if(i + side2 > e){
        	sum += dt[e];
        }else{
        	sum += dt[i + side2];
        }
        dt[i] = sum / aveNum;
    }
}

//---------------------------------------------------------------------------
/*
	欠陥画素判定
    	Histogram 0 または th * ヒスト最大度数以下となった両端を欠陥画素と判定
*/
bool CTDAT::BadFind(bool **bad, const float threshold)
{
	const int AVENUM = 21;		//Histogram平均化個数

    int th;		//histがth以上の時は正常とする
    int *hist;		//Histogram 平均処理用buffer
    int sum;		//平均化用
    int i, j;
    int min;
    int max;
    bool badExist;

    badExist = false;

    hist = new int[DtMax + 1];

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	bad[i][j] = false;

//Dark
	Histogram(0, SubPixel - 1, 0, MainPixel - 1);
    Hist[DtMax] = 0;
    Hist[0] = 0;

//histogramの平均化
	for(i = 0 ; i <= DtMax ; ++i)
    	hist[i] = Hist[i];

    sum = 0;
    for(i = 0 ; i < AVENUM ; ++i)
    	sum += Hist[i];
    Hist[AVENUM / 2] = sum / AVENUM;
    for(i = AVENUM / 2 + 1 ; i <= DtMax - AVENUM / 2 ; ++i){
    	sum = sum - hist[i - AVENUM / 2 - 1] + hist[i + AVENUM / 2];
        Hist[i] = sum / AVENUM;
    }

    min = max = HistMaxQL();
    th = Hist[max] * threshold;

//    while(Dark.Hist[min] != 0){
    while(/* Hist[min] > Hist[min - 1] || */ Hist[min - 1] > th){
    	--min;
    	if(min == 0) break;
    }

//    while(Dark.Hist[Max] != 0){
    while(/* Hist[max] > Hist[max + 1] || */ Hist[max + 1] > th){
        ++max;
    	if(max == DtMax) break;
    }

    for(i = 0 ; i < SubPixel ; ++i){
    	for(j = 0 ; j < MainPixel ; ++j){
        	if(Dt[i][j] < min || Dt[i][j] > max){
                 bad[i][j] = true;
                 badExist = true;
            }
        }
    }

    delete[] hist;

    return(badExist);
}

//---------------------------------------------------------------------------
/*
	主走査方向奇数ライン取り出し
*/
void CTDAT::ExtractOddLine()
{
    int i, j;
    int main;
    CTDAT temp;

    main = MainPixel / 2;

    temp.NewDt(SubPixel, main, Bit);

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < main ; ++j)
        	temp.Dt[i][j] = Dt[i][j * 2 + 1];

    NewDt(SubPixel, main, Bit);

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < main ; ++j)
            Dt[i][j] = temp.Dt[i][j];
}
//---------------------------------------------------------------------------
/*
	主走査方向偶数ライン取り出し
*/
void CTDAT::ExtractEvenLine()
{
    int i, j;
    int main;
    CTDAT temp;

//	main = (MainPixel + 1) / 2.0;       // + 1は四捨五入のため
	main = (MainPixel + 1) / 2;//tsuji修正（画素数が奇数のときのための＋１。整数演算に修正）

    temp.NewDt(SubPixel, main, Bit);

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < main ; ++j)
        	temp.Dt[i][j] = Dt[i][j * 2];

    NewDt(SubPixel, main, Bit);

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < main ; ++j)
            Dt[i][j] = temp.Dt[i][j];
}
//---------------------------------------------------------------------------
/*
	主走査方向奇数ライン取り出し（ttsuji）
*/
void CTDAT::ExtractMainOddLine()
{
	ExtractOddLine();
}
//---------------------------------------------------------------------------
/*
	主走査方向偶数ライン取り出し（ttsuji）
*/
void CTDAT::ExtractMainEvenLine()
{
	ExtractEvenLine();
}
//---------------------------------------------------------------------------
/*
	副走査方向奇数ライン取り出し（ttsuji）
*/
void CTDAT::ExtractSubOddLine()
{
	int i, j;
	int SubHalf;
	CTDAT temp;

	SubHalf = SubPixel / 2;

	temp.NewDt(SubHalf, MainPixel, Bit);

	for (i = 0; i < SubHalf ; i++)
		for (j = 0; j < MainPixel ; j++)
			temp.Dt[i][j] = Dt[i * 2 + 1][j];

	NewDt(SubHalf, MainPixel, Bit);

	for (i = 0; i < SubHalf; i++)
		for (j = 0; j < MainPixel; j++)
			Dt[i][j] = temp.Dt[i][j];
}

//---------------------------------------------------------------------------
/*
	副走査方向偶数ライン取り出し（ttsuji）
*/
void CTDAT::ExtractSubEvenLine()
{
	int i, j;
	int SubHalf;
	CTDAT temp;

	SubHalf = (SubPixel + 1)/ 2;//tsuji修正（画素数が奇数のときのための＋１。整数演算に修正）

	temp.NewDt(SubHalf, MainPixel, Bit);

	for (i = 0; i < SubHalf ; i++)
		for (j = 0; j < MainPixel ; j++)
			temp.Dt[i][j] = Dt[i * 2][j];

	NewDt(SubHalf, MainPixel, Bit);

	for (i = 0; i < SubHalf; i++)
		for (j = 0; j < MainPixel; j++)
			Dt[i][j] = temp.Dt[i][j];
}


//---------------------------------------------------------------------------
/*
	主方向の移動平均
		Averaging個数が偶数の時前半dataを多くする。

    	引数	aveNum -> Averaging個数
*/
void CTDAT::MainMoveAve(const int iLength)
{
    int iMainCnt, iSubCnt;
    int iSubPos, iAddPos;
    int iHalfNum = iLength / 2;        //移動平均個数の半分
    int iDtSum;
    int iNum;
    int iNormalEndPos = MainPixel - iHalfNum;
    int  *piOrgDt;

    piOrgDt = new int[MainPixel];
    for(iSubCnt = 0 ; iSubCnt < SubPixel ; ++iSubCnt){
        for(iMainCnt = 0 ; iMainCnt < MainPixel ; ++iMainCnt){
            piOrgDt[iMainCnt] = Dt[iSubCnt][iMainCnt];
        }
        iAddPos = 1;
        iDtSum = Dt[iSubCnt][0];
        for(iMainCnt = 1 ; iMainCnt <= iHalfNum ; ++iMainCnt){
            iDtSum = iDtSum + piOrgDt[iAddPos] + piOrgDt[iAddPos + 1];
            iAddPos += 2;
            SetVal(iSubCnt, iMainCnt, float(iDtSum) / float(iAddPos));
        }
        iSubPos = 0;
        for(iMainCnt = iHalfNum + 1; iMainCnt < iNormalEndPos; ++iMainCnt){
            iDtSum = iDtSum - piOrgDt[iSubPos] + piOrgDt[iAddPos];
            SetVal(iSubCnt, iMainCnt, float(iDtSum) / float(iLength));
            ++iSubPos;
            ++iAddPos;
        }
        iNum = iLength - 2;
        for(iMainCnt = iNormalEndPos; iMainCnt < MainPixel; ++iMainCnt){
            iDtSum = iDtSum - piOrgDt[iSubPos] - piOrgDt[iSubPos + 1];
            SetVal(iSubCnt, iMainCnt, float(iDtSum) / float(iNum));
            iSubPos += 2;
            iNum -= 2;
        }
    }
}
//---------------------------------------------------------------------------
/*
	主方向の移動平均
		Averaging個数が偶数の時前半dataを多くする。

    	引数	aveNum -> Averaging個数
*/
void CTDAT::MainMoveAveAveExpand(const int aveNum)
{
    int i, j;
    int halfNum;        //移動平均個数の半分
    double sum;     //移動平均値を出すための合計
    double edgeSum;     //移動平均するとき、端部データの平均値を用いる。平均値計算用合計
    unsigned short int edge;     //移動平均するとき、端部データの平均値を用いる。その平均値
    CTDAT copy;      //画像のcopy
//    float residual;

    halfNum = aveNum / 2;

//移動平均する画像のcopy  端部データの平均値を両端に付け加える
    copy.NewDt(SubPixel, MainPixel + aveNum, Bit);
    for(j = 0 ; j < SubPixel ; ++j){
        edgeSum = 0;     //端部データの平均値計算
        for(i = 0 ; i <= halfNum ; ++i){
            edgeSum += Dt[j][i];
        }
        edge = edgeSum / (halfNum + 1);
        for(i = 0 ; i < halfNum ; ++i){
            copy.Dt[j][i] = edge;
        }

        for(i = 0 ; i < MainPixel ; ++i){
            copy.Dt[j][i + halfNum] = Dt[j][i];
        }

        edgeSum = 0;     //端部データの平均値計算
        for(i = MainPixel - halfNum - 1; i < MainPixel ; ++i){
            edgeSum += Dt[j][i];
        }
        edge = edgeSum / (halfNum + 1);
        for(i = 0 ; i < halfNum ; ++i){
            copy.Dt[j][MainPixel + halfNum + i] = edge;
        }
    }

//移動平均
    for(j = 0 ; j < SubPixel ; ++j){
        sum = 0.0;
        for(i = 0 ; i < aveNum ; ++i){
        	sum += copy.Dt[j][i];
        }
		Dt[j][0] = (unsigned short int)(sum / aveNum);
//        residual = sum / aveNum - Dt[j][0];
//        if((residual > float(rand()) / float(RAND_MAX)) && (Dt[j][0] < DtMax)) ++Dt[j][0];
        for(i = 1 ; i < MainPixel ; ++i){
        	sum = sum - copy.Dt[j][i - 1] +  copy.Dt[j][i + aveNum - 1];
			Dt[j][i] = (unsigned short int)(sum / aveNum);
//            residual = sum / aveNum - Dt[j][i];
//            if((residual > float(rand()) / (float(RAND_MAX)) && Dt[j][i] < DtMax)) ++Dt[j][i];
        }
    }
}
//---------------------------------------------------------------------------
//ttsuji（SPC画像期待値ソフトの端点処理を反映）
void CTDAT::MoveAveSPC(const int iMLength, const int iSLength, int bunbo)
{
	if (iMLength % 2 == 0 || iSLength % 2 == 0 || (1 == iMLength && 1 == iSLength)){//主副とも1pixのときは何もしないよう修正
		return;
	}
	int iMHalfNum = iMLength/2;
	int	iSHalfNum = iSLength/2;

	double tmp = 1.0 / ( iMLength * iSLength );
	int bunshi = (int)(tmp * bunbo);//切り捨て(ttsuji)
	CTDAT img_copy;      //画像のcopy
	Copy(img_copy);

	for( int i = 0 ; i < SubPixel ; i++ ){
		for( int j = 0 ; j < MainPixel ; j++ ){
			__int64 ret = 0;
			for( int l = -iSHalfNum ; l <= iSHalfNum ; l++ ){
				for( int k = -iMHalfNum ; k <= iMHalfNum  ; k++ ){
					int row = i + l;
					int col = j + k;
					if( (i + l) < 0 ) row = 0;
					if( (i + l) > (SubPixel - 1))
						row = SubPixel - 1;

					if( (j + k) < 0 ) col = 0 ;
					if( (j + k) > (MainPixel - 1) )
						col = MainPixel -1;
					ret += img_copy.Dt[row][col];
				}
			}
			ret *= bunshi;//切り捨て
			ret /= bunbo;//切り捨て

			if (ret < 0) {
				Dt[i][j] = 0;
			}
			else if (ret > DtMax) {
				Dt[i][j] = DtMax;
			}
			else{
				Dt[i][j] = ret;
			}
		}
	}
}

//---------------------------------------------------------------------------
/*
	副方向Median filter処理
		両端は1, 3,,,iNumまで広げながら処理する。
        filter長>画素数の時何もしない。

    	引数	iNum -> filter長
*/
void CTDAT::SubMedian(const int iNum)
{
	if(iNum == 1 || iNum % 2 == 0 || iNum > SubPixel){
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
	unsigned short int *pnOrg, *pnOrder;

	pnOrg = new unsigned short int[SubPixel];
	pnOrder = new unsigned short int[iNum];

	for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
		iPos = 1;
		iEnd = 0;
		iRemPos = 0;
		iAddPos = 1;
		iMed = 0;
		for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
			pnOrg[iSCnt] = Dt[iSCnt][iMCnt];
		}

		pnOrder[0] = pnOrg[0];
		while(iAddPos < iNum){
			iOrdCnt = iEnd;
			while(iOrdCnt >= 0 && pnOrder[iOrdCnt] > pnOrg[iAddPos]){
				pnOrder[iOrdCnt + 1] = pnOrder[iOrdCnt];
				--iOrdCnt;
			}
				//pnOrder[iOrdCnt] <= pnOrg[iAddPos]
			pnOrder[iOrdCnt + 1] = pnOrg[iAddPos];
			++iAddPos;
			++iEnd;

			iOrdCnt = iEnd;
			while(iOrdCnt >= 0 && pnOrder[iOrdCnt] > pnOrg[iAddPos]){
				pnOrder[iOrdCnt + 1] = pnOrder[iOrdCnt];
				--iOrdCnt;
			}
				//pnOrder[iOrdCnt] <= pnOrg[iAddPos]
			pnOrder[iOrdCnt + 1] = pnOrg[iAddPos];
			++iAddPos;
			++iEnd;

			++iMed;
			Dt[iPos][iMCnt] = pnOrder[iMed];
			++iPos;
		}

		while(iAddPos < SubPixel){
            iOrder = iFindPos(pnOrg[iRemPos], pnOrder, 0, iEnd);
/*
			iOrder = iEnd;
			while(pnOrder[iOrder] != pnOrg[iRemPos]){
				--iOrder;
			}
*/
			++iRemPos;

			if(pnOrg[iAddPos] >= pnOrder[iOrder]){
                ++iOrder;
                while(iOrder <= iEnd && pnOrder[iOrder] < pnOrg[iAddPos]){
                    pnOrder[iOrder - 1] = pnOrder[iOrder];
                    ++iOrder;
                }
                pnOrder[iOrder - 1] = pnOrg[iAddPos];
            }else{
                --iOrder;
                while(iOrder >= 0 && pnOrder[iOrder] > pnOrg[iAddPos]){
                    pnOrder[iOrder + 1] = pnOrder[iOrder];
                    --iOrder;
                }
                pnOrder[iOrder + 1] = pnOrg[iAddPos];
            }
			++iAddPos;

			Dt[iPos][iMCnt] = pnOrder[iMed];
			++iPos;
		}

		while(iRemPos < SubPixel - 2){
            iOrder = iFindPos(pnOrg[iRemPos], pnOrder, 0, iEnd);
			for(iCnt = iOrder; iCnt < iEnd; ++iCnt){
				pnOrder[iCnt] = pnOrder[iCnt + 1];
			}
			++iRemPos;
			--iEnd;

            iOrder = iFindPos(pnOrg[iRemPos], pnOrder, 0, iEnd);
			for(iCnt = iOrder; iCnt < iEnd; ++iCnt){
				pnOrder[iCnt] = pnOrder[iCnt + 1];
			}
			++iRemPos;
			--iEnd;

			--iMed;
			Dt[iPos][iMCnt] = pnOrder[iMed];
			++iPos;
		}
	}
	delete[] pnOrg;
	delete[] pnOrder;
}


//---------------------------------------------------------------------------
/*
	副方向Median filter処理
		両端は端iNum/2+1画素の平均をiNum/2個広げた処理。
		filter長>画素数の時何もしない。

		引数	iNum -> filter長
*/void CTDAT::SubMedianAveExpand( const int iNum )
{
	if( iNum <=1 || iNum&1 == 0 || iNum>SubPixel ){
	  return;
	}

	int iOrgSubPixel = SubPixel;

	SubAveExpand( iNum/2 );
	SubMedian( iNum );
	Format( iOrgSubPixel, MainPixel, -(iNum/2), 0, 0);

}

//---------------------------------------------------------------------------
/*
	副方向Median filter処理
		両端は端画素の画素値をiNum/2個広げた処理。
		filter長>画素数の時何もしない。

		引数	iNum -> filter長
*/void CTDAT::SubMedianBorderPixelExpand( const int iNum )
{
	if( iNum <=1 || iNum&1 == 0 || iNum>SubPixel ){
	  return;
	}

	int iOrgSubPixel = SubPixel;

	SubBorderPixelExpand(iNum/2);
	SubMedian(iNum);
	Format(iOrgSubPixel, MainPixel, -(iNum/2), 0, 0);

}

//---------------------------------------------------------------------------
/*
	副方向両端を端iNum+1画素の平均を使ってiNum広げる。
		iNum+1>画素数の時何もしない。

		引数	iNum -> 延長サイズ
*/
void CTDAT::SubAveExpand( const int iNum )
{
	if( iNum <=0 || iNum+1>SubPixel ){
	  return;
	}

	Format( SubPixel+2*iNum, MainPixel, iNum, 0, 0 );


	for( int iM=0; iM<MainPixel; iM++){
	  int iSt=0;
	  int iEd=0;

	  for( int iS=0; iS<=iNum; iS++ ){
		iSt += Dt[iS+iNum][iM];
		iEd += Dt[SubPixel-iNum	-iS-1][iM];
	  }

	  iSt /= (iNum+1);
	  iEd /= (iNum+1);

	  for( int iS=0; iS<iNum; iS++ ){
		Dt[iS][iM] = iSt;
		Dt[SubPixel-iS-1][iM] = iEd;
	  }

	}

}

//---------------------------------------------------------------------------
/*
	副方向両端を端画素の画素値を使ってiNum広げる。
		iNum+1>画素数の時何もしない。

		引数	iNum -> 延長サイズ
*/
void CTDAT::SubBorderPixelExpand(const int iNum)
{
	if( iNum <=0 || iNum+1>SubPixel ){
	  return;
	}

	Format( SubPixel+2*iNum, MainPixel, iNum, 0, 0 );

	for (int iM = 0; iM < MainPixel; iM++){
	  for (int iS = 0; iS < iNum; iS++){
		Dt[iS][iM] = Dt[iNum][iM];
		Dt[SubPixel-1-iS][iM] = Dt[SubPixel-1-iNum][iM];
	  }
	}
}
//---------------------------------------------------------------------------
/*  //2010.11.04 追加
	副方向の端からiNum画素をiNumの画素値で上書きする。
		iNum+1>画素数の時何もしない。

		引数	iNum -> 延長サイズ
*/
void CTDAT::SubPixelExpand( const int iNum )
{
	if( iNum <=0 || iNum+1>SubPixel ){
	  return;
	}


	for( int iM=0; iM<MainPixel; iM++){

	  for( int iS=0; iS<iNum; iS++ ){
		Dt[iS][iM] = Dt[iNum][iM];
		Dt[SubPixel-iS-1][iM] = Dt[SubPixel-iNum-1][iM];
	  }

	}

}

//---------------------------------------------------------------------------
//ttsuji追加（SPC（FPGAエミュレータ）の処理にあわせたもの）
void CTDAT::SubMedianSPC(const int size)
{
	if (size % 2 == 0){
		return;
	}

	int S_dy = size / 2;
	int *med;
	int *ret_med;
	med = new int [size];
	ret_med = new int [size];
	CTDAT img_tmp(SubPixel, MainPixel, Bit);

	int i, j, l;
	for (i = 0 ; i < SubPixel; i++){
		for (j = 0 ; j < MainPixel; j++){
			for (l = -S_dy ; l <= S_dy ; l++){
                int row = i + l;
                int col = j;
#if 0
				if (i + l < 0) row = S_dy + l;
				if (i + l > SubPixel - 1)
					row = SubPixel - 1 + l - S_dy;
#endif
				if (row < 0) row = 0;
				if (row > SubPixel - 1)
					row = SubPixel - 1 ;
				if (col < 0) col = 0;
				if (col > MainPixel - 1)
					col = MainPixel - 1;

				int id = S_dy + l ;
				med[id] = Dt[row][col];
			}
			quick_sort(med , ret_med , size);
			img_tmp.Dt[i][j] = ret_med[size/2];
        }
    }

	*this = img_tmp;

	delete [] med;
    delete [] ret_med;

    return;
}


//---------------------------------------------------------------------------
/*
	副方向の移動平均
		Averaging個数が偶数の時前半dataを多くする。

    	引数	aveNum -> Averaging個数
*/
void CTDAT::SubMoveAve(const int iLength)
{
    int iMainCnt, iSubCnt;
    int iSubPos, iAddPos;
    int iHalfNum = iLength / 2;        //移動平均個数の半分
    int iDtSum;
    int iNum;
    int iNormalEndPos = SubPixel - iHalfNum;
    int *piOrgDt;

    piOrgDt = new int[SubPixel];
    for(iMainCnt = 0 ; iMainCnt < MainPixel ; ++iMainCnt){
        for(iSubCnt = 0 ; iSubCnt < SubPixel ; ++iSubCnt){
            piOrgDt[iSubCnt] = Dt[iSubCnt][iMainCnt];
        }
        iAddPos = 1;
        iDtSum = Dt[0][iMainCnt];
        for(iSubCnt = 1 ; iSubCnt <= iHalfNum ; ++iSubCnt){
            iDtSum = iDtSum + piOrgDt[iAddPos] + piOrgDt[iAddPos + 1];
            iAddPos += 2;
            SetVal(iSubCnt, iMainCnt, float(iDtSum) / float(iAddPos));
        }
        iSubPos = 0;
        for(iSubCnt = iHalfNum + 1; iSubCnt < iNormalEndPos; ++iSubCnt){
            iDtSum = iDtSum - piOrgDt[iSubPos] + piOrgDt[iAddPos];
            SetVal(iSubCnt, iMainCnt, float(iDtSum) / float(iLength));
            ++iSubPos;
            ++iAddPos;
        }
        iNum = iLength - 2;
        for(iSubCnt = iNormalEndPos; iSubCnt < SubPixel; ++iSubCnt){
            iDtSum = iDtSum - piOrgDt[iSubPos] - piOrgDt[iSubPos + 1];
            SetVal(iSubCnt, iMainCnt, float(iDtSum) / float(iNum));
            iSubPos += 2;
            iNum -= 2;
        }
    }
}
//---------------------------------------------------------------------------
/*
	副方向の移動平均
		Averaging個数が偶数の時前半dataを多くする。

    	引数	aveNum -> Averaging個数
*/
void CTDAT::SubMoveAveAveExpand(const int aveNum)
{
    int i, j;
    int halfNum;        //移動平均個数の半分
    double sum;     //移動平均値を出すための合計
    double edgeSum;     //移動平均するとき、端部データの平均値を用いる。平均値計算用合計
    unsigned short int edge;     //移動平均するとき、端部データの平均値を用いる。その平均値
    CTDAT copy;      //画像のcopy
//    float residual;

    halfNum = aveNum / 2;

//Trend除去する画像のcopy  端部データの平均値を上下に付け加える
    copy.NewDt(SubPixel + aveNum,MainPixel, Bit);
    for(i = 0 ; i < MainPixel ; ++i){
        edgeSum = 0;     //端部データの平均値計算
        for(j = 0 ; j <= halfNum ; ++j){
            edgeSum += Dt[j][i];
        }
        edge = edgeSum / (halfNum + 1);
        for(j = 0 ; j < halfNum ; ++j){
            copy.Dt[j][i] = edge;
        }

        for(j = 0 ; j < SubPixel ; ++j){
            copy.Dt[j + halfNum][i] = Dt[j][i];
        }

        edgeSum = 0;     //端部データの平均値計算
        for(j = SubPixel - halfNum - 1; j < SubPixel ; ++j){
            edgeSum += Dt[j][i];
        }
        edge = edgeSum / (halfNum + 1);
        for(j = 0 ; j < halfNum ; ++j){
            copy.Dt[SubPixel + halfNum + j][i] = edge;
        }
    }

//移動平均
    for(i = 0 ; i < MainPixel ; ++i){
        sum = 0.0;
        for(j = 0 ; j <  aveNum ; ++j){
        	sum += copy.Dt[j][i];
        }
        Dt[0][i] =  (unsigned short int)(sum / aveNum);
//        residual = sum / aveNum - Dt[0][i];
//        if((residual > float(rand()) / float(RAND_MAX)) && (Dt[0][i] < DtMax)) ++Dt[0][i];
        for(j = 1 ; j < SubPixel ; ++j){
        	sum = sum - copy.Dt[j - 1][i] +  copy.Dt[j + aveNum - 1][i];
            Dt[j][i] = (unsigned short int)(sum / aveNum);
//            residual = sum / aveNum - Dt[j][i];
//            if((residual > float(rand()) / float(RAND_MAX)) && (Dt[j][i] < DtMax)) ++Dt[j][i];
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
void CTDAT::Filter(const float fFilter[], const int iLength, const bool isMain)
{
    int iHalfNum = iLength / 2;
    int iNormalEndPos;
    int iMainCnt, iSubCnt;
    int iDtCnt, iDtStartPos, iDtEndPos;
    int iFilterCnt, iFilterStartPos;
    float   fFilterSum;
    float   fDtSum;
    float *pfOrgDt;

    if(isMain){
        iNormalEndPos = MainPixel - iHalfNum;
        pfOrgDt = new float[MainPixel];
        for(iSubCnt = 0 ; iSubCnt < SubPixel ; ++iSubCnt){
            for(iMainCnt = 0 ; iMainCnt < MainPixel ; ++iMainCnt){
                pfOrgDt[iMainCnt] = Dt[iSubCnt][iMainCnt];
            }
            iDtEndPos = 3;
            iFilterStartPos = iHalfNum - 1;
            for(iMainCnt = 1 ; iMainCnt < iHalfNum ; ++iMainCnt){
                fDtSum = fFilter[iFilterStartPos] * pfOrgDt[0];
                fFilterSum = fFilter[iFilterStartPos];
                for(iDtCnt = 1, iFilterCnt = iFilterStartPos + 1; iDtCnt < iDtEndPos; ++iDtCnt, ++iFilterCnt){
                    fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
                    fFilterSum += fFilter[iFilterCnt];
                }
                SetVal(iSubCnt, iMainCnt, fDtSum / fFilterSum);
                iDtEndPos += 2;
                --iFilterStartPos;
            }
            iDtStartPos = 0;
            for(iMainCnt = iHalfNum; iMainCnt < iNormalEndPos; ++iMainCnt){
                fDtSum = fFilter[0] * pfOrgDt[iDtStartPos];
                for(iDtCnt = iDtStartPos + 1, iFilterCnt = 1; iFilterCnt < iLength; ++iDtCnt, ++iFilterCnt){
                    fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
                }
                SetVal(iSubCnt, iMainCnt, fDtSum);
                ++iDtStartPos;
            }
            iDtStartPos = MainPixel - iLength + 2;
            iFilterStartPos = 1;
            for(iMainCnt = iNormalEndPos; iMainCnt < MainPixel; ++iMainCnt){
                fDtSum = fFilter[iFilterStartPos] * pfOrgDt[iDtStartPos];
                fFilterSum = fFilter[iFilterStartPos];
                for(iDtCnt = iDtStartPos + 1, iFilterCnt = iFilterStartPos + 1; iDtCnt < MainPixel ; ++iDtCnt, ++iFilterCnt){
                    fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
                    fFilterSum += fFilter[iFilterCnt];
                }
                SetVal(iSubCnt, iMainCnt, fDtSum / fFilterSum);
                iDtStartPos += 2;
                ++iFilterStartPos;
            }
        }
    }else{
        iNormalEndPos = SubPixel - iHalfNum;
        pfOrgDt = new float[SubPixel];
        for(iMainCnt = 0 ; iMainCnt < MainPixel ; ++iMainCnt){
            for(iSubCnt = 0 ; iSubCnt < SubPixel ; ++iSubCnt){
                pfOrgDt[iSubCnt] = Dt[iSubCnt][iMainCnt];
            }
            iDtEndPos = 3;
            iFilterStartPos = iHalfNum - 1;
            for(iSubCnt = 1 ; iSubCnt < iHalfNum ; ++iSubCnt){
                fDtSum = fFilter[iFilterStartPos] * pfOrgDt[0];
                fFilterSum = fFilter[iFilterStartPos];
                for(iDtCnt = 1, iFilterCnt = iFilterStartPos + 1; iDtCnt < iDtEndPos; ++iDtCnt, ++iFilterCnt){
                    fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
                    fFilterSum += fFilter[iFilterCnt];
                }
                SetVal(iSubCnt, iMainCnt, fDtSum / fFilterSum);
                iDtEndPos += 2;
                --iFilterStartPos;
            }
            iDtStartPos = 0;
            for(iSubCnt = iHalfNum; iSubCnt < iNormalEndPos; ++iSubCnt){
                fDtSum = fFilter[0] * pfOrgDt[iDtStartPos];
                for(iDtCnt = iDtStartPos + 1, iFilterCnt = 1; iFilterCnt < iLength; ++iDtCnt, ++iFilterCnt){
                    fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
                }
                SetVal(iSubCnt, iMainCnt, fDtSum);
                ++iDtStartPos;
            }
            iDtStartPos = SubPixel - iLength + 2;
            iFilterStartPos = 1;
            for(iSubCnt = iNormalEndPos; iSubCnt < SubPixel; ++iSubCnt){
                fDtSum = fFilter[iFilterStartPos] * pfOrgDt[iDtStartPos];
                fFilterSum = fFilter[iFilterStartPos];
                for(iDtCnt = iDtStartPos + 1, iFilterCnt = iFilterStartPos + 1; iDtCnt < SubPixel ; ++iDtCnt, ++iFilterCnt){
                    fDtSum += fFilter[iFilterCnt] * pfOrgDt[iDtCnt];
                    fFilterSum += fFilter[iFilterCnt];
                }
                SetVal(iSubCnt, iMainCnt, fDtSum / fFilterSum);
                iDtStartPos += 2;
                ++iFilterStartPos;
            }
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
void CTDAT::Filter_long(const int iFilter[], const int iLength, const bool isMain)
{
    int iHalfNum = iLength / 2;
    int iNormalEndPos;
    int iMainCnt, iSubCnt;
    int iDtCnt, iDtStartPos, iDtEndPos;
    int iFilterCnt, iFilterStartPos;
	int iFilterSum = 0;
	long   lDtSum;
	long *plOrgDt;

	int iFilterSum2 = 0;
	for (int i = 0; i < iLength; i++) {
		iFilterSum2 += iFilter[i];
	}

	if(isMain){
		iNormalEndPos = MainPixel - iHalfNum;
		plOrgDt = new long[MainPixel];
		for(iSubCnt = 0 ; iSubCnt < SubPixel ; ++iSubCnt){
			for(iMainCnt = 0 ; iMainCnt < MainPixel ; ++iMainCnt){
				plOrgDt[iMainCnt] = Dt[iSubCnt][iMainCnt];
			}
			iDtEndPos = 3;
			iFilterStartPos = iHalfNum - 1;
			for(iMainCnt = 1 ; iMainCnt < iHalfNum ; ++iMainCnt){
				lDtSum = iFilter[iFilterStartPos] * plOrgDt[0];
				iFilterSum = iFilter[iFilterStartPos];
				for(iDtCnt = 1, iFilterCnt = iFilterStartPos + 1; iDtCnt < iDtEndPos; ++iDtCnt, ++iFilterCnt){
					lDtSum += iFilter[iFilterCnt] * plOrgDt[iDtCnt];
					iFilterSum += iFilter[iFilterCnt];
				}
#if 1
				SetValWithTruncation(iSubCnt, iMainCnt, (float)lDtSum / iFilterSum);
#else
				SetVal(iSubCnt, iMainCnt, (float)lDtSum / iFilterSum);
#endif
				iDtEndPos += 2;
				--iFilterStartPos;
			}
			iDtStartPos = 0;
			for(iMainCnt = iHalfNum; iMainCnt < iNormalEndPos; ++iMainCnt){
				lDtSum = iFilter[0] * plOrgDt[iDtStartPos];
				for(iDtCnt = iDtStartPos + 1, iFilterCnt = 1; iFilterCnt < iLength; ++iDtCnt, ++iFilterCnt){
					lDtSum += iFilter[iFilterCnt] * plOrgDt[iDtCnt];
				}
#if 1
				SetValWithTruncation(iSubCnt, iMainCnt, (float)lDtSum / iFilterSum2);
#else
				SetVal(iSubCnt, iMainCnt, (float)lDtSum / iFilterSum2);
#endif
				++iDtStartPos;
			}
			iDtStartPos = MainPixel - iLength + 2;
			iFilterStartPos = 1;
			for(iMainCnt = iNormalEndPos; iMainCnt < MainPixel; ++iMainCnt){
				lDtSum = iFilter[iFilterStartPos] * plOrgDt[iDtStartPos];
				iFilterSum = iFilter[iFilterStartPos];
				for(iDtCnt = iDtStartPos + 1, iFilterCnt = iFilterStartPos + 1; iDtCnt < MainPixel ; ++iDtCnt, ++iFilterCnt){
					lDtSum += iFilter[iFilterCnt] * plOrgDt[iDtCnt];
					iFilterSum += iFilter[iFilterCnt];
				}
#if 1
				SetValWithTruncation(iSubCnt, iMainCnt, (float)lDtSum / iFilterSum);
#else
				SetVal(iSubCnt, iMainCnt, (float)lDtSum / iFilterSum);
#endif
				iDtStartPos += 2;
				++iFilterStartPos;
			}
		}
	}else{
		iNormalEndPos = SubPixel - iHalfNum;
		plOrgDt = new long[SubPixel];
		for(iMainCnt = 0 ; iMainCnt < MainPixel ; ++iMainCnt){
			for(iSubCnt = 0 ; iSubCnt < SubPixel ; ++iSubCnt){
				plOrgDt[iSubCnt] = Dt[iSubCnt][iMainCnt];
			}
			iDtEndPos = 3;
			iFilterStartPos = iHalfNum - 1;
			for(iSubCnt = 1 ; iSubCnt < iHalfNum ; ++iSubCnt){
				lDtSum = iFilter[iFilterStartPos] * plOrgDt[0];
				iFilterSum = iFilter[iFilterStartPos];
				for(iDtCnt = 1, iFilterCnt = iFilterStartPos + 1; iDtCnt < iDtEndPos; ++iDtCnt, ++iFilterCnt){
					lDtSum += iFilter[iFilterCnt] * plOrgDt[iDtCnt];
					iFilterSum += iFilter[iFilterCnt];
				}
#if 1
				SetValWithTruncation(iSubCnt, iMainCnt, (float)lDtSum / iFilterSum);
#else
				SetVal(iSubCnt, iMainCnt, (float)lDtSum / iFilterSum);
#endif
				iDtEndPos += 2;
				--iFilterStartPos;
			}
			iDtStartPos = 0;
			for(iSubCnt = iHalfNum; iSubCnt < iNormalEndPos; ++iSubCnt){
				lDtSum = iFilter[0] * plOrgDt[iDtStartPos];
				for(iDtCnt = iDtStartPos + 1, iFilterCnt = 1; iFilterCnt < iLength; ++iDtCnt, ++iFilterCnt){
					lDtSum += iFilter[iFilterCnt] * plOrgDt[iDtCnt];
				}
#if 1
				SetValWithTruncation(iSubCnt, iMainCnt, (float)lDtSum / iFilterSum2);
#else
				SetVal(iSubCnt, iMainCnt, (float)lDtSum / iFilterSum2);
#endif
				++iDtStartPos;
			}
			iDtStartPos = SubPixel - iLength + 2;
			iFilterStartPos = 1;
			for(iSubCnt = iNormalEndPos; iSubCnt < SubPixel; ++iSubCnt){
				lDtSum = iFilter[iFilterStartPos] * plOrgDt[iDtStartPos];
				iFilterSum = iFilter[iFilterStartPos];
				for(iDtCnt = iDtStartPos + 1, iFilterCnt = iFilterStartPos + 1; iDtCnt < SubPixel ; ++iDtCnt, ++iFilterCnt){
					lDtSum += iFilter[iFilterCnt] * plOrgDt[iDtCnt];
					iFilterSum += iFilter[iFilterCnt];
				}
#if 1
				SetValWithTruncation(iSubCnt, iMainCnt, (float)lDtSum / iFilterSum);
#else
				SetVal(iSubCnt, iMainCnt, (float)lDtSum / iFilterSum);
#endif
				iDtStartPos += 2;
				++iFilterStartPos;
			}
		}
	}

	delete[] plOrgDt;
}
//---------------------------------------------------------------------------
/*
	主方向Median filter処理
		両端は1, 3,,,iNumまで広げながら処理する。
        filter長>画素数の時何もしない。

    	引数	iNum -> filter長
*/
void CTDAT::MainMedian(const int iNum)
{
	if(iNum == 1 || iNum % 2 == 0 || iNum > MainPixel){
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
	unsigned short int *pnOrg, *pnOrder;

	pnOrg = new unsigned short int[MainPixel];
	pnOrder = new unsigned short int[iNum];

	for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
		iPos = 1;
		iEnd = 0;
		iRemPos = 0;
		iAddPos = 1;
		iMed = 0;
		for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
			pnOrg[iMCnt] = Dt[iSCnt][iMCnt];
		}

		pnOrder[0] = pnOrg[0];
		while(iAddPos < iNum){
			iOrdCnt = iEnd;
			while(iOrdCnt >= 0 && pnOrder[iOrdCnt] > pnOrg[iAddPos]){
				pnOrder[iOrdCnt + 1] = pnOrder[iOrdCnt];
				--iOrdCnt;
			}
				//pnOrder[iOrdCnt] <= pnOrg[iAddPos]
			pnOrder[iOrdCnt + 1] = pnOrg[iAddPos];
			++iAddPos;
			++iEnd;

			iOrdCnt = iEnd;
			while(iOrdCnt >= 0 && pnOrder[iOrdCnt] > pnOrg[iAddPos]){
				pnOrder[iOrdCnt + 1] = pnOrder[iOrdCnt];
				--iOrdCnt;
			}
				//pnOrder[iOrdCnt] <= pnOrg[iAddPos]
			pnOrder[iOrdCnt + 1] = pnOrg[iAddPos];
			++iAddPos;
			++iEnd;

			++iMed;
			Dt[iSCnt][iPos] = pnOrder[iMed];
			++iPos;
		}

		while(iAddPos < MainPixel){
            iOrder = iFindPos(pnOrg[iRemPos], pnOrder, 0, iEnd);
/*
			iOrder = iEnd;
			while(pnOrder[iOrder] != pnOrg[iRemPos]){
				--iOrder;
			}
*/
			++iRemPos;

			if(pnOrg[iAddPos] >= pnOrder[iOrder]){
                ++iOrder;
                while(iOrder <= iEnd && pnOrder[iOrder] < pnOrg[iAddPos]){
                    pnOrder[iOrder - 1] = pnOrder[iOrder];
                    ++iOrder;
                }
                pnOrder[iOrder - 1] = pnOrg[iAddPos];
            }else{
                --iOrder;
                while(iOrder >= 0 && pnOrder[iOrder] > pnOrg[iAddPos]){
                    pnOrder[iOrder + 1] = pnOrder[iOrder];
                    --iOrder;
                }
                pnOrder[iOrder + 1] = pnOrg[iAddPos];
            }
			++iAddPos;

			Dt[iSCnt][iPos] = pnOrder[iMed];
			++iPos;
		}

		while(iRemPos < MainPixel - 2){
            iOrder = iFindPos(pnOrg[iRemPos], pnOrder, 0, iEnd);
			for(iCnt = iOrder; iCnt < iEnd; ++iCnt){
				pnOrder[iCnt] = pnOrder[iCnt + 1];
			}
			++iRemPos;
			--iEnd;

            iOrder = iFindPos(pnOrg[iRemPos], pnOrder, 0, iEnd);
			for(iCnt = iOrder; iCnt < iEnd; ++iCnt){
				pnOrder[iCnt] = pnOrder[iCnt + 1];
			}
			++iRemPos;
			--iEnd;

			--iMed;
			Dt[iSCnt][iPos] = pnOrder[iMed];
			++iPos;
		}
	}
	delete[] pnOrg;
	delete[] pnOrder;
}

//---------------------------------------------------------------------------
/*
	主方向Median filter処理
		両端は端iNum/2+1画素の平均をiNum/2個広げた処理。
		filter長>画素数の時何もしない。

		引数	iNum -> filter長
*/void CTDAT::MainMedianAveExpand ( const int iNum )
{
	if( iNum <=1 || iNum&1 == 0 || iNum>MainPixel ){
	  return;
	}

	int iOrgMainPixel = MainPixel;

	MainAveExpand( iNum/2 );
	MainMedian( iNum );
	Format( SubPixel, iOrgMainPixel, 0, -(iNum/2), 0);

}
//---------------------------------------------------------------------------
/*
	主方向両端を端iNum+1画素の平均を使ってiNum広げる。
		iNum+1>画素数の時何もしない。

		引数	iNum -> 延長サイズ
*/void CTDAT::MainAveExpand( const int iNum )
{
	if( iNum <=0 || iNum+1>MainPixel ){
	  return;
	}

	Format( SubPixel, MainPixel+2*iNum, 0, iNum, 0 );


	for( int iS=0; iS<SubPixel; iS++){
	  int iSt=0;
	  int iEd=0;

	  for( int iM=0; iM<=iNum; iM++ ){
		iSt += Dt[iS][iM+iNum];
		iEd += Dt[iS][MainPixel-iNum-iM-1];
	  }

	  iSt /= (iNum+1);
	  iEd /= (iNum+1);

	  for( int iM=0; iM<iNum; iM++ ){
		Dt[iS][iM] = iSt;
		Dt[iS][MainPixel-iM-1] = iEd;
	  }

	}

}
//---------------------------------------------------------------------------
/*  //ttsuji追加160720（透視TFT段差補正のMedian端処理用として）
	主方向Median filter処理
		両端は端iNum/2画素の平均をiNum/2個広げた処理。
		filter長>画素数の時何もしない。

		引数	iNum -> filter長
*/void CTDAT::MainMedianAveExpand2( const int iNum )
{
	if( iNum <=1 || iNum&1 == 0 || iNum>MainPixel ){
	  return;
	}

	int iOrgMainPixel = MainPixel;

	MainAveExpand2( iNum/2 );
	MainMedian( iNum );
	Format( SubPixel, iOrgMainPixel, 0, -(iNum/2), 0);

}
//---------------------------------------------------------------------------
/*   //ttsuji追加160720（透視TFT段差補正のMedian端処理用として）
	主方向両端を端iNum画素の平均を使ってiNum広げる。
		iNum+1>画素数の時何もしない。

		引数	iNum -> 延長サイズ
*/void CTDAT::MainAveExpand2( const int iNum )
{
	if( iNum <=0 || iNum+1>MainPixel ){
	  return;
	}

	Format( SubPixel, MainPixel+2*iNum, 0, iNum, 0 );


	for( int iS=0; iS<SubPixel; iS++){
	  int iSt=0;
	  int iEd=0;

	  for( int iM=0; iM<iNum; iM++ ){
		iSt += Dt[iS][iM+iNum];
		iEd += Dt[iS][MainPixel-iNum-iM-1];
	  }

	  iSt /= iNum;
	  iEd /= iNum;

	  for( int iM=0; iM<iNum; iM++ ){
		Dt[iS][iM] = iSt;
		Dt[iS][MainPixel-iM-1] = iEd;
	  }

	}

}
//---------------------------------------------------------------------------
/*  //2010.11.04 追加
	主方向の端からiNum画素をiNumの画素値で上書きする。
		iNum+1>画素数の時何もしない。

		引数	iNum -> 延長サイズ
*/
void CTDAT::MainPixelExpand( const int iNum )
{
	if( iNum <=0 || iNum+1>MainPixel ){
	  return;
	}


	for( int iS=0; iS<SubPixel; iS++){

	  for( int iM=0; iM<iNum; iM++ ){
		Dt[iS][iM] = Dt[iS][iNum];
		Dt[iS][MainPixel-iM-1] = Dt[iS][MainPixel-iNum-1];
	  }

	}

}

//---------------------------------------------------------------------------
//ttsuji追加（SPC（FPGAエミュレータ）の処理にあわせたもの）
void CTDAT::MainMedianSPC(const int size)
{
	if (size % 2 == 0) {
		return;
	}

    int S_dx = size / 2 ;
    int *med;
	int *ret_med;
	med = new int [size];
    ret_med = new int [size];
	CTDAT img_tmp(SubPixel, MainPixel, Bit);

	int i, j, k;
	for (i = 0; i < SubPixel; i++){
		for (j = 0; j < MainPixel; j++){
			for (k = -S_dx; k <= S_dx; k++){
				int row = i;
				int col = j + k;
#if 0
				if( j + k < 0 ) col = S_dx + k ;
				if( j + k > MainPixel - 1 )
					col = MainPixel -1 + k - S_dx ;
#endif

				if (row < 0)
					row = 0;
				if (row > SubPixel - 1)
					row = SubPixel - 1 ;
				if (col < 0)
					col = 0;
				if (col > MainPixel - 1)
					col = MainPixel - 1;

				int id = S_dx+k ;
				med[id] = Dt[row][col];
			}
			quick_sort(med , ret_med , size);
			img_tmp.Dt[i][j] = ret_med[size/2];
		}
	}

	*this = img_tmp;

    delete [] med;
    delete [] ret_med;

    return;
}

//---------------------------------------------------------------------------
/*
	shading補正
        元画像またはshadingの狭い方の領域のみ補正
        shading data = 0のときはDtMaxへclip
*/
void CTDAT::Shading(const CTDAT& shading,     //	shading補正（画像）
        const float fReg)       //規格化値
{
    int iMCnt, iSCnt;
    int iMPixel, iSPixel;

    if(MainPixel < shading.MainPixel){
        iMPixel = shading.MainPixel;
    }else{
        iMPixel = MainPixel;
    }
    if(SubPixel < shading.SubPixel){
        iSPixel = shading.SubPixel;
    }else{
        iSPixel = SubPixel;
    }
    for(iSCnt = 0; iSCnt < iSPixel; ++iSCnt){
        for(iMCnt = 0; iMCnt < iMPixel; ++iMCnt){
            if(shading.Dt[iSCnt][iMCnt] == 0){
                Dt[iSCnt][iMCnt] = DtMax;
            }else{
                SetVal(iSCnt, iMCnt, float(Dt[iSCnt][iMCnt]) /
                        float(shading.Dt[iSCnt][iMCnt]) * fReg);
            }
        }
    }
}
//---------------------------------------------------------------------------
/*
	横へ画像連結
    	bit数は連結する画像の大きい方とする

        引数	TTMA side	横に連結する画像
        		const int background	連結する画像サイズが異なる場合の隙間を
                						埋める値。引数無しの時は最大値。
*/
void CTDAT::SideConnect(const CTDAT& side, int background)
{
	int i,j;
    int bit;
    CTDAT temp;

    temp.NewDt(SubPixel, MainPixel, Bit);		//元画像の退避
    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	temp.Dt[i][j] = Dt[i][j];

    if(temp.Bit > side.Bit){		//bit数は連結する画像の大きい方とする
    	bit = temp.Bit;
    }else{
    	bit = side.Bit;
    }

    if(background == -1) background = (1 << bit) - 1;		//background 引数無しの時は最大値

    if(temp.SubPixel > side.SubPixel){		//連結画像の設定
    	NewDt(temp.SubPixel, temp.MainPixel + side.MainPixel, bit);
    }else{
    	NewDt(side.SubPixel, temp.MainPixel + side.MainPixel, bit);
    }
    FillValue((unsigned short int)background);	//backgroundで埋める

    for(i = 0 ; i < temp.SubPixel ; ++i)		//元画像をcopy
    	for(j = 0 ; j < temp.MainPixel ; ++j)
        	Dt[i][j] = temp.Dt[i][j];

    for(i = 0 ; i < side.SubPixel ; ++i)		//連結画像をcopy
    	for(j = 0 ; j < side.MainPixel ; ++j)
        	Dt[i][j + temp.MainPixel] = side.Dt[i][j];
}

//---------------------------------------------------------------------------
/*
	画素取り出し
    引数×n(n=0,1,,,)番目の画素を抜き出した画像に変換する
*/
void CTDAT::Extract(const int iSub, const int iSubOffset,
            const int iMain, const int iMainOffset)
{
    int iMainCnt, iSubCnt;
    int iMPixel, iSPixel;
    CTDAT temp;

    temp.NewDt(SubPixel, MainPixel, Bit);

    iSPixel = 0;
    for(iSubCnt = iSubOffset ; iSubCnt < SubPixel ; iSubCnt += iSub){
        iMPixel = 0;
    	for(iMainCnt = iMainOffset ; iMainCnt < MainPixel ; iMainCnt += iMain){
        	temp.Dt[iSPixel][iMPixel] = Dt[iSubCnt][iMainCnt];
            ++iMPixel;
        }
        ++iSPixel;
    }

    NewDt(iSPixel, iMPixel, Bit);

    for(iSubCnt = 0 ; iSubCnt < SubPixel ; ++iSubCnt)
    	for(iMainCnt = 0 ; iMainCnt < MainPixel ; ++iMainCnt)
            Dt[iSubCnt][iMainCnt] = temp.Dt[iSubCnt][iMainCnt];
}

//---------------------------------------------------------------------------
/*
	Dtを全てnValにする

    	引数	nVal    埋める値
*/
void CTDAT::FillValue(const unsigned short int nVal)
{
    int iMainCnt, iSubCnt;

    for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt)
        for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt)
            Dt[iSubCnt][iMainCnt] = nVal;
}

//---------------------------------------------------------------------------
/*
	上下入れ換え
*/
void CTDAT::UpDown()
{
    int i, j;
    CTDAT temp;

    temp.NewDt(SubPixel, MainPixel, Bit);

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	temp.Dt[i][j] = Dt[i][j];

    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
            Dt[i][j] = temp.Dt[SubPixel - 1 - i][j];
}

//---------------------------------------------------------------------------
/*
    画像回転。
    主副入れ替えと反転で作れるが、わかりやすさと
    １回の処理で出来たほうがいいと思うので。
*/
void CTDAT::Rotate(const int angle)
{
    CTDAT temp;
    temp.NewDt(SubPixel, MainPixel, Bit);  // 回転後、destructorで消される
    int Mpixel = MainPixel;  // 途中で書き換わるので、
    int Spixel = SubPixel;   // 混乱せぬように変数を定義しておく

    // 元データ保存
    for(int iS = 0 ; iS < SubPixel ; ++iS){
      for(int iM = 0 ; iM < MainPixel ; ++iM){
       	temp.Dt[iS][iM] = Dt[iS][iM];
      }
    }

    // 左90度        主副入れ替え+左右反転でもOK
    if( 90 == angle ){
      NewDt( Mpixel, Spixel, Bit );
      for(int iS=0; iS<Mpixel; iS++){
        for(int iM=0; iM<Spixel; iM++){
          Dt[iS][iM] = temp.Dt[Spixel-iM-1][iS];
        }
      }
    // 180度       左右反転+上下反転でもOK
    } else if( 180 == angle ){
      NewDt( Spixel, Mpixel, Bit );
      for(int iS=0; iS<Spixel; iS++){
        for(int iM=0; iM<Mpixel; iM++){
          Dt[iS][iM] = temp.Dt[Spixel-iS-1][Mpixel-iM-1];
        }
      }
    // 左270度       主副入れ替え+上下反転でもOK
    } else if( 270 == angle ){
      NewDt( Mpixel, Spixel, Bit );
      for(int iS=0; iS<Mpixel; iS++){
        for(int iM=0; iM<Spixel; iM++){
          Dt[iS][iM] = temp.Dt[iM][Mpixel-iS-1];
        }
      }
    } else {
      // 角度が90の倍数でないときは何もしない
    }

}
//---------------------------------------------------------------------------
//	CW方向90°回転
void CTDAT::RotateCW()
{
    CTDAT temp(*this);
	NewDt(MainPixel, SubPixel, Bit);
	for(int iSCnt = 0, iTempSCnt = SubPixel - 1; iSCnt < SubPixel; ++iSCnt, --iTempSCnt){
    	for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
            Dt[iSCnt][iMCnt] = temp.Dt[iMCnt][iTempSCnt];
        }
    }
}
//---------------------------------------------------------------------------
//画像を任意の点を基準に回転させる(170219ttsuji)
void CTDAT::RotateAnyAngle(double theta, int centerX, int centerY)
{
//	CTDAT temp(*this);
	CTDAT temp(SubPixel, MainPixel, Bit);
	int iM, iS;

	for (iS = 0; iS < SubPixel; iS++){
		for (iM = 0; iM < MainPixel; iM++){
			temp.Dt[iS][iM] = this->Dt[iS][iM];
		}
	}

	int index;
	int widthStep;
	float pointX, pointY;
	double radian = theta / 180.0 * PI;
	double cosRadian = cos(radian);
	double sinRadian = sin(radian);
	int iXBorder, iYBorder;//回転前の画像境界が回転後の画像の内側に初めて入るときの座標
	bool bFindXBorder(false), bFindYBorder(false);

	for (iS = 0; iS < SubPixel; iS++){
		for (iM = 0; iM < MainPixel; iM++){
			pointX = (float)(((iM - centerX) * cosRadian - (iS - centerY) * sinRadian + centerX));
			pointY = (float)(((iM - centerX) * sinRadian + (iS - centerY) * cosRadian + centerY));
			// pointX, pointYが入力画像の有効範囲にあれば出力画像へ代入する
			if(pointX >= 0.0 && pointX < MainPixel && pointY >= 0.0 && pointY < SubPixel){
#if 0			//近傍方式
				Dt[iS][iM] = temp.Dt[(int)pointY][(int)pointX];
#else			//バイリニア補間方式
				Dt[iS][iM] = temp.GetBilinear(pointY,pointX);
#endif
				if (!bFindXBorder) {
					iXBorder = iM;
					bFindXBorder = true;
				}
			}
			else {
				Dt[iS][iM] = 0;
			}

		}
	}
}
//---------------------------------------------------------------------------
/*
画像を任意の点を基準に回転させる(210301ttsuji)
回転前の画像の境界よりも外側の画素を仮想的に作成して回転後の画像を作成する

bool bExtendVirtualQL   //回転前の画像の境界よりも外側の画素を仮想的に作成して回転後の画像を作成するかのフラグ
	int iInternalWidth   	//回転前の画像の境界よりも何画素内側の画像を使用するか（境界が異常な場合の処理）default = 0

*/

void CTDAT::RotateAnyAngle(double theta, int centerX, int centerY, bool bExtendVirtualQL, int iInternalWidth)
{
	CTDAT temp(*this);
	CTDAT ImgExtAreaFlag(*this);//回転前の画像の領域外である場合1,そうでない場合0の2値画像
   	ImgExtAreaFlag.FillValue(0);

	int iM, iS;
	int index;
	int widthStep;
	int pointX, pointY;
	double radian = theta / 180.0 * PI;
	double cosRadian = cos(radian);
	double sinRadian = sin(radian);
	int iXBorder(-1), iYBorder(-1);//回転前の画像境界が回転後の画像の内側に初めて入るときの座標
	bool bFindXBorder(false), bFindYBorder(false);

	for (iS = 0; iS < SubPixel; iS++){
		for (iM = 0; iM < MainPixel; iM++){
			pointX = (int)((iM - centerX) * cosRadian - (iS - centerY) * sinRadian + centerX);
			pointY = (int)((iM - centerX) * sinRadian + (iS - centerY) * cosRadian + centerY);

			// pointX, pointYが入力画像の有効範囲にあれば出力画像へ代入する
//			if(pointX >= 0 && pointX < MainPixel && pointY >= 0 && pointY < SubPixel){
			if(pointX >= iInternalWidth && pointX < MainPixel - iInternalWidth && pointY >= iInternalWidth && pointY < SubPixel - iInternalWidth){
				Dt[iS][iM] = temp.Dt[pointY][pointX];

				if (!bFindXBorder & !bFindYBorder) {//両方ともfalseの場合
					iXBorder = iM;
					bFindXBorder = true;
				}
			}
			else {
				Dt[iS][iM] = 0;
				ImgExtAreaFlag.SetVal(iS, iM, 1);

				if (bFindXBorder & !bFindYBorder) {//XのみTrueの場合
					iYBorder = MainPixel - iM;//MainPixel - 1 - (iM - 1)
					bFindYBorder = true;
				}
			}

		}
	}

	//回転前の画像の境界よりも外側の画素を仮想的に作成して回転後の画像を作成する場合
	if (bExtendVirtualQL) {
		if (!iXBorder) {
        	return;
		}
		//回転前の画像の境界よりも外側の画素を境界に関して線対称になるように内側の画素をコピーする
		//x原点側、y原点側の空白を埋める
		LineSymmetricalCopy(iXBorder, iYBorder, ImgExtAreaFlag);

		//x終端側、y原点側の空白を埋める
		RtoL();	ImgExtAreaFlag.RtoL();//左右反転
		LineSymmetricalCopy(iYBorder, iXBorder, ImgExtAreaFlag);
		RtoL();	ImgExtAreaFlag.RtoL();//左右反転

		//x原点側、y終端側の空白を埋める
		UpDown();	ImgExtAreaFlag.UpDown();//上下反転
		LineSymmetricalCopy(iYBorder, iXBorder, ImgExtAreaFlag);
		UpDown();	ImgExtAreaFlag.UpDown();//上下反転

		//x終端側、y終端側の空白を埋める
		RtoL();	ImgExtAreaFlag.RtoL();//左右反転
		UpDown();	ImgExtAreaFlag.UpDown();//上下反転
		LineSymmetricalCopy(iXBorder, iYBorder, ImgExtAreaFlag);
		UpDown();	ImgExtAreaFlag.UpDown();//上下反転
		RtoL();	ImgExtAreaFlag.RtoL();//左右反転
	}

}

//---------------------------------------------------------------------------
/*
	回転処理に際して、回転前の画像の境界よりも外側の画素を境界に関して線対称になるように内側の画素をコピーする（x原点側、y原点側の空白を埋める）　ttsuji追加210302

int iXBorder			//回転後画像におけるy=0における境界のx座標
int iYBorder            //回転後画像におけるx=0における境界のy座標
TDAT& ImgExtAreaFlag    //回転後画像における境界よりも外側のフラグ画像

*/
void CTDAT::LineSymmetricalCopy(int iXBorder, int iYBorder, CTDAT& ImgExtAreaFlag)
{
	if (!DtExist) {
        return;
	}

	int iM, iS;
	double dXInside, dYInside;
	double dBunbo = iXBorder * iXBorder + iYBorder * iYBorder;
	double dA = (double)iXBorder * iXBorder - iYBorder * iYBorder;
	for (iS = 0; iS <= iYBorder; iS++){
		for (iM = 0; iM <= iXBorder; iM++){
			if (ImgExtAreaFlag.Dt[iS][iM]) {//回転前の画像の領域外の場合
				dYInside = (2 * iXBorder * iXBorder * iYBorder - 2 * iXBorder * iYBorder * iM - dA * iS) / dBunbo;
				dXInside = iM + (double)iYBorder / iXBorder * (dYInside - iS);
				if (round(dYInside) < 0 || round(dYInside) > SubPixel - 1 || round(dXInside) < 0 || round(dXInside) > MainPixel - 1) {
					continue;
				}
 #if 0
				Dt[iS][iM] = Dt[round(dYInside)][round(dXInside)];
 #elif 0
				Dt[iS][iM] = Dt[(int)dYInside + 1][(int)dXInside + 1];//切り上げることで右辺が境界部の0QLとなることがなくなる　→まだ不足
 #elif 0
				Dt[iS][iM] = Dt[(int)dYInside + 2][(int)dXInside + 2];//切り上げることで右辺が境界部の0QLとなることがなくなる　→まだ不足
 #elif 0
				Dt[iS][iM] = Dt[(int)dYInside + 3][(int)dXInside + 3];//切り上げることで右辺が境界部の0QLとなることがなくなる
 #else
				Dt[iS][iM] = Dt[(int)dYInside + 6][(int)dXInside + 6];//切り上げることで右辺が境界部の0QLとなることがなくなる
 #endif
			}
		}
	}
}
void CTDAT::TrimArea(int& iS0, int& iM0, int& iS1, int& iM1)
{
    if(iM0 < 0){
        iM0 = 0;
    }
    if(iS0 < 0){
        iS0 = 0;
    }
    if(MainPixel <= iM1){
        iM1 = MainPixel - 1;
    }
    if(SubPixel <= iS1){
        iS1 = SubPixel - 1;
    }
    if(iM1 < iM0){
        if(MainPixel <= iM0){
            iM0 = MainPixel - 1;
        }
        iM1 = iM0;
    }
    if(iS1 < iS0){
        if(SubPixel <= iS0){
            iS0 = SubPixel - 1;
        }
        iS1 = iS0;
    }
}

void CTDAT::Clip(int iMin, int iMax)
{
	int iTmp;
	//大小関係チェック
	if (iMin > iMax) {
		iTmp = iMin;
		iMin = iMax;
		iMax = iTmp;
	}
	//クリップ処理
	for (int iS = 0; iS < SubPixel; iS++){
		for (int iM = 0; iM < MainPixel; iM++){
			Dt[iS][iM] = CLIP(Dt[iS][iM], iMin, iMax);
		}
	}
}
//画像上の長方形の辺の画素値を修正する
CTDAT& CTDAT::FillRectLineWithQLShift(TRECTANGLE rect, const int iQLShift)
{
	int iM0 = rect.GetM0();
	int iS0 = rect.GetS0();
	int iM1 = rect.GetM1();
	int iS1 = rect.GetS1();

	bool bX, bY;
	int iM, iS;
	for (iS = 0; iS < SubPixel; iS++) {
		for (iM = 0; iM < MainPixel; iM++) {
			if (iM0 <= iM && iM <= iM1) {
				bX = true;
			}
			else{
				bX = false;
			}

			if (iS0 <= iS && iS <= iS1) {
				bY = true;
			}
			else{
				bY = false;
			}

			if ((bX & (iS0 == iS || iS == iS1)) ||
				(bY & (iM0 == iM || iM1 == iM))){
				SetVal(iS, iM, Dt[iS][iM] + iQLShift);
			}
		}
	}

	return *this;
}
//---------------------------------------------------------------------------
//ttsuji追加
//
//画像上の長方形の辺の画素値を修正する

CTDAT& CTDAT::FillRectAreaWithQLShift(TRECTANGLE rect, const int iQLShift)
{
	int iM0 = rect.GetM0();
	int iS0 = rect.GetS0();
	int iM1 = rect.GetM1();
	int iS1 = rect.GetS1();

	bool bX, bY;
	int iM, iS;
	for (iS = 0; iS < SubPixel; iS++) {
		for (iM = 0; iM < MainPixel; iM++) {
			if (iM0 <= iM && iM <= iM1) {
				bX = true;
			}
			else{
				bX = false;
			}

			if (iS0 <= iS && iS <= iS1) {
				bY = true;
			}
			else{
				bY = false;
			}

			if (bX & bY){
				SetVal(iS, iM, Dt[iS][iM] + iQLShift);
			}
		}
	}

	return *this;
}


//---------------------------------------------------------------------------
//ttsuji追加

CTDAT& CTDAT::FillCrossPointWithQLShift(int iS, int iM, const int iQLShift, const int iSize)
{
	if (iSize <= 0) {
		return *this;
	}

	int iS0 = CLIP(iS - iSize, 0, SubPixel - 1);
	int iS1 = CLIP(iS + iSize, 0, SubPixel - 1);
	int iM0 = CLIP(iM - iSize, 0, MainPixel - 1);
	int iM1 = CLIP(iM + iSize, 0, MainPixel - 1);

	for (int iSCnt = iS0; iSCnt <= iS1; iSCnt++) {
		for (int iMCnt = iM0; iMCnt <= iM1; iMCnt++) {
			if (iS == iSCnt || iM == iMCnt) {
				SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + iQLShift);
			}
		}
	}

	return *this;
}
//---------------------------------------------------------------------------
//ttsuji追加

CTDAT& CTDAT::FillCrossPointWithQLShift(int iS, int iM, const int iQLShift, const int iSSize, const int iMSize)
{
	if (iSSize <= 0 || iMSize <= 0 ) {
		return *this;
	}

	int iS0 = CLIP(iS - iSSize, 0, SubPixel - 1);
	int iS1 = CLIP(iS + iSSize, 0, SubPixel - 1);
	int iM0 = CLIP(iM - iMSize, 0, MainPixel - 1);
	int iM1 = CLIP(iM + iMSize, 0, MainPixel - 1);

	for (int iSCnt = iS0; iSCnt <= iS1; iSCnt++) {
		for (int iMCnt = iM0; iMCnt <= iM1; iMCnt++) {
			if (iS == iSCnt || iM == iMCnt) {
				SetVal(iSCnt, iMCnt, Dt[iSCnt][iMCnt] + iQLShift);
			}
		}
	}

	return *this;
}

//---------------------------------------------------------------------------
//点(iM, iS)が2点(iXS, iXE), (iXE, iYE)	を結ぶ線分の上にあるか否か判定する(ttsuji)

bool CTDAT::IsOnLineSegment(int iS, int iM, int iYS, int iXS, int iYE, int iXE)
{

	return CTDATProc::IsOnLineSegment( iS, iM, iYS, iXS, iYE, iXE);
}

//---------------------------------------------------------------------------
//ttsuji追加
//
//指定画素の画素値を上下左右に指定した範囲だけ十字状にDupe拡大する
CTDAT& CTDAT::DupePixelHVLine(int iSC, int iMC, int iSDupe, int iMDupe)
{
	int iS, iM;
	int iSS = CLIP(iSC - iSDupe, 0, SubPixel - 1);
	int iSE = CLIP(iSC + iSDupe, 0, SubPixel - 1);
	int iMS = CLIP(iMC - iMDupe, 0, MainPixel - 1);
	int iME = CLIP(iMC + iMDupe, 0, MainPixel - 1);

	for (iS = iSS; iS <= iSE; iS++) {
		if (iSC == iS) {
			continue;
		}
		SetVal(iS, iMC, Dt[iSC][iMC]);
	}

	for (iM = iMS; iM <= iME; iM++) {
		if (iMC == iM) {
			continue;
		}
		SetVal(iSC, iM, Dt[iSC][iMC]);
	}

	return *this;
}

//---------------------------------------------------------------------------
//ttsuji追加
//
//指定座標の1ラインを指定QLで上塗りする
void CTDAT::AddLineQL(bool bHorizontal, int iCoord, int iQL)
{
	int iM, iS;
	if (bHorizontal) {//横ライン
		if (0 > iCoord || iCoord >= SubPixel) {
			return;
		}

		for (iM = 0; iM < MainPixel; iM++) {
			SetVal(iCoord, iM, iQL);
			//Dtに直接いれる
		}
	}
	else{//縦ライン
		if (0 > iCoord || iCoord >= MainPixel) {
			return;
		}

		for (iS = 0; iS < SubPixel; iS++) {
			SetVal(iS, iCoord, iQL);
		}
	}

	return;
}

//座標を指定して、バイリニア補間の値を返す
unsigned short CTDAT::GetBilinear(const float fSub, const float fMai){

	CTDATProc procObj(SubPixel,MainPixel,Bit,Dt);

	return procObj.GetBilinear(fSub,fMai);

}
//座標と拡大率を指定して画像の拡大縮小を行う
bool CTDAT::Scaling(float centerX, float centerY, float Xscale, float Yscale, unsigned short FillQL){

	CTDATProc procObj(SubPixel,MainPixel,Bit,Dt);

	return procObj.Scaling(centerX, centerY, Xscale, Yscale, FillQL);

}
//平行移動を行う(小数での入力可能)
bool CTDAT::ShiftXY(float deltaX, float deltaY, unsigned short FillQL){

	CTDATProc procObj(SubPixel,MainPixel,Bit,Dt);

	return procObj.ShiftXY(deltaX, deltaY, FillQL);
}

//左右別の拡大率で画像の拡大縮小を行う
bool CTDAT::ScalingLR(float Lscale, float Rscale, unsigned short FillQL){

	CTDATProc procObj(SubPixel,MainPixel,Bit,Dt);

	return procObj.ScalingLR(Lscale, Rscale, FillQL);

}




