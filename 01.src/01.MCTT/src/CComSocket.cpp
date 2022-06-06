/*
 * CComSocket.cpp
 *
 *  Created on: 2021/05/07
 *      Author: 10097663
 */

#include "CComSocket.h"
#include "CUnitLog.h"
#include "CDataControl.h"

CComSocket::CComSocket(){

	for(int i=0;i<MAX_SOCKET;i++){
		m_DstSock[i] = -1;
		m_ConnectState[i] = ECS_DISCONNECT;
	}
	m_SrcSock = -1;
	m_SocketNum = 0;
	m_NewCmdDstSock = -1;
	m_ServerClient = 0;
	m_IpAddr = "";
	m_DistPort = 65000;
	FD_ZERO(&m_cmdfds);
}
CComSocket::CComSocket(int in_ServerClient) {

	for(int i=0;i<MAX_SOCKET;i++){
		m_DstSock[i] = -1;
		m_ConnectState[i] = ECS_DISCONNECT;
	}
	m_SrcSock = -1;
	m_SocketNum = 0;
	m_NewCmdDstSock = -1;
	m_ServerClient = in_ServerClient;
	m_IpAddr = "";
	m_DistPort = 65000;
	FD_ZERO(&m_cmdfds);

}
//サーバ/クライアント設定
void CComSocket::SetServerClient(int in_ServerClient){

	m_ServerClient = in_ServerClient;

}
CComSocket::~CComSocket() {
	// TODO Auto-generated destructor stub
}

//接続開始
int CComSocket::Setup(unsigned short portNum, string ipAddr){

	m_DistPort = portNum;
	m_IpAddr = ipAddr;

	switch( m_ServerClient){
	case CONNECT_TYPE_SERV:
		return setupServer();
		break;
	case CONNECT_TYPE_CLIENT:
		return setupClient();
		break;
	default:
		printf("Setup() ERROR!!!\n");
		return -1;
		break;
	}

	return 0;
}

//接続処理
EConnectState CComSocket::Connect(int no){


	if (MAX_SOCKET <= no){
		return ECS_DISCONNECT;
	}

	return m_ConnectState[no];
}

//接続状態を取得する
EConnectState CComSocket::GetConnectState(int no){

	if (MAX_SOCKET <= no){
		return ECS_DISCONNECT;
	}

	return m_ConnectState[no];
}
//接続確認
EConnectState CComSocket::CheckConnecting(int no){

	if (MAX_SOCKET <= no){
		return ECS_DISCONNECT;
	}

	if (m_ConnectState[no] == ECS_CONNECTED){
		return ECS_CONNECTED;
	} else if (m_ConnectState[no] == ECS_WAIT_ACCEPT){
		//接続待ちであれば、接続したかの確認を行う
		connectingServer(no);
	} else if (m_ConnectState[no] == ECS_CLIENT_SETUP){
		//クライアントで未接続であれば、接続処理を行う
		connectingClient();
	} else if (m_ConnectState[no] == ECS_DISCONNECT){
		switch( m_ServerClient){
		case CONNECT_TYPE_SERV:
			// Do nothing
			break;
		case CONNECT_TYPE_CLIENT:
			// Do nothing
			break;
		}
	}

	return m_ConnectState[no];

}
//接続断処理
void CComSocket::Disconnect(int no){

	if (m_ConnectState[no] != ECS_DISCONNECT){
		SIMLOG_NORM("[SOCKET] -----CComSocket::Disconnect()------");
		m_ConnectState[no] = ECS_DISCONNECT;
		closesocket(m_DstSock[no]);
		closesocket(m_SrcSock);
		if (no == 0){
			m_SocketNum = 0;
		}
	}

}
/* -------------------------------------------------------------------
* @brief    送信
* @param	in_pSendData	送信データ
* @param	in_Size			送信データサイズ
* @retval   送信バイト数		負の場合はエラー
* @date     2021/05/18	MS T.Taneichi
 --------------------------------------------------------------------- */
int CComSocket::SendData( const char* in_pSendData, unsigned long in_Size, int no){

	if (m_ConnectState[no] != ECS_CONNECTED){	//未接続時は何もしない
		return -1;
	}

	int ret = 0;

	ret = send(m_DstSock[no], in_pSendData, (int)in_Size, 0);


	return ret;
}
/* -------------------------------------------------------------------
* @brief    受信
* @param	in_pSendData	受信データ
* @param	in_Size			受信最大データサイズ
* @retval   受信バイト数
* @date     2021/05/18	MS T.Taneichi
 --------------------------------------------------------------------- */
int CComSocket::RecvData( char* out_pRecvData, unsigned long in_MaxSize, int no){

	if (m_ConnectState[no] != ECS_CONNECTED){	//未接続時は何もしない
		return -1;
	}

	int ret = 0;

	ret = recv(m_DstSock[no], out_pRecvData, in_MaxSize, 0);

	return ret;
}
/* -------------------------------------------------------------------
* @brief    接続先IPアドレスの取得
* @param	なし
* @retval   接続先IPアドレス(4byte)
* @date     2021/05/18	MS T.Taneichi
 --------------------------------------------------------------------- */
unsigned long CComSocket::GetDstIpAddr(int no){

	if (m_ConnectState[no] == ECS_CONNECTED){
		return m_dstAddr[no].sin_addr.S_un.S_addr;
	}

	return 0;		//未接続の場合は0を返す
}
/* -------------------------------------------------------------------
* @brief    サーバ接続準備処理
* @param	なし
* @retval 	ソケット番号	0スタートでインクリメントされる
* @date     2021/05/18	MS T.Taneichi
 --------------------------------------------------------------------- */
int CComSocket::setupServer(){

	m_ConnectState[m_SocketNum] = ECS_DISCONNECT;

	//Windows用ネットワーク
	if (CDataControl::GetInstance()->m_bWSAInitFlag == false){
		WSADATA wsaData;	//Ether用デバイス
		WSAStartup(MAKEWORD(2,0), &wsaData);
		CDataControl::GetInstance()->m_bWSAInitFlag = true;
	}

	if (m_SocketNum == 0){			//初回のみ行う

		memset(&m_srcAddr, 0, sizeof(m_srcAddr));
		m_srcAddr.sin_family = AF_INET;
		m_srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		m_srcAddr.sin_port = htons(m_DistPort);				//ポート番号

		/* ソケットの生成 */
		m_SrcSock = (int)socket(AF_INET, SOCK_STREAM, 0);
		if (m_SrcSock == INVALID_SOCKET){
			SIMLOG_NORM("[SOCKET] socket() ERROR!!!");
			printf("[SOCKET] socket() ERROR!!!\n");
			return -1;
		}

		//	/*　ソケットオプションで同じポートを使用可能とする */
		//	BOOL yes = 1;
		//	setsockopt(m_CmdSrcSock, SOL_SOCKET, SO_REUSEADDR, (const char* )&yes, sizeof(yes));

		/* ソケットのバインド */
		if (bind(m_SrcSock, (struct sockaddr *) &m_srcAddr, sizeof(m_srcAddr)) == SOCKET_ERROR){
			SIMLOG_NORM("[SOCKET] bind() ERROR!!! %d", WSAGetLastError());
			return -1;
		}

		/* 接続の許可 */
		if (listen(m_SrcSock, MAX_SOCKET) == SOCKET_ERROR){
			SIMLOG_NORM("[SOCKET] listen() ERROR!!!");
			return -1;
		}
		u_long val=1;

#ifdef __MINGW32__
		ioctlsocket(m_SrcSock, FIONBIO, &val);
#endif

		//accept待ち処理
		FD_ZERO(&m_cmdfds);
		FD_SET(m_SrcSock, &m_cmdfds);
		printf("[SOCKET] Start Accept()\n");
		SIMLOG_NORM("[SOCKET] Start Accept");
	}
	m_ConnectState[m_SocketNum] = ECS_WAIT_ACCEPT;

	m_SocketNum++;

	return (m_SocketNum-1);

}

//クライアント接続準備処理
int CComSocket::setupClient(){

	struct sockaddr_in dstAddr;
	struct hostent *host;

	//Windows用ネットワーク
	if (CDataControl::GetInstance()->m_bWSAInitFlag == false){
		WSADATA wsaData;	//Ether用デバイス
		WSAStartup(2, &wsaData);
		CDataControl::GetInstance()->m_bWSAInitFlag = true;
	}

	//sockaddr_in 構造体のセット
	memset((void*)&m_srcAddr,0x0,sizeof(m_srcAddr));
	m_srcAddr.sin_family = AF_INET;

	m_srcAddr.sin_addr.S_un.S_addr = inet_addr(m_IpAddr.c_str());
	host = gethostbyname(m_IpAddr.c_str());

	m_srcAddr.sin_addr.S_un.S_addr = *(unsigned int *)host->h_addr_list[0];
	m_srcAddr.sin_port = htons(m_DistPort);

	//ソケット生成
	m_DstSock[0] = (int)socket(AF_INET, SOCK_STREAM, 0);

	m_ConnectState[0] = ECS_CLIENT_SETUP;

	//接続
	return connectingClient();

}
/* -------------------------------------------------------------------
* @brief    サーバ接続処理
* @param	なし
* @retval 	結果		RET_OK/それ以外
* @date     2021/05/18	MS T.Taneichi
 --------------------------------------------------------------------- */
int CComSocket::connectingServer(int no){

	if (MAX_SOCKET <= no){
		return RET_NG;
	}

	struct timeval t_val = {0,1000};

	if (m_ConnectState[no] != ECS_WAIT_ACCEPT){
		return RET_NG;
	}

	int select_ret;
	int dstAddrSize;
	fd_set fds;

	//コマンド用ソケット
	//accept待ち処理
	FD_ZERO(&m_cmdfds);
	FD_SET(m_SrcSock, &m_cmdfds);
	memcpy(&fds, &m_cmdfds, sizeof(fd_set));
	select_ret = select(0, &fds, NULL, NULL, &t_val);
	//printf("select_ret:%d no:%d\n",select_ret, no);
	if (select_ret > 0) {
		//accept処理
		dstAddrSize = sizeof(struct sockaddr_in);
		int tmpDstSock;
		m_DstSock[no] = (int)accept(m_SrcSock, (struct sockaddr*)&m_dstAddr[no], &dstAddrSize);
		m_ConnectState[no] = ECS_CONNECTED;
		printf("[SOCKET] Server Connected!     %08x\n",m_dstAddr[no].sin_addr.S_un.S_addr);
		SIMLOG_NORM("[SOCKET] Server Connected!    %08x\n",m_dstAddr[no].sin_addr.S_un.S_addr);
	}

	return RET_OK;
}

//クライアント接続処理
int CComSocket::connectingClient(){

	//接続
	if (connect(m_DstSock[0], (struct sockaddr *)&m_srcAddr, sizeof(m_srcAddr)) < 0){
		//SIMLOG_NORM("connectingClient CONNECT ERR!!!");
		//printf("connectingClient CONNECT ERR!!!\n");
		return RET_NG;
	}

	u_long val=1;
#ifdef __MINGW32__
		ioctlsocket(m_DstSock[0], FIONBIO, &val);
#endif

	SIMLOG_NORM("[SOCKET] connectingClient() CONNECT OK!");
	printf("[SOCKET] connectingClient() CONNECT OK!\n");
	m_ConnectState[0] = ECS_CONNECTED;

	return RET_OK;
}
