/*
 * Matrix.h
 *
 *  Created on: 2021/09/30
 *      Author: 10097663
 */

#ifndef IMGTOOL_MATRIX_H_
#define IMGTOOL_MATRIX_H_

#include <iostream>
#include <assert.h>
#include <math.h>
#include <vector>

using namespace std;

//定数
const double	Matrix_EPS = 1.0e-6;	// 正則判定精度

class Matrix {
public:
	int		_dim;
	vector< vector<double> >	_row;	//行ベクトル×_dim

public:
	Matrix(void);
	Matrix(const int dim);
	Matrix(const Matrix& mtx);
	Matrix(const vector<vector<double>> data);
	virtual ~Matrix(void);

	Matrix&		operator=(const Matrix& mtx);
	Matrix&		operator=(const vector<vector<double>> data);
	Matrix&		operator=(const double*);
	vector<double>&	operator[](int i);
	double		operator()(int i, int j);
	Matrix		operator+(const Matrix& mtx);
	Matrix		operator-(const Matrix& mtx);
	Matrix		operator*(double);
	vector<double>	operator*(const vector<double>&);
	Matrix		operator*(const Matrix& mtx);

	int dim()	{ return _dim;	}
	void clear();
	Matrix	trv(void);
	Matrix	inv(double eps = Matrix_EPS);;
	vector<double>	eqt(const vector<double>&, double eps = Matrix_EPS);

	bool	inv(Matrix& out_Matrix, double eps = Matrix_EPS);
	bool	eqt(const vector<double>& data, vector<double>& data2, double eps = Matrix_EPS);

	void	getfrom(istream& s);
	void	putto(ostream& s);

};

#endif /* IMGTOOL_MATRIX_H_ */
