/*
 * CUtil.cpp
 *
 *  Created on: 2021/09/28
 *      Author: 10097663
 */

#include "CUtil.h"
#include <stdio.h>
#include <stdlib.h>

CUtil::CUtil() {
	// TODO 自動生成されたコンストラクター・スタブ

}

CUtil::~CUtil() {
	// TODO Auto-generated destructor stub
}

//int型のデータをファイルに出力する
void CUtil::MakeIntFile(string fileName, int* data, int dataSize){

	FILE*	fp;

	if ((fp = fopen(fileName.c_str(),"w")) == NULL){
		printf("Open file %s error!!!\n", fileName.c_str());
		return;
	}

	if (data == NULL){
		printf("data is null!!\n");
		return;
	}

	for(int i=0;i<dataSize; i++){
		fprintf(fp, "%d\n", data[i]);
	}

	fclose(fp);

}
//double型のデータをファイルに出力する
void CUtil::MakeDoubleFile(string fileName, double* data, int dataSize){

	FILE*	fp;

	if ((fp = fopen(fileName.c_str(),"w")) == NULL){
		printf("Open file %s error!!!\n", fileName.c_str());
		return;
	}

	if (data == NULL){
		printf("data is null!!\n");
		return;
	}

	for(int i=0;i<dataSize; i++){
		fprintf(fp, "%7.4lf\n", data[i]);
	}

	fclose(fp);
}
