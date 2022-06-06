/*
 * CMainLoop.cpp
 *
 *  Created on: 2021/05/10
 *      Author: 10097663
 */

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include "CMainLoop.h"
#include "CUnitLog.h"
#include "CTimerManager.h"
#include "MCTTError.h"
#include "CDataControl.h"

#define TRIGGER_FILE_PATH	"Trig/ImgGetTrigger.txt"
#define RESULT_FILE_PATH	"Trig/ImgResult.txt"
#define LOGTRIG_FILE_PATH	"Trig/LogGetTrigger.txt"

CMainLoop::CMainLoop() {

	m_State = EMLS_DISCONNECT;
	m_ImgState = EMIS_PREP;
	m_pSE_Ctrl1 = new CSEController(10);
	m_pSE_Ctrl2 = new CSEController(20);
	m_pSE_Main = NULL;
	m_pSE_Sub = NULL;
	m_pRasBox = NULL;
	m_pRotStage = NULL;
	m_pMP_Ctrl = new CMPController();
	m_bSE1Connect = false;
	m_bSE2Connect = false;
	m_bMPConnect = false;
	m_bRasBoxConnect = false;
	m_bRasBoxUse = false;
	m_bRotUse = false;
	m_bSEBothWait[0] = false;
	m_bSEBothWait[1] = false;
	m_GetImageNum = 0;
	m_BothOffsetGet = false;

}

CMainLoop::~CMainLoop() {
}

//メインループ処理実行
void CMainLoop::Execute(){

	SIMLOG_NORM("[ALL] ===== Start Main Loop  =====");
	CTimerManager* pTimerManager = CTimerManager::GetInstance();
	pTimerManager->RegisterTimer(this);

	CDataControl *pDataControl = CDataControl::GetInstance();
	m_bRasBoxUse = pDataControl->m_bUseRasBox;
	m_bRotUse = pDataControl->m_bUseStage;
	m_pRasBox = new CRasBox(m_bRasBoxUse);
	m_pRotStage = new CRotStage(m_bRotUse);

	CComSocket	MC_MainSocket;
	CComSocket	MC_ImgSocket1;
	CComSocket	MC_ImgSocket2;
	MC_MainSocket.SetServerClient(CONNECT_TYPE_SERV);
	MC_ImgSocket1.SetServerClient(CONNECT_TYPE_SERV);
	MC_ImgSocket2.SetServerClient(CONNECT_TYPE_SERV);
	m_pSE_Ctrl1->SetNotifyObject(this);
	m_pSE_Ctrl2->SetNotifyObject(this);
	m_pMP_Ctrl->SetNotifyObject(this);
	m_pRasBox->SetNotifyObj(this);
	m_pRotStage->SetNotifyObj(this);
	m_pSE_Ctrl1->SetSocketObject(&MC_MainSocket, SE_SOCK_TYPE_MAIN);
	m_pSE_Ctrl2->SetSocketObject(&MC_MainSocket, SE_SOCK_TYPE_MAIN);
	m_pSE_Ctrl1->SetSocketObject(&MC_ImgSocket1, SE_SOCK_TYPE_IMG);
	m_pSE_Ctrl2->SetSocketObject(&MC_ImgSocket2, SE_SOCK_TYPE_IMG);

	m_pSE_Ctrl1->Connect();
	m_pMP_Ctrl->Connect();
	m_pRasBox->Initialize();
	m_pRotStage->Initialize();

	while(1){
		//printf("loop 1st\n");
		//SE通信処理
		m_pSE_Ctrl1->Update();
		m_pSE_Ctrl2->Update();

		//printf("loop 2nd\n");
		//MP通信処理
		m_pMP_Ctrl->Update();

		m_pRasBox->Update();
		m_pRotStage->Update();

		//printf("loop 3rd\n");
		//トリガファイル処理
		if (m_State == EMLS_IDLE){
			CheckTrigFile();
		}

		//printf("loop 4th\n");
		//タイムアウト処理
		pTimerManager->Update();

		//ログ取得処理
		CheckLogSaveTrigFile();

		Sleep(10);		//msecで指定
	}

}

//SE状態通知
void CMainLoop::UpdateSE(ESENotifyType type, int SENo){

	SIMLOG_NORM("[MAIN] CMain UpdateSE type:%d SENo:%d",type, SENo);

	switch(type){
	case ESENT_CONNECT:			//接続完了
		if (m_bSE1Connect == false && m_pSE_Ctrl1->GetConnectState() == ECS_CONNECTED){
			m_bSE1Connect = true;
			if (m_bSE2Connect == false){
				m_pSE_Ctrl2->Connect();
			}
		} else if (m_bSE2Connect == false && m_pSE_Ctrl2->GetConnectState() == ECS_CONNECTED){
			m_bSE2Connect = true;
		}

		if (m_bSE1Connect && m_bSE2Connect){
			SIMLOG_NORM("[MAIN] =========================");
			SIMLOG_NORM("[MAIN] BOTH DETECTOR CONNECT :-)");
			SIMLOG_NORM("[MAIN] =========================");
			SetMainState(EMLS_IDLE);
			if (m_pSE_Ctrl1->GetSENo() == 1){
				m_pSE_Main = m_pSE_Ctrl1;
				m_pSE_Sub = m_pSE_Ctrl2;
			} else {
				m_pSE_Main = m_pSE_Ctrl2;
				m_pSE_Sub = m_pSE_Ctrl1;
			}
			m_pSE_Main->SetImgPortNum(61001);
			m_pSE_Sub->SetImgPortNum(61002);
			printf("[MAIN] ==== SE Main:%x SE Sub:%x\n",m_pSE_Main, m_pSE_Sub);
		}
		break;
	case ESENT_DISCONNECT:		//接続切れ
		break;
	case ESENT_PREP_COMP:		//SE準備完了
		execUpdateSE_PrepComp(SENo);
		break;
	case ESENT_SHOTGO_ON:		//ShotGo ON
		execUpdateSE_ShotGoOn();
		break;
	case ESENT_SHOTGO_OFF:		//ShotGo OFF
		execUpdateSE_ShotGoOff();
		break;
	case ESENT_DATA_COMP:		//画像データ受信完了
		execUpdateSE_DataComp(SENo);
		break;
	case ESENT_AFTER_COMP:		//画像データ取得後処理完了
		execUpdateSE_AfterComp(SENo);
		break;
	}

}

//MP状態通知
void CMainLoop::UpdateMP(EMPNotifyType type){

	SIMLOG_NORM("[MAIN] CMain UpdateMP:%d",type);

	switch(type){
	case EMPNT_SW1_ON:		//SW1 On
		execUpdateMP_SW1(true);
		break;
	case EMPNT_SW1_OFF:		//SW1 Off
		execUpdateMP_SW1(false);
		break;
	case EMPNT_SW2_ON:		//SW2 On
		execUpdateMP_SW2(true);
		break;
	case EMPNT_SW2_OFF:		//SW2 Off
		execUpdateMP_SW2(false);
		break;
	case EMPNT_CONNECT:		//接続完了
		execUpdateMP_Connect();
		break;
	case EMPNT_DISCONNECT:	//接続切れ
		execUpdateMP_Disconnect();
		break;
	}

}
//RasBox状態通知
void CMainLoop::UpdateRB(ERasBoxNotifyType type){

	SIMLOG_NORM("[MAIN] CMain UpdateRasBox:%d",type);

	switch(type){
	case ERBNT_CONNECT:
		m_bRasBoxConnect = true;
		break;
	case ERBNT_DISCONNECT:
		m_bRasBoxConnect = false;
		break;
	case ERBNT_RES_COMPEX:
		break;
	}

}

//ステージ状態通知
void CMainLoop::UpdateRS(ERotStageNotifyType type){

	SIMLOG_NORM("[MAIN] CMain UpdateRotateStage:%d",type);

}
//タイムアウト時実施メソッド
void CMainLoop::UpdateTimeout(){

	SIMLOG_NORM("[MAIN] CMain UpdateTimeout");

	if (m_State == EMLS_OFFSET_IMG && m_ImgState == EMIS_SW1_ON){
		SetEMIState(EMIS_SW2_ON);
		//SW2 Onを通知する
		if (m_pSE_Main != NULL){
			m_pSE_Main->SetShotSW(SE_SW2,true);			//サブ側には送る必要なし
		}
	}

}
//状態設定
void CMainLoop::SetMainState(EML_STATE in_state){

	if (m_State == in_state){
		return;
	}

	SIMLOG_NORM("[CMain STATE]   %s ->  %s", getStateName(m_State).c_str(), getStateName(in_state).c_str());

	m_State = in_state;

}
//EMI状態設定
void CMainLoop::SetEMIState(EMI_STATE in_state){

	if (m_ImgState == in_state){
		return;
	}

	SIMLOG_NORM("[CMain Img STATE]   %s ->  %s", getEMIStateName(m_ImgState).c_str(), getEMIStateName(in_state).c_str());

	m_ImgState = in_state;

}

//トリガファイル処理
void CMainLoop::CheckTrigFile(){

	static int s_loopCnt = 0;		//10回に1回しか処理しない目的
	s_loopCnt++;
	s_loopCnt = s_loopCnt % 10;
	if (s_loopCnt  != 0){
		return;
	}

	if (!loadTriggerFile()){
		return;
	}

	if (!m_bSE1Connect || !m_bSE2Connect){
		printf("[MAIN] SE1 or SE2 is not connected!!!\n");
		SIMLOG_NORM("[MAIN] SE1 or SE2 is not connected!!!");
		makeResultFile( MCET_SE_CONNECT_ERR, "SE Connection Error");
		return;
	}

	m_GetImageNum = 0;			//最初は画像取得枚数は0枚とする
	m_BothOffsetGet = false;	//オフセット取得はfalseとする

	//画像読取開始
	startImageGet();

}
//トリガファイルロード処理
bool CMainLoop::loadTriggerFile(){

	FILE* fp;

	//ファイルオープン
	if ((fp = fopen(TRIGGER_FILE_PATH, "r")) == NULL) {
		return false;
	}

	SIMLOG_NORM("[MAIN] CheckTrigFile() %s opened.",(char*)TRIGGER_FILE_PATH);

	char buf[1024];
	int iTemp;
	int procType = 321;			//処理種別
	char folderName[256];		//ファイル名1
	char fileName[256];			//ファイル名2
	memset(buf,0,sizeof(buf));
	memset(folderName,0,sizeof(folderName));
	memset(fileName,0,sizeof(fileName));
	unsigned long lineNum = 0;				//読み込み行数
	bool bEndBracket;

	while( fgets(buf, 1024, fp)){
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
		switch(lineNum){
		case 0:				//画像種別
			sscanf(buf,"%d", &procType);
			switch(procType){
			case 0:
				m_ImgGetParam.eImgType = EIT_OFFSET;	//オフセット取得
				break;
			case 1:
				m_ImgGetParam.eImgType = EIT_EXPOSURE;	//照射画像取得
				break;
			case 2:
				m_ImgGetParam.eImgType = EIT_BOTH;		//両方取得
				break;
			default:
				m_ImgGetParam.eImgType = EIT_EXPOSURE;
				break;
			}
			break;
		case 1:				//画像フォルダ名
			sscanf(buf,"%s", folderName);
			break;
		case 2:				//画像ファイル名
			sscanf(buf,"%s", fileName);
			break;
		case 3:				//画像取得枚数
			sscanf(buf,"%d", &m_ImgGetParam.imageNum);
			break;
		case 4:				//回転ステージ使用？
			sscanf(buf,"%d", &iTemp);
			m_ImgGetParam.rotStage = (iTemp == 1) ? true : false;
			break;
		case 5:				//回転ステージ回転タイプ
			sscanf(buf,"%d", &m_ImgGetParam.rotType);
			break;
		case 6:				//ラズパイ使用
			sscanf(buf,"%d", &iTemp);
			m_ImgGetParam.rasUse = (iTemp == 1) ? true : false;
			break;
		}
		lineNum++;
	}

	if (lineNum < 7){
		SIMLOG_NORM("[MAIN] ---- ImgGetTrigger.txt load Error!! lineNum:%d",lineNum);
		makeResultFile( MCET_TRIG_PARAM_ERR, "TriggerFile Parameter Error");
		return false;
	}

	m_ImgGetParam.imageFolder = folderName;
	m_ImgGetParam.imageFile = fileName;

	fclose(fp);
	remove(TRIGGER_FILE_PATH);		//トリガファイル削除

	SIMLOG_NORM("[MAIN] ------------ ImgGetTrigger.txt detected -------------");
	if (m_ImgGetParam.eImgType == EIT_OFFSET){
		SIMLOG_NORM("[MAIN] Offset Image");
	} else if (m_ImgGetParam.eImgType == EIT_BOTH){
		SIMLOG_NORM("[MAIN] Both Image");
	} else {
		SIMLOG_NORM("[MAIN] Exposure Image");
	}
	SIMLOG_NORM("[MAIN] %s %s Num:%d ",m_ImgGetParam.imageFolder.c_str(), m_ImgGetParam.imageFile.c_str(), m_ImgGetParam.imageNum);
	if (m_ImgGetParam.rotStage){
		SIMLOG_NORM("[MAIN] Rotate Stage Use type:%d", m_ImgGetParam.rotType);
	} else {
		SIMLOG_NORM("[MAIN] Rotater Stage Not Use");
	}
	if (m_ImgGetParam.rasUse){
		SIMLOG_NORM("[MAIN] RaspPI Use");
	} else {
		SIMLOG_NORM("[MAIN] RaspPI Not Use");
	}

	return true;
}
//ログ保存トリガファイル処理
void CMainLoop::CheckLogSaveTrigFile(){

	static int s_loopCnt = 0;		//10回に1回しか処理しない目的
	s_loopCnt++;
	s_loopCnt = s_loopCnt % 10;
	if (s_loopCnt  != 0){
		return;
	}

	FILE* fp;

	//ファイルオープン
	if ((fp = fopen(LOGTRIG_FILE_PATH, "r")) == NULL) {
		return;
	}

	SIMLOG_NORM("[MAIN] CheckLogTrigFile() %s opened.",(char*)TRIGGER_FILE_PATH);

	char buf[1024];
	int iTemp;
	int procType = 321;			//処理種別
	int angleType = 0;			//角度種別
	unsigned long lineNum = 0;				//読み込み行数

	while( fgets(buf, 1024, fp)){
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
		switch(lineNum){
		case 0:				//処理種別取得
			sscanf(buf,"%d", &procType);
			break;
		case 1:
			if (procType == 1){	//回転処理
				sscanf(buf,"%d", &angleType);
			}
			break;
		}
		lineNum++;
	}

	switch(procType){
	case 0:	//ログ保存
		LOG_UPDATE();
		break;
	case 1:	//回転ステージ移動
		m_pRotStage->RotateWithAngle(angleType);
		break;
	}


	fclose(fp);
	remove(LOGTRIG_FILE_PATH);		//トリガファイル削除

}
//画像取得開始
void CMainLoop::startImageGet(){

	SIMLOG_NORM("[MAIN] ==== Start Reading %d / %d ====", m_GetImageNum, m_ImgGetParam.imageNum);
	printf("====================================\n");
	printf("==== Start Reading %d / %d ====\n", m_GetImageNum, m_ImgGetParam.imageNum);
	printf("====================================\n");

	char	strImgNum[10] = "";
	sprintf(strImgNum,"%03d", (m_GetImageNum+1));

	//待ち合わせフラグ設定
	m_bSEBothWait[0] = false;
	m_bSEBothWait[1] = false;

	string	ImgFilePathBase = m_ImgGetParam.imageFolder + "/" + m_ImgGetParam.imageFile;
	string	ImgFilePathL = ImgFilePathBase;
	string	ImgFilePathR = ImgFilePathBase;
	if (m_ImgGetParam.eImgType == EIT_BOTH && m_BothOffsetGet == false){		//両方取得対応
		ImgFilePathL += "_Off";
		ImgFilePathR += "_Off";
	}
	ImgFilePathL = ImgFilePathL + "_L" + strImgNum;
	ImgFilePathR = ImgFilePathR + "_R" + strImgNum;

	printf("[MAIN] ==== SE Main:%x SE Sub:%x\n",m_pSE_Main, m_pSE_Sub);

	if (m_ImgGetParam.eImgType == EIT_OFFSET || (m_ImgGetParam.eImgType == EIT_BOTH && m_BothOffsetGet == false)){
		//オフセット画像取得
		SetMainState(EMLS_OFFSET_IMG);
		SetEMIState(EMIS_PREP);
		m_pSE_Main->ReadPrepare(0, ImgFilePathL);
		m_pSE_Sub->ReadPrepare(0, ImgFilePathR);
	}	else {
		//通常画像取得
		SetMainState(EMLS_EXP_IMG);
		SetEMIState(EMIS_PREP);
		m_pSE_Main->ReadPrepare(1, ImgFilePathL);
		m_pSE_Sub->ReadPrepare(1, ImgFilePathR);
	}


}

//SE読取準備完了受付処理
void CMainLoop::execUpdateSE_PrepComp(int SENo){

	m_bSEBothWait[SENo-1] = true;

	if (isBothSE_OK()){
		SIMLOG_NORM("[MAIN]  ---------------------------------------------------------");
		SIMLOG_NORM("[MAIN]  ---VVVVVVVVV   SE1 & SE2 Preparation is OK   VVVVVVVVV---");
		SIMLOG_NORM("[MAIN]  ---------------------------------------------------------");
		printf("[MAIN]  ---------------------------------------------------------\n");
		printf("[MAIN]  ---VVVVVVVVV   SE1 & SE2 Preparation is OK   VVVVVVVVV---\n");
		printf("[MAIN]  ---------------------------------------------------------\n");
	}

	if (m_State == EMLS_OFFSET_IMG && m_ImgState == EMIS_PREP){
		if (isBothSE_OK()){
			SetEMIState(EMIS_SW1_ON);
			//SW1 Onを通知する
			if (m_pSE_Main != NULL){
				m_pSE_Main->SetShotSW(SE_SW1,true);			//サブSEには送る必要なし
			}
			//タイマー設定
			this->StopTimer();
			//this->SetTimer(2200, false);
			this->SetTimer(1000, false);
			this->StartTimer();
		}
	} else if (m_State == EMLS_EXP_IMG && m_ImgState == EMIS_PREP){
		//上位にReadyを通知することが可能であれば行う。	TODO
		if (isBothSE_OK()){
			if (m_bRasBoxUse){
				m_pRasBox->ReqExposureStart(true, true, true, 10);
			}
		}
	}

}
//SE ShotGoOn受付処理
void CMainLoop::execUpdateSE_ShotGoOn(){

	if (m_State == EMLS_OFFSET_IMG && m_ImgState == EMIS_SW2_ON){
		SetEMIState(EMIS_SHOTGO);
	} else if (m_State == EMLS_EXP_IMG && m_ImgState == EMIS_SW2_ON){	//照射モード時
		SetEMIState(EMIS_SHOTGO);
		m_pMP_Ctrl->SetShotGo(true);
	}

}
//SE ShotGoOff受付処理
void CMainLoop::execUpdateSE_ShotGoOff(){

	//読取後後処理済みフラグのクリア
	m_bSEBothWait[0] = false;
	m_bSEBothWait[1] = false;

	if (m_State == EMLS_OFFSET_IMG && m_ImgState == EMIS_SHOTGO){
		//SW2 Offを通知する
		if (m_pSE_Main != NULL){
			m_pSE_Main->SetShotSW(SE_SW2,false);			//サブSEには送る必要なし
		}
	} else if (m_State == EMLS_EXP_IMG && m_ImgState == EMIS_SHOTGO){	//照射モード時
		m_pMP_Ctrl->SetShotGo(false);
	}

	SetEMIState(EMIS_DATA_RECV);
	m_pSE_Main->ReadImg();
	m_pSE_Sub->ReadImg();

}
//SE データ受信完了処理
void CMainLoop::execUpdateSE_DataComp(int SENo){

	if ((m_ImgState == EMIS_SHOTGO || m_ImgState == EMIS_DATA_RECV)){
		SetEMIState(EMIS_SW1_OFF);
		//SW1 Offを通知する
		if (m_pSE_Main != NULL  && m_State == EMLS_OFFSET_IMG){
			m_pSE_Main->SetShotSW(SE_SW1,false);			//サブSEには送る必要なし
		}
	}

}
//SE 画像データ取得後処理完了処理
void CMainLoop::execUpdateSE_AfterComp(int SENo){

	m_bSEBothWait[SENo-1] = true;

	//ここで1画像取得のシーケンス完了
	//取得枚数が到達していなければ、画像をさらに取得していく
	if (m_ImgState == EMIS_SW1_OFF && isBothSE_OK()){

		if (m_ImgGetParam.eImgType == EIT_BOTH && m_BothOffsetGet == false){
			m_BothOffsetGet = true;
		} else {
			m_BothOffsetGet = false;
			m_GetImageNum++;
		}

		if (m_ImgGetParam.imageNum > m_GetImageNum){
			startImageGet();
		} else {
			//全データ取得済み
			SIMLOG_NORM("[MAIN] =============================");
			SIMLOG_NORM("[MAIN] ===== ALL SEQ COMPLETE  =====");
			SIMLOG_NORM("[MAIN] =============================");
			printf("[MAIN] =============================\n");
			printf("[MAIN] ===== ALL SEQ COMPLETE  =====\n");
			printf("[MAIN] =============================\n");
			SetMainState(EMLS_IDLE);
			//TODO			上位に通知する
			makeResultFile( MCET_SUCCEED, "Images are acquired normally.");
		}
	}

}
//MP SW1_ON/OFF受付処理
void CMainLoop::execUpdateMP_SW1(bool bOn){

	if (m_State != EMLS_EXP_IMG){		//照射撮影時以外は無視する
		return;
	}

	if (m_ImgState == EMIS_PREP){
		if (bOn){
			SetEMIState(EMIS_SW1_ON);
			if (isBothSE_OK() && m_ImgState == EMIS_SW1_ON){
				if (m_pSE_Main != NULL){
					m_pSE_Main->SetShotSW(SE_SW1,true);			//サブSEには送る必要なし
				}
			}
		}
	}

}

//MP SW2_ON/OFF受付処理
void CMainLoop::execUpdateMP_SW2(bool bOn){

	if (m_State != EMLS_EXP_IMG){		//照射撮影時以外は無視する
		return;
	}

	if (bOn && m_ImgState == EMIS_SW1_ON){
		SetEMIState(EMIS_SW2_ON);
		//SW2 Onを通知する
		if (m_pSE_Main != NULL){
			m_pSE_Main->SetShotSW(SE_SW2,true);			//サブ側には送る必要なし
		}
	}

}

//MP 接続受付処理
void CMainLoop::execUpdateMP_Connect(){

}

//MP 接続断受付処理
void CMainLoop::execUpdateMP_Disconnect(){

}
//SE両方とも準備OKか
bool CMainLoop::isBothSE_OK(){

	if (m_bSEBothWait[0] == true && m_bSEBothWait[1] == true){
		return true;
	}

	return false;
}
//状態変数から状態名を返す
string CMainLoop::getStateName(EML_STATE in_state){

	std::vector<std::string> strList;

	//挿入順序に気をつける. Enum変更でこちらも変更すること
	strList.push_back("EMLS_DISCONNECT");
	strList.push_back("EMLS_IDLE");
	strList.push_back("EMLS_OFFSET_IMG");
	strList.push_back("EMLS_EXP_IMG");

	return strList[in_state];
}

//状態変数から状態名を返す
string CMainLoop::getEMIStateName(EMI_STATE in_state){

	std::vector<std::string> strList;

	//挿入順序に気をつける. Enum変更でこちらも変更すること
	strList.push_back("EMIS_PREP");
	strList.push_back("EMIS_SW1_ON");
	strList.push_back("EMIS_SW2_ON");
	strList.push_back("EMIS_SHOTGO");
	strList.push_back("EMIS_DATA_RECV");
	strList.push_back("EMIS_SW1_OFF");

	return strList[in_state];
}
//結果ファイル作成
void CMainLoop::makeResultFile(int errCode, string description){

	FILE*	fp;

	//キャリブ用トリガファイルの検出処理
	if ((fp = fopen(RESULT_FILE_PATH, "w")) == NULL){
		printf(" %s file open error!!!\n", RESULT_FILE_PATH);
		return;
	}

	fprintf(fp, "#Image Acquisition Result\n");
	fprintf(fp, "%d\n", errCode);
	fprintf(fp, "%s\n", description.c_str());

	fclose(fp);
}
