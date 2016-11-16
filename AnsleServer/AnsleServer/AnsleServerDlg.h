// AnsleServerDlg.h : ͷ�ļ�
//
#pragma once
#include "2DGraph.h"
#include <stdlib.h>
#include "TCPServer_CE.h"
#include "TCPCustom_CE.h"
#include "LedCtrl.h"
#include "GPRS.h"
#include "afxwin.h"
/////////////////////////////////////
#define ECG_SIZE   500
/////////////////////////////////////
//����ͨ�û�����
typedef struct  _DATA_BUF
{
	DWORD dwBufLen;
	char* sBuf;	
	TCHAR szAddress[MAX_PATH];
}DATA_BUF,*PDATA_BUF;
//�������ݴ洢��
typedef struct ReceiveDataNode
{
	int ECG[ECG_SIZE];
	int SPO2;
	int BP[2];
	int Pulse;
	struct ReceiveDataNode *next;
}ReceiveDataNode;
ReceiveDataNode *InitReceiveDataNode();
//////////////////////////////////////////////////////
//ȫ�ֱ���
static C2DGraph m_2DGraph;  //��ά���߶���
static CLedCtrl m_led;
static CLedCtrl m_led1;
static CLedCtrl m_led2;
static CLedCtrl m_led3;
static int m_pointCount = 0;    //���ߵ����
static int ecg;
// CAnsleServerDlg �Ի���
class CAnsleServerDlg : public CDialog
{
// ����
public:
	CAnsleServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ANSLESERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//TCP�������ݴ�����
	afx_msg LONG OnRecvTCPData(WPARAM wParam,LPARAM lParam);
	//�ͻ������ӶϿ���Ϣ����
	afx_msg LONG OnClientConnect(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	/*�ĵ��ź���ʾ*/
private:
//	C2DGraph m_2DGraph;  //��ά���߶���
//	int m_pointCount;    //���ߵ����
	//���Ը���
//	int ecg;

private:
	//����CTCPServer_CE����
	CTCPServer_CE m_tcpServer;
	CGPRS m_GPRS;
	//�õ����ص�IP��ַ
	CString GetLocalIP();
	void PrintStatusInfo(CString strinfo);
	BOOL IS_GPRSInit;
	BOOL IS_LISTEN;
	CString GPRS_IPAddress;

public:
	//����˿ں�
	DWORD m_localPort;
//	CLedCtrl m_led;
//	CLedCtrl m_led1;
//	CLedCtrl m_led2;
//	CLedCtrl m_led3;

private:
	//�ͻ������ӽ����¼�������
	static void CALLBACK	OnClientConnect(void* pOwner,CTCPCustom_CE *pTcpCustom);
	//�ͻ���SOCKET�ر��¼�������
	static void  CALLBACK OnClientClose(void* pOwner,CTCPCustom_CE*pTcpCustom);
	//���������յ����Կͻ��˵�����
	static  void CALLBACK OnClientRead(void* pOwner,CTCPCustom_CE* pTcpCustom,const char * buf,DWORD dwBufLen );
	//�ͻ���Socket�����¼�������
	static  void CALLBACK OnClientError(void* pOwner,CTCPCustom_CE* pTcpCustom,int nErrorCode);
	//��������Socket�����¼�������
	static void CALLBACK OnServerError(void* pOwner,CTCPServer_CE* pTcpServer_CE,int nErrorCode);

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);//��ʱ����Ϣ������
	afx_msg void OnBnClickedMonitor();
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedSetMsgcentrTelcode();
	afx_msg void OnBnClickedInitsms();
	CString m_status;
};
