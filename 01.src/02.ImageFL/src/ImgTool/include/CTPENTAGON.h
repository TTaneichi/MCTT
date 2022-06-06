/*
 * CTPENTAGON.h
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#ifndef CTPENTAGON_H_
#define CTPENTAGON_H_

//---------------------------------------------------------------------------
//任意の五角形クラス(ttsuji)

class TPENTAGON
{
private:
	int iM0, iM1, iM2, iM3, iM4, iS0, iS1, iS2, iS3, iS4;
public:
	void SetM0(const int iVal){iM0 = iVal;}
	void SetM1(const int iVal){iM1 = iVal;}
	void SetM2(const int iVal){iM2 = iVal;}
	void SetM3(const int iVal){iM3 = iVal;}
	void SetM4(const int iVal){iM4 = iVal;}
	void SetS0(const int iVal){iS0 = iVal;}
	void SetS1(const int iVal){iS1 = iVal;}
	void SetS2(const int iVal){iS2 = iVal;}
	void SetS3(const int iVal){iS3 = iVal;}
	void SetS4(const int iVal){iS4 = iVal;}
	int GetM0() const {return(iM0);}
	int GetM1() const {return(iM1);}
	int GetM2() const {return(iM2);}
	int GetM3() const {return(iM3);}
	int GetM4() const {return(iM4);}
	int GetS0() const {return(iS0);}
	int GetS1() const {return(iS1);}
	int GetS2() const {return(iS2);}
	int GetS3() const {return(iS3);}
	int GetS4() const {return(iS4);}
	int GetVectorProduct(int iM, int iS, int flagSide);//五角形のうち指定した辺とある点の外積（ある点が四角形の内外になるか判定するため）
	TPENTAGON BinningPentagonWithBitShift(const int iBin);//五角形をビットシフトでビニング
	TPENTAGON& ClipPentagon(int iMMin, int iMMax, int iSMin, int iSMax);

	TPENTAGON(){iM0 = iM1 = iM2 = iM3 = iM4 = iS0 = iS1 = iS2 = iS3 = iS4 = 0;}
	TPENTAGON(const int iMain0, const int iMain1, const int iMain2, const int iMain3, const int iMain4, const int iSub0, const int iSub1, const int iSub2, const int iSub3, const int iSub4)
		:iM0(iMain0), iM1(iMain1), iM2(iMain2), iM3(iMain3), iM4(iMain4), iS0(iSub0), iS1(iSub1), iS2(iSub2), iS3(iSub3), iS4(iSub4){;}
	~TPENTAGON();
};

#endif /* CTPENTAGON_H_ */
