/*********************************************************************
 * @file   CSETranslator.h
 * @brief  SE通信の通信データ解析クラス
 * @note   ここに簡単な説明を記入する
 *
 * @author T.Taneichi
 * @date  2021/05/13 T.Taneichi
 * @license Copyright (C) 2021 FUJIFILM Corporation. All rights reserved.
 *********************************************************************/
#ifndef CSETRANSLATOR_H_
#define CSETRANSLATOR_H_

#include <stdio.h>
#include <string>
using namespace std;
#include "Common.h"

//SEコマンド定義
enum ESECommandType {
	ESECT_REQ_REGWRITE = 0,		//レジスタ書き込み要求
	ESECT_RSP_REGWRITE,			//レジスタ書き込み応答
	ESECT_REQ_REGREAD,			//レジスタ読み込み要求
	ESECT_RSP_REGREAD,			//レジスタ読み込み応答
	ESECT_NTF_OUT_DATA,			//データ出力指示
	ESECT_REQ_DELETEIMG,		//データ削除要求
	ESECT_RSP_DELETEIMG,		//データ削除応答
	ESECT_REQ_READY,			//Ready要求
	ESECT_RSP_READY,			//Ready応答
	ESECT_REQ_NOT_READY,		//NotReady要求
	ESECT_RSP_NOT_READY,		//NotReady応答 10
	ESECT_REQ_IMGCONF,			//画像読取設定要求
	ESECT_RSP_IMGCONF,			//画像読取設定応答
	ESECT_REQ_CONNECT,			//データ転送接続要求
	ESECT_RSP_CONNECT,			//データ転送接続応答
	ESECT_REQ_END_OUT,			//データ出力終了要求
	ESECT_RSP_END_OUT,			//データ出力終了応答
	ESECT_REQ_SET_TIME,			//時刻設定要求
	ESECT_RSP_SET_TIME,			//時刻設定応答
	ESECT_REQ_WLAN_RESET,		//RMV-WLANリセット要求
	ESECT_RSP_WLAN_RESET,		//RMV-WLANリセット応答
	ESECT_NTF_SHOTSW1,			//ShotSw1通知
	ESECT_NTF_SHOTSW2,			//ShotSw2通知 20
	ESECT_NTF_SHOTGO,			//ShotGo通知
	ESECT_NTF_ACCUM,			//ACCUM通知
	ESECT_NTF_WAKEUP,			//起動通知
	ESECT_NTF_WLANERR,			//WLANエラー通知
	ESECT_NTF_RMVERR,			//RMVエラー通知
	ESECT_NTF_RMVRESET,			//RMVシーケンスリセット通知
	ESECT_NTF_SE_STATE,			//SE状態通知
	ESECT_NTF_RMVACK,			//RMV ACK通知
	ESECT_CMD_MAX
};

//メッセージ解析結果
enum ESECommandTransValidation {
	VALID_COMMAND_RES = 0,		//適切に認識した
	INVALID_COMMAND_RES, 		//認識できないコマンドIDの受信
	INVALID_PARAMETER_RES,		//パラメータが不適切
	INVALID_DATA_LENGTH,		//データサイズが不適切
	INVALID_POINTER				//渡されたデータの型が不適切
};

//データ削除要求パラメータ
typedef struct _SDeleteDataParam {
	unsigned char	exp_mode;	//撮影モード
	unsigned char	delNum;		//本画像削除枚数
	unsigned short	dataMen;	//データ面指定
} SDeleteDataParam;

//画像読取設定
typedef struct _SImageConfParam {
	unsigned short	subPixel;	//副走査ライン数
	unsigned short	transLine;	//転送ライン数単位
} SImageConfParam;

//データ転送接続要求
typedef struct _SConnectPort {
	unsigned short	dataConnectID;	//データ通信制御ID
	unsigned short	portNum;		//ポート番号
} SConnectPort;

//データ出力終了応答
typedef struct _SEndOutputData {
	unsigned short	state;			//ステータス
	unsigned short	dataConnectID;	//データ通信制御ID
} SEndOutputData;

//時刻設定要求
typedef struct _SSetTimeData {
	unsigned short	year;
	unsigned char	month;
	unsigned char	day;
	unsigned char	hour;
	unsigned char	min;
	unsigned char	sec;
} SSetTimeData;

//SE通信データ(コマンド毎)
typedef union {
	SRegSet	RegSet;					//レジスタ設定
	bool	bShotSW;				//ショットSW状態(SW1/SW2/ShotGo兼用)
	SDeleteDataParam	delParam;	//データ削除要求
	SImageConfParam	imagConf;		//画像読取設定
	SConnectPort	connectPort;	//データ転送接続要求
	SEndOutputData	endOutputData;	//データ出力終了応答
	SSetTimeData	timeData;		//時刻情報設定
	unsigned short commonData;		//共通データ
	unsigned long  commonLongData;	//共通Longデータ
	unsigned short common2Data[2];	//共通データ2
} SECmdData;

//SEコマンド構造体
typedef struct _SETransData {
	ESECommandType eCommandType;			//コマンドタイプ
	SECmdData		data;						//データ構造体
} SETransData;

class CSETranslator {
public:
	CSETranslator();
	virtual ~CSETranslator();

	//送信データ作成
	bool MakeMessage(unsigned char* out_makeData, int &datalength, SETransData in_cmdData);

	//受信データ解析
	ESECommandTransValidation TranslateMessage(unsigned char* in_transRawData, SETransData &out_cmdData, int &useLength);

	//コマンド名取得
	string	GetCmdName(ESECommandType in_CmdType);

private:
	//個別コマンドのデータ部作成
	bool makeEachCmdMessage(unsigned char* out_makeData, SETransData in_cmdData);

	//個別コマンドのデータ解析
	ESECommandTransValidation transEachCmdMessage(unsigned char* in_transRawData, SETransData &out_cmdData);

	//レジスタデータ作成
	bool makeRegData(unsigned char* out_makeData, SETransData in_cmdData);

	//データ削除要求データ作成
	bool makeReqDeleteImg(unsigned char* out_makeData, SETransData in_cmdData);

	//画像読取設定データ作成
	bool makeImgConfData(unsigned char* out_makeData, SETransData in_cmdData);

	//データ転送接続要求データ作成
	bool makeConnectData(unsigned char* out_makeData, SETransData in_cmdData);

	//データ出力終了応答データ作成
	bool makeDataOutEndData(unsigned char* out_makeData, SETransData in_cmdData);

	//時刻情報設定要求データ作成
	bool makeSetTimeData(unsigned char* out_makeData, SETransData in_cmdData);

	//共通データ作成(UShort)
	bool makeCommonData(unsigned char* out_makeData, SETransData in_cmdData);

	//共通データ作成(ULong)
	bool makeCommonLongData(unsigned char* out_makeData, SETransData in_cmdData);

	//共通データ2作成(UShort*2)
	bool makeCommonData2(unsigned char* out_makeData, SETransData in_cmdData);

	//ショットSwデータ作成
	bool makeShotSWData(unsigned char* out_makeData, SETransData in_cmdData);

	//レジスタデータ解析
	ESECommandTransValidation transRegData(unsigned char* in_transRawData, SETransData &out_cmdData);

	//データ削除要求データ解析
	ESECommandTransValidation transReqDeleteImg(unsigned char* in_transRawData, SETransData &out_cmdData);

	//画像読取設定データ解析
	ESECommandTransValidation transImgConfData(unsigned char* in_transRawData, SETransData &out_cmdData);

	//データ転送接続要求データ解析
	ESECommandTransValidation transConnectData(unsigned char* in_transRawData, SETransData &out_cmdData);

	//データ出力終了応答データ解析
	ESECommandTransValidation transDataOutEndData(unsigned char* in_transRawData, SETransData &out_cmdData);

	//共通データ解析(UShort)
	ESECommandTransValidation transCommonData(unsigned char* in_transRawData, SETransData &out_cmdData);

	//共通データ解析(ULong)
	ESECommandTransValidation transCommonLongData(unsigned char* in_transRawData, SETransData &out_cmdData);

	//ショットSwデータ解析
	ESECommandTransValidation transShotSWData(unsigned char* in_transRawData, SETransData &out_cmdData);

};

#endif /* CSETRANSLATOR_H_ */
