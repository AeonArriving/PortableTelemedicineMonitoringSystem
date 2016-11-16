// GPRSCONTDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GPRSCONTDlg.h"

#define BUFFER_LENGTH 32
// GPRSCONTDlg �Ի���

IMPLEMENT_DYNAMIC(GPRSCONTDlg, CDialog)

GPRSCONTDlg::GPRSCONTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GPRSCONTDlg::IDD, pParent)
{
//	m_status = _T("");
	m_remotehost = _T("");
	m_remoteport = 0;
}

GPRSCONTDlg::~GPRSCONTDlg()
{
}

void GPRSCONTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_STATUS, m_status);
	DDX_Text(pDX, IDC_EDIT_REMOTEHOST, m_remotehost);
	DDX_Text(pDX, IDC_EDIT_REMOTEPORT, m_remoteport);
}


BEGIN_MESSAGE_MAP(GPRSCONTDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SET_MSGCENTR_TELCODE, &GPRSCONTDlg::OnBnClickedSetMsgcentrTelcode)
	ON_BN_CLICKED(IDC_SENDMSG, &GPRSCONTDlg::OnBnClickedSendmsg)
	ON_BN_CLICKED(IDC_INITSMS, &GPRSCONTDlg::OnBnClickedInitsms)
END_MESSAGE_MAP()

BOOL GPRSCONTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);				// Set big icon
	SetIcon(m_hIcon, FALSE);			// Set small icon
	CenterWindow(GetDesktopWindow());	// center to the hpc screen
	///////////////////////////////////////////////////////////////////////
	// ��ʼ�� GPRS ģ�鷢�Ͷ���ר��
	SetDlgItemText(IDC_SENDMSG_TELCODE, _T("18392621985"));		
	SetDlgItemText(IDC_EDIT_SENDMSG, _T("I Need Your IP Address"));	
	SetDlgItemText(IDC_MSGCENTR_TELCODE, _T("+8613800290500"));
	m_GPRS.OnGPRSRecv = OnGPRSRecv;	/* ���ûص�����	*/
	//////////////////////////////////////////////////////////////////////
	///��������ר��
	m_remotehost = _T("192.168.16.1");
	m_remoteport = 5000;
	UpdateData(FALSE);

	return TRUE;
}


// GPRSCONTDlg ��Ϣ�������
/*******************************************************************************************
��������: CALLBACK GPRSCONTDlg::OnGPRSRecv
��    ��: GPRS ���ջص�����, ���е绰�����Է��һ�ʱ, ��ִ�иú���
�������: DWORD UserParam: �û��ڵ��� GPRS_Init() ����ʱ����Ĳ���.
		  DWORD Status	 : GPRS ״̬
		  CString strData: ״̬��Ӧ���ַ���, ����е绰����, ����ַ�Ϊ�������	
�������: ��
��    ��: ��
********************************************************************************************/
void CALLBACK GPRSCONTDlg::OnGPRSRecv(DWORD UserParam, DWORD Status, CString strData)
{

}

/*******************************************************************************************
��������: OnTimer
��    ��: ��ʱ��������
********************************************************************************************/
void GPRSCONTDlg::OnTimer(UINT nIDEvent) 
{
	BOOL ret;
	CString strTelCode, strMsg;
	ret = m_GPRS.GPRS_ReadShortMessage(1, &strTelCode, &strMsg);// ��ȡ�� 0 ������ 
	if (ret == TRUE)
	{
		SetDlgItemText(IDC_RECVMSG_TELCODE, strTelCode);// ��ʾ�绰����
		SetDlgItemText(IDC_DISP_RECVMSG, strMsg);//��ʾ��������
		BOOL ret = IsWanted(strMsg,L"IP");
		if(ret == TRUE)
		{
			KillTimer(1);
			CString Ip = GetIpFromSMS(L"IP",L"END",strMsg); /*��ȡ����IP��ַ*/
			m_remotehost = Ip;
			SetDlgItemText(IDC_EDIT_REMOTEHOST, Ip);
			m_GPRS.GPRS_DeleteShortMsg(1);	
			m_GPRS.GPRS_ClosePort();
		}
		else
			m_GPRS.GPRS_DeleteShortMsg(1);/* ɾ������ */
	}
	CDialog::OnTimer(nIDEvent);
}
BOOL GPRSCONTDlg::IsWanted(CString sms,CString cmd)
{
	if(sms.Find(cmd,0) != 0)
	{
		PrintStatusInfo(_T("δ�յ���ȷ����Ӧ���ţ������·����������"));
		return FALSE;
	}
	else 
		return TRUE;
}
CString GPRSCONTDlg::GetIpFromSMS(CString scmd,CString ecmd,CString sms)
{
	CString smsstr;
	int len1,len2;
	len1 = scmd.GetLength();
	len2 = ecmd.GetLength();
	int pos1,pos2;
	pos1 = sms.Find(scmd,0) + len1;
	pos2 = sms.Find(ecmd,0);
	smsstr = sms.Mid(pos1,pos2-pos1);
	return smsstr;
}
void GPRSCONTDlg::OnBnClickedSetMsgcentrTelcode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strCode;

	GetDlgItemText(IDC_MSGCENTR_TELCODE, strCode);

	BOOL ret = m_GPRS.GPRS_SetShortMSGCenterTel(strCode);/* ���ö����к��� */
	if (ret == TRUE)
		PrintStatusInfo(_T("���ö������ĺ���ɹ�."));
	else
		PrintStatusInfo(_T("���ö������ĺ���ʧ��."));
}

void GPRSCONTDlg::OnBnClickedSendmsg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strTelCode, strMsg;

	GetDlgItemText(IDC_SENDMSG_TELCODE, strTelCode);/* ��ȡ���Ͷ��ŵ绰���뼰����*/	
	GetDlgItemText(IDC_EDIT_SENDMSG, strMsg);	

	if ((strTelCode == "") || (strMsg == ""))
	{/* �ж����������Ƿ���ȷ */
		PrintStatusInfo(_T("�绰�����������ݲ���Ϊ��."));
		return;
	}

	BOOL ret = m_GPRS.GPRS_SendShortMessage(strTelCode, strMsg);/* ���Ͷ��� */
	if (ret == TRUE)
		PrintStatusInfo(_T("���ŷ��ͳɹ�."));
	else
		PrintStatusInfo(_T("���ŷ���ʧ��."));
}
void GPRSCONTDlg::OnBnClickedInitsms()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL ret = m_GPRS.GPRS_Init(_T("COM2:"), 115200, (DWORD)this);
	if (ret == FALSE)
	{
		PrintStatusInfo(_T("GPRS��ʼ��ʧ��, �����Ƿ�װ��ȷ."));
	}
	else
	{
		PrintStatusInfo(_T("GPRS��ʼ���ɹ�."));
		m_GPRS.GPRS_DeleteShortMsg(1);/* ɾ���� 1 ������*/
		SetTimer(1, 1500, NULL);			/* ÿ 1.5 ���ȡһ�ζ��� */
	}
}
void GPRSCONTDlg::PrintStatusInfo(CString strinfo)
{
	CString str;
	str.Format(_T("%s\r\n"),strinfo);
	m_status += str;
	UpdateData(FALSE);
}