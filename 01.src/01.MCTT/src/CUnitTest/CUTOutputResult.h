/*
 * CUTOutputResult.h
 * テスト結果の出力用クラス
 *  Created on: 2018/06/28
 *      Author: 10097663
 */

#ifndef CUTOUTPUTRESULT_H_
#define CUTOUTPUTRESULT_H_

class CUTOutputResult {
public:
	CUTOutputResult();

	~CUTOutputResult();

	//結果ファイル名を指定する
	void SetResultFileName(char* strFileName);

	//集計用の結果ファイル名を指定する
	void SetAllResultFileName(char* strFileName);

	//結果ファイルへ出力する
	void OutputResult(const char* output);

	//集計用の結果ファイルへ出力する
	void OutputAllResult(const char* output);

private:
	char* m_pStrResultFileName;		//結果ファイル名

	char* m_pStrAllResultFileName;	//集計用結果ファイル名

	bool m_bMadeResultFile;			//結果ファイル作成済み

	bool m_bMadeAllResultFile;			//集計用ファイル作成済み

};

#endif /* CUTOUTPUTRESULT_H_ */
