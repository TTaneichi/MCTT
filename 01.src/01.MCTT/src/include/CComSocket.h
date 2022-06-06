/*
 * CComSocket.h
 *
 *  Created on: 2021/05/07
 *      Author: 10097663
 */

#ifndef CCOMSOCKET_H_
#define CCOMSOCKET_H_


#include <winsock2.h>
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>
#include <stdlib.h>
#include <ws2tcpip.h>
#include "Common.h"

#include <string>
#include <vector>
using namespace std;


#define SOCKET_TYPE_ETHER	0		//ソケットタイプether
#define CONNECT_TYPE_SERV	0		//サーバ接続設定
#define CONNECT_TYPE_CLIENT	1		//クライアント接続設定

#define MAX_SOCKET			3		//最大クライアントソケット数

class CComSocket {
public:
	CComSocket();
	CComSocket(int in_ServerClient);
	virtual ~CComSocket();

	//サーバ/クライアント設定
	void SetServerClient(int in_ServerClient);

	//接続開始
	int Setup(unsigned short portNum, string ipAddr);

	//接続処理
	EConnectState Connect(int no = 0);

	//接続状態を取得する
	EConnectState GetConnectState(int no = 0);

	//接続確認
	EConnectState CheckConnecting(int no = 0);

	//接続断処理
	void Disconnect(int no = 0);

	//送信
	int SendData( const char* in_pSendData, unsigned long in_Size, int no = 0);

	//受信
	int RecvData( char* out_pRecvData, unsigned long in_MaxSize, int no = 0);

	//接続先IPアドレスの取得
	unsigned long GetDstIpAddr(int no = 0);

private:
	//サーバ接続準備処理
	int setupServer();

	//クライアント接続準備処理
	int setupClient();

	//サーバ接続処理
	int connectingServer(int no = 0);

	//クライアント接続処理
	int connectingClient();


	EConnectState	m_ConnectState[MAX_SOCKET];

	struct sockaddr_in m_srcAddr;	//送信元ソケット構造体
	struct sockaddr_in m_dstAddr[MAX_SOCKET];	//送信先ソケット構造体
	int m_SrcSock;					//ソース用ソケット
	int m_DstSock[MAX_SOCKET];		//送り先用ソケット
	int m_SocketNum;				//送り先ソケット数
	fd_set	m_cmdfds;				//select用
	int m_NewCmdDstSock;			//コマンド送り先用ソケット(再接続)
	int m_ServerClient;				//サーバorクライアント(0:サーバ/1:クライアント)
	string m_IpAddr;				//接続先IPアドレス(クライアント用)
	unsigned short	m_DistPort;		//接続先ポート(サーバ/クライアント兼用)

};

#endif /* CCOMSOCKET_H_ */
