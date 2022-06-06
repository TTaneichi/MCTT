/*
 * TRECTANGLE.h
 *
 *  Created on: 2021/03/31
 *      Author: 10097663
 */

#ifndef TRECTANGLE_H_
#define TRECTANGLE_H_

class TRECTANGLE {
private:
	int iM0, iM1, iS0, iS1;
	int iMWidth, iSWidth;

public:
	void Set(const int iMain0, const int iMain1, const int iSub0, const int iSub1);
	void SetM0(const int iVal){iM0 = iVal; iMWidth = iM1 - iM0 + 1;}
	void SetM1(const int iVal){iM1 = iVal; iMWidth = iM1 - iM0 + 1;}
	void SetS0(const int iVal){iS0 = iVal; iSWidth = iS1 - iS0 + 1;}
	void SetS1(const int iVal){iS1 = iVal; iSWidth = iS1 - iS0 + 1;}
	void SetMWidth(const int iVal){iMWidth = iVal;}
	void SetSWidth(const int iVal){iSWidth = iVal;}

	void IncM0(){++iM0; --iMWidth;}
	void IncM1(){++iM1; ++iMWidth;}
	void IncS0(){++iS0; --iSWidth;}
	void IncS1(){++iS1; ++iSWidth;}

	void DecM0(){--iM0; ++iMWidth;}
	void DecM1(){--iM1; --iMWidth;}
	void DecS0(){--iS0; ++iSWidth;}
	void DecS1(){--iS1; --iSWidth;}

	int GetM0() const {return(iM0);}
	int GetM1() const {return(iM1);}
	int GetS0() const {return(iS0);}
	int GetS1() const {return(iS1);}

	int GetMWidth() const	{return(iMWidth);}
	int GetSWidth() const	{return(iSWidth);}
	int GetArea()	const	{return (iMWidth * iSWidth);}

	TRECTANGLE(){iM0 = iM1 = iS0 = iS1 = 0; iMWidth = iSWidth = 0;}
	TRECTANGLE(const int iMain0, const int iMain1, const int iSub0, const int iSub1);
	TRECTANGLE& operator =(const TRECTANGLE& copy);
	~TRECTANGLE();

};

#endif /* TRECTANGLE_H_ */
