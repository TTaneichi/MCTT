/*
 * CLeastSquare.cpp
 *
 *  Created on: 2021/04/06
 *      Author: 10097663
 */

#include "CLeastSquare.h"
#include "Matrix.h"
#include <stdlib.h>
#include <math.h>
#include <CStatUtil.h>

//---------------------------------------------------------------------------
CLeastSquare::CLeastSquare()
{
	m_Num = 0;
	m_x.clear();
	m_y.clear();
	m_solve.clear();
	m_flag = _LINEAR;
	m_CorCoef = 0.0;
	m_CorCoef2 = 0.0;
}
//---------------------------------------------------------------------------
CLeastSquare::CLeastSquare(int Num, double* x, double* y, int flag)
{
	m_CorCoef = 0.0;
	m_CorCoef2 = 0.0;

	int i;
	int cnt = 0;
	m_Num = Num;
	m_flag = flag;

	switch (m_flag) {
	case _LINEAR:
		m_solve.resize(2);//解は2つ
		m_x.resize(Num);
		m_y.resize(Num);
		for(i=0;i<Num;i++){
			m_x[i] = x[i];
			m_y[i] = y[i];
		}
		break;
	case _TWO_DIM:
		m_solve.resize(3);//解は3つ
		m_x.resize(Num);
		m_y.resize(Num);
		for(i=0;i<Num;i++){
			m_x[i] = x[i];
			m_y[i] = y[i];
		}
		break;
	case _THREE_DIM:
		m_solve.resize(4);//解は4つ
		m_x.resize(Num);
		m_y.resize(Num);
		for(i=0;i<Num;i++){
			m_x[i] = x[i];
			m_y[i] = y[i];
		}
		break;
	case _EXPONENTIAL:
		m_solve.resize(2);//解は2つ
#if 0
		m_x.set(x, Num);
		m_y = Log(y, Num);
#else
		for (i = 0; i < Num; i++) {
			if (y[i] > 0) {
				cnt++;
			}
		}
		if (cnt == Num) {
			m_x.resize(Num);
			m_y.resize(Num);
			for(i=0;i<Num;i++){
				m_x[i] = x[i];
				m_y[i] = log(y[i]);
			}
		}
		else{
			m_Num = cnt;
			cnt = 0;
			double* xtmp = new double[m_Num];
			double* ytmp = new double[m_Num];
			for (i = 0; i < Num; i++) {
				if (y[i] > 0) {
					xtmp[cnt] = x[i];
					ytmp[cnt] = y[i];
					cnt++;
				}
			}
			m_x.resize(m_Num);
			m_y.resize(m_Num);
			for(i=0;i<m_Num;i++){
				m_x[i] = xtmp[i];
				m_y[i] = log(ytmp[i]);
			}

			delete[] xtmp;
			delete[] ytmp;
		}
#endif
		break;
	case _POWER:
		m_solve.resize(2);//解は2つ

		for (i = 0; i < Num; i++) {
			if (y[i] > 0 && x[i] > 0) {
				cnt++;
			}
		}
		if (cnt == Num) {
			m_x.resize(Num);
			m_y.resize(Num);
			for(i=0;i<Num;i++){
				m_x[i] = log(x[i]);
				m_y[i] = log(y[i]);
			}
		}
		else{
			m_Num = cnt;
			cnt = 0;
			double* xtmp = new double[m_Num];
			double* ytmp = new double[m_Num];
			for (i = 0; i < Num; i++) {
				if (y[i] > 0 && x[i] > 0) {
					xtmp[cnt] = x[i];
					ytmp[cnt] = y[i];
					cnt++;
				}
			}
			m_x.resize(m_Num);
			m_y.resize(m_Num);
			for(i=0;i<m_Num;i++){
				m_x[i] = log(xtmp[i]);
				m_y[i] = log(ytmp[i]);
			}

			delete[] xtmp;
			delete[] ytmp;
		}
		break;
	default:
		;
	}
}
//---------------------------------------------------------------------------
CLeastSquare::~CLeastSquare()
{
	Clear();
}
//---------------------------------------------------------------------------
void CLeastSquare::Clear()
{
	m_Num = 0;
	m_flag = _LINEAR;

	m_x.clear();
	m_y.clear();
	m_solve.clear();
}
//---------------------------------------------------------------------------
bool CLeastSquare::Calc()
{

	int i;
	double x_sigma(0.), x2_sigma(0.), x3_sigma(0.), x4_sigma(0.), x5_sigma(0.), x6_sigma(0.);
	double y_sigma(0.);
	double xy_sigma(0.), x2y_sigma(0.), x3y_sigma(0.);
	double Sxx, Syy, Sxy;//分散、共分散
	vector<double> v2(2), v3(3), v4(4);
	Matrix mtx2(2), mtx2_inv(2), mtx3(3), mtx3_inv(3), mtx4(4), mtx4_inv(4);
	double bunbo;
	CStatUtil	util;

	switch (m_flag) {
	case _LINEAR:
	case _EXPONENTIAL:
	case _POWER:
		x_sigma  = util.GetSumation(m_x);
		y_sigma  = util.GetSumation(m_y);
		x2_sigma = util.GetSumationSquare(m_x);
		for (i = 0; i < m_Num; i++) {
			xy_sigma += m_x[i] * m_y[i];
		}

#if 1 	//まじめに逆行列を計算
		v2[0] = xy_sigma;
		v2[1] = y_sigma;

		mtx2[0][0] = x_sigma;
		mtx2[0][1] = x2_sigma;
		mtx2[1][0] = m_Num;
		mtx2[1][1] = x_sigma;

		//ゼロ行列のときは実行エラーで落ちるため、FALSEを返す（150810 ttsuji）
		if (fabs(mtx2[0][0]) < LS_EPS &&
			fabs(mtx2[0][1]) < LS_EPS &&
			fabs(mtx2[1][0]) < LS_EPS &&
			fabs(mtx2[1][1]) < LS_EPS) {
			return false;
		}

		mtx2_inv = mtx2.inv();
		m_solve = mtx2_inv * v2;
#else
		//あらかじめ解析的に解いた解の公式への代入
		bunbo = m_Num * x2_sigma - x_sigma * x_sigma;
		if (fabs(bunbo) < LS_EPS) {
			return false;
		}
		m_solve[0] = (m_Num * xy_sigma - x_sigma * y_sigma) / bunbo;
		m_solve[1] = (x2_sigma * y_sigma - x_sigma * xy_sigma) / bunbo;
#endif
    	//相関係数の計算
		Sxx = util.GetMeanSquare(m_x) - util.GetMean(m_x) * util.GetMean(m_x);
		Syy = util.GetMeanSquare(m_y) - util.GetMean(m_y) * util.GetMean(m_y);
		Sxy = xy_sigma / m_Num - x_sigma * y_sigma / (m_Num * m_Num);

 #if 1 	//相関係数が計算できない例外処理（140907 ttsuji）
		if (! (Sxx * Syy)) {
			m_CorCoef = m_CorCoef2 = 1.0e6;//とりあえずありえない値を入力
			break;
		}
 #endif
		m_CorCoef  = Sxy / sqrt(Sxx * Syy);
		m_CorCoef2 = m_CorCoef * m_CorCoef;
		break;
	case _TWO_DIM:
		x_sigma  = util.GetSumation(m_x);
		y_sigma  = util.GetSumation(m_y);
		x2_sigma = util.GetSumationSquare(m_x);
		x3_sigma = util.GetSumationCube(m_x);
		x4_sigma = util.GetSumationQuadruplicate(m_x);
		for (i = 0; i < m_Num; i++) {
			xy_sigma += m_x[i] * m_y[i];
			x2y_sigma += m_x[i] * m_x[i] * m_y[i];
		}

		//まじめに逆行列を計算
		v3[0] = x2y_sigma;
		v3[1] = xy_sigma;
		v3[2] = y_sigma;

		mtx3[0][0] = x2_sigma;
		mtx3[0][1] = x3_sigma;
		mtx3[0][2] = x4_sigma;

		mtx3[1][0] = x_sigma;
		mtx3[1][1] = x2_sigma;
		mtx3[1][2] = x3_sigma;

		mtx3[2][0] = m_Num;
		mtx3[2][1] = x_sigma;
		mtx3[2][2] = x2_sigma;

		mtx3_inv = mtx3.inv();
		m_solve = mtx3_inv * v3;

		//相関係数の計算
		Sxx = util.GetMeanSquare(m_x) - util.GetMean(m_x) * util.GetMean(m_x);
		Syy = util.GetMeanSquare(m_y) - util.GetMean(m_y) * util.GetMean(m_y);
		Sxy = xy_sigma / m_Num - x_sigma * y_sigma / (m_Num * m_Num);
		m_CorCoef  = Sxy / sqrt(Sxx * Syy);
		m_CorCoef2 = m_CorCoef * m_CorCoef;
		break;
	case _THREE_DIM:
		//未実装
		break;
	default:
    	return false;
	}

	return true;
}
//---------------------------------------------------------------------------
//対数変換（自然対数）
vector<double> CLeastSquare::Log(const vector<double>& src)
{
	vector<double> dst;
	for (int i = 0; i < src.size(); i++) {
		if (src[i] > 0.0){
			dst.push_back(log(src[i]));		//自然対数
		}
	}
	return dst;
}
//---------------------------------------------------------------------------
//対数変換
vector<double> CLeastSquare::Log(const double* src, int Num)
{
	vector<double> dst;
	for(int i=0;i<Num;i++){
		if (src[i] > 0.0){
			dst.push_back(log(src[i]));
		}
	}
	return dst;
}
//---------------------------------------------------------------------------
//指数変換
vector<double> CLeastSquare::Exp(const vector<double>& src)
{
	vector<double> dst;
	for (int i = 0; i < src.size(); i++) {
		dst.push_back(exp(src[i]));
	}
	return dst;
}
//---------------------------------------------------------------------------
//指数変換
vector<double> CLeastSquare::Exp(const double* src, int Num)
{
	vector<double> dst;
	for (int i = 0; i < Num; i++) {
		dst.push_back(exp(src[i]));
	}
	return dst;
}

