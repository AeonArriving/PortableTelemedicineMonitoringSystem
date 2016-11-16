#pragma once

#include <winsock.h>
//#pragma comment(lib,"winsock.lib")
class CTCPCustom_CE;
class CTCPServer_CE;

//����ͻ������ӽ����¼�
typedef void (CALLBACK* ONCLIENTCONNECT)(void* pOwner,CTCPCustom_CE*);
//����ͻ���SOCKET�ر��¼�
typedef void (CALLBACK* ONCLIENTCLOSE)(void* pOwner,CTCPCustom_CE*);
//����ͻ��˵������ݽ����¼�
typedef void (CALLBACK* ONCLIENTREAD)(void* pOwner,CTCPCustom_CE*,const char * buf,DWORD dwBufLen );
//����ͻ���Socket�����¼�
typedef void (CALLBACK* ONCLIENTERROR)(void* pOwner,CTCPCustom_CE*,int nErrorCode);
//�����������Socket�����¼�
typedef void (CALLBACK* ONSERVERERROR)(void* pOwner,CTCPServer_CE*,int nErrorCode);

class CTCPServer_CE
{
public:
	CTCPServer_CE(void);
	~CTCPServer_CE(void);
public:
  int m_LocalPort; //���÷���˿ں�
  void * m_pOwner;   //��������
private:
	SOCKET m_ServerSocket;     //TCP�������socket
	HANDLE m_serverThreadHandle;  //ͨѶ�߳̾��
	HANDLE m_exitThreadEvent;  //ͨѶ�߳��˳��¼����
public:  //�����¼�
	//�ͻ������ӽ����¼����ص�����
	ONCLIENTCONNECT    OnClientConnect;
	//�ͻ������ӶϿ��¼����ص�����
	ONCLIENTCLOSE OnClientClose;
	//�ͻ��˽��������¼����ص�����
	ONCLIENTREAD       OnClientRead;
	//�ͻ��˷��������¼����ص�����
	ONCLIENTERROR      OnClientError;
	//�������˷��������¼�,�ص�����
	ONSERVERERROR	   OnServerError;
private:
	//TCP�����������̺߳���
	static DWORD WINAPI SocketThreadFunc(PVOID lparam);
public:
	//ɾ���ͻ��˶���
	void RemoteClient(CTCPCustom_CE *pClient /*�ͻ��˶���*/);
public:
	//��TCP����
	int Open();
public:
	//�ر�TCP����
	int Close();
public:
	//��������
	BOOL SendData(CTCPCustom_CE* pCustomCE, const char * buf , DWORD dwBufLen);
};
