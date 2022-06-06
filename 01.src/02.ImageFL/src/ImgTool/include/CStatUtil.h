/*
 * CStatUtil.h
 * 統計処理用のユーティリティクラス
 *  Created on: 2021/10/01
 *      Author: 10097663
 */

#ifndef IMGTOOL_CSTATUTIL_H_
#define IMGTOOL_CSTATUTIL_H_

#include <iostream>
#include <vector>
using namespace std;

#include <math.h>

class CStatUtil {
public:
	CStatUtil();
	virtual ~CStatUtil();

	//要素の総和を求める
	template<typename T> double GetSumation(const vector<T>& a){
		double sum = 0.0;
		for(int i=0;i<a.size();i++){
			sum += a[i];
		}
		return sum;
	}

	//要素の二乗和を求める
	template<typename T> double GetSumationSquare(const vector<T>& a){
		double sum = 0.0;
		for(int i=0;i<a.size();i++){
			sum += a[i]*a[i];
		}
		return sum;
	}

	//要素の三乗和を求める
	template<typename T> double GetSumationCube(const vector<T>& a){
		double sum = 0.0;
		for(int i=0;i<a.size();i++){
			sum += a[i]*a[i]*a[i];
		}
		return sum;
	}

	//要素の四乗和を求める
	template<typename T> double GetSumationQuadruplicate(const vector<T>& a){
		double sum = 0.0;
		for(int i=0;i<a.size();i++){
			sum += a[i]*a[i]*a[i]*a[i];
		}
		return sum;
	}

	//要素の平均を求める
	template<typename T> double GetMean(const vector<T>& a){

		double mean = 0.0;
		if (a.size()>0){
			mean = GetSumation(a) / a.size();
		}
		return mean;
	}

	//要素の分散を求める
	template<typename T> double GetVariance(const vector<T>& a){

		double mean = GetMean(a);
		double meanSquare = GetMeanSquare(a);
		double variance = meanSquare - mean * mean;
		return variance;
	}

	//要素の標準偏差を求める
	template<typename T> double GetStandardDeviation(const vector<T>& a){

		double variance = GetVariance(a);
		return sqrt(variance);
	}

	//要素の二乗平均を求める
	template<typename T> double GetMeanSquare(const vector<T>& a){

		double mean = GetSumationSquare(a) / a.size();
		return mean;
	}

	//要素の三乗平均を求める
	template<typename T> double GetMeanCube(const vector<T>& a){

		double mean = GetSumationCube(a) / a.size();
		return mean;
	}

	//要素の四乗平均を求める
	template<typename T> double GetMeanQuadruplicate(const vector<T>& a){

		double mean = GetSumationQuadruplicate(a) / a.size();
		return mean;
	}

	//内積を求める
	template<typename T> double GetScalarProduct(const vector<T>& a, const vector<T>& b){

		if (a.size() != b.size()){
			return 0.0;
		}

		double dScalarProduct = 0.0;
		for(int i=0;i<a.size();i++){
			dScalarProduct += a[i] * b[i];
		}
		return dScalarProduct;
	}

};

#endif /* IMGTOOL_CSTATUTIL_H_ */
