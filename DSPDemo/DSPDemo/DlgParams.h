#pragma once


// CDlgParams �Ի���

class CDlgParams : public CDialog
{
	DECLARE_DYNAMIC(CDlgParams)

public:
	CDlgParams(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgParams();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	//
	UINT m_portNo;		//���ں�
	UINT m_baud;		//������
	UINT m_parity;		//��żУ��
	UINT m_databits;	//����λ
	UINT m_stopbits;	//ֹͣλ
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
