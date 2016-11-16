/******************************************
// DSPDemoDlg.h
//����
******************************************/
#pragma once
#include "C2DGraph.h"
#include "Serial.h"
#include "afxcmn.h"
//ÿ������ڵ��������ɵ����������
#define ECG_SIZE   400
////////////////////////////////////////
//���������ݴ����ݽṹ
typedef struct ECGDataNode
{
	int ECG[ECG_SIZE];
	struct ECGDataNode *next; 
}ECGDataNode;
///////////////////////////////////////
ECGDataNode  *InitECGDataNode();

// CDSPDemoDlg �Ի���
class CDSPDemoDlg : public CDialog
{
// ����
public:
	CDSPDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DSPDEMO_DIALOG };
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
	// ���ڽ������ݴ�����
	afx_msg LONG OnRecvSerialData(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()	
/*�ź���ʾ*/
private:
	 CEdit *pEdtRecvMsg;
	 C2DGraph m_2DGraph;  //��ά���߶���
	 int m_pointCount;    //���ߵ����
	 int ecgpoint; 
	 int flag;
	/////////////////////////////////////////////////////
	CSerial* pCSerial;
	static void CALLBACK OnSerialRead(DWORD UserParam,
									  char* buf,
									  DWORD bufLen);
	static DWORD WINAPI ThreadProc(LPVOID lpParameter);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);//��ʱ����Ϣ������
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnEnChangeEdtRecv();
};
