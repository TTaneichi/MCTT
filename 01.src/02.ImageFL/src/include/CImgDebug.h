/*
 * CImgDebug.h
 *
 *  Created on: 2021/07/12
 *      Author: 10097663
 */

#ifndef CIMGDEBUG_H_
#define CIMGDEBUG_H_

class CImgDebug {
public:
	CImgDebug();
	virtual ~CImgDebug();

	//拡大縮小の確認
	void CheckScaling();

	//平行移動の確認
	void CheckShiftXY();

	//行列クラスの動作確認
	void CheckMatrix();

	//CStatUtilクラスの動作確認
	void CheckCStatUtil();

};

#endif /* CIMGDEBUG_H_ */
