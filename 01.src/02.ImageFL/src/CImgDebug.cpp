/*
 * CImgDebug.cpp
 *
 *  Created on: 2021/07/12
 *      Author: 10097663
 */

#include "CImgDebug.h"
#include "CTTMA.h"
#include "Matrix.h"
#include "CStatUtil.h"

CImgDebug::CImgDebug() {
	// TODO 自動生成されたコンストラクター・スタブ

}

CImgDebug::~CImgDebug() {
	// TODO Auto-generated destructor stub
}

//拡大縮小の確認
void CImgDebug::CheckScaling(){

	TTMA	orgImg;

	orgImg.ReadTma("chest.tma");

//	orgImg.Scaling(100, 500, 0.2, 1.5, 30000);
	orgImg.ScalingLR(0.4, 0.99, 0);

	orgImg.SaveTmaTdt("scaling.tma");

}
//平行移動の確認
void CImgDebug::CheckShiftXY(){

	TTMA	orgImg;

	orgImg.ReadTma("chest.tma");

	orgImg.ShiftXY(0.75,0.75);

	orgImg.SaveTmaTdt("scaling.tma");
}
//行列クラスの動作確認
void CImgDebug::CheckMatrix(){

	Matrix mtx(2);

	mtx._row[0][0] = 3;
	mtx._row[1][0] = 5;
	mtx._row[0][1] = 1;
	mtx._row[1][1] = 2;

	printf("input data-----\n");
	printf("%7.4lf %7.4lf\n",mtx._row[0][0], mtx._row[1][0]);
	printf("%7.4lf %7.4lf\n",mtx._row[0][1], mtx._row[1][1]);

	Matrix invMtx = mtx.inv();

	printf("invert data----\n");
	printf("%7.4lf %7.4lf\n",invMtx._row[0][0], invMtx._row[1][0]);
	printf("%7.4lf %7.4lf\n",invMtx._row[0][1], invMtx._row[1][1]);

	Matrix timesMtx = mtx * invMtx;

	printf("times data ----\n");
	printf("%7.4lf %7.4lf\n",timesMtx._row[0][0], timesMtx._row[1][0]);
	printf("%7.4lf %7.4lf\n",timesMtx._row[0][1], timesMtx._row[1][1]);

}
//CStatUtilクラスの動作確認
void CImgDebug::CheckCStatUtil(){

	CStatUtil	util;

	vector<double> checkDataD;
	checkDataD.push_back(1.5);
	checkDataD.push_back(5.5);
	checkDataD.push_back(-4);

	printf("Sum:%7.4lf Mean:%7.4lf\n",util.GetSumation(checkDataD), util.GetMean(checkDataD));
	printf("SumSquare:%7.4lf SumCube:%7.4lf\n",util.GetSumationSquare(checkDataD), util.GetSumationCube(checkDataD));
	printf("SumQuad:%7.4lf Naiseki:%7.4lf\n",util.GetSumationQuadruplicate(checkDataD), util.GetScalarProduct(checkDataD,checkDataD));

	vector<int> dataI;
	dataI.push_back(3);
	dataI.push_back(5);
	dataI.push_back(-7);
	dataI.push_back(-13);

	printf("Sum:%7.4lf Mean:%7.4lf\n",util.GetSumation(dataI), util.GetMean(dataI));
	printf("SumSquare:%7.4lf SumCube:%7.4lf\n",util.GetSumationSquare(dataI), util.GetSumationCube(dataI));
	printf("SumQuad:%7.4lf Naiseki:%7.4lf\n",util.GetSumationQuadruplicate(dataI), util.GetScalarProduct(dataI,dataI));

}

