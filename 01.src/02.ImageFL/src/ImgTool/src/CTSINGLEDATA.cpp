/*
 * CTSINGLEDATA.cpp
 *
 *  Created on: 2021/04/01
 *      Author: 10097663
 */

#include "CTSINGLEDATA.h"
#include "imageData.h"
#include <math.h>
#include <stdlib.h>
#include <vector>

TSINGLE_DATA& TSINGLE_DATA::operator /=(const int oper2)
{
    for(int iCnt = 0; iCnt < iDtNum; ++iCnt){
        dDt[iCnt] /= oper2;
    }
    return(*this);
}
//---------------------------------------------------------------------------
/*
	deconstructor
*/
TSINGLE_DATA::~TSINGLE_DATA()
{
	DeleteDt();
}
//---------------------------------------------------------------------------
/*
	copy constructor
*/
TSINGLE_DATA::TSINGLE_DATA(const TSINGLE_DATA &copy)
{
    int iCnt;

    NewDt(copy.iDtNum);
    for(iCnt = 0; iCnt < iDtNum; ++iCnt){
        dDt[iCnt] = copy.dDt[iCnt];
    }
}
//---------------------------------------------------------------------------
/*
	operator =
*/
TSINGLE_DATA&   TSINGLE_DATA::operator =(const TSINGLE_DATA &copy)
{
    int iCnt;

    NewDt(copy.iDtNum);
    for(iCnt = 0; iCnt < iDtNum; ++iCnt){
        dDt[iCnt] = copy.dDt[iCnt];
    }
    return(*this);
}
//---------------------------------------------------------------------------
/*
	絶対値最大位置を返す
*/
int    TSINGLE_DATA::iAbsMaxPos()
{
    int iPos = 0;
    double  dMax;
    double  dAbs;
    int iCnt;

    dMax = dDt[0];

    for(iCnt = 1; iCnt < iDtNum; ++iCnt){
        dAbs = fabs(dDt[iCnt]);
        if(dAbs > dMax){
            dMax = dAbs;
            iPos = iCnt;
        }
    }
    return(iPos);
}
//---------------------------------------------------------------------------
/*
        指定された領域範囲で絶対値最大位置を返す。
*/
int     TSINGLE_DATA::iFindAbsMaxPos(const int iStart, const int iEnd)
{
    int iPos = 0;
    double  dMax;
    double  dAbs;
    int iCnt;

    dMax = dDt[iStart];

    for(iCnt = iStart; iCnt < iEnd; iCnt++)
    {
        dAbs = fabs(dDt[iCnt]);
        if(dAbs > dMax){
            dMax = dAbs;
            iPos = iCnt;
        }
    }
    return iPos;

}
//---------------------------------------------------------------------------
/*
	差分絶対値最大位置を返す
*/
int    TSINGLE_DATA::iAbsSabunMax(const int iStart, const int iEnd)
{
    int iPos = iStart;
    double  dMax;
    double  dSabun;
    int iCnt;

	if(iEnd >= iDtNum || iStart >= iDtNum) return(-1);

    dMax = fabs(dDt[iStart + 1] - dDt[iStart]);

    for(iCnt = iStart + 2; iCnt <= iEnd; ++iCnt){
        dSabun = fabs(dDt[iCnt] - dDt[iCnt - 1]);
        if(dSabun > dMax){
            dMax = dSabun;
            iPos = iCnt - 1;
        }
    }
    return(iPos);
}
//---------------------------------------------------------------------------
/*

*/
void TSINGLE_DATA::CopyTo(TSINGLE_DATA& destination)
{
    destination.NewDt(iDtNum);
	for(int i = 0; i < iDtNum; ++i){
        destination.dDt[i] = dDt[i];
    }
}
//---------------------------------------------------------------------------
/*
	iStartからiEndまでの平均値を返す
*/
double    TSINGLE_DATA::dAve(const int iStart, const int iEnd)
{
    double   dSum = dDt[iStart];
    int iCnt;

    for(iCnt = iStart + 1; iCnt <= iEnd; ++iCnt){
        dSum += dDt[iCnt];
    }
    return(dSum /(iEnd - iStart + 1));
}
//---------------------------------------------------------------------------
/*
	iStartからiEndまでの総和を返す
*/
double   TSINGLE_DATA::dSum()
{
	double dSum = dDt[0];
	for(int iCnt = 1; iCnt < iDtNum; ++iCnt){
		dSum += dDt[iCnt];
	}
	return(dSum);
}
//---------------------------------------------------------------------------
/*
	iStartからiEndまでの中央値を返す
*/
double TSINGLE_DATA::dMedian(const int iStart, const int iEnd)
{
	int iNum = iEnd - iStart + 1;
	double* tmpDt = new double[iNum];
	for (int i = 0; i < iNum; i++) {
		 tmpDt[i] = dDt[i + iStart];
	}
	qsort(tmpDt, iNum, sizeof(double), DoubleCmp);
	double median = tmpDt[iNum / 2];
	delete[] tmpDt;
    return(median);
}
//---------------------------------------------------------------------------
/*
	data削除
*/
void    TSINGLE_DATA::DeleteDt()
{
	if(bDtExist) delete[] dDt;
}
//---------------------------------------------------------------------------
/*
	直線近似でデータをつなぎ最初にedgeを横切る位置を返す。
*/
double  TSINGLE_DATA::dFindNanameEdge()
{
    const int   iEDGE_BLUR = 10;        //edgeのぼけの影響を除く画素数

    double  dSabun;
	int	iCnt;
	double	dA, dB;		//直線の式の係数
    double  dMin = 0.0;
    double  dMax = 0.0;
    int iMinPos = 0;
    int iMaxPos = 0;
    double  dSumBack = 0.0;
    double  dSumJigu = 0.0;
    double  dAveBack, dAveJigu, dHalf;

//差分 差分Min位置Max位置を見つける
    for(iCnt = 0; iCnt < iDtNum - 1; ++iCnt){
        dSabun = dDt[iCnt + 1] - dDt[iCnt];
        if(dMin > dSabun){
            dMin = dSabun;
            iMinPos = iCnt;
        } else if(dMax < dSabun){
            dMax = dSabun;
            iMaxPos = iCnt;
        }
    }

//Backgroundと治具の平均値を求めて半値を決める
    for(iCnt = 0; iCnt < iMinPos - iEDGE_BLUR; ++iCnt){
        dSumBack += dDt[iCnt];
    }
    for(iCnt = iMinPos + iEDGE_BLUR; iCnt < iMaxPos - iEDGE_BLUR; ++iCnt){
        dSumJigu += dDt[iCnt];
    }
    for(iCnt = iMaxPos + iEDGE_BLUR; iCnt < iDtNum; ++iCnt){
        dSumBack += dDt[iCnt];
    }
    dAveBack = dSumBack / (iDtNum - (iMaxPos - iMinPos));
    dAveJigu = dSumJigu / (iMaxPos - iMinPos);
    dHalf = (dAveBack + dAveJigu) / 2.0;

//直線近似でデータをつなぎ最初にedgeを横切る位置を求める
    iCnt = 0;
    while(dDt[iCnt] > dHalf){
        ++iCnt;
    }
    dA = dDt[iCnt] - dDt[iCnt - 1];
    if(dA == 0.0) return(double(iCnt - 1));
    dB = dDt[iCnt] - dA * iCnt;
    return((dHalf - dB) / dA);
}
//---------------------------------------------------------------------------
/*
	直線近似でデータをつなぎ最初にedgeを横切る位置を返す。
*/
double  TSINGLE_DATA::dFindNanameEdgeDD()
{
//    const int   iEDGE_BLUR = 10;        //edgeのぼけの影響を除く画素数
    const int   iOFFSET = 2;

	int	iCnt;
	double	dA, dB;		//直線の式の係数
    double  dMin = 0.0;
//    double  dMax = 0.0;
    int iMinPos = 0;
    TSINGLE_DATA    sabun, sabun2;

    sabun.NewDt(iDtNum - 1);
    sabun2.NewDt(iDtNum - 2);

//差分 差分Min位置Max位置を見つける
    for(iCnt = 0; iCnt < iDtNum - 1; ++iCnt){
        sabun.dDt[iCnt] = dDt[iCnt + 1] - dDt[iCnt];
        if(dMin > sabun.dDt[iCnt]){
            dMin = sabun.dDt[iCnt];
            iMinPos = iCnt;
        }/* else if(dMax < sabun.dDt[iCnt]){
            dMax = sabun.dDt[iCnt];
            iMaxPos = iCnt;
        }  */
    }

//2次差分
    for(iCnt = 0; iCnt < sabun2.iDtNum; ++iCnt){
        sabun2.dDt[iCnt] = sabun.dDt[iCnt + 1] - sabun.dDt[iCnt];
    }


//直線近似でデータをつなぎ最初にedgeを横切る位置を求める
    iCnt = iMinPos - iOFFSET;
    while(sabun2.dDt[iCnt] * sabun2.dDt[iCnt + 1] >= 0 ){
        ++iCnt;
    }
    dA = sabun2.dDt[iCnt + 1] - sabun2.dDt[iCnt];
    if(dA == 0.0) return(double(iCnt));
    dB = sabun2.dDt[iCnt] - dA * iCnt;
    return(-dB / dA);
}
//---------------------------------------------------------------------------
/*
	両端dRatioの範囲の平均値から半値を求め、直線近似でデータをつなぎ最初に半値を与
	える位置を返す。両端の値が一致の時は0.0を返す。
*/
double    TSINGLE_DATA::dFindHalfPos(const double dRatio)
{
	int	iSEnd = int(iDtNum * dRatio);
	int	iEStart = iDtNum - iSEnd;
	double	dSAve,	dEAve;
	double	dHalf;		//半値
	double	dSum;
	int	iCnt;
	double	dA, dB;		//直線の式の係数

	dSum = dDt[0];		//先頭の平均値
	for(iCnt = 1; iCnt < iSEnd; ++iCnt){
		dSum += dDt[iCnt];
	}
	dSAve = dSum / iSEnd;
	dSum = dDt[iEStart];		//後端の平均値
	for(iCnt = iEStart + 1; iCnt < iDtNum; ++iCnt){
		dSum += dDt[iCnt];
	}
	dEAve = dSum / iSEnd;
	dHalf = (dSAve + dEAve) / 2.0;

	if(dEAve > dSAve){		//増加
		iCnt = 0;
		while(dDt[iCnt] < dHalf){
			++iCnt;
		}
		dA = dDt[iCnt] - dDt[iCnt - 1];
		if(dA == 0.0) return(double(iCnt - 1));
		dB = dDt[iCnt] - dA * iCnt;
		return((dHalf - dB) / dA);
	}else if(dEAve < dSAve){		//減少
		iCnt = 0;
		while(dDt[iCnt] > dHalf){
			++iCnt;
		}
		dA = dDt[iCnt] - dDt[iCnt - 1];
		if(dA == 0.0) return(double(iCnt - 1));
		dB = dDt[iCnt] - dA * iCnt;
		return((dHalf - dB) / dA);
	}else{		//両端の値が一致の時は0.0を返す
		return(0.0);
	}
}
//---------------------------------------------------------------------------
/*
	直線近似でデータをつなぎ最初にdValueを横切る位置を返す。
*/
double    TSINGLE_DATA::dFindPos(const double dValue)
{
    if(iDtNum < 2){
        return(0.0);
    }

	int	iCnt;
	double	dA, dB;		//直線の式の係数

	if(dValue > dDt[0]){		//増加
		iCnt = 1;
		while(dDt[iCnt] < dValue){
			++iCnt;
            if(iCnt == iDtNum){
                return(0.0);
            }
		}
		dA = dDt[iCnt] - dDt[iCnt - 1];
		if(dA == 0.0) return(double(iCnt - 1));
		dB = dDt[iCnt] - dA * iCnt;
		return((dValue - dB) / dA);
	}else if(dValue < dDt[0]){		//減少
		iCnt = 1;
		while(dDt[iCnt] > dValue){
			++iCnt;
            if(iCnt == iDtNum){
                return(0.0);
            }
		}
		dA = dDt[iCnt] - dDt[iCnt - 1];
		if(dA == 0.0) return(double(iCnt - 1));
		dB = dDt[iCnt] - dA * iCnt;
		return((dValue - dB) / dA);
	}else{		//一致の時は0.0を返す
		return(0.0);
	}
}
//---------------------------------------------------------------------------
/*
	直線近似でデータをつなぎ最初にdValueを横切る位置を返す。
*/
double    TSINGLE_DATA::dFindPos(const double dValue, const int iS)
{
    if(iS > iDtNum - 2){
        return(0.0);
    }

	int	iCnt;
	double	dA, dB;		//直線の式の係数

	if(dValue > dDt[iS]){		//増加
		iCnt = iS + 1;
		while(dDt[iCnt] < dValue){
			++iCnt;
            if(iCnt == iDtNum){
                return(0.0);
            }
		}
		dA = dDt[iCnt] - dDt[iCnt - 1];
		if(dA == 0.0) return(double(iCnt - 1));
		dB = dDt[iCnt] - dA * iCnt;
		return((dValue - dB) / dA);
	}else if(dValue < dDt[iS]){		//減少
		iCnt = iS + 1;
		while(dDt[iCnt] > dValue){
			++iCnt;
            if(iCnt == iDtNum){
                return(0.0);
            }
		}
		dA = dDt[iCnt] - dDt[iCnt - 1];
		if(dA == 0.0) return(double(iCnt - 1));
		dB = dDt[iCnt] - dA * iCnt;
		return((dValue - dB) / dA);
	}else{		//一致の時は0.0を返す
		return(0.0);
	}
}
//---------------------------------------------------------------------------
/*
     iStをスタート位置とし、前後に極大値or極小値を与える位置を探し
     その次の差分が０になる位置または極大値or極小値を与える位置を探す。
*/
void    TSINGLE_DATA::FindPeakNextPos(const int iSt, int& iPrev, int& iAft)
{
    int pos;
    pos = iSt;

    //前方向(Prev)に探す
    if(dDt[pos] > dDt[pos-1])
    {   //減少方向
        while(dDt[pos] > dDt[pos-1])
        {
           pos--;
        } // この時点でposは最初の極小値の位置になっている

        //増加方向
        while(dDt[pos] < dDt[pos-1])
        {
           if(dDt[pos] >= 0){
               break;
           }
           pos--;
        } // この時点でposは最初の極小値の次の差分0または極大値になっている。
        iPrev = pos;
    }else if(dDt[pos] < dDt[pos-1])
    {
        //増加方向
        while(dDt[pos] < dDt[pos-1])
        {
           pos--;
        }  //この時点でposは最初の極大値の位置になっている

        //減少方向
        while(dDt[pos] > dDt[pos-1])
        {
            if(dDt[pos] <= 0){
                break;
            }
            pos--;
        } // この時点でposは最初の極大値の次の差分0または極小値になっている
        iPrev = pos;
    }else{
       //まずありえないけど、どうしようかな・・・
    }

    pos = iSt;
    //後方向(Aft)に探す
    if(dDt[pos] > dDt[pos+1])
    {   //減少方向
        while(dDt[pos] > dDt[pos+1])
        {
            pos++;
        } // この時点でposは最初の極小値を与える位置になる

        //増加方向
        while(dDt[pos] < dDt[pos+1])
        {
            if(dDt[pos] >= 0){
                break;
            }
            pos++;
        } //この時点でposは最初の極小値の次の差分０又は極大値になる
        iAft = pos;
    }else if(dDt[pos] < dDt[pos+1])
    {
        //増加方向
        while(dDt[pos] < dDt[pos+1])
        {
           pos++;
        } //この時点でposは最初の極大値を与える位置になる

        //減少方向
        while(dDt[pos] > dDt[pos+1])
        {
            if(dDt[pos] <= 0){
                break;
            }
            pos++;
        }
        iAft = pos;
    }else{
       //まずありえないけど、どうしようかな・・・
    }
}
//---------------------------------------------------------------------------
/*
	data領域確保
*/
void    TSINGLE_DATA::NewDt(const int iNum)
{
	if(bDtExist) delete[] dDt;
    iDtNum = iNum;
    dDt = new double[iNum];
    bDtExist = true;
}
//---------------------------------------------------------------------------
/*
	差分 dDt[iCnt] = dDt[iCnt + 1] -dDt[iCnt];
    dDtNumは1減る
*/
void TSINGLE_DATA::Sabun()
{
    TSINGLE_DATA    copy;
    int iCnt;

    copy.NewDt(iDtNum - 1);
    for(iCnt = 0; iCnt < copy.iDtNum; ++iCnt){
        copy.dDt[iCnt] = dDt[iCnt + 1] - dDt[iCnt];
    }
    NewDt(copy.iDtNum);
    for(iCnt = 0; iCnt < copy.iDtNum; ++iCnt){
        dDt[iCnt] = copy.dDt[iCnt];
    }
}
//---------------------------------------------------------------------------
/*
	差分 dDt[iCnt] = dDt[iCnt + 1] -dDt[iCnt];
    差分min位置とmax位置を返す
*/
void TSINGLE_DATA::SabunMinMaxPos(int& iSabunMinPos, int& iSabunMaxPos)
{
    int iCnt;
    double  dMin, dMax;
    double  dSabun;

    iSabunMinPos = iSabunMaxPos = 0;
    dMin = dDt[1] - dDt[0];
    dMax = dMin;

    for(iCnt = 2; iCnt < iDtNum; ++iCnt){
        dSabun = dDt[iCnt] - dDt[iCnt - 1];
        if(dSabun < dMin){
            iSabunMinPos = iCnt - 1;
            dMin = dSabun;
        }else if(dSabun > dMax){
            iSabunMaxPos = iCnt - 1;
            dMax = dSabun;
        }
    }
}

//---------------------------------------------------------------------------
/*
	Dataを全て引数の値にする
*/
void    TSINGLE_DATA::Fill(const double dVAL)
{
    int iCnt;

    for(iCnt = 0; iCnt < iDtNum; ++iCnt){
        dDt[iCnt] = dVAL;
    }
}
//---------------------------------------------------------------------------
/*
	Data Copy
        data個数のerror checkは行わない
*/
void    TSINGLE_DATA::FormatCopy(TSINGLE_DATA& destination, //copy先
                                const int iDestHead,    //copy先のcopy先頭位置
                                const int iSourceHead,  //copy元のcopy先頭位置
                                const int iSourceNum)   //copy data個数
{
    int iCnt;

    for(iCnt = 0; iCnt < iSourceNum; ++iCnt){
        destination.dDt[iDestHead + iCnt] = dDt[iSourceHead + iCnt];
    }
}
//---------------------------------------------------------------------------
void TSINGLE_DATA::FWHM(const int iPeak, int& iSt, int& iEnd)
{
    int iCnt;
    double dHalf = dDt[iPeak] / 2.0;

    if(dHalf > 0.0){
        iCnt = iPeak - 1;
        while(dDt[iCnt] > dHalf && iCnt >= 0){
            --iCnt;
        }
        if(dHalf - dDt[iCnt] < dDt[iCnt + 1] - dHalf){
            iSt = iCnt;
        }else{
            iSt = iCnt + 1;
        }

        iCnt = iPeak + 1;
        while(dDt[iCnt] > dHalf && iCnt < iDtNum){
            ++iCnt;
        }
        if(dHalf - dDt[iCnt] < dDt[iCnt - 1] - dHalf){
            iEnd = iCnt;
        }else{
            iEnd = iCnt + 1;
        }
    }else{
        iCnt = iPeak - 1;
        while(dDt[iCnt] < dHalf && iCnt >= 0){
            --iCnt;
        }
        if(dDt[iCnt] - dHalf < dHalf - dDt[iCnt + 1]){
            iSt = iCnt;
        }else{
            iSt = iCnt + 1;
        }

        iCnt = iPeak + 1;
        while(dDt[iCnt] < dHalf && iCnt < iDtNum){
            ++iCnt;
        }
        if(dDt[iCnt] - dHalf < dHalf - dDt[iCnt - 1]){
            iEnd = iCnt;
        }else{
            iEnd = iCnt + 1;
        }
    }
}
//---------------------------------------------------------------------------
/*

*/
void    TSINGLE_DATA::LinExpand(const int iExp)
{
	TSINGLE_DATA org;
	CopyTo(org);

	NewDt(org.iDtNum * iExp);
	if(org.iDtNum == 1){
		for(int iCnt = 0; iCnt < iDtNum; ++iCnt){
			dDt[iCnt] = org.dDt[0];
		}
	}else{
		if(iExp % 2 == 0){
			int iOrgCnt1 = 0;
			int iOrgCnt2 = 1;
			double dR1 = (iExp + iExp / 2 - 0.5) * org.dDt[iOrgCnt1];
			double dR2 = (- iExp / 2 + 0.5) * org.dDt[iOrgCnt2];
			int iCnt = 0;
			for( ; iCnt < iExp + iExp / 2; ++iCnt){
				dDt[iCnt] = (dR1 + dR2)/ iExp;
				dR1 -= org.dDt[iOrgCnt1];
				dR2 += org.dDt[iOrgCnt2];
			}
			for(iOrgCnt1 = 1, iOrgCnt2 = 2; iOrgCnt2 < org.iDtNum; ++iOrgCnt1, ++iOrgCnt2){
				dR1 = (iExp - 0.5) * org.dDt[iOrgCnt1];
				dR2 = 0.5 * org.dDt[iOrgCnt2];
				for(int i = 0; i < iExp; ++i, ++iCnt){
					dDt[iCnt] = (dR1 + dR2)/ iExp;
					dR1 -= org.dDt[iOrgCnt1];
					dR2 += org.dDt[iOrgCnt2];
				}
			}
			for( ; iCnt < iDtNum; ++iCnt){
				dDt[iCnt] = (dR1 + dR2)/ iExp;
				dR1 -= org.dDt[iOrgCnt1];
				dR2 += org.dDt[iOrgCnt2];
			}
		}else{
			int iOrgCnt1 = 0;
			int iOrgCnt2 = 1;
			double dR1 = (iExp + iExp / 2) * org.dDt[iOrgCnt1];
			double dR2 = (- iExp / 2) * org.dDt[iOrgCnt2];
			int iCnt = 0;
			for( ; iCnt < iExp + iExp / 2; ++iCnt){
				dDt[iCnt] = (dR1 + dR2)/ iExp;
				dR1 -= org.dDt[iOrgCnt1];
				dR2 += org.dDt[iOrgCnt2];
			}
			for(iOrgCnt1 = 1, iOrgCnt2 = 2; iOrgCnt2 < org.iDtNum; ++iOrgCnt1, ++iOrgCnt2){
				dR1 = iExp * org.dDt[iOrgCnt1];
				dR2 = 0.0;
				for(int i = 0; i < iExp; ++i, ++iCnt){
					dDt[iCnt] = (dR1 + dR2)/ iExp;
					dR1 -= org.dDt[iOrgCnt1];
					dR2 += org.dDt[iOrgCnt2];
				}
			}
			for( ; iCnt < iDtNum; ++iCnt){
				dDt[iCnt] = (dR1 + dR2)/ iExp;
				dR1 -= org.dDt[iOrgCnt1];
				dR2 += org.dDt[iOrgCnt2];
			}
		}
	}
}
//---------------------------------------------------------------------------
/*
  後端の画素の拡大率を調整する
*/
void TSINGLE_DATA::LinExpandCnvRest(const int iExp, const int iRestNum)
{
	TSINGLE_DATA org;
    CopyTo(org);

	NewDt(org.iDtNum * iExp + iRestNum);
	if(org.iDtNum == 1){
		for(int iCnt = 0; iCnt < iDtNum; ++iCnt){
			dDt[iCnt] = org.dDt[0];
		}
	}else{
		int iOrgCnt1, iOrgCnt2;
		double dR1, dR2;
		int iCnt = 0;
		if(iExp % 2 == 0){
			iOrgCnt1 = 0;
			iOrgCnt2 = 1;
			dR1 = (iExp + iExp / 2 - 0.5) * org.dDt[iOrgCnt1];
			dR2 = (- iExp / 2 + 0.5) * org.dDt[iOrgCnt2];
			for( ; iCnt < iExp + iExp / 2; ++iCnt){
				dDt[iCnt] = (dR1 + dR2)/ iExp;
				dR1 -= org.dDt[iOrgCnt1];
				dR2 += org.dDt[iOrgCnt2];
			}
			for(iOrgCnt1 = 1, iOrgCnt2 = 2; iOrgCnt2 < org.iDtNum - 1; ++iOrgCnt1, ++iOrgCnt2){
				dR1 = (iExp - 0.5) * org.dDt[iOrgCnt1];
				dR2 = 0.5 * org.dDt[iOrgCnt2];
				for(int i = 0; i < iExp; ++i, ++iCnt){
					dDt[iCnt] = (dR1 + dR2)/ iExp;
					dR1 -= org.dDt[iOrgCnt1];
					dR2 += org.dDt[iOrgCnt2];
				}
			}
			if(iRestNum % 2 == 0){
				dR1 = (iExp + iRestNum / 2 - 0.5) * org.dDt[iOrgCnt1];
				dR2 = 0.5 * org.dDt[iOrgCnt2];
				for( ; iCnt < iDtNum; ++iCnt){
					dDt[iCnt] = (dR1 + dR2)/ (iExp + iRestNum / 2);
					dR1 -= org.dDt[iOrgCnt1];
					dR2 += org.dDt[iOrgCnt2];
				}
			}else{
				dR1 = (iExp + iRestNum / 2) * org.dDt[iOrgCnt1];
				dR2 = 0.5 * org.dDt[iOrgCnt2];
				for( ; iCnt < iDtNum; ++iCnt){
					dDt[iCnt] = (dR1 + dR2)/ (iExp + iRestNum / 2 + 0.5);
					dR1 -= org.dDt[iOrgCnt1];
					dR2 += org.dDt[iOrgCnt2];
				}
			}
		}else{
			iOrgCnt1 = 0;
			iOrgCnt2 = 1;
			dR1 = (iExp + iExp / 2) * org.dDt[iOrgCnt1];
			dR2 = (- iExp / 2) * org.dDt[iOrgCnt2];
			for( ; iCnt < iExp + iExp / 2; ++iCnt){
				dDt[iCnt] = (dR1 + dR2)/ iExp;
				dR1 -= org.dDt[iOrgCnt1];
				dR2 += org.dDt[iOrgCnt2];
			}
			for(iOrgCnt1 = 1, iOrgCnt2 = 2; iOrgCnt2 < org.iDtNum - 1; ++iOrgCnt1, ++iOrgCnt2){
				dR1 = iExp * org.dDt[iOrgCnt1];
				dR2 = 0.0;
				for(int i = 0; i < iExp; ++i, ++iCnt){
					dDt[iCnt] = (dR1 + dR2)/ iExp;
					dR1 -= org.dDt[iOrgCnt1];
					dR2 += org.dDt[iOrgCnt2];
				}
			}
			if(iRestNum % 2 == 0){
				dR1 = (iExp + iRestNum / 2) * org.dDt[iOrgCnt1];
				dR2 = 0.0;
				for( ; iCnt < iDtNum; ++iCnt){
					dDt[iCnt] = (dR1 + dR2)/ (iExp + iRestNum / 2);
					dR1 -= org.dDt[iOrgCnt1];
					dR2 += org.dDt[iOrgCnt2];
				}
			}else{
				dR1 = (iExp + iRestNum / 2 + 0.5) * org.dDt[iOrgCnt1];
				dR2 = 0.0;
				for( ; iCnt < iDtNum; ++iCnt){
					dDt[iCnt] = (dR1 + dR2)/ (iExp + iRestNum / 2 + 0.5);
					dR1 -= org.dDt[iOrgCnt1];
					dR2 += org.dDt[iOrgCnt2];
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
/*
  後端の画素の拡大率を調整する
*/
void TSINGLE_DATA::LinExpandCnvTopSideRest(const int iExp, const int iRestNum)
{
	TSINGLE_DATA org;
	CopyTo(org);

	NewDt(org.iDtNum * iExp + iRestNum);
	if(org.iDtNum == 1){
		for(int iCnt = 0; iCnt < iDtNum; ++iCnt){
			dDt[iCnt] = org.dDt[0];
		}
	}else{
		int iOrgCnt1 = 0;
		int iOrgCnt2 = 1;
		int iCnt = 0;
		double dX2 = iExp + iRestNum + iExp / 2.0 - 0.5;
		double dX1 = (iExp + iRestNum) / 2.0 - 0.5;
		double dR1 = dX2 * org.dDt[iOrgCnt1];
		double dR2 = -dX1 * org.dDt[iOrgCnt2];
		for( ; iCnt < iRestNum + iExp + iExp / 2; ++iCnt){
			dDt[iCnt] = (dR1 + dR2)/ (dX2 - dX1);
			dR1 -= org.dDt[iOrgCnt1];
			dR2 += org.dDt[iOrgCnt2];
		}

		if(iExp % 2 == 0){
			for(iOrgCnt1 = 1, iOrgCnt2 = 2; iOrgCnt2 < org.iDtNum; ++iOrgCnt1, ++iOrgCnt2){
				dR1 = (iExp - 0.5) * org.dDt[iOrgCnt1];
				dR2 = 0.5 * org.dDt[iOrgCnt2];
				for(int i = 0; i < iExp; ++i, ++iCnt){
					dDt[iCnt] = (dR1 + dR2)/ iExp;
					dR1 -= org.dDt[iOrgCnt1];
					dR2 += org.dDt[iOrgCnt2];
				}
			}
		}else{
			for(iOrgCnt1 = 1, iOrgCnt2 = 2; iOrgCnt2 < org.iDtNum; ++iOrgCnt1, ++iOrgCnt2){
				dR1 = iExp * org.dDt[iOrgCnt1];
				dR2 = 0.0;
				for(int i = 0; i < iExp; ++i, ++iCnt){
					dDt[iCnt] = (dR1 + dR2)/ iExp;
					dR1 -= org.dDt[iOrgCnt1];
					dR2 += org.dDt[iOrgCnt2];
				}
			}
		}

		iOrgCnt1 = org.iDtNum - 2;
		iOrgCnt2 = org.iDtNum - 1;
		for( ; iCnt < iDtNum; ++iCnt){
			dDt[iCnt] = (dR1 + dR2)/ iExp;
			dR1 -= org.dDt[iOrgCnt1];
			dR2 += org.dDt[iOrgCnt2];
		}
	}
}
//---------------------------------------------------------------------------
/*
	Median filter
*/
void    TSINGLE_DATA::Median(const int iNum)
{
    int iXCnt, iCnt;
    int *piOrder;       //順番
    double *pdOriginal;        //original data
    int iOrder;     //追加dataの順位を示すcounter
    int iIncludePos, iRemovePos;        //data追加位置と除去位置
    int iCrtPos;        //補正位置
    int iEndOrder = iNum - 1;
    int iMedian;

    piOrder = new int[iNum];
    pdOriginal = new double[iDtNum];

    for(iXCnt = 0 ; iXCnt < iDtNum ; ++iXCnt){
        pdOriginal[iXCnt] = dDt[iXCnt];
    }

    iMedian = 0;
    iCrtPos = 0;
    piOrder[0] = 0;
    iIncludePos = 1;
    while(iIncludePos < iNum){
        iOrder = iIncludePos;
        for(iCnt = 0; iCnt < iIncludePos; ++iCnt){
            if(pdOriginal[iCnt] > pdOriginal[iIncludePos]){
                ++piOrder[iCnt];
                --iOrder;
            }
        }
        piOrder[iIncludePos] = iOrder;
        ++iIncludePos;
        iOrder = iIncludePos;
        for(iCnt = 0; iCnt < iIncludePos; ++iCnt){
            if(pdOriginal[iCnt] > pdOriginal[iIncludePos]){
                ++piOrder[iCnt];
                --iOrder;
            }
        }
        ++iMedian;
        ++iCrtPos;
        piOrder[iIncludePos] = iOrder;
        for(iCnt = 0; iCnt <= iIncludePos; ++iCnt){
            if(piOrder[iCnt] == iMedian){
                dDt[iCrtPos] = pdOriginal[iCnt];
                break;
            }
        }
        ++iIncludePos;
	}

    iRemovePos = 0;
    for(; iIncludePos < iDtNum ; ++iIncludePos){
        iOrder = iEndOrder;
        if(piOrder[0] == iMedian) dDt[iCrtPos] = pdOriginal[iRemovePos];
        for(iCnt = 1 ; iCnt < iNum ; ++iCnt){
            if(piOrder[iCnt] == iMedian) dDt[iCrtPos] = pdOriginal[iRemovePos + iCnt];
            if(pdOriginal[iRemovePos + iCnt] >= pdOriginal[iRemovePos]){
                piOrder[iCnt - 1] = piOrder[iCnt] - 1;
            }else{
                piOrder[iCnt - 1] = piOrder[iCnt];
            }
            if(pdOriginal[iRemovePos + iCnt] > pdOriginal[iIncludePos]){
                ++piOrder[iCnt - 1];
                --iOrder;
            }
        }
        piOrder[iEndOrder] = iOrder;
        ++iRemovePos;
        ++iCrtPos;
    }
    while(iRemovePos < iDtNum - 1){
        if(piOrder[0] == iMedian) dDt[iCrtPos] = pdOriginal[iRemovePos];
        for(iCnt = iRemovePos + 1; iCnt < iDtNum; ++iCnt){
            if(piOrder[iCnt - iRemovePos] == iMedian) dDt[iCrtPos] =
					pdOriginal[iCnt];
            if(pdOriginal[iCnt] >= pdOriginal[iRemovePos]){
                piOrder[iCnt - iRemovePos - 1] = piOrder[iCnt - iRemovePos] - 1;
			}else{
                piOrder[iCnt - iRemovePos - 1] = piOrder[iCnt - iRemovePos];
			}
        }
        ++iRemovePos;
        for(iCnt = iRemovePos + 1; iCnt < iDtNum; ++iCnt){
            if(pdOriginal[iCnt] >= pdOriginal[iRemovePos]){
                piOrder[iCnt - iRemovePos - 1] = piOrder[iCnt - iRemovePos] - 1;
            }else{
                piOrder[iCnt - iRemovePos - 1] = piOrder[iCnt - iRemovePos];
            }
        }
        ++iRemovePos;
        ++iCrtPos;
        --iMedian;
    }
    delete[] piOrder;
    delete[] pdOriginal;
}
//---------------------------------------------------------------------------
/*
	移動平均（端点処理は平均する画素数を２ずつ減らす）
*/
void    TSINGLE_DATA::MoveAve(const int iNum)
{
    int iCnt;
    int iSubPos, iAddPos;
    int iHalfNum = iNum / 2;        //移動平均個数の半分
	double	dSum;
    int iLen;
    int iNormalEndPos = iDtNum - iHalfNum;
	TSINGLE_DATA copy;

	if(iNum > iDtNum) return;

	copy.NewDt(iDtNum);
	for(iCnt = 0; iCnt < iDtNum; ++iCnt){
		copy.dDt[iCnt] = dDt[iCnt];
	}

    iAddPos = 1;
    dSum = copy.dDt[0];
    for(iCnt = 1; iCnt <= iHalfNum ; ++iCnt){
        dSum += (copy.dDt[iAddPos] + copy.dDt[iAddPos + 1]);
        iAddPos += 2;
        dDt[iCnt] = dSum / iAddPos;
    }

    iSubPos = 0;
    for(iCnt = iHalfNum + 1; iCnt < iNormalEndPos; ++iCnt){
        dSum += (- copy.dDt[iSubPos] + copy.dDt[iAddPos]);
        dDt[iCnt] = dSum / iNum;
        ++iSubPos;
        ++iAddPos;
    }

	iLen = iNum - 2;
    for(iCnt = iNormalEndPos; iCnt < iDtNum; ++iCnt){
        dSum += (- copy.dDt[iSubPos] - copy.dDt[iSubPos + 1]);
        dDt[iCnt] = dSum / iLen;
        iSubPos += 2;
        iLen -= 2;
    }
}
//---------------------------------------------------------------------------
/*
	移動平均（端点処理なし）
*/
void    TSINGLE_DATA::MoveAve2(const int iNum)
{
	int iCnt;
	int iSubPos=0;
	int iAddPos=iNum;
	int iPos = iNum / 2;        //移動平均個数の半分
	double	dSum=0.0;

	TSINGLE_DATA copy;

	if(iNum > iDtNum) return;

	copy.NewDt(iDtNum);
	for(iCnt = 0; iCnt < iDtNum; ++iCnt){
		copy.dDt[iCnt] = dDt[iCnt];
	}

	for(iCnt=0;iCnt<iNum;++iCnt){
		dSum += copy.dDt[iCnt];
	}
	dDt[iPos]=dSum/iNum;
	++iPos;

	while(iAddPos<iDtNum){
		dSum+=copy.dDt[iAddPos]-copy.dDt[iSubPos];
		dDt[iPos]=dSum/iNum;
		++iPos;
		++iAddPos;
		++iSubPos;
	}
}
//---------------------------------------------------------------------------
/*
	(-1, 0, 1)差分
		戻り値	差分絶対値位置
*/
int   TSINGLE_DATA::SabunToAndFro(int iStPos, int iEndPos)
{
	int	iCnt;
	TSINGLE_DATA copy;
	int	iMaxPos;
	double	dMax = 0.0;

	if(iStPos < 1) iStPos = 1;
	if(iEndPos > iDtNum - 2) iEndPos = iDtNum - 2;
	copy.NewDt(iDtNum);
	for(iCnt = 0; iCnt < iDtNum; ++iCnt){
		copy.dDt[iCnt] = dDt[iCnt];
	}
	iMaxPos = iStPos;
	for(iCnt = iStPos; iCnt <= iEndPos; ++iCnt){
		dDt[iCnt] = copy.dDt[iCnt + 1] - copy.dDt[iCnt - 1];
		if(fabs(dDt[iCnt]) > dMax){
			dMax = fabs(dDt[iCnt]);
			iMaxPos = iCnt;
		}
	}
	return(iMaxPos);
}
//---------------------------------------------------------------------------
/*
	Save data
*/
bool   TSINGLE_DATA::SaveWithIndex(const string fileName)
{
    fstream file;
    int iCnt;

	if(bDtExist == false) return(false);
    file.open(fileName.c_str(), ios::out|ios::trunc);
    if(file.fail()) return(false);
    file << '\t'  << fileName.c_str() << endl;
    for(iCnt = 0; iCnt < iDtNum; ++iCnt){
        file << iCnt << '\t' << dDt[iCnt] << endl;
    }
    if(file.fail()) return(false);
    file.close();
    return(true);
}
//---------------------------------------------------------------------------
/*
    operator -=(const TSINGLE_DAT& oper2)
*/
TSINGLE_DATA& TSINGLE_DATA::operator -=(const TSINGLE_DATA& oper2)
{
    int iCnt;
    int iNum;

    if(iDtNum >= oper2.iDtNum){
        iNum = oper2.iDtNum;
    }else{
        iNum = iDtNum;
    }
    for(iCnt = 0; iCnt < iNum; ++iCnt){
        dDt[iCnt] -= oper2.dDt[iCnt];
    }
    return(*this);
}
//---------------------------------------------------------------------------
/*
	引数のdata値に近い位置を返す。同じ値の時には小さいほうを返す。
*/
int TSINGLE_DATA::iValPosSearch(const double dVal)
{
    int iCnt;
    int iPos = 0;
    double dAbsDeltaMax = fabs(dDt[0] - dVal);
    double dAbsDelta;

    for(iCnt = 1; iCnt < iDtNum; ++iCnt){
        dAbsDelta = fabs(dDt[iCnt] - dVal);
        if(dAbsDelta < dAbsDeltaMax){
            iPos = iCnt;
            dAbsDeltaMax = dAbsDelta;
        }
    }
    return(iPos);
}
//---------------------------------------------------------------------------

int TSINGLE_DATA::dFindHalfPos(double& dB0, double& dB1, int iSEnd)
{
//	int	iSEnd = 100;


	int	iEStart = iDtNum - iSEnd;
	double	dSAve,	dEAve;
	double	dHalf;		//半値
	double	dSum;
	int	iCnt;
	double	dA, dB;		//直線の式の係数

	dSum = dDt[0];		//先頭の平均値
	for(iCnt = 1; iCnt < iSEnd; ++iCnt){
		dSum += dDt[iCnt];
	}
	dSAve = dSum / iSEnd;
	dB0 = dSAve;

	dSum = dDt[iEStart];		//後端の平均値
	for(iCnt = iEStart + 1; iCnt < iDtNum; ++iCnt){
		dSum += dDt[iCnt];
	}
	dEAve = dSum / iSEnd;
	dB1 = dEAve;
	dHalf = (dSAve + dEAve) / 2.0;

	if(dEAve > dSAve){		//増加
		iCnt = 0;
		while(dDt[iCnt] < dHalf){
			++iCnt;
		}
		dA = dDt[iCnt] - dDt[iCnt - 1];
		if(dA == 0.0) return(double(iCnt - 1));
		dB = dDt[iCnt] - dA * iCnt;
		return((dHalf - dB) / dA);
	}else if(dEAve < dSAve){		//減少
		iCnt = 0;
		while(dDt[iCnt] > dHalf){
			++iCnt;
		}
		dA = dDt[iCnt] - dDt[iCnt - 1];
		if(dA == 0.0) return(double(iCnt - 1));
		dB = dDt[iCnt] - dA * iCnt;
		return((dHalf - dB) / dA);
	}else{		//両端の値が一致の時は0.0を返す
		return(0.0);
	}
}
//---------------------------------------------------------------------------
/*
	データ位置をずらす
		iOffset > 0	->ずらす
		iOffset < 0 ->切り出し
		dBack		->ずらした部分を埋めるdata
*/
void  TSINGLE_DATA::Format(const int iOffset, double dBack)
{
	int	iCnt;
	TSINGLE_DATA temp;

	if(iOffset >= 0){
		for(iCnt = iDtNum - iOffset - 1; iCnt >=0; --iCnt){
			dDt[iCnt + iOffset] = dDt[iCnt];
		}
		for(iCnt = 0; iCnt < iOffset; ++iCnt){
			dDt[iCnt] = dBack;
		}
	}else{
		for(iCnt = -iOffset; iCnt < iDtNum; ++iCnt){
			dDt[iCnt + iOffset] = dDt[iCnt];
		}
		for(iCnt = iDtNum - iOffset; iCnt < iDtNum; ++iCnt){
			dDt[iCnt] = dBack;
		}
	}
}
//---------------------------------------------------------------------------
void TSINGLE_DATA::Bin(const int iBin)
{
    TSINGLE_DATA org;
    CopyTo(org);

    NewDt(roundUp((float)iDtNum / iBin));

    int iCnt = 0;
    for(int i = 0; i < iDtNum - 1; ++i){
        double dSum = org.dDt[iCnt];
        ++iCnt;
        for(int iBinCnt = 1; iBinCnt < iBin; ++iBinCnt, ++iCnt){
            dSum += org.dDt[iCnt];
        }
        dDt[i] = dSum / iBin;
    }

    int iNum = 1;
    double dSum = org.dDt[iCnt];
    ++iCnt;
    for(; iCnt < org.iDtNum; ++iNum, ++iCnt){
        dSum += org.dDt[iCnt];
    }
    dDt[iDtNum - 1] = dSum / iNum;
}
//--------------------------------------------------------------------------
/*
　縮小処理
	余りの分は最後の画素数を増やす
*/
//
void TSINGLE_DATA::BinInvRest(const int iBin)
{
	TSINGLE_DATA org;
	CopyTo(org);

	NewDt(iDtNum / iBin);

	int iCnt = 0;
	for(int i = 0; i < iDtNum - 1; ++i){
		double dSum = org.dDt[iCnt];
		++iCnt;
		for(int iBinCnt = 1; iBinCnt < iBin; ++iBinCnt, ++iCnt){
			dSum += org.dDt[iCnt];
		}
		dDt[i] = dSum / iBin;
	}

	int iNum = 1;
	double dSum = org.dDt[iCnt];
	++iCnt;
	for(; iCnt < org.iDtNum; ++iNum, ++iCnt){
		dSum += org.dDt[iCnt];
	}
	dDt[iDtNum - 1] = dSum / iNum;
}
//--------------------------------------------------------------------------
/*
　縮小処理
	余りは先頭側に持ってくる
	余りの分は最後の画素数を増やす
*/
//
void TSINGLE_DATA::BinInvTopSideRest(const int iBin)
{
	TSINGLE_DATA org;
	CopyTo(org);

	NewDt(iDtNum / iBin);

	int iCnt = org.iDtNum - 1;
	for(int i = iDtNum - 1; i > 0; --i){
		double dSum = org.dDt[iCnt];
		--iCnt;
		for(int iBinCnt = 1; iBinCnt < iBin; ++iBinCnt, --iCnt){
			dSum += org.dDt[iCnt];
		}
		dDt[i] = dSum / iBin;
	}

	int iNum = 1;
	double dSum = org.dDt[iCnt];
	--iCnt;
	for(; iCnt >= 0; ++iNum, --iCnt){
		dSum += org.dDt[iCnt];
	}
	dDt[0] = dSum / iNum;
}

//--------------------------------------------------------------------------
bool TSINGLE_DATA::bReadFromText(const string fileName)
{
	ifstream file(fileName.c_str());
	if(file.fail()){
//		Application->MessageBox((fileName + "が読めません").c_str(), "Error", MB_OK);
		return(false);
	}

	vector<float> fDt;
	const int iMAX_LEN = 1000;
	char *tempChar = new char[iMAX_LEN];
	file.getline(tempChar, iMAX_LEN);
	while(!file.eof()){
		int iCnt;
		file >> iCnt;
		if(file.eof()){
			break;
		}
		float fVal;
		file >> fVal;
		fDt.push_back(fVal);
	}

	NewDt(fDt.size());
	for(int i = 0; i < iDtNum; ++i){
		dDt[i] = fDt[i];
	}
	return(true);
}
//--------------------------------------------------------------------------
/*
	operator *=(const float fVal)
*/
TSINGLE_DATA&    TSINGLE_DATA::operator *=(const float fVal)
{
    for(int i = 0; i < iDtNum; ++i){
        dDt[i] *= fVal;
    }
	return(*this);
}
//---------------------------------------------------------------------------
/*
	operator +=(const ARRAY& oper2)
*/
TSINGLE_DATA&    TSINGLE_DATA::operator += (const TSINGLE_DATA& oper2)
{
	int	iCnt;
	int	iShortNum;

	if(iDtNum <= oper2.iDtNum){
		iShortNum = iDtNum;
	}else{
		iShortNum = oper2.iDtNum;
	}

	for(iCnt = 0; iCnt < iShortNum; ++iCnt){
		dDt[iCnt] += oper2.dDt[iCnt];
	}
	return(*this);
}
//--------------------------------------------------------------------------
void    TSINGLE_DATA::ClearDt()
{
	int	iCnt;

	for(iCnt = 0; iCnt < iDtNum; ++iCnt){
		dDt[iCnt] = 0.0;
	}
}
//---------------------------------------------------------------------------
//ttsuji追加
TSINGLE_DATA& TSINGLE_DATA::operator +=(const float fVal)
{
	for (int i = 0; i < iDtNum; i++){
		dDt[i] += fVal;
	}
	return(*this);
}
//---------------------------------------------------------------------------
//ttsuji追加
TSINGLE_DATA& TSINGLE_DATA::operator -=(const float fVal)
{
	for (int i = 0; i < iDtNum; i++){
		dDt[i] -= fVal;
	}
	return(*this);
}

