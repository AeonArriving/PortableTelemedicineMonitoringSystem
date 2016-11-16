#pragma once

#include "winsock.h"
#include "TCPServer_CE.h"
//#pragma comment(lib,"winsock.lib")

class CTCPCustom_CE
{
public:
	CTCPCustom_CE(void);
	~CTCPCustom_CE(void);
private:
    //ͨѶ�̺߳���
	static DWORD WINAPI SocketThreadFunc(PVOID lparam);
public:
	//��socket������ͨѶ�߳�
	BOOL Open(CTCPServer_CE *pTCPServer);
	//�ر�socket���ر��̣߳��ͷ�Socket��Դ
	BOOL Close();
public:
	//��������
	BOOL SendData(const char * buf , DWORD dwBufLen);
public:
	CTCPServer_CE * m_pTCPServer_CE; //����TCP����˼���Socket
	CString m_RemoteHost; //Զ������IP��ַ
	DWORD m_RemotePort; //Զ�������˿ں�
	SOCKET m_socket;      //ͨѶSocket���
private:
	HANDLE m_exitThreadEvent;  //ͨѶ�߳��˳��¼����
	HANDLE m_tcpThreadHandle;  //ͨѶ�߳̾��
};
