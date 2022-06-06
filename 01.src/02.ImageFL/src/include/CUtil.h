/*
 * CUtil.h
 * ユーティリティクラス
 *  Created on: 2021/09/28
 *      Author: 10097663
 */

#ifndef CUTIL_H_
#define CUTIL_H_

#include "string"

using namespace std;

class CUtil {
public:
	CUtil();
	virtual ~CUtil();

	//int型のデータをファイルに出力する
	void MakeIntFile(string fileName, int* data, int dataSize);

	//double型のデータをファイルに出力する
	void MakeDoubleFile(string fileName, double* data, int dataSize);

};

#endif /* CUTIL_H_ */
