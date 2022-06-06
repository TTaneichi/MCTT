/*
 * Matrix.cpp
 *
 *  Created on: 2021/09/30
 *      Author: 10097663
 */

#include "Matrix.h"

Matrix::Matrix() {
	_dim = 0;
	_row.clear();

}

Matrix::~Matrix() {
	clear();
}

//	次元のみ指定コンストラクタ
Matrix::Matrix(const int dim)
{
	if (dim <= 0)
	{
		_dim = 0;
		_row.clear();
		return;
	}

	_dim = dim;
	_row.resize(_dim);
	for (int i = 0; i < _dim; i++) {
		_row[i].resize(_dim);
	}

}

//	コピーコンストラクタ
Matrix::Matrix(const Matrix& mtx)
{
	_dim = mtx._dim;
	_row.resize(_dim);

	for (int i = 0; i < _dim; i++){
		_row[i] = mtx._row[i];
	}
}
//	ベクトル配列指定コンストラクタ
Matrix::Matrix(const vector<vector<double>> data)
{
	_dim = data[0].size();
	_row.resize(_dim);

	for (int i = 0; i < _dim; i++){
		_row[i] = data[i];
	}

}

#if 0
//	２次元配列指定コンストラクタ
//	【！】	行と列が入れ替わる
Matrix::Matrix(const double* val, const int	dim)
{
	assert (!(_dim - dim));
	for (int i = 0; i < _dim; i++)
	{
		for (int j = 0; j < _dim; j++)
		{
			_row[i][j] = val[j][i];
		}
	}
}
#endif

//	クリア
void Matrix::clear()
{
	_row.clear();
	_dim = 0;
}


//	代入演算子
Matrix& Matrix::operator=(const Matrix& mtx)
{
	if (this == &mtx)
	{
		return *this;
	}

	_dim = mtx._dim;
	_row.resize(_dim);

	for (int i = 0; i < _dim; i++){
		_row[i] = mtx._row[i];
	}

	return *this;
}


//	ベクトル配列代入演算子
Matrix& Matrix::operator=(const vector<vector<double>> data)
{
	_dim = data[0].size();
	_row.resize(_dim);

	for (int i = 0; i < _dim; i++){
		_row[i] = data[i];
	}

	return *this;

}

#if 0
//	２次元配列代入演算子
//	【！】	行と列が入れ替わる
Matrix&  Matrix::operator=(const double val[_dim][_dim])
{
	for (int i = 0; i < _dim; i++)
	{
		for (int j = 0 ; j < _dim; j++)
		{
			_row[i][j] = val[j][i];
		}
	}
	return *this;
}
#endif

vector<double>&	Matrix::operator[](int i){

	return _row[i];

}
double	Matrix::operator()(int i, int j){

	return _row[i][j];
}

//	加算演算子
Matrix Matrix::operator+(const Matrix& mtx)
{
	assert (!(_dim - mtx._dim));

	Matrix  res(_dim);
	for (int i = 0; i < _dim; i++){
		for (int j=0; j < _dim; j++){
			res._row[i][j] = this->_row[i][j] + mtx._row[i][j];
		}
	}

	return res;
}


//	減算演算子
Matrix Matrix::operator-(const Matrix& mtx)
{
	assert (!(_dim - mtx._dim));

	Matrix  res(_dim);
	for (int i = 0; i < _dim; i++){
		for (int j=0; j < _dim; j++){
			res._row[i][j] = _row[i][j] - mtx._row[i][j];
		}
	}
	return res;
}


#if 0
//	スカラ積算演算子
Matrix Matrix::operator*(double k) const
{
	Matrix	res;
	for (int i = 0; i < _dim; i++)
	{
		res._row[i] = _row[i] * k;
	}
	return res;
}
#endif

//	ベクトル積算演算子
vector<double> Matrix::operator*(const vector<double> &data)
{
	assert (!(_dim - data.size()));

	vector<double>	res(_dim);
	for (int i = 0; i < _dim; i++){
		res[i] = 0.0;
		for(int j=0;j<_dim; j++){
			res[i] += _row[j][i] * data[j];
		}
	}
	return res;
}


//	(マトリクス)積算演算子
Matrix Matrix::operator*(const Matrix&	mtx)
{
	assert (!(_dim - mtx._dim));

	Matrix	res(_dim);
	for (int i = 0; i < _dim; i++){
		for (int j = 0; j < _dim; j++){
			res._row[i][j] = 0.0;
			for (int k = 0; k < _dim; k++){
				res._row[i][j] += _row[k][j] * mtx._row[i][k];
			}
		}
	}
	return res;
}


//	転置
Matrix Matrix::trv()
{
	Matrix res(_dim);
	for (int i = 0; i < _dim; i++){
		for (int j = 0; j < _dim; j++){
				  res._row[i][j] = _row[j][i];
		}
	}
	return res;
}


//	逆行列
//	【！】 行列が正則であることが前提
Matrix Matrix::inv(double eps)
{
	Matrix res(_dim);

	assert(inv(res, eps));

	return res;
}


//	方程式の解
//	【！】	行列が正則であることが前提
vector<double> Matrix::eqt(const vector<double>	&vct, double eps)
{
	vector<double> res(_dim);
	assert(eqt(vct, res, eps));
	return res;
}


//	逆行列
//	【！】	とりあえず。効率は悪い。
bool Matrix::inv(Matrix& mtx, double eps)
{
	if (!_dim)
	{
		return false;
	}

	Matrix res(_dim);
	for (int i = 0; i < _dim; i++){
		vector<double> b;
		b.resize(_dim);
		for(int j=0;j<_dim;j++){
			b[j] = 0.0;
		}
		b[i] = 1.0;
		if (!eqt(b, res._row[i], eps)){
			   return false;
		}
/*
#ifdef	DEBUG
		cerr << b;
		cerr << res[i] << endl;
#endif	// DEBUG
*/
	}

	mtx = res.trv();

	return true;
}


//	方程式を解く(ガウスの消去法)
bool Matrix::eqt(const vector<double>& vct, vector<double>& b, double eps)
{
	if (!_dim)
	{
		return false;
	}

	Matrix	a(*this);
	b = vct;

	for (int i = 0; i < _dim; i++){
		// 部分(行のみ)ピボット選択
		int	max = i;
		for (int j = i + 1; j < _dim; j++){
			if (fabs(a._row[j][i]) > fabs(a._row[max][i])){
				max = j;
			}
		}
		if (fabs(a._row[max][i]) < eps){
			return false;		// 正則でない！
		}
		if (max != i){
			double	tmp;
			for (int k = i; k < _dim; k++){
				tmp = a._row[i][k];
				a._row[i][k] = a._row[max][k];
				a._row[max][k] = tmp;
			}
			tmp = b[i];
			b[i] = b[max];
			b[max] = tmp;
		}
/*
#ifdef	DEBUG
		cerr << "[" << i << "]===\n";
		cerr << a << b;
#endif	// DEBUG
*/

		// 前進消去
		for (int j = i + 1; j < _dim; j++){
			a._row[j][i] /= (-a._row[i][i]);
			for (int k = i + 1; k < _dim; k++){
				a._row[j][k] += (a._row[j][i] * a[i][k]);
			}
			b[j] += (a._row[j][i] * b[i]);
		}
/*
#ifdef	DEBUG
		cerr << "------\n";
		cerr << a << b << endl;
#endif	// DEBUG
*/
	}

	//	後退代入
	b[_dim-1] /= a._row[_dim-1][_dim-1];
	for (int i = _dim - 2; i >= 0; i--){
		for (int j = i + 1; j < _dim; j++)
		{
			b[i] -= (a._row[i][j] * b[j]);
		}
		b[i] /= a._row[i][i];
	}

	return true;
}


//	入力
void Matrix::getfrom(istream& s)
{
//	for (int i = 0; i < _dim; i++) {
//		_row[i].getfrom(s);
//	}
}


//	出力
void Matrix::putto(ostream& s)
{
//	for (int i = 0; i < _dim; i++) {
//		_row[i].putto(s);
//	}
}


#if 0
//	入力
istream& operator>>(istream& s, Matrix& t)
{
	for (int i = 0; i < _dim; i++)
	{
		s >> t[i];
	}
	return s;
}


//	出力
ostream& operator<<(ostream& s, const Matrix& t)
{
	for (int i = 0; i < _dim; i++)
	{
		s << t[i];
	}
	return s;
}
#endif

