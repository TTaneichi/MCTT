/*
 * Common.h
 *
 *  Created on: 2021/05/07
 *      Author: 10097663
 */

#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include <string>
using namespace std;

#define RET_OK	0
#define RET_NG	-1


//接続状態
enum EConnectState {
	ECS_DISCONNECT = 0,		//未接続状態
	ECS_WAIT_ACCEPT,		//接続待ち(サーバ側)
	ECS_CLIENT_SETUP,		//接続待ち(クライアント側)
	ECS_CONNECTED,			//接続済み
};

//SEレジスタセット
typedef struct _SRegSet {
	unsigned short regAddr;
	unsigned short data_H;
	unsigned short data_L;
} SRegSet;

//画像種別
enum EImageType {
	EIT_OFFSET = 0,		//オフセット画像
	EIT_EXPOSURE,		//照射画像
	EIT_BOTH,			//オフセット&照射画像の両方を取得
	EIT_MAX				//最大値
};

//画像取得パラメータ
typedef struct _SImgGetParam{
	string		triggerFile;		//トリガファイル名
	EImageType	eImgType;			//取得画像タイプ
	string		imageFolder;		//画像保存先フォルダ名
	string		imageFile;			//画像ファイル名(拡張子無)
	int			imageNum;			//画像取得枚数(1-999)
	bool		rotStage;			//回転ステージを使用するか
	int			rotType;			//回転ステージ回転タイプ
	bool		rasUse;				//ラズパイを使用するか
} SImgGetParam;


#endif /* INCLUDE_COMMON_H_ */
