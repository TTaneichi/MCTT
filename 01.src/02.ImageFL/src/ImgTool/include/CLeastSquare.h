/*
 * CLeastSquare.h
 *
 *  Created on: 2021/04/06
 *      Author: 10097663
 */

#ifndef CLEASTSQUARE_H_
#define CLEASTSQUARE_H_

#include <vector>

using namespace std;

#define _EXPONENTIAL	(0) //指数fitting
#define _POWER			(1) //冪乗fitting
#define _LINEAR			(2) //直線fitting
#define _TWO_DIM		(3) //放物線fitting
#define _THREE_DIM		(4) //3次関数fitting

//	定数
const double	LS_EPS = 1.0e-6;	// 正則判定精度

//---------------------------------------------------------------------------
class CLeastSquare
{
private:
	int		m_Num;//サンプル点数
	vector<double>		m_x;//ｘ変数
	vector<double>		m_y;//ｙ変数
	vector<double>		m_solve;
	int		m_flag;//近似曲線フラグ（線形、多項式、指数等）
	double	m_CorCoef;//相関係数
	double	m_CorCoef2;//相関係数の2乗
	void 	DebugPrint();

public:
	CLeastSquare();
	CLeastSquare(int Num, double* x, double* y, int flag);
	~CLeastSquare();
	void Clear();
	bool Calc();
	vector<double> GetSolve()	{return m_solve;}
	double GetCorCoef()	{return m_CorCoef;}
	double GetCorCoef2()	{return m_CorCoef2;}
	int GetNumOfSample()	{return m_Num;}

	vector<double> Log(const vector<double>& src);//対数変換
	vector<double> Exp(const vector<double>& src);//指数変換
	vector<double> Log(const double* src, int Num);
	vector<double> Exp(const double* src, int Num);
};


#endif /* CLEASTSQUARE_H_ */
