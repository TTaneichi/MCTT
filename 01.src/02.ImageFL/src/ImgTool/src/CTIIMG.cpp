/*
 * CTIIMG.cpp
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#include "CTIIMG.h"
#include "CTSIMG.h"
#include "CTTMA.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
	deconstructor
*/
TIIMG::~TIIMG()
{
	DeleteDt();
}
//---------------------------------------------------------------------------
/*
	Dt[][]の領域確保
    	引数	main ->	主画素数
        		sub ->	副画素数
*/
TIIMG::TIIMG(const int iM, const int iS)
    :MainPixel(iM), SubPixel(iS), DtExist(true)
{
    Dt = new int*[MainPixel];
    for (int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
		Dt[iMCnt] = new int[SubPixel];
    }
}
//---------------------------------------------------------------------------
/*
	Dt[][]の領域確保
    	引数	main ->	主画素数
        		sub ->	副画素数
*/
TIIMG::TIIMG(const CTDAT &copy)
//	:MainPixel(copy.MainPixel), SubPixel(copy.SubPixel), DtExist(true)
:DtExist(false) //falseにしてDtを一旦強制Deleteする（ttsuji）
{
#if 0
	Dt = new int*[MainPixel];
	for (int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
		Dt[iMCnt] = new int[SubPixel];
	}

	//ttsuji追加
	for(int i = 0 ; i < SubPixel ; ++i)
		for(int j = 0 ; j < MainPixel ; ++j)
			Dt[j][i] = copy.Dt[i][j];
#else
	*this = copy;
#endif
}
//---------------------------------------------------------------------------
/*
	operator =
*/
TIIMG& TIIMG::operator =(const TIIMG &copy)
{
	int i,j;

	NewDt(copy.MainPixel, copy.SubPixel);
	for(i = 0 ; i < SubPixel ; ++i)
		for(j = 0 ; j < MainPixel ; ++j)
			Dt[j][i] = copy.Dt[j][i];

	return(*this);
}
//---------------------------------------------------------------------------
/*
	operator =
*/
TIIMG& TIIMG::operator =(const CTDAT &copy)
{
	int i,j;

	NewDt(copy.MainPixel, copy.SubPixel);
	for(i = 0 ; i < SubPixel ; ++i)
		for(j = 0 ; j < MainPixel ; ++j)
			Dt[j][i] = copy.Dt[i][j];

	return(*this);
}
//---------------------------------------------------------------------------
/*
	operator =
*/
TIIMG& TIIMG::operator =(const TSIMG &copy)
{
	int i,j;

    NewDt(copy.MainPixel, copy.SubPixel);
    for(i = 0 ; i < SubPixel ; ++i)
    	for(j = 0 ; j < MainPixel ; ++j)
        	Dt[j][i] = copy.Dt[j][i];

	return(*this);
}
//---------------------------------------------------------------------------
/*
	operator -=(const TDAT& oper2)
*/
TIIMG& TIIMG::operator -=(const CTDAT& oper2)
{
    if(MainPixel >= oper2.MainPixel && SubPixel >= oper2.SubPixel){
        for(int iMCnt = 0; iMCnt < oper2.MainPixel; ++iMCnt){
    	    for(int iSCnt = 0; iSCnt < oper2.SubPixel; ++iSCnt){
        	    Dt[iMCnt][iSCnt] -= oper2.Dt[iSCnt][iMCnt];
            }
        }
        return(*this);
    }

    TIIMG temp;
    int iMain;
    int iSub;
    temp.NewDt(MainPixel, SubPixel);
    for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
    	for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
        	temp.Dt[iMCnt][iSCnt] = Dt[iMCnt][iSCnt];
        }
    }

    if(MainPixel >= oper2.MainPixel){
        iMain = MainPixel;
    }else{
        iMain = oper2.MainPixel;
    }
    if(SubPixel >= oper2.SubPixel){
        iSub = SubPixel;
    }else{
        iSub = oper2.SubPixel;
    }
    NewDt(iMain, iSub);

    for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
    	for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
            if(iMCnt >= temp.MainPixel || iSCnt >= temp.SubPixel){
        	    Dt[iMCnt][iSCnt] = 0;
            }else{
        	    Dt[iMCnt][iSCnt] = temp.Dt[iMCnt][iSCnt];
            }
            if(iMCnt < oper2.MainPixel && iSCnt < oper2.SubPixel){
//        	    Dt[iMCnt][iSCnt] -= oper2.Dt[iMCnt][iSCnt];//bug
        	    Dt[iMCnt][iSCnt] -= oper2.Dt[iSCnt][iMCnt];//ttsuji修正 090904
			}
        }
    }
    return(*this);
}
//---------------------------------------------------------------------------
/*
	operator +=(const TDAT& oper2)
*/
TIIMG& TIIMG::operator +=(const CTDAT& oper2)
{
	//thisとiimgの画像サイズが同等、あるいはthisの方が大きい場合（通常はこのケース）
    if(MainPixel >= oper2.MainPixel && SubPixel >= oper2.SubPixel){
        for(int iMCnt = 0; iMCnt < oper2.MainPixel; ++iMCnt){
    	    for(int iSCnt = 0; iSCnt < oper2.SubPixel; ++iSCnt){
				Dt[iMCnt][iSCnt] += oper2.Dt[iSCnt][iMCnt];
            }
        }
        return(*this);
    }

    TIIMG temp;
    int iMain;
    int iSub;
    temp.NewDt(MainPixel, SubPixel);
    for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
    	for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
			temp.Dt[iMCnt][iSCnt] = Dt[iMCnt][iSCnt];
        }
    }

    if(MainPixel >= oper2.MainPixel){
        iMain = MainPixel;
    }else{
        iMain = oper2.MainPixel;
    }
    if(SubPixel >= oper2.SubPixel){
        iSub = SubPixel;
    }else{
        iSub = oper2.SubPixel;
    }
    NewDt(iMain, iSub);

    for(int iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
    	for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
            if(iMCnt >= temp.MainPixel || iSCnt >= temp.SubPixel){
        	    Dt[iMCnt][iSCnt] = 0;
            }else{
        	    Dt[iMCnt][iSCnt] = temp.Dt[iMCnt][iSCnt];
            }
            if(iMCnt < oper2.MainPixel && iSCnt < oper2.SubPixel){
//				Dt[iMCnt][iSCnt] -= oper2.Dt[iMCnt][iSCnt];//bug
				Dt[iMCnt][iSCnt] -= oper2.Dt[iSCnt][iMCnt];//ttsuji修正 090904
			}
        }
    }
	return(*this);
}
//---------------------------------------------------------------------------
//ttsuji追加 090904
/*
	operator +=(const TIIMG& iimg)
*/
TIIMG& TIIMG::operator +=(const TIIMG& iimg)
{
	//thisとiimgの画像サイズが同等、あるいはthisの方が大きい場合（通常はこのケース）
	if (MainPixel >= iimg.MainPixel && SubPixel >= iimg.SubPixel){
		for (int iMCnt = 0; iMCnt < iimg.MainPixel; iMCnt++){
			for (int iSCnt = 0; iSCnt < iimg.SubPixel; iSCnt++){
				Dt[iMCnt][iSCnt] += iimg.Dt[iMCnt][iSCnt];
			}
		}
		return *this;
	}


	//thisの方がiimgよりも画像サイズが小さい場合
	TIIMG temp;
	int iMain;
	int iSub;
	temp.NewDt(MainPixel, SubPixel);
	for	(int iMCnt = 0; iMCnt < MainPixel; iMCnt++){
		for (int iSCnt = 0; iSCnt < SubPixel; iSCnt++){
			temp.Dt[iMCnt][iSCnt] = Dt[iMCnt][iSCnt];  //tempにthisをコピー
		}
	}

	if(MainPixel >= iimg.MainPixel){
		iMain = MainPixel;
	}else{
		iMain = iimg.MainPixel;
	}
	if(SubPixel >= iimg.SubPixel){
		iSub = SubPixel;
	}else{
		iSub = iimg.SubPixel;
	}
	NewDt(iMain, iSub);//thisとiimgで画像サイズの大きい方の辺で画像メモリを確保

	for (int iMCnt = 0; iMCnt < MainPixel; iMCnt++){
		for (int iSCnt = 0; iSCnt < SubPixel; iSCnt++){
			if(iMCnt >= temp.MainPixel || iSCnt >= temp.SubPixel){
				Dt[iMCnt][iSCnt] = 0;
			}else{
				Dt[iMCnt][iSCnt] = temp.Dt[iMCnt][iSCnt];
			}
			if(iMCnt < iimg.MainPixel && iSCnt < iimg.SubPixel){
				Dt[iMCnt][iSCnt] += iimg.Dt[iMCnt][iSCnt];
			}
		}
	}
	return *this;
}
//---------------------------------------------------------------------------
/*
	operator -=(const TDAT& oper2)
*/
TIIMG& TIIMG::operator -=(const TIIMG& oper2)
{
	int i, j;
    TIIMG temp;
    int main;
    int sub;

    if(MainPixel >= oper2.MainPixel && SubPixel >= oper2.SubPixel){
        for(i = 0 ; i < oper2.MainPixel ; ++i){
    	    for(j = 0 ; j < oper2.SubPixel ; ++j){
        	    Dt[i][j] -= oper2.Dt[i][j];
            }
        }
        return(*this);
    }

    temp.NewDt(MainPixel, SubPixel);
    for(i = 0 ; i < MainPixel ; ++i){
    	for(j = 0 ; j < SubPixel ; ++j){
        	temp.Dt[i][j] = Dt[i][j];
        }
    }

    main = MainPixel;
    sub = SubPixel;
    if(main < oper2.MainPixel) main = oper2.MainPixel;
    if(sub < oper2.SubPixel) sub = oper2.SubPixel;
    NewDt(main, sub);

    for(i = 0 ; i < main ; ++i){
    	for(j = 0 ; j < sub ; ++j){
        	Dt[i][j] = 0;
        }
    }
    for(i = 0 ; i < temp.MainPixel ; ++i){
    	for(j = 0 ; j < temp.SubPixel ; ++j){
        	Dt[i][j] = temp.Dt[i][j];
        }
    }
    for(i = 0 ; i < oper2.MainPixel ; ++i){
    	for(j = 0 ; j < oper2.SubPixel ; ++j){
        	Dt[i][j] -= oper2.Dt[i][j];
        }
    }
    return(*this);
}
//---------------------------------------------------------------------------
/*
	operator + int
*/
//ttsuji追加
TIIMG& TIIMG::operator +(const int iOffset)
//TIIMG TIIMG::operator +(const int iOffset)
{
	TIIMG iimg;
	iimg = *this;
	int i, j;
	for (i = 0 ; i < SubPixel ; i++){
		for (j = 0 ; j < MainPixel ; j++){
			Dt[j][i] = iimg.Dt[j][i] + iOffset;
		}
	}
	return *this;
}
//---------------------------------------------------------------------------
//ttsuji
TIIMG& TIIMG::operator =(const int iVal)
{
	Fill(iVal);
	return *this;
}
//---------------------------------------------------------------------------
//ttsuji
TIIMG& TIIMG::operator /=(const CTDAT dat)
{
	int iMCnt, iSCnt;
	for (iMCnt = 0; iMCnt < MainPixel; iMCnt++) {
		for (iSCnt = 0; iSCnt < SubPixel; iSCnt++) {
			if (0 == dat.Dt[iSCnt][iMCnt]) {
				Dt[iMCnt][iSCnt] = 0;
			}
			else{
				Dt[iMCnt][iSCnt] /= dat.Dt[iSCnt][iMCnt];//切り捨て
			}
		}
	}

	return *this;
}
//---------------------------------------------------------------------------
//ttsuji
TIIMG& TIIMG::operator *=(const int iVal)
{
	if (!iVal) {
		*this = 0;
		return *this;
	}
	int iMCnt, iSCnt;
	for (iMCnt = 0; iMCnt < MainPixel; iMCnt++) {
		for (iSCnt = 0; iSCnt < SubPixel; iSCnt++) {
			Dt[iMCnt][iSCnt] *= iVal;
		}
	}

	return *this;
}
//---------------------------------------------------------------------------
//ttsuji
TIIMG& TIIMG::operator *=(const float fVal)
{
	if (!fVal) {
		*this = 0;
		return *this;
	}
	int iMCnt, iSCnt;
	for (iMCnt = 0; iMCnt < MainPixel; iMCnt++) {
		for (iSCnt = 0; iSCnt < SubPixel; iSCnt++) {
#if 0
			Dt[iMCnt][iSCnt] *= fVal;//切り捨て
#else
			SetVal(iMCnt, iSCnt, Dt[iMCnt][iSCnt] * fVal);//四捨五入
#endif
		}
	}

	return *this;
}
//---------------------------------------------------------------------------
//ttsuji
TIIMG& TIIMG::operator /=(const int iVal)
{
	int iMCnt, iSCnt;
	for (iMCnt = 0; iMCnt < MainPixel; iMCnt++) {
		for (iSCnt = 0; iSCnt < SubPixel; iSCnt++) {
			Dt[iMCnt][iSCnt] /= iVal;//切り捨て
		}
	}

	return *this;
}
//---------------------------------------------------------------------------
//ttsuji
TIIMG& TIIMG::operator *=(const TIIMG& oper2)
{
	int i, j;
	TIIMG temp;
    int main;
    int sub;

	if (MainPixel >= oper2.MainPixel && SubPixel >= oper2.SubPixel){
		for (i = 0 ; i < oper2.MainPixel ; i++){
			for (j = 0 ; j < oper2.SubPixel ; j++){
				Dt[i][j] *= oper2.Dt[i][j];
			}
		}
	}
	else{
		temp.NewDt(MainPixel, SubPixel);
		for (i = 0 ; i < MainPixel ; i++){
			for (j = 0 ; j < SubPixel ; j++){
				temp.Dt[i][j] = Dt[i][j];
			}
		}

		main = MainPixel;
		sub = SubPixel;
		if (main < oper2.MainPixel){
			main = oper2.MainPixel;
		}
		if (sub < oper2.SubPixel){
			sub = oper2.SubPixel;
		}
		NewDt(main, sub);

		for (i = 0 ; i < main ; i++){
			for (j = 0 ; j < sub ; j++){
				Dt[i][j] = 0;
			}
		}
		for (i = 0 ; i < temp.MainPixel ; i++){
			for (j = 0 ; j < temp.SubPixel ; j++){
				Dt[i][j] = temp.Dt[i][j];
			}
		}
		for (i = 0 ; i < oper2.MainPixel ; i++){
			for (j = 0 ; j < oper2.SubPixel ; j++){
				Dt[i][j] *= oper2.Dt[i][j];
			}
		}
	}

	return *this;
}
//---------------------------------------------------------------------------
/*
	引数が画像範囲内であれば    true
*/
bool TIIMG::bCheckPos(const int iM, const int iS) const
{
    if(iM < 0 || iS < 0 || iM >= MainPixel || iS >= SubPixel){
        return(false);
    }else{
        return(true);
    }
}
//---------------------------------------------------------------------------
/*
	Dt[][]のdelete
*/
void TIIMG::DeleteDt()
{
    int i;

    if(DtExist){
		for(i = 0 ; i < MainPixel ; ++i){
        	delete[] Dt[i];
        }
        delete[] Dt;

        DtExist = false;
    }
}
//---------------------------------------------------------------------------
//ttsuji追加

double TIIMG::dAve(const int iM1, const int iS1, const int iM2, const int iS2)
{
	int iMCnt, iSCnt;
	double dSum = 0.0;

	for (iSCnt = iS1; iSCnt <= iS2; iSCnt++){
		for (iMCnt = iM1; iMCnt <= iM2; iMCnt++){
			dSum += Dt[iMCnt][iSCnt];
		}
	}
	return(dSum / (iS2 - iS1 + 1) / (iM2 - iM1 + 1));
}
//---------------------------------------------------------------------------
//ttsuji追加

double TIIMG::dAve(const TROI_Lag Roi)
{
	int iMCnt, iSCnt;
	double dSum = 0.0;

	int iM0 = Roi.GetX0();
	int iM1 = Roi.GetX1();
	int iS0 = Roi.GetY0();
	int iS1 = Roi.GetY1();

	for (iSCnt = iS0; iSCnt <= iS1; iSCnt++){
		for (iMCnt = iM0; iMCnt <= iM1; iMCnt++){
			dSum += Dt[iMCnt][iSCnt];
		}
	}
	return(dSum / (iS1 - iS0 + 1) / (iM1 - iM0 + 1));
}
//---------------------------------------------------------------------------
/*
	RMS計算

*/
double TIIMG::dRMS(int iM0, int iS0, int iM1, int iS1)
{
    if(iM0 < 0){
        iM0 = 0;
    }
    if(MainPixel <= iM0){
        iM0 = MainPixel - 1;
    }
    if(iM1 < iM0){
        iM1 = iM0;
    }
    if(MainPixel <= iM1){
        iM1 = MainPixel - 1;
    }
    if(iS0 < 0){
        iS0 = 0;
    }
    if(SubPixel <= iS0){
        iS0 = SubPixel - 1;
    }
    if(iS1 < iS0){
        iS1 = iS0;
    }
    if(SubPixel <= iS1){
        iS1 = SubPixel - 1;
    }

    double dSS = 0.0;		//2乗和
    double dS = 0.0;		//和

    for(int iMCnt = iM0; iMCnt <= iM1; ++iMCnt){
    	for(int iSCnt = iS0; iSCnt <= iS1; ++iSCnt){
            dSS += Dt[iMCnt][iSCnt] * Dt[iMCnt][iSCnt];
            dS += Dt[iMCnt][iSCnt];
        }
    }

    return(sqrt((dSS - dS * dS / (iM1 - iM0 + 1) / (iS1 - iS0 + 1)) / (iM1 - iM0 + 1) / (iS1 - iS0 + 1)));
}
//---------------------------------------------------------------------------
/*
	全dataをfValにする
*/
void TIIMG::Fill(const int iVal)
{
    int iMCnt, iSCnt;

    for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
        for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
            Dt[iMCnt][iSCnt] = iVal;
        }
    }
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
void TIIMG::Format(const int main, const int sub, const int mSt, const int sSt, const int back)
{
    int i, j;
    int endMain, endSub;        //元画像データの取り出し終了画素位置
    TIIMG temp;

    temp.NewDt(main, sub);

    temp.Fill(back);		//全画素背景色にする

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
                temp.Dt[j + mSt][i + sSt] = Dt[j][i];

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
            	temp.Dt[j + mSt][i] = Dt[j][i - sSt];

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
            	temp.Dt[j][i + sSt] = Dt[j - mSt][i];

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
            	temp.Dt[j][i] = Dt[j - mSt][i - sSt];
    }

    NewDt(main, sub);
    for(i = 0 ; i < main ; ++i)
    	for(j = 0 ; j < sub ; ++j)
        	Dt[i][j] = temp.Dt[i][j];
}
//---------------------------------------------------------------------------
/*
	Formatしてcopy
*/
void TIIMG::CopyFormat(const int iOrgMain, const int iOrgSub,
			TIIMG& CopyImg, const int iCopyMainPixel, const int iCopySubPixel,
							 const int iCopyOrgMain, const int iCopyOrgSub)
{
	int iMainCnt, iSubCnt;

    for(iMainCnt = 0; iMainCnt < iCopyMainPixel; ++iMainCnt){
        for(iSubCnt = 0; iSubCnt < iCopySubPixel; ++iSubCnt){
			Dt[iOrgMain + iMainCnt][iOrgSub + iSubCnt] =
					CopyImg.Dt[iCopyOrgMain + iMainCnt][iCopyOrgSub + iSubCnt];
		}
	}
}
//---------------------------------------------------------------------------
/*

*/
int TIIMG::iFindPos(const int iVal,
            int iDt[],
            int iS,
            int iE)
{
    int iHalf = (iS + iE) / 2;

    if(iDt[iHalf] < iVal){
        return(iFindPos(iVal, iDt, iHalf + 1, iE));
    }else if(iDt[iHalf] > iVal){
        return(iFindPos(iVal, iDt, iS, iHalf - 1));
    }else{
        return(iHalf);
    }
}
//---------------------------------------------------------------------------
/*
	Median
*/
int TIIMG::iMedian(int& iM0, int& iS0, int& iM1, int& iS1)
{
    TrimArea(iM0, iS0, iM1, iS1);
    int iNum = (iM1 - iM0 + 1) * (iS1 - iS0 + 1);
    int* piD = new int[iNum];

    int iCnt = 0;
    for(int iMCnt = iM0; iMCnt <= iM1; ++iMCnt){
        for(int iSCnt = iS0; iSCnt <= iS1; ++iSCnt){
            piD[iCnt] = Dt[iMCnt][iSCnt];
            ++iCnt;
        }
    }
    qsort(piD, iNum, sizeof(int), IntCmp);
    int iResult = piD[iNum / 2];
    delete piD;
    return(iResult);
}
//---------------------------------------------------------------------------
/*
	主方向Median filter処理
		両端は1, 3,,,iNumまで広げながら処理する。
        filter長>画素数の時何もしない。

    	引数	iNum -> filter長
*/
void TIIMG::MainMedian(const int iNum)
{
	if(iNum > MainPixel || iNum % 2 == 0){
		return;
	}

	int iMCnt, iSCnt;
	int iCnt, iOrdCnt;
    int iOrder;     //除くdataのpiOrder配列の位置
	int iPos;
	int iEnd;		//piOrder配列の有効数
	int iRemPos;
	int iAddPos;
	int iMed;
	int *piOrg, *piOrder;

	piOrg = new int[MainPixel];
	piOrder = new int[iNum];

	for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
		iPos = 1;
		iEnd = 0;
		iRemPos = 0;
		iAddPos = 1;
		iMed = 0;
		for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
			piOrg[iMCnt] = Dt[iMCnt][iSCnt];
		}

		piOrder[0] = piOrg[0];
		while(iAddPos < iNum){
			iOrdCnt = iEnd;
			while(iOrdCnt >= 0 && piOrder[iOrdCnt] > piOrg[iAddPos]){
				piOrder[iOrdCnt + 1] = piOrder[iOrdCnt];
				--iOrdCnt;
			}
				//piOrder[iOrdCnt] <= piOrg[iAddPos]
			piOrder[iOrdCnt + 1] = piOrg[iAddPos];
			++iAddPos;
			++iEnd;

			iOrdCnt = iEnd;
			while(iOrdCnt >= 0 && piOrder[iOrdCnt] > piOrg[iAddPos]){
				piOrder[iOrdCnt + 1] = piOrder[iOrdCnt];
				--iOrdCnt;
			}
				//piOrder[iOrdCnt] <= piOrg[iAddPos]
			piOrder[iOrdCnt + 1] = piOrg[iAddPos];
			++iAddPos;
			++iEnd;

			++iMed;
			Dt[iPos][iSCnt] = piOrder[iMed];
			++iPos;
		}

		while(iAddPos < MainPixel){
            iOrder = iFindPos(piOrg[iRemPos], piOrder, 0, iEnd);
/*
			iOrder = iEnd;
			while(piOrder[iOrder] != piOrg[iRemPos]){
				--iOrder;
			}
*/
			++iRemPos;

			if(piOrg[iAddPos] >= piOrder[iOrder]){
                ++iOrder;
                while(iOrder <= iEnd && piOrder[iOrder] < piOrg[iAddPos]){
                    piOrder[iOrder - 1] = piOrder[iOrder];
                    ++iOrder;
                }
                piOrder[iOrder - 1] = piOrg[iAddPos];
            }else{
                --iOrder;
                while(iOrder >= 0 && piOrder[iOrder] > piOrg[iAddPos]){
                    piOrder[iOrder + 1] = piOrder[iOrder];
                    --iOrder;
                }
                piOrder[iOrder + 1] = piOrg[iAddPos];
            }
			++iAddPos;

			Dt[iPos][iSCnt] = piOrder[iMed];
			++iPos;
		}

		while(iRemPos < MainPixel - 2){
            iOrder = iFindPos(piOrg[iRemPos], piOrder, 0, iEnd);
			for(iCnt = iOrder; iCnt < iEnd; ++iCnt){
				piOrder[iCnt] = piOrder[iCnt + 1];
			}
			++iRemPos;
			--iEnd;

            iOrder = iFindPos(piOrg[iRemPos], piOrder, 0, iEnd);
			for(iCnt = iOrder; iCnt < iEnd; ++iCnt){
				piOrder[iCnt] = piOrder[iCnt + 1];
			}
			++iRemPos;
			--iEnd;

			--iMed;
			Dt[iPos][iSCnt] = piOrder[iMed];
			++iPos;
		}
	}
	delete[] piOrg;
	delete[] piOrder;
}
//---------------------------------------------------------------------------
/*  //ttsuji 200903 (DEVO3 ラグ・オフセット分離補正コンペアのMedian橋画素処理用として)
	主方向Median filter処理
		両端は端iNum/2画素の平均をiNum/2個広げた処理。
		filter長>画素数の時何もしない。

		引数	iNum -> filter長
*/
void TIIMG::MainMedianAveExpand2(const int iNum)
{
	if (iNum <= 1 || iNum&1 == 0 || iNum > MainPixel){
		return;
	}

	int iOrgMainPixel = MainPixel;

	MainAveExpand2(iNum / 2);
	MainMedian(iNum);
	Format(iOrgMainPixel, SubPixel, -(iNum / 2), 0, 0);
}
//---------------------------------------------------------------------------
/*   //ttsuji 200903 (DEVO3 ラグ・オフセット分離補正コンペアのMedian橋画素処理用として)
	主方向両端を端iNum画素の平均を使ってiNum広げる。
		iNum+1>画素数の時何もしない。

		引数	iNum -> 延長サイズ
*/
void TIIMG::MainAveExpand2(const int iNum)
{
	if (iNum <= 0 || iNum + 1 > MainPixel){
		return;
	}

	Format(MainPixel + 2 * iNum, SubPixel, iNum, 0, 0);


	for (int iS = 0; iS < SubPixel; iS++){
		int iSt = 0;
		int iEd = 0;

		for (int iM = 0; iM < iNum; iM++){
			iSt += Dt[iM + iNum][iS];
			iEd += Dt[MainPixel - iNum - iM - 1][iS];
		}

		iSt /= iNum;
		iEd /= iNum;

		for (int iM = 0; iM < iNum; iM++){
			Dt[iM][iS]					= iSt;
			Dt[MainPixel - iM - 1][iS]	= iEd;
		}
	}
}
//---------------------------------------------------------------------------
/*
	主方向の移動平均
		Averaging個数が偶数の時前半dataを多くする。

    	引数	iLength -> Averaging個数
*/
void TIIMG::MainMoveAve(const int iLength)
{
    int iMainCnt, iSubCnt;
    int iSubPos, iAddPos;
    int iHalfNum = iLength / 2;        //移動平均個数の半分
    float   fLength = iLength;
    int   iDtSum;
    int iNum;
    int iNormalEndPos = MainPixel - iHalfNum;
    int   *piOrgDt;

    piOrgDt = new int[MainPixel];
    for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
        for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
            piOrgDt[iMainCnt] = Dt[iMainCnt][iSubCnt];
        }
        iAddPos = 1;
        iDtSum = Dt[0][iSubCnt];
        for(iMainCnt = 1; iMainCnt <= iHalfNum; ++iMainCnt){
            iDtSum += piOrgDt[iAddPos] + piOrgDt[iAddPos + 1];
            iAddPos += 2;
            Dt[iMainCnt][iSubCnt] = int((float)iDtSum / iAddPos + 0.5);
        }
        iSubPos = 0;
        for(iMainCnt = iHalfNum + 1; iMainCnt < iNormalEndPos; ++iMainCnt){
            iDtSum += - piOrgDt[iSubPos] + piOrgDt[iAddPos];
            Dt[iMainCnt][iSubCnt] = int(iDtSum / fLength + 0.5);
            ++iSubPos;
            ++iAddPos;
        }
        iNum = iLength - 2;
        for(iMainCnt = iNormalEndPos; iMainCnt < MainPixel; ++iMainCnt){
            iDtSum += - piOrgDt[iSubPos] - piOrgDt[iSubPos + 1];
            Dt[iMainCnt][iSubCnt] = int((float)iDtSum / iNum + 0.5);
            iSubPos += 2;
            iNum -= 2;
        }
    }
    delete[] piOrgDt;
}
//---------------------------------------------------------------------------
/*
	Dt[][]の領域確保
    	引数	main ->	主画素数
        		sub ->	副画素数
*/
void TIIMG::NewDt(const int main, const int sub)
{
	if(DtExist){
		if(MainPixel == main && SubPixel == sub){
			return;
		}else{
			DeleteDt();
		}
	}

	Dt = new int*[main];
	for (int i = 0 ; i < main ; ++i){
		Dt[i] = new int[sub];
    }
    MainPixel = main;
    SubPixel = sub;
    DtExist = true;
}
//---------------------------------------------------------------------------
/*
	Dtへfloatを四捨五入して代入
*/
void TIIMG::SetVal(const int iMain, const int iSub, const float fVal)
{
    if(fVal >= 0.0f){
        Dt[iMain][iSub] = int(fVal + 0.5f);
    }else{
        Dt[iMain][iSub] = int(fVal - 0.5f);
    }
}
//---------------------------------------------------------------------------
/*
	副方向Median filter処理
		両端は1, 3,,,iNumまで広げながら処理する。
        filter長>画素数の時何もしない。

    	引数	iNum -> filter長
*/
void TIIMG::SubMedian(const int iNum)
{
	if(iNum > SubPixel || iNum % 2 == 0){
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
	int *piOrg, *piOrder;

	piOrg = new int[SubPixel];
	piOrder = new int[iNum];

	for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
		iPos = 1;
		iEnd = 0;
		iRemPos = 0;
		iAddPos = 1;
		iMed = 0;
		for(iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
			piOrg[iSCnt] = Dt[iMCnt][iSCnt];
		}

		piOrder[0] = piOrg[0];
		while(iAddPos < iNum){
			iOrdCnt = iEnd;
			while(iOrdCnt >= 0 && piOrder[iOrdCnt] > piOrg[iAddPos]){
				piOrder[iOrdCnt + 1] = piOrder[iOrdCnt];
				--iOrdCnt;
			}
			piOrder[iOrdCnt + 1] = piOrg[iAddPos];
			++iAddPos;
			++iEnd;

			iOrdCnt = iEnd;
			while(iOrdCnt >= 0 && piOrder[iOrdCnt] > piOrg[iAddPos]){
				piOrder[iOrdCnt + 1] = piOrder[iOrdCnt];
				--iOrdCnt;
			}
			piOrder[iOrdCnt + 1] = piOrg[iAddPos];
			++iAddPos;
			++iEnd;

			++iMed;
			Dt[iMCnt][iPos] = piOrder[iMed];
			++iPos;
		}

		while(iAddPos < SubPixel){
            iOrder = iFindPos(piOrg[iRemPos], piOrder, 0, iEnd);
			++iRemPos;

			if(piOrg[iAddPos] >= piOrder[iOrder]){
                ++iOrder;
                while(iOrder <= iEnd && piOrder[iOrder] < piOrg[iAddPos]){
                    piOrder[iOrder - 1] = piOrder[iOrder];
                    ++iOrder;
                }
                piOrder[iOrder - 1] = piOrg[iAddPos];
            }else{
                --iOrder;
                while(iOrder >= 0 && piOrder[iOrder] > piOrg[iAddPos]){
                    piOrder[iOrder + 1] = piOrder[iOrder];
                    --iOrder;
                }
                piOrder[iOrder + 1] = piOrg[iAddPos];
            }
			++iAddPos;

			Dt[iMCnt][iPos] = piOrder[iMed];
			++iPos;
		}

		while(iRemPos < SubPixel - 2){
			iOrder = iFindPos(piOrg[iRemPos], piOrder, 0, iEnd);
			for(iCnt = iOrder; iCnt < iEnd; ++iCnt){
				piOrder[iCnt] = piOrder[iCnt + 1];
			}
			++iRemPos;
			--iEnd;

			iOrder = iFindPos(piOrg[iRemPos], piOrder, 0, iEnd);
			for(iCnt = iOrder; iCnt < iEnd; ++iCnt){
				piOrder[iCnt] = piOrder[iCnt + 1];
			}
			++iRemPos;
			--iEnd;

			--iMed;
			Dt[iMCnt][iPos] = piOrder[iMed];
			++iPos;
		}
	}
	delete[] piOrg;
	delete[] piOrder;
}
//---------------------------------------------------------------------------
/*  //ttsuji 200903 (DEVO3 ラグ・オフセット分離補正コンペアのMedian橋画素処理用として)
	副方向Median filter処理
		両端は端iNum/2画素の平均をiNum/2個広げた処理。
		filter長>画素数の時何もしない。

		引数	iNum -> filter長
*/
void TIIMG::SubMedianAveExpand2(const int iNum)
{
#if 0
	MainSub();
	MainMedianAveExpand2(iNum);
	MainSub();
#else
	if (iNum <= 1 || iNum&1 == 0 || iNum > SubPixel){
		return;
	}

	int iOrgSubPixel = SubPixel;

	SubAveExpand2(iNum / 2);
	SubMedian(iNum);
	Format(MainPixel, iOrgSubPixel, 0, -(iNum / 2), 0);
#endif
}
//---------------------------------------------------------------------------
/*   //ttsuji 200903 (DEVO3 ラグ・オフセット分離補正コンペアのMedian橋画素処理用として)
	副方向両端を端iNum画素の平均を使ってiNum広げる。
		iNum+1>画素数の時何もしない。

		引数	iNum -> 延長サイズ
*/
void TIIMG::SubAveExpand2(const int iNum)
{
	if (iNum <= 0 || iNum + 1 > SubPixel){
		return;
	}

	Format(MainPixel, SubPixel + 2 * iNum, 0, iNum, 0);


	for (int iM = 0; iM < MainPixel; iM++){
		int iSt = 0;
		int iEd = 0;

		for (int iS = 0; iS < iNum; iS++){
			iSt += Dt[iM][iS + iNum];
			iEd += Dt[iM][SubPixel - iNum - iS - 1];
		}

		iSt /= iNum;
		iEd /= iNum;

		for (int iS = 0; iS < iNum; iS++){
			Dt[iM][iS]					= iSt;
			Dt[iM][SubPixel - iS - 1]	= iEd;
		}
	}
}
//---------------------------------------------------------------------------
/*
	副方向の移動平均
		Averaging個数が偶数の時前半dataを多くする。

		引数	aveNum -> Averaging個数
*/
void TIIMG::SubMoveAve(const int iLength)
{
	int iMainCnt, iSubCnt;
	int iSubPos, iAddPos;
	int iHalfNum = iLength / 2;        //移動平均個数の半分
	float   fLength = iLength;
	int   iDtSum;
    int iNum;
    int iNormalEndPos = SubPixel - iHalfNum;
    int   *piOrgDt;

    piOrgDt = new int[SubPixel];
    for(iMainCnt = 0; iMainCnt < MainPixel; ++iMainCnt){
        for(iSubCnt = 0; iSubCnt < SubPixel; ++iSubCnt){
            piOrgDt[iSubCnt] = Dt[iMainCnt][iSubCnt];
        }
        iAddPos = 1;
        iDtSum = Dt[iMainCnt][0];
        for(iSubCnt = 1; iSubCnt <= iHalfNum; ++iSubCnt){
            iDtSum += piOrgDt[iAddPos] + piOrgDt[iAddPos + 1];
            iAddPos += 2;
            Dt[iMainCnt][iSubCnt] = int((float)iDtSum / iAddPos + 0.5);
        }
        iSubPos = 0;
        for(iSubCnt = iHalfNum + 1; iSubCnt < iNormalEndPos; ++iSubCnt){
            iDtSum += - piOrgDt[iSubPos] + piOrgDt[iAddPos];
            Dt[iMainCnt][iSubCnt] = int((float)iDtSum / fLength + 0.5);
            ++iSubPos;
            ++iAddPos;
        }
        iNum = iLength - 2;
        for(iSubCnt = iNormalEndPos; iSubCnt < SubPixel; ++iSubCnt){
            iDtSum += - piOrgDt[iSubPos] - piOrgDt[iSubPos + 1];
            Dt[iMainCnt][iSubCnt] = int((float)iDtSum / iNum + 0.5);
            iSubPos += 2;
            iNum -= 2;
        }
	}
    delete[] piOrgDt;
}
//---------------------------------------------------------------------------
/*

*/
void    TIIMG::TrimArea(int& iM0, int& iS0, int& iM1, int& iS1)
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
//---------------------------------------------------------------------------
/*

*/
void TIIMG::RotateCW()
{
	TIIMG temp(*this);
	NewDt(SubPixel, MainPixel);
	for(int iMCnt = 0, iTempMCnt = MainPixel - 1; iMCnt < MainPixel; ++iMCnt, --iTempMCnt){
		for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
			Dt[iMCnt][iSCnt] = temp.Dt[iSCnt][iTempMCnt];
		}
	}
}
//---------------------------------------------------------------------------
/*
    画像回転。
*/
void TIIMG::Rotate(const int angle)
{
	TIIMG temp(*this);
	int Mpixel = MainPixel;  // 途中で書き換わるので、
	int Spixel = SubPixel;   // 混乱せぬように変数を定義しておく

    // 左90度        主副入れ替え+左右反転でもOK
    if( 90 == angle ){
	  NewDt(Spixel, Mpixel);
	  for(int iM=0; iM<Spixel; iM++){
		for(int iS=0; iS<Mpixel; iS++){
		  Dt[iM][iS] = temp.Dt[iS][Spixel-iM-1];
        }
      }
    // 180度       左右反転+上下反転でもOK
    } else if( 180 == angle ){
	  NewDt(Mpixel, Spixel);
	  for(int iM=0; iM<Mpixel; iM++){
		for(int iS=0; iS<Spixel; iS++){
		  Dt[iM][iS] = temp.Dt[Mpixel-iM-1][Spixel-iS-1];
        }
      }
    // 左270度       主副入れ替え+上下反転でもOK
    } else if( 270 == angle ){
	  NewDt(Spixel, Mpixel);
	  for(int iM=0; iM<Spixel; iM++){
		for(int iS=0; iS<Mpixel; iS++){
		  Dt[iM][iS] = temp.Dt[Mpixel-iS-1][iM];
        }
      }
    } else {
      // 角度が90の倍数でないときは何もしない
    }

}
//---------------------------------------------------------------------------
/*
	左右入れ換え
*/
void TIIMG::RtoL()
{
    int i, j;
    TIIMG temp;

	temp.NewDt(MainPixel, SubPixel);

	for(j = 0 ; j < MainPixel ; ++j)
		for(i = 0 ; i < SubPixel ; ++i)
			temp.Dt[j][i] = Dt[j][i];

//    NewDt(SubPixel, MainPixel, Bit);
	for(j = 0 ; j < MainPixel ; ++j)
		for(i = 0 ; i < SubPixel ; ++i)
			Dt[j][i] = temp.Dt[MainPixel - 1 - j][i];
}
//---------------------------------------------------------------------------
/*

*/
bool TIIMG::bReadFromText(const string& fileName, const int iSkipLine,
		const char cMark,       //text　区切り文字
		TIIMG& maxSizeImg)       //読み取り最大画素数と画像情報が設定されていること　画像dataは破壊される
{
	fstream file(fileName.c_str(), ios::in);
	if(file.fail()){
		return(false);
	}

	const int iMAX_LEN = 21 * maxSizeImg.SubPixel;
	char *tempChar = new char[iMAX_LEN];

	for(int iCnt = 0; iCnt < iSkipLine; ++iCnt){
		file.getline(tempChar, iMAX_LEN);
	}

	maxSizeImg.Fill(0);
	int iMCnt = 0;
	int iSMax = 0;
	while(!file.eof()){
		file.getline(tempChar, iMAX_LEN);
		char *pt = tempChar;
		if(*pt < ' '){
			break;
		}
		char *pos;
		int iSCnt = 0;
		while((pos = strchr(pt, cMark)) != NULL){
			*pos = 0;
			maxSizeImg.SetVal(iMCnt, iSCnt, atof(pt));
			pt = pos + 1;
			while(*pt == ' '){
				++pt;
			}
			++iSCnt;
			if(iSCnt >= maxSizeImg.SubPixel - 1){
				break;
			}
		}
		maxSizeImg.SetVal(iMCnt, iSCnt, atof(pt));
		++iMCnt;
		++iSCnt;
		if(iSCnt >= maxSizeImg.MainPixel){
			break;
		}
		if(iSCnt > iSMax){
			iSMax = iSCnt;
		}
	}

	NewDt(iMCnt, iSMax);
	for(iMCnt = 0; iMCnt < MainPixel; ++iMCnt){
		for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
			Dt[iMCnt][iSCnt] = maxSizeImg.Dt[iMCnt][iSCnt];
		}
	}
	delete[] tempChar;

	return(true);
}
//---------------------------------------------------------------------------
//ttsuji
void TIIMG::SaveTmaTdt(string str)
{
	TTMA tmp(SubPixel, MainPixel, 16);
	for (int i = 0; i < SubPixel; i++){
		for (int j = 0; j < MainPixel; j++){
			tmp.SetVal(i, j, Dt[j][i]);
		}
	}
#if 0
	tmp.SaveTmaTdt(str);
#else
	tmp.WriteTma(str, false);
#endif
}
//---------------------------------------------------------------------------
//
void TIIMG::SaveTmaTdt(string str, const unsigned short unOff)
{
#if 0		//TODO とりあえずコメントアウト
	TTMA tmp(SubPixel, MainPixel, 16);
	for (int i = 0; i < SubPixel; i++){
		for (int j = 0; j < MainPixel; j++){
			tmp.SetVal(i, j, Dt[j][i] + unOff);
		}
	}
#if 0
	tmp.SaveTmaTdt(str);
#else
	tmp.WriteTma(str, false);
#endif

#endif
}
//---------------------------------------------------------------------------
/*
	textでfile出力
*/
bool TIIMG::bSaveAsText(const string fileName)
{
    fstream file(fileName.c_str(), ios::out|ios::trunc);
    if(file.fail()){
        return(false);
	}

	for(int iSCnt = 0; iSCnt < SubPixel; ++iSCnt){
		file << Dt[0][iSCnt];
        for(int iMCnt = 1; iMCnt < MainPixel; ++iMCnt){
			file << '\t' << Dt[iMCnt][iSCnt];
        }
        file << '\n';
    }

    if(file.fail()){
        return(false);
    }
	return(true);
}
//---------------------------------------------------------------------------
//ttsuji
void TIIMG::Clip(int iMin, int iMax)
{
	for (int i = 0; i < SubPixel; i++){
		for (int j = 0; j < MainPixel; j++){
			Dt[j][i] = CLIP(Dt[j][i], 0, 65535);
		}
	}
}
//---------------------------------------------------------------------------
//ttsuji 140927 (LongPanelのMCコンペア用として)

#define _BITSHIFT     //フィルター処理を浮動小数点と2冪のビットシフトとで時間比較のためちゃんとビットシフトで実装した(ttsuji 150625)

bool TIIMG::SubExpandMultiAve(const string fileName)
{
    int iCnt;
    int iFCnt;
    int iMainCnt, iSubCnt;
    int halfNum;        //移動平均個数の半分
    double sum;     //移動平均値を出すための合計
	double edgeSum;     //移動平均するとき、端部データの平均値を用いる。平均値計算用合計
    float edge;     //移動平均するとき、端部データの平均値を用いる。その平均値
//    float *pfOrgDt;
	int *piOrgDt;
	int iPos;
    fstream file;
    int iFNum;      //移動平均段数
    int *piLen;     //移動平均長さ
	float   *pfFactor;      //移動平均重み
	float   fReg;       //移動平均規格化値
	int *piAddPos, *piSubPos;       //移動平均加算・減算位置

	file.open(fileName.c_str(), ios::in);
	if(file.fail()) return(false);
	file >> iFNum;
	piLen = new int[iFNum];
	pfFactor = new float[iFNum];
	piAddPos = new int[iFNum];
	piSubPos = new int[iFNum];

	fReg = 0.0;
	for(iCnt = 0; iCnt < iFNum; ++iCnt){
		file >> piLen[iCnt];
		file >> pfFactor[iCnt];
		fReg += pfFactor[iCnt] * piLen[iCnt];
	}
	file.close();
#ifdef _BITSHIFT
	int iShiftBit = 0;
	while (iShiftBit < 20){
		float fDiv = fReg / pow(2, iShiftBit);
		if (0.99 < fDiv && fDiv < 1.01) {
			break;
		}
		else{
			iShiftBit++;
		}
	}
	if (iShiftBit > 19) {
    	return false;
	}
#endif

	//多段フィルターの最も長いフィルター長が1（フィルターかけないことと同義）ならば何もせず終了（ttsuji追加）
	if (1 == piLen[0]){
		return true;
	}

	halfNum = piLen[0] / 2;
	if(halfNum > SubPixel){
		return(false);
	}
	piOrgDt = new int[SubPixel + piLen[0]];

	for(iMainCnt = 0 ; iMainCnt < MainPixel ; ++iMainCnt){

//Trend除去する画像のcopy  端部データの平均値を左右に付け加える
		edgeSum = (double)Dt[iMainCnt][0];     //端部データの平均値計算
		for(iSubCnt = 1 ; iSubCnt < halfNum ; ++iSubCnt){
			edgeSum += (double)Dt[iMainCnt][iSubCnt];
		}
		edge = edgeSum / halfNum;
		for(iPos = 0 ; iPos < halfNum ; ++iPos){
			piOrgDt[iPos] = round(edge);//四捨五入
		}

		for(iSubCnt = 0 ; iSubCnt < SubPixel ; ++iSubCnt){
			piOrgDt[iPos] = (double)Dt[iMainCnt][iSubCnt];
			++iPos;
		}

		edgeSum = (double)Dt[iMainCnt][SubPixel - halfNum];     //端部データの平均値計算
		for(iSubCnt = SubPixel - halfNum + 1; iSubCnt < SubPixel ; ++iSubCnt){
			edgeSum += (double)Dt[iMainCnt][iSubCnt];
		}
		edge = edgeSum / halfNum;
		for(iSubCnt = 0 ; iSubCnt < halfNum ; ++iSubCnt){
			piOrgDt[iPos] = round(edge);//四捨五入
			++iPos;
		}
//end of data copy
//移動平均
		for(iFCnt = 0; iFCnt < iFNum; ++iFCnt){        //加減算位置計算
			piAddPos[iFCnt] = halfNum + piLen[iFCnt] / 2 + 1;
			piSubPos[iFCnt] = halfNum - piLen[iFCnt] / 2;
		}
		sum = 0.0;
		for(iFCnt = 0; iFCnt < iFNum; ++iFCnt){
			for(iCnt = piSubPos[iFCnt]; iCnt < piAddPos[iFCnt]; ++iCnt){
				sum += pfFactor[iFCnt] * piOrgDt[iCnt];
			}
		}
#ifdef _BITSHIFT
		Dt[iMainCnt][0] = (int)sum >> iShiftBit;
		int iDt = (int)(sum / fReg);//切り捨てることでビットシフトと揃える
#else
		Dt[iMainCnt][0] = (int)(sum / fReg);//切り捨てることでビットシフトと揃える
//		Dt[iMainCnt][0] = round(sum / fReg);
#endif
		for(iSubCnt = 1 ; iSubCnt < SubPixel ; ++iSubCnt){
			for(iFCnt = 0; iFCnt < iFNum; ++iFCnt){
				sum += pfFactor[iFCnt] * (piOrgDt[piAddPos[iFCnt]] - piOrgDt[piSubPos[iFCnt]]);
				++piAddPos[iFCnt];
				++piSubPos[iFCnt];
			}
#ifdef _BITSHIFT
			Dt[iMainCnt][iSubCnt] = (int)sum >> iShiftBit;
#else
			Dt[iMainCnt][iSubCnt] = (int)(sum / fReg);//切り捨てることでビットシフトと揃える
//			Dt[iMainCnt][iSubCnt] = round(sum / fReg);
#endif
		}
	}
	delete[] piOrgDt;
    delete[] piLen;
    delete[] pfFactor;
    delete[] piAddPos;
    delete[] piSubPos;

	return true;
}
//---------------------------------------------------------------------------
//ttsuji 150917 (LongPanel粒状抑制処理のLaplacianピラミッド処理のMCコンペア用として)

bool TIIMG::SubExpandMultiAve(const string fileName, const int iShiftBit)
{
    int iCnt;
    int iFCnt;
    int iMainCnt, iSubCnt;
    int halfNum;        //移動平均個数の半分
    double sum;     //移動平均値を出すための合計
	double edgeSum;     //移動平均するとき、端部データの平均値を用いる。平均値計算用合計
    float edge;     //移動平均するとき、端部データの平均値を用いる。その平均値
//    float *pfOrgDt;
	int *piOrgDt;
	int iPos;
    fstream file;
    int iFNum;      //移動平均段数
    int *piLen;     //移動平均長さ
	float   *pfFactor;      //移動平均重み
	float   fReg;       //移動平均規格化値
	int *piAddPos, *piSubPos;       //移動平均加算・減算位置

	file.open(fileName.c_str(), ios::in);
	if(file.fail()) return(false);
	file >> iFNum;
	piLen = new int[iFNum];
	pfFactor = new float[iFNum];
	piAddPos = new int[iFNum];
	piSubPos = new int[iFNum];

	fReg = 0.0;
	for(iCnt = 0; iCnt < iFNum; ++iCnt){
		file >> piLen[iCnt];
		file >> pfFactor[iCnt];
		fReg += pfFactor[iCnt] * piLen[iCnt];
	}
	file.close();

	//多段フィルターの最も長いフィルター長が1（フィルターかけないことと同義）ならば何もせず終了（ttsuji追加）
	if (1 == piLen[0]){
		return true;
	}

	halfNum = piLen[0] / 2;
	if(halfNum > SubPixel){
		return(false);
	}
	piOrgDt = new int[SubPixel + piLen[0]];

	for(iMainCnt = 0 ; iMainCnt < MainPixel ; ++iMainCnt){

//Trend除去する画像のcopy  端部データの平均値を左右に付け加える
		edgeSum = (double)Dt[iMainCnt][0];     //端部データの平均値計算
		for(iSubCnt = 1 ; iSubCnt < halfNum ; ++iSubCnt){
			edgeSum += (double)Dt[iMainCnt][iSubCnt];
		}
		edge = edgeSum / halfNum;
		for(iPos = 0 ; iPos < halfNum ; ++iPos){
			piOrgDt[iPos] = round(edge);//四捨五入
		}

		for(iSubCnt = 0 ; iSubCnt < SubPixel ; ++iSubCnt){
			piOrgDt[iPos] = (double)Dt[iMainCnt][iSubCnt];
			++iPos;
		}

		edgeSum = (double)Dt[iMainCnt][SubPixel - halfNum];     //端部データの平均値計算
		for(iSubCnt = SubPixel - halfNum + 1; iSubCnt < SubPixel ; ++iSubCnt){
			edgeSum += (double)Dt[iMainCnt][iSubCnt];
		}
		edge = edgeSum / halfNum;
		for(iSubCnt = 0 ; iSubCnt < halfNum ; ++iSubCnt){
			piOrgDt[iPos] = round(edge);//四捨五入
			++iPos;
		}
//end of data copy
//移動平均
		for(iFCnt = 0; iFCnt < iFNum; ++iFCnt){        //加減算位置計算
			piAddPos[iFCnt] = halfNum + piLen[iFCnt] / 2 + 1;
			piSubPos[iFCnt] = halfNum - piLen[iFCnt] / 2;
		}
		sum = 0.0;
		for(iFCnt = 0; iFCnt < iFNum; ++iFCnt){
			for(iCnt = piSubPos[iFCnt]; iCnt < piAddPos[iFCnt]; ++iCnt){
				sum += pfFactor[iFCnt] * piOrgDt[iCnt];
			}
		}

		Dt[iMainCnt][0] = (int)sum >> iShiftBit;
		int iDt = (int)(sum / fReg);//切り捨てることでビットシフトと揃える

		for(iSubCnt = 1 ; iSubCnt < SubPixel ; ++iSubCnt){
			for(iFCnt = 0; iFCnt < iFNum; ++iFCnt){
				sum += pfFactor[iFCnt] * (piOrgDt[piAddPos[iFCnt]] - piOrgDt[piSubPos[iFCnt]]);
				++piAddPos[iFCnt];
				++piSubPos[iFCnt];
			}
			Dt[iMainCnt][iSubCnt] = (int)sum >> iShiftBit;
		}
	}
	delete[] piOrgDt;
    delete[] piLen;
    delete[] pfFactor;
    delete[] piAddPos;
    delete[] piSubPos;

	return true;
}
//---------------------------------------------------------------------------
//ttsuji 140927 (LongPanelのMCコンペア用として)
//　主副反転

void TIIMG::MainSub()
{
    int i, j;
    TIIMG temp;

    temp.NewDt(MainPixel, SubPixel);

	for(i = 0 ; i < MainPixel ; i++){
		for(j = 0 ; j < SubPixel ; j++){
			temp.Dt[i][j] = Dt[i][j];
		}
	}

	NewDt(SubPixel, MainPixel);

	for(i = 0 ; i < MainPixel ; ++i)
		for(j = 0 ; j < SubPixel ; ++j)
			Dt[i][j] = temp.Dt[j][i];
}
//---------------------------------------------------------------------------
/*
//ttsuji 150612 (LongPanelの短冊横傘副作用調査用として)

	副方向のFilter処理（フィルターは面積１の浮動小数点型）
		端部は平均値で拡大して処理
*/
void TIIMG::SubExpandFilter(const float fFilter[], const int iLength)
{
	int iHalfNum = iLength / 2;
    int iMCnt, iSCnt;
    float   fDtSum;
    float   fAve;
	int *piOrgDt;
    int iPos;
    int iDtCnt;

	piOrgDt = new int[SubPixel + 2 * iHalfNum];
	for (iMCnt = 0; iMCnt < MainPixel ; iMCnt++){
			//平均値で拡大
		fDtSum = 0.0;
		for (iDtCnt = 0; iDtCnt <= iHalfNum; iDtCnt++){
			fDtSum += (float)Dt[iMCnt][iDtCnt];
		}
		fAve = fDtSum / (iHalfNum + 1);

		iPos = 0;
		for (iDtCnt = 0; iDtCnt < iHalfNum; iDtCnt++){
			piOrgDt[iPos] = round(fAve);
			iPos++;
		}

			//そのまま
		for (iSCnt = 0; iSCnt < SubPixel; iSCnt++){
			piOrgDt[iPos] = Dt[iMCnt][iSCnt];
			iPos++;
		}

			//平均値で拡大
		fDtSum = 0.0;
		for (iDtCnt = 0; iDtCnt <= iHalfNum; iDtCnt++){
			fDtSum += (float)Dt[iMCnt][SubPixel - 1 - iDtCnt];
		}
		fAve = fDtSum / (iHalfNum + 1);

		for(iDtCnt = 0; iDtCnt < iHalfNum; iDtCnt++){
			piOrgDt[iPos] = round(fAve);
			iPos++;
		}
			//filter処理
		iPos = 0;
		for(iSCnt = 0; iSCnt < SubPixel; iSCnt++){
			fDtSum = 0.0;
			for(iDtCnt = 0; iDtCnt < iLength; iDtCnt++){
				fDtSum += fFilter[iDtCnt] * piOrgDt[iPos + iDtCnt];
			}
			Dt[iMCnt][iSCnt] = round(fDtSum);
            iPos++;
        }
    }
    delete[] piOrgDt;
}

