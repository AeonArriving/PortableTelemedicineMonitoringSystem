
// DSPDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DSPDemo.h"
#include "DSPDemoDlg.h"
#include "DlgParams.h"
#include "DSP.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
////////////////////////////////////////////////////
#define MAX_SIZE  4096
#define MIN_SIZE  0
#define N 100
static int ecg;//Ҫ��ʾ�ĵ��Y����
//���崮�����ݽ�����Ϣ����
#define WM_RECV_SERIAL_DATA          WM_USER + 101
////////////////////////////////////////////////////
ECGDataNode  *InitECGDataNode()
{
	ECGDataNode *L;
	L=(ECGDataNode*)malloc(sizeof(ECGDataNode));
	ASSERT(L!=NULL);
	memset(L->ECG,0,ECG_SIZE*sizeof(int));
	L->next = NULL;
	return L;
}
///////////////////////////////////////////////////
ECGDataNode *ECG = InitECGDataNode();
ECGDataNode *Ptr_ECG = ECG;
ECGDataNode *Ptr_Next_ECG = ECG;
ECGDataNode *Ptr_Next_DSP = ECG;
ECGDataNode *Ptr_Next_FOR = ECG;
//////////////////////////////////////////////////
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDSPDemoDlg �Ի���




CDSPDemoDlg::CDSPDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDSPDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pCSerial = NULL;
}

void CDSPDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDSPDemoDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_RECV_SERIAL_DATA,OnRecvSerialData)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CDSPDemoDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDSPDemoDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDSPDemoDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDOK, &CDSPDemoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDSPDemoDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON4, &CDSPDemoDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDSPDemoDlg::OnBnClickedButton5)
	ON_EN_CHANGE(IDC_EDT_RECV, &CDSPDemoDlg::OnEnChangeEdtRecv)
END_MESSAGE_MAP()


// CDSPDemoDlg ��Ϣ�������

BOOL CDSPDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ShowWindow(SW_MINIMIZE);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//��ʼ����ʾ�ؼ����������С
	CRect rect;
	rect.left = 10;
	rect.top = 10;
	rect.right = 690;
	rect.bottom = 260;
	//�������߿ؼ�ʵ��
	m_2DGraph.Create(_T(""),_T(""),WS_VISIBLE | WS_CHILD, rect, this,0,NULL) ; 
	////////////////////////////////////////////
	m_pointCount = 0;
	ecgpoint = 0;
	ecg = 0;
	flag = 0;
	pEdtRecvMsg = (CEdit*)GetDlgItem(IDC_EDT_RECV);
	//������ӵ��ʱ��
//	SetTimer(1,20,NULL);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDSPDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDSPDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDSPDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
DWORD CDSPDemoDlg::ThreadProc(LPVOID lpParameter)
{
	CDSPDemoDlg* args = NULL;
	args = (CDSPDemoDlg*)lpParameter;
//	FILE* fp;
//	fp = fopen("ECG.txt","r");
	return 1;

}
//��ʱ����Ϣ������
void CDSPDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	int nRandomY;//Ҫ��ʾ�ĵ��Y����
//	 //����ĸ�ֵ���˲�����
	{//ƽ���˲�
		int ECGtemp = 0;
		for(int i=0;i<N;i++)
		{
			int currnum = ecgpoint+i;
			if(currnum >= ECG_SIZE )
			{
				Ptr_Next_DSP = Ptr_ECG->next;
				ECGtemp += Ptr_Next_DSP->ECG[currnum-ECG_SIZE];
			}
			else
			{
				ECGtemp += Ptr_ECG->ECG[currnum];
			}
		}
		Ptr_ECG->ECG[ecgpoint] = (int)(ECGtemp/N);
	}
	nRandomY = Ptr_ECG->ECG[ecgpoint];
	if(nRandomY<100)
	{
		nRandomY = 100;
	}
	/////////////////////////
	ecgpoint++;
	if(ecgpoint == ECG_SIZE)
	{
		if(Ptr_ECG->next != NULL)
		{
			ecgpoint=0;
			Ptr_ECG = Ptr_ECG->next;
		}
		else
		{
			KillTimer(1);
		}
	}				
	//������ߵ�������X_MAX���㣬��ɾ����1���㡣
	if (m_pointCount > X_MAX )
	{
		m_2DGraph.DeleteFirstPoint();
		m_pointCount--;
	}				
	//��������ӵ�
	m_2DGraph.AppendPoint(nRandomY);				
	TRACE(L" y = %d \n",nRandomY);
	m_pointCount++;
	
	/////////////////////////
	CDialog::OnTimer(nIDEvent);
}

void CALLBACK CDSPDemoDlg::OnSerialRead(DWORD UserParamr,char* buf,DWORD bufLen)
{
	char *pRecvBuf; //���ջ�����
	//�õ�������ָ��
	CDSPDemoDlg* pThis = (CDSPDemoDlg*)UserParamr;
	//�����յĻ�����������pRecvBuf��
	pRecvBuf = new char[bufLen];
	ZeroMemory(pRecvBuf,bufLen);
	MoveMemory(pRecvBuf,buf,bufLen);
	//////////////////////////////////////////////////////
	CString strOldRecv = L"";
	CString strRecv = L"";
	strRecv = CString(buf);
	DWORD id;
	HANDLE handle;
	handle = CreateThread(NULL,0,ThreadProc,NULL,0,&id);
	if(strRecv.Find(_T(","),0)>=0)
	{
		//////////////////////////////////////
		//������յ�������
		CString num = L"";
		int pos = 0;
		while (!strRecv.IsEmpty())
		{
			int arg = 0;
			pos = strRecv.Find(_T(","),0);
			if(pos == 0)
			{//","
				strRecv.Delete(0,pos+1);
				continue;
			}
			else
			{
				num = strRecv.Left(pos);
				int len = num.GetLength();
				strRecv.Delete(0,pos+1);
				Ptr_Next_ECG->ECG[ecg] = _ttoi(num);
				ecg++;
				if(ecg == ECG_SIZE)
				{
					ecg = 0;
					Ptr_Next_ECG->next = InitECGDataNode();
					Ptr_Next_ECG = Ptr_Next_ECG->next;
				}
			}
		}
	//////////////////////////////////////
	}
	//�����첽��Ϣ����ʾ�յ��������ݣ���Ϣ�����꣬Ӧ�ͷ��ڴ�
	pThis->PostMessage(WM_RECV_SERIAL_DATA,WPARAM(pRecvBuf),bufLen);
	delete[] pRecvBuf;
	pRecvBuf = NULL;
}
// ���ڽ������ݴ�����
LONG CDSPDemoDlg::OnRecvSerialData(WPARAM wParam,LPARAM lParam)
{
	flag++;
	//���ڽ��յ���BUF
	char *pRecvBuf; //���ջ�����
//	CHAR *pBuf = (CHAR*)wParam;
	//���ڽ��յ���BUF����
	DWORD dwBufLen = lParam;
	//�����յĻ�����������pRecvBuf��
	pRecvBuf = new char[dwBufLen];
	ZeroMemory(pRecvBuf,dwBufLen);
	MoveMemory(pRecvBuf,(CHAR*)wParam,dwBufLen);
	//////////////////////////////////////////////////////
	CString strOldRecv = L"";
	CString strRecv = L"";
	strRecv = CString(pRecvBuf);
	ASSERT(pEdtRecvMsg != NULL);
	//�õ����տ��е���ʷ�ı�
	pEdtRecvMsg->GetWindowTextW(strOldRecv);	
	/////////////////////////////////////////////////////////////////
	//���½��յ����ı���ӵ����տ���
	pEdtRecvMsg->SetSel(-1,-1);
//	pEdtRecvMsg->ReplaceSel(strRecv);
	strOldRecv = strOldRecv+_T("\r\n")+ strRecv;
	pEdtRecvMsg->SetWindowTextW(strOldRecv);
	if(strOldRecv.GetLength()>=2048)
	{
		pEdtRecvMsg->SetWindowTextW(_T(""));
	}
	//�ͷ��ڴ�
	//���տ�
	delete[] pRecvBuf;
	pRecvBuf = NULL;
	/////////////////////////////////////////////////////////////////
	if(flag>2*N)
	{
		flag--;
		int nRandomY;//Ҫ��ʾ�ĵ��Y����
	//	 //����ĸ�ֵ���˲�����
		{//ƽ���˲�
			int ECGtemp = 0;
			for(int i=0;i<N;i++)
			{
				int currnum = ecgpoint+i;
				if(currnum >= ECG_SIZE )
				{
					Ptr_Next_DSP = Ptr_ECG->next;
					ECGtemp += Ptr_Next_DSP->ECG[currnum-ECG_SIZE];
				}
				else
				{
					ECGtemp += Ptr_ECG->ECG[currnum];
				}
			}
			Ptr_ECG->ECG[ecgpoint] = (int)(ECGtemp/N);
		}
		nRandomY = Ptr_ECG->ECG[ecgpoint];
		if(nRandomY<100)
		{
			nRandomY = 100;
		}
		/////////////////////////
		ecgpoint++;
		if(ecgpoint == ECG_SIZE)
		{
			if(Ptr_ECG->next != NULL)
			{
				ecgpoint=0;
				Ptr_Next_FOR = Ptr_ECG;
				Ptr_ECG = Ptr_ECG->next;

				Ptr_Next_FOR->next = NULL;
				free(Ptr_Next_FOR);
				Ptr_Next_FOR = NULL;
			}
			else
			{
//				KillTimer(1);
			}
		}				
		//������ߵ�������X_MAX���㣬��ɾ����1���㡣
		if (m_pointCount > X_MAX )
		{
			m_2DGraph.DeleteFirstPoint();
			m_pointCount--;
		}				
		//��������ӵ�
		m_2DGraph.AppendPoint(nRandomY);				
		TRACE(L" y = %d \n",nRandomY);
		m_pointCount++;
	}
	return 0;
}
void CDSPDemoDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//�жϴ����Ƿ��Ѿ���
	if (pCSerial != NULL)
	{
		pCSerial->Close();
		delete pCSerial;
		pCSerial = NULL;
	}
	//���ڲ�������Ի���
	CDlgParams dlgParams;	
	if (dlgParams.DoModal() == IDOK)
	{
		TCHAR szPort[15];
		wsprintf(szPort, L"COM%d:",dlgParams.m_portNo);
		pCSerial = new CSerial;
		pCSerial->m_OnSeriesRead = OnSerialRead;//���ڽ��ճɹ��ص����� 
		BOOL ret;
		ret = pCSerial->Open((DWORD)this,
							  szPort,
							  dlgParams.m_baud,
							  8,
							  0,
							  1);
		if (ret == FALSE)	//�򿪴���, ����λΪ8,ֹͣλΪ1,��У��λ
		{
			AfxMessageBox(L"���ڴ�ʧ��");
		}
		else
		{

		}
	}
}

void CDSPDemoDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (pCSerial != NULL)
	{
		pCSerial->Close();
		delete pCSerial;
		pCSerial = NULL;
	}
}

void CDSPDemoDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	char * buf  =NULL;  //���巢�ͻ�����
	DWORD dwBufLen = 0;   //���巢�ͻ���������
	CString strSend = L"";

	//�õ����������
	CEdit *pEdtSendMsg = (CEdit*)GetDlgItem(IDC_EDT_SEND);
	ASSERT(pEdtSendMsg != NULL);
	//�������û�д򿪣�ֱ�ӷ���
	if (pCSerial == NULL)
	{
		AfxMessageBox(L"����δ��");
		return;
	}
	//�õ������͵��ַ���
	pEdtSendMsg->GetWindowTextW(strSend);

	//�������͵��ַ���ת���ɵ��ֽڣ����з���
	buf = new char[strSend.GetLength()*2+1];
	ZeroMemory(buf,strSend.GetLength()*2+1);
	//ת���ɵ��ֽڽ��з���	
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,
						strSend.GetBuffer(strSend.GetLength()),
						strSend.GetLength(),
						buf,
						strSend.GetLength()*2,
						NULL,NULL);

	dwBufLen = strlen(buf)+1;
	//�����ַ���
	pCSerial->SendData(buf,dwBufLen);

	//�ͷ��ڴ�
	delete[] buf;
	buf = NULL;
}

void CDSPDemoDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}

void CDSPDemoDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

void CDSPDemoDlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//��������
	CEdit *pEdtSendMsg = (CEdit*)GetDlgItem(IDC_EDT_SEND);
	ASSERT(pEdtSendMsg != NULL);
	pEdtSendMsg->SetWindowTextW(_T(""));
	//���տ����
	CEdit *pEdtRecvMsg = (CEdit*)GetDlgItem(IDC_EDT_RECV);
	ASSERT(pEdtRecvMsg != NULL);
	//�õ����տ��е���ʷ�ı�
	pEdtRecvMsg->SetWindowTextW(_T(""));
}

void CDSPDemoDlg::OnBnClickedButton5()
{
	//������ӵ��ʱ��
//	SetTimer(1,20,NULL);
	
}

void CDSPDemoDlg::OnEnChangeEdtRecv()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
