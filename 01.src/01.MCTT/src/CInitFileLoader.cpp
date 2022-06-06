/*
 * CInitFileLoader.cpp
 *
 *  Created on: 2021/05/07
 *      Author: 10097663
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
using namespace std;
#include "CInitFileLoader.h"
#include "CDataControl.h"
#include "CUnitLog.h"


#define INIT_FILE_PATH	"Init.txt"


CInitFileLoader::CInitFileLoader() {
	// TODO 自動生成されたコンストラクター・スタブ

}

CInitFileLoader::~CInitFileLoader() {
	// TODO Auto-generated destructor stub
}

bool CInitFileLoader::LoadInitFile(){

	SIMLOG_NORM("[FILE] ====   LoadInitFile()   ===");

	bool bRet = true;

	//設定ファイルのロード処理
	FILE *fp;

	//ファイルオープン
	if ((fp = fopen(INIT_FILE_PATH, "r")) == NULL) {
		printf("CInitFileLoader::LoadInitFile() Open Error!!! %s\n", INIT_FILE_PATH);
		return false;
	}

	char buf[1024];
	memset(buf,0,sizeof(buf));
	bool bBracketChecking = false;			//括弧内解析中
	unsigned long lineNum = 0;				//読み込み行数
	bool bEndBracket;

	while( fgets(buf, 1024, fp)){
		lineNum++;

		for(int i=0;i<1024;i++){
			if (buf[i]== 0x0a){	//改行コード削除
				buf[i]= 0;
				break;
			}
		}
		if (buf[0] == '#'){		//コメント行対応
			memset(buf,0,sizeof(buf));
			continue;
		}

		checkAndSetData(buf, 1024);
		memset(buf,0,sizeof(buf));
	}

	fclose(fp);

	return bRet;
}
//読み込んだ文字列をチェックしてデータとして格納する
void CInitFileLoader::checkAndSetData(char* buf, int size){


	char strItem[512];
	char strValue[512];
	memset(strItem,0,sizeof(strItem));
	memset(strValue,0,sizeof(strValue));
	CDataControl *pDataControl = CDataControl::GetInstance();

	//左の項目と右の項目に分割する
	char *adr = strchr( buf , ( int )'=' );
	if (adr == NULL){
		return;
	}

	memcpy(strItem, buf, adr-buf);
	memcpy(strValue, &adr[1], strlen(&adr[1]));
	printf("strItem:%s strValue:%s\n", strItem, strValue);
	SIMLOG_NORM("[FILE] strItem:%s strValue:%s", strItem, strValue);

	if (0 == strcmp(strItem,(char*)"IPADDR_SE1")){
		printf("IPADDR_SE1:%s\n",strValue);
		pDataControl->SetSE_IPADDR(strValue, SE_NUM1);
	} else if (0 == strcmp(strItem,(char*)"IPADDR_SE2")){
		printf("IPADDR_SE2:%s\n",strValue);
		pDataControl->SetSE_IPADDR(strValue, SE_NUM2);
	} else if (0 == strcmp(strItem,(char*)"AAAB")){
		printf("AAAB\n");
	} else if (0 == strcmp(strItem,(char*)"STAGE")){
//		printf("STAGE %s\n",strValue);
		if (0==strcmp(strValue, "1")){
			pDataControl->m_bUseStage = true;
		} else {
			pDataControl->m_bUseStage = false;
		}
	} else if (0 == strcmp(strItem,(char*)"RASBOX")){
//		printf("RASBOX %s\n",strValue);
		if (0 == strcmp(strValue, "1")){
			pDataControl->m_bUseRasBox = true;
		} else {
			pDataControl->m_bUseRasBox = false;
		}
	} else if (0 == strcmp(strItem, (char*)"ROT_SERI")){
		printf("RotSeri:%s\n",strValue);
		pDataControl->SetRotStageFileName(strValue);
	}

}

