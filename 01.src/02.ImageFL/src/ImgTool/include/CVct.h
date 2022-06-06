/*
 * CVct.h
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#ifndef CVCT_H_
#define CVCT_H_

#include 	<sys/types.h>
#include	<iostream>
#include	<assert.h>

using namespace std;

#define _EPS 1.0e-6	// 浮動小数点精度


//class	istream;
//class	ostream;


class	Vct {
private:
	double*	_val;
	int	_dim;
public:
	Vct(void);
	Vct(const int);
	Vct(const Vct&);
	Vct(const double*, const int);
	~Vct(void);

	Vct&		operator=(const Vct&);
	double&		operator[](int i)		{ return _val[i]; }
	double		operator[](int i) const		{ return _val[i]; }
	Vct			operator+(const Vct&) const;
	Vct			operator-(const Vct&) const;
	Vct			operator*(double) const;
	double		operator*(const Vct&) const;

	void        clear();
	const int	dim() const			{ return _dim; }
	bool       	set(const int dim);
	bool		set(const double* dVal, const int dim);
	bool		set(const int* iVal, const int dim);
	bool		set(const double* dPoint1, const double* dPoint2, const int dim);//Point1とPoint2を結ぶベクトルとして定義

	double*		data(void)			{ return _val; }
	const double*	data(void) const		{ return _val; }
	double 		GetSumation();//総和
	double		GetMean();//平均
	double		GetVariance();//分散
	double		GetStandardDeviation();//標準偏差
	double		GetSumationSquare();//2乗総和
	double		GetMeanSquare();//2乗平均
	double		GetSumationCube();//3乗総和
	double		GetMeanCube();//3乗平均
	double		GetSumationQuadruplicate();//4乗総和
	double		GetMeanQuadruplicate();//4乗平均
	double		GetScalarProduct(Vct v);//内積
	Vct			GetVectorProduct(Vct v);//3次元ベクトルの外積
 	double 		GetNorm();//ノルム
	Vct			GetUnitVector();//単位ベクトルになるようにノルムを変換
	Vct			Get2DOrthogonalVector();//2Dベクトルの直交方向ベクトルを取得
	Vct			Get2DRotationVector(double dDegAngle);//2Dベクトルを回転させたベクトルを取得

	void		getfrom(istream& s);
	void		putto(ostream&) const;
};

#endif /* CVCT_H_ */
