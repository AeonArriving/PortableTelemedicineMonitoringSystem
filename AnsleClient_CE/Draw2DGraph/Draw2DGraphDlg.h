// Draw2DGraphDlg.h : ͷ�ļ�
//
#pragma once
#include "CESerials.h"
#include "CESeries.h"
///////////////////////////////
#include "TCPClient_CE.h"
#include "GPRSCONTDlg.h"
//////////////////////////////
#include "2DGraph.h"
#include "LedCtrl.h"
#include "GPRS.h"
/////////////////////////////
#include "afxcmn.h"
#include "afxwin.h"
//////////////////////////////////////////////////////
//��Ϣ����
#define WM_RECV_SERIAL_DATA			WM_USER + 101
#define WM_COMM_MESSAGE				WM_USER + 102
//����TCP �ͻ��˽��յ�������Ϣ
#define WM_RECV_TCP_DATA			WM_USER + 103
//����TCP�ͻ������ӶϿ���Ϣ
#define WM_TCP_CLIENT_DISCONNECT	WM_USER + 104
//////////////////////////////////////////////////////
//ȫ�ֱ���
static C2DGraph m_2DGraph;  //��ά���߶���
static CTCPClient_CE m_tcpClient;//����CTCPClient_CE����
static CLedCtrl m_led;
static CLedCtrl m_led1;
static CLedCtrl m_led2;
static CLedCtrl m_led3;
static int m_pointCount = 0;    //���ߵ����
static int ecgpoint = 0;
//////////////////////////////////////////////////////
// CDraw2DGraphDlg �Ի���
class CDraw2DGraphDlg : public CDialog
{
// ����
public:
	CDraw2DGraphDlg(CWnd* pParent = NULL);	// ��׼���캯��
	CDraw2DGraphDlg *pDlg;

// �Ի�������
	enum { IDD = IDD_DRAW2DGRAPH_DIALOG };


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
	// ���ڽ������ݴ�����
	afx_msg LONG OnRecvSerialData(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnCommMessage(WPARAM pbuf,LPARAM lbufLen);
	//TCP�������ݴ�����
	afx_msg LONG OnRecvTCPData(WPARAM wParam,LPARAM lParam);
	//�ͻ������ӶϿ���Ϣ����
	afx_msg LONG OnClientDisconnect(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	//���崮�ڽ������ݺ�������
	static void CALLBACK OnSerialRead(void * pOwner,BYTE* buf,DWORD bufLen);
	//�������ݽ����¼�������
	static void CALLBACK OnRead(CWnd* pWnd,const char * buf,int len );
	//���ڶ�ȡ�¼�
	static void OnSeriesRead(CWnd* pWnd,BYTE* buf,int bufLen);

private:
	//TCP���ӶϿ��¼�������
	static void  CALLBACK OnDisConnect(void* pOwner);
	//�������ݽ����¼�������
	static void  CALLBACK OnTcpRead(void* pOwner,const char * buf,DWORD dwBufLen );
	//Socket�����¼�������
	static void CALLBACK OnTcpError(void* pOwner,int nErrorCode);

/*�ĵ��ź���ʾ*/
private:
//	C2DGraph m_2DGraph;  //��ά���߶���
//	int m_pointCount;    //���ߵ����
	int number;
	
private:
	//����CTCPClient_CE����
//	CTCPClient_CE m_tcpClient;
	CString	m_remotehost;
	int		m_remoteport;
public:
//    int ecgpoint;
	int ecgsend;
	GPRSCONTDlg *remotemode;
	BOOL ISOpenRemoteMode;
	int IsClicked;

protected:
	CGPRS m_GPRS;

/*GPRS����*/
protected:
	CCESerials *m_pSerial1;//���ڶ���ָ��
	CCESeries *m_pSerial2;//���ڶ���ָ��

public:
	//////////////////////////////////////
	//��������
	BOOL Spo2;
	BOOL Pulse;

	//////////////////////////////////////
private:
	//////////////////////////////////////
	//�̺߳�������
//	static DWORD WINAPI FilterThread(PVOID pArg);
	static DWORD WINAPI ECGThread(PVOID pArg);
	static DWORD WINAPI HBPThread(PVOID pArg);
//	static DWORD WINAPI LBPThread(PVOID pArg);
//	static DWORD WINAPI SPO2Thread(PVOID pArg);
//	static DWORD WINAPI PulseThread(PVOID pArg);
	static DWORD WINAPI SendThread(PVOID pArg);

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);//��ʱ����Ϣ������
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedRemotemode();
	afx_msg void OnBnClickedEcg();
	afx_msg void OnBnClickedBoolpressure();
	afx_msg void OnBnClickedSpo2();
	afx_msg void OnBnClickedPulse();
};
