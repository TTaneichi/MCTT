/*
 * CVct.cpp
 *
 *  Created on: 2021/04/02
 *      Author: 10097663
 */

#include "CVct.h"
#include <math.h>

//	ベクトル演算
//                                           GetVectorProduct
//
//---------------------------------------------------------------------------
//	デフォルトコンストラクタ
Vct::Vct():	_dim(0), _val(0)
{
	;
}
//---------------------------------------------------------------------------
//	次元のみ指定コンストラクタ
Vct::Vct(const int	dim)
{
	if (dim < 0) {
		_dim = 0;
		_val = 0;
		return;
	}

//	clear();

	_dim = dim;
	_val = new double[_dim];
	for (int i = 0; i < _dim; i++) {
		_val[i] = 0;
	}
}
//---------------------------------------------------------------------------
//	コピーコンストラクタ
Vct::Vct(const Vct&	vct)
{
//	clear();

	_dim = vct._dim;
	_val = new double[_dim];
	for (int i = 0; i < _dim; i++) {
		_val[i] = vct._val[i];
	}
}
//---------------------------------------------------------------------------
//	配列指定コンストラクタ
Vct::Vct(const double*	val, const int	dim)
{
	if (dim < 0) {
		_dim = 0;
		_val = 0;
		return;
	}

//	clear();

	_dim = dim;
	_val = new double[_dim];
	for (int i = 0; i < _dim; i++) {
		_val[i] = val[i];
	}
}
//---------------------------------------------------------------------------
//	デストラクタ
Vct::~Vct()
{
	clear();
}
//---------------------------------------------------------------------------
//	クリア
void Vct::clear()
{
#if 0
	delete[] _val;
	_dim = 0;
#else
	if (_val) {
		delete[] _val;
		_dim = 0;
		_val = NULL;
	}
 #if 1
	else{
		_dim = 0;
		_val = NULL;
	}
 #endif
#endif
}
//---------------------------------------------------------------------------
//	次元のみ設定
bool Vct::set(const int dim)
{
//	clear();

	if (dim <= 0)
        {
		   return false;         // データはクリア
	}

	_dim = dim;
	_val = new double[_dim];

	for (int i = 0; i < _dim; i++)
        {
	   _val[i] = 0;
	}

	return true;
}
//---------------------------------------------------------------------------
//	配列による設定
bool Vct::set(const double* val, const int dim)
{
	if (!val) {
		return false;
	}
//	clear();

	if (dim <= 0) {
		return false;         // データはクリア
	}

	_dim = dim;
	_val = new double[_dim];
	for (int i = 0; i < _dim; i++) {
		_val[i] = val[i];
	}

	return true;
}
//---------------------------------------------------------------------------
//	配列による設定
bool Vct::set(const int* val, const int dim)
{
	if (!val) {
		return false;
	}
//	clear();

	if (dim <= 0) {
		return false;         // データはクリア
	}

	_dim = dim;
	_val = new double[_dim];
	for (int i = 0; i < _dim; i++) {
		_val[i] = (double)val[i];
	}

	return true;
}
//---------------------------------------------------------------------------
//Point1とPoint2を結ぶベクトルとして定義

bool Vct::set(const double* dPoint1, const double* dPoint2, const int dim)
{
	if (!dPoint1 || !dPoint2) {
		return false;
	}

	if (dim <= 0) {
		return false;         // データはクリア
	}

	_dim = dim;
	_val = new double[_dim];
#if 0
	for (int i = 0; i < _dim; i++) {
		_val[i] = dPoint2[i] - dPoint1[i];
	}

	//第0成分が＋になるようにする
	if (_val[0] < 0) {
		*this = (*this) * (-1.0);
	}
#else
	double dCoef = 1.0;
	if (dPoint2[0] - dPoint1[0] < 0){
    	dCoef = -1.0;
	}
	for (int i = 0; i < _dim; i++) {
		_val[i] = (dPoint2[i] - dPoint1[i]) * dCoef;
	}
#endif
	return true;
}
//---------------------------------------------------------------------------
//	代入演算子
Vct& Vct::operator=(const Vct& vct)
{
	if (this == &vct) {
		return *this;
	}

//	clear();

	_dim = vct._dim;
	_val = new double[_dim];
	for (int i = 0; i < _dim; i++) {
		_val[i] = vct._val[i];
	}

	return *this;
}
//---------------------------------------------------------------------------
//	加算演算子
Vct Vct::operator+(const Vct& vct)
const
{
	assert (!(_dim - vct._dim));

	Vct	res(_dim);
	for (int i = 0; i < _dim; i++) {
		res._val[i] = _val[i] + vct._val[i];
	}
	return res;
}
//---------------------------------------------------------------------------
//	減算演算子
Vct		Vct::operator-(const Vct& vct)
const
{
	assert (!(_dim - vct._dim));

	Vct	res(_dim);
	for (int i = 0; i < _dim; i++) {
		res._val[i] = _val[i] - vct._val[i];
	}
	return res;
}
//---------------------------------------------------------------------------
#if 1
//	スカラ積算演算子
Vct Vct::operator*(double k)
const
{
	Vct	res(_dim);
	for (int i = 0; i < _dim; i++) {
		res._val[i] = k * _val[i];
	}
	return res;
}
#endif
//---------------------------------------------------------------------------
//	(内)積算演算子
double Vct::operator*(const Vct& vct)
const
{
	assert (!(_dim - vct._dim));

	double	res = 0;
	for (int i = 0; i < _dim; i++) {
		res += _val[i] * vct._val[i];
	}
	return res;
}
//---------------------------------------------------------------------------
//	入力
void Vct::getfrom(istream& s)
{

	for (int i = 0; i < _dim; i++) {
		s >> _val[i];
	}
}
//---------------------------------------------------------------------------
//	出力
void Vct::putto(ostream& s)
const
{
	for (int i = 0; i < _dim; i++) {
		s << _val[i];
		if (i < _dim - 1)
			s << " ";
	}
	s << endl;
}
//---------------------------------------------------------------------------
#if 0
//	入力
istream& operator>>(istream& s, Vct& t)
{
	for (int i = 0; i < _dim; i++) {
		s >> t[i];
	}
	return s;
}
//---------------------------------------------------------------------------
//	出力
ostream& operator<<(ostream& s, const Vct& t)
{
	for (int i = 0; i < _dim - 1; i++) {
		s << t[i] << "\t";
	}
	s << t[Vct::_dim - 1] << endl;
	return s;
}
#endif
//---------------------------------------------------------------------------
//総和
double Vct::GetSumation()
{
	double sum = 0.0;
	for (int i = 0; i < _dim; i++) {
		sum += _val[i];
	}
	return sum;
}
//---------------------------------------------------------------------------
//平均
double Vct::GetMean()
{
	double Mean = GetSumation() / _dim;
	return Mean;
}
//---------------------------------------------------------------------------
//2乗総和
double Vct::GetSumationSquare()
{
	double sum = 0.0;
	for (int i = 0; i < _dim; i++) {
		sum += _val[i] * _val[i];
	}
	return sum;
}
//---------------------------------------------------------------------------
//2乗平均
double Vct::GetMeanSquare()
{
	double Mean = GetSumationSquare() / _dim;
	return Mean;
}
//---------------------------------------------------------------------------
//3乗総和
double Vct::GetSumationCube()
{
	double sum = 0.0;
	for (int i = 0; i < _dim; i++) {
		sum += _val[i] * _val[i] * _val[i];
	}
	return sum;
}
//---------------------------------------------------------------------------
//3乗平均
double Vct::GetMeanCube()
{
	double Mean = GetSumationCube() / _dim;
	return Mean;
}
//---------------------------------------------------------------------------
//4乗総和
double Vct::GetSumationQuadruplicate()
{
	double sum = 0.0;
	for (int i = 0; i < _dim; i++) {
		sum += _val[i] * _val[i] * _val[i] * _val[i];
	}
	return sum;
}
//---------------------------------------------------------------------------
//4乗平均
double Vct::GetMeanQuadruplicate()
{
	double Mean = GetSumationQuadruplicate() / _dim;
	return Mean;
}
//---------------------------------------------------------------------------
//分散
double Vct::GetVariance()
{
	double Mean = GetMean();
	double MeanSquare = GetMeanSquare();
	double Variance = MeanSquare - Mean * Mean;
	return Variance;
}
//---------------------------------------------------------------------------
//標準偏差
double Vct::GetStandardDeviation()
{
	double Variance = GetVariance();
    return sqrt(Variance);
}
//---------------------------------------------------------------------------
//内積
double Vct::GetScalarProduct(Vct v)
{
	double dScalarProduct = 0.0;
	for (int i = 0; i < _dim; i++) {
		dScalarProduct += _val[i] * v._val[i];
	}

	return dScalarProduct;
}
//---------------------------------------------------------------------------
//3次元ベクトルの外積(自分自身のベクトル×ベクトルv）
Vct Vct::GetVectorProduct(Vct v)
{
	int iDim = 3;
	Vct vVectorProduct(iDim);
	if (_dim != iDim) {
		return vVectorProduct;//成分0のベクトルを返す
	}

	double dVct[3];
	dVct[0] = _val[1] * v._val[2] - _val[2] * v._val[1];
	dVct[1] = _val[2] * v._val[0] - _val[0] * v._val[2];
	dVct[2] = _val[0] * v._val[1] - _val[1] * v._val[0];

	vVectorProduct.set(dVct, iDim);

	return vVectorProduct;
}
//---------------------------------------------------------------------------
//ノルム

double Vct::GetNorm()
{
	double dSum = 0.0;
	for (int i = 0; i < _dim; i++) {
		dSum += _val[i] * _val[i];
	}
	double dNorm = sqrt(dSum);

	return dNorm;
}
//---------------------------------------------------------------------------
//単位ベクトルになるようにノルムを変換

Vct Vct::GetUnitVector()
{
	double dNorm = GetNorm();
#if 1 //エラー処理を追加
	if (dNorm < _EPS) {//dNorm = 0と判断
		Vct v;//0ベクトル
		return v;
	}
	double dInv = 1 / dNorm;
#endif

	Vct v;
	v = (*this) * dInv;

	return v;
}
//---------------------------------------------------------------------------
//直行するベクトルを取得

Vct Vct::Get2DOrthogonalVector()
{
	if (2 != _dim) {
		return *this;//何もしない
	}

	Vct v(*this);

	//ｘ成分が＋になるように90度回転
	if (_val[1] >= 0) {
		v._val[0] = _val[1];
		v._val[1] = -_val[0];
	}
	else{
		v._val[0] = -_val[1];
		v._val[1] = _val[0];
	}

	return v;
}
//---------------------------------------------------------------------------
//2Dベクトルを回転させたベクトルを取得

Vct Vct::Get2DRotationVector(double dDegAngle)
{
	if (2 != _dim) {
		return *this;//何もしない
	}

	double dCos = cos(dDegAngle * M_PI / 180);
	double dSin = sin(dDegAngle * M_PI / 180);

	Vct v(*this);

	v._val[0] = _val[0] * dCos - _val[1] * dSin;
	v._val[1] = _val[0] * dCos + _val[1] * dSin;

	return v;
}

