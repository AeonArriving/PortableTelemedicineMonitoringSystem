
// AnsleServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AnsleServer.h"
#include "AnsleServerDlg.h"
#include "math.h"
#define N 10
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
////////////////////////////////////////////////////////////
ReceiveDataNode *InitDataNode()
{
	ReceiveDataNode *L;
	L=(ReceiveDataNode*)malloc(sizeof(ReceiveDataNode));
	ASSERT(L!=NULL);
	memset(L->ECG,0,ECG_SIZE*sizeof(int));
	int SPO2 = 0;
	memset(L->BP,0,2*sizeof(int));
	int Pulse = 0;
	L->next = NULL;
	return L;
}
ReceiveDataNode *ECG = InitDataNode();
ReceiveDataNode *Ptr_ECG = ECG;
ReceiveDataNode *Ptr_next = ECG;
/////////////////////////////////////////////////////////////
//����TCP �յ��ͻ���������Ϣ
#define WM_RECV_TCP_DATA		WM_USER + 101
//����TCP�ͻ���������Ϣ
#define WM_TCP_CLIENT_CONNECT	WM_USER + 102
/////////////////////////////////////////////////////////////

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


// CAnsleServerDlg �Ի���

CAnsleServerDlg::CAnsleServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnsleServerDlg::IDD, pParent)
	, m_localPort(0)
	, m_status(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAnsleServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, LOCALPORT, m_localPort);
	DDX_Control(pDX, IDC_LED, m_led);
	DDX_Control(pDX, IDC_LED1, m_led1);
	DDX_Control(pDX, IDC_LED2, m_led2);
	DDX_Control(pDX, IDC_LED3, m_led3);
	DDX_Text(pDX, IDC_STATE, m_status);
}

BEGIN_MESSAGE_MAP(CAnsleServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_MESSAGE(WM_RECV_TCP_DATA,OnRecvTCPData)
	ON_MESSAGE(WM_TCP_CLIENT_CONNECT,OnClientConnect)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(MONITOR, &CAnsleServerDlg::OnBnClickedMonitor)
	ON_BN_CLICKED(CLOSE, &CAnsleServerDlg::OnBnClickedClose)
	ON_BN_CLICKED(IDC_SET_MSGCENTR_TELCODE, &CAnsleServerDlg::OnBnClickedSetMsgcentrTelcode)
	ON_BN_CLICKED(IDC_INITSMS, &CAnsleServerDlg::OnBnClickedInitsms)
END_MESSAGE_MAP()


// CAnsleServerDlg ��Ϣ�������

BOOL CAnsleServerDlg::OnInitDialog()
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

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//��ʼ����ʾ�ؼ����������С
	CRect rect;
	rect.left = 20;
	rect.top = 30;
	rect.right = 675;
	rect.bottom = 300;
	//�������߿ؼ�ʵ��
	m_2DGraph.Create(_T(""),_T(""),WS_VISIBLE | WS_CHILD, rect, this,0,NULL) ; 
	
	//�˿ںų�ʼ��
	m_localPort = 5000;
	UpdateData(FALSE);
	///////////////////////////////////////////////////////////////////////
	//���ñ���ɫ
	m_led.SetColorBackGround(RGB(0,0,0));
	//����ǰ��ɫ
	m_led.SetColorForeGround(RGB(0,255,255));
	m_led.Display(0);
    //���ö�ʱ��
	SetTimer(2,20,NULL);
	/////////////////////////////////////////////////////////////////////
	//���ñ���ɫ
	m_led1.SetColorBackGround(RGB(0,0,0));
	//����ǰ��ɫ
	m_led1.SetColorForeGround(RGB(0,255,255));
	m_led1.Display(0);
    //���ö�ʱ��
	SetTimer(3,20,NULL);
	//////////////////////////////////////////////////////////////////
	//���ñ���ɫ
	m_led2.SetColorBackGround(RGB(0,0,0));
	//����ǰ��ɫ
	m_led2.SetColorForeGround(RGB(0,255,255));
	m_led2.Display(0);
    //���ö�ʱ��
	SetTimer(4,20,NULL);
	//////////////////////////////////////////////////////////////////
	//���ñ���ɫ
	m_led3.SetColorBackGround(RGB(0,0,0));
	//����ǰ��ɫ
	m_led3.SetColorForeGround(RGB(0,255,255));
	m_led3.Display(0);
    //���ö�ʱ��
	SetTimer(5,20,NULL);
	//////////////////////////////////////////////////////////////////
	SetDlgItemText(IDC_SENDMSG_TELCODE, _T("18710846798"));		
	SetDlgItemText(IDC_EDIT_SENDMSG, _T("1.��ʼ��SMS\r\n2.��÷�����IP\r\n3.����Զ����������"));
	SetDlgItemText(IDC_MSGCENTR_TELCODE, _T("+8613800290500"));
	IS_GPRSInit = FALSE;//GPRSδ��ʼ��
	IS_LISTEN = FALSE;//����ipδ��������
	GPRS_IPAddress = _T("");
	//////////////////////////////////////////////////////////////////
	m_pointCount = 0;
	ecg = 0;
	//������ӵ��ʱ��
	SetTimer(1,100,NULL);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CAnsleServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
void CAnsleServerDlg::OnPaint()
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
HCURSOR CAnsleServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//��ʱ����Ϣ������
void CAnsleServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch(nIDEvent)
	{
		case 1:
			{//�ĵ���ʾ��������
				//����ĸ�ֵ���˲�����
				int nRandomY;//Ҫ��ʾ�ĵ��Y����
				{//ƽ���˲�
					int ECGtemp = 0;
					for(int i=0;i<N;i++)
					{
						ECGtemp += Ptr_ECG->ECG[(ecg+i)%ECG_SIZE];
					}
					Ptr_ECG->ECG[ecg] = (int)(ECGtemp/N);
				}
//				Ptr_ECG->ECG[ecgpoint] = (Ptr_ECG->ECG[ecgpoint]-500)*5;
//				nRandomY = (Ptr_ECG->ECG[ecg]*5/1024)*1000;
				nRandomY = (Ptr_ECG->ECG[ecg]-600)*7;

				//������ߵ�������***���㣬��ɾ����1���㡣
				if (m_pointCount > X_MAX )
				{
					m_2DGraph.DeleteFirstPoint();
					m_pointCount--;
				}
				
				//��������ӵ�
				m_2DGraph.AppendPoint(nRandomY);	
				


				m_pointCount++;
				/////////////////////////
				//������������ʵ�ʹ��̲���
				ecg++;
				if(ecg == ECG_SIZE)
				{
					ecg=0;
					if(Ptr_ECG->next != NULL)
						Ptr_ECG = Ptr_ECG->next;
					else 
						nRandomY = 0;
				}
				CDialog::OnTimer(nIDEvent);
				break;
			}
		case 2:
			{//Ѫѹ��ʾ��������
				//����LED��ʾ����
//				int led = rand()%1000;
				int led = Ptr_ECG->BP[0];
				m_led.Display(led);  	
				CDialog::OnTimer(nIDEvent);
				break;
			}
		case 3:
			{//Ѫѹ��ʾ��������
				//����LED��ʾ����
//				int led1 = rand()%100;
				int led1 =Ptr_ECG->BP[1];
				m_led1.Display(led1);  	
				CDialog::OnTimer(nIDEvent);
				break;
			}
		case 4:
			{//Ѫ�����Ͷ���ʾ��������
				//����LED��ʾ����
//				int led2 = rand()%100;
				int led2 =Ptr_ECG->SPO2;
				m_led2.Display(led2);  	
				CDialog::OnTimer(nIDEvent);
				break;
			}
		case 5:
			{//������ʾ��������
				//����LED��ʾ����
//				int led3 = rand()%100;
				int led3 =Ptr_ECG->Pulse;
				m_led3.Display(led3);  	
				CDialog::OnTimer(nIDEvent);
				break;
			}
		case 6:
			{//���Ž��մ�������
				BOOL ret;
				CString strTelCode, strMsg;
				ret = m_GPRS.GPRS_ReadShortMessage(1, &strTelCode, &strMsg);// ��ȡ�� 0 ������
				if (ret == TRUE)
				{
					SetDlgItemText(IDC_RECVMSG_TELCODE, strTelCode);//��ʾ�绰���� 
					SetDlgItemText(IDC_DISP_RECVMSG, strMsg);	// ��ʾ��������
					m_GPRS.GPRS_DeleteShortMsg(1);	
					CString askIP = _T("I Need Your IP Address");
					if(strMsg.Find(askIP,0)>=0)
					{
						SetDlgItemText(IDC_SENDMSG_TELCODE,strTelCode);
						//��ͻ��˻��ͱ���IP��ַ
						CString myIPAddress;
						myIPAddress = _T("IP")+GetLocalIP()+_T("END");
						BOOL ret = m_GPRS.GPRS_SendShortMessage(strTelCode, myIPAddress);//���Ͷ���
						SetDlgItemText(IDC_EDIT_SENDMSG, myIPAddress);
						if (ret == TRUE)
						{
							KillTimer(6);
							PrintStatusInfo(_T("���ŷ��ͳɹ�."));
						}
							
						else
							PrintStatusInfo(_T("���ŷ���ʧ��."));
					}
				}
				CDialog::OnTimer(nIDEvent);
				break;
			}
		default:
			{
				break;
			}
	}	
}

/////////////////////////////////////////////////////////////////////////////////////////////
//SMS ��GPRS��������
/////////////////////////////////////////////////////////////////////////////////////////////
void CAnsleServerDlg::OnBnClickedSetMsgcentrTelcode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strCode;
	GetDlgItemText(IDC_MSGCENTR_TELCODE, strCode);
	BOOL ret = m_GPRS.GPRS_SetShortMSGCenterTel(strCode);	//���ö����к��� 
	if (ret == TRUE)
		PrintStatusInfo(_T("���ö������ĺ���ɹ�."));
	else
		PrintStatusInfo(_T("���ö������ĺ���ʧ��."));
}

void CAnsleServerDlg::OnBnClickedInitsms()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL ret = m_GPRS.GPRS_Init(_T("COM4:"), 115200, (DWORD)this);
	if (ret == FALSE)
	{
	//	m_GPRS.GPRS_ClosePort();
		PrintStatusInfo(_T("��ʼ��ʧ��, �����Ƿ�װ��ȷ"));
	}	
	else
	{
		PrintStatusInfo(_T("SMS��ʼ���ɹ�"));
		m_GPRS.GPRS_DeleteShortMsg(1);	//ɾ���� 1 ������
		SetTimer(6, 5000, NULL);			/* ÿ 1 ���ȡһ�ζ��� */
	}
}
/////////////////////////////////////////////////////////////////////////////////////
//TCP-IP���Ӵ�������
/////////////////////////////////////////////////////////////////////////////////////
//�ͻ������ӽ����¼�������
void CALLBACK  CAnsleServerDlg::OnClientConnect(void* pOwner,CTCPCustom_CE* pTcpCustom)
{
	TCHAR *szAddress =NULL;
	DWORD dwBufLen = pTcpCustom->m_RemoteHost.GetLength() + 1;
	szAddress = new TCHAR[dwBufLen];
	ZeroMemory(szAddress,dwBufLen*2);
	//�����ڴ棬�õ��ͻ���IP��ַ
	wcscpy(szAddress,pTcpCustom->m_RemoteHost);
	
	CAnsleServerDlg *pThis = (CAnsleServerDlg*)pOwner;
	
	//�����첽��Ϣ����ʾ�пͻ������ӣ���Ϣ���������Ҫ�ͷ��ڴ�
	pThis->PostMessage(WM_TCP_CLIENT_CONNECT,0,LPARAM(szAddress));
}

//�ͻ���SOCKET�ر��¼�������
void  CALLBACK CAnsleServerDlg::OnClientClose(void* pOwner,CTCPCustom_CE* pTcpCustom)
{
	TCHAR *szAddress =NULL;
	DWORD dwBufLen = pTcpCustom->m_RemoteHost.GetLength() + 1;
	szAddress = new TCHAR[dwBufLen];
	ZeroMemory(szAddress,dwBufLen*2);
	//�����ڴ棬�õ��ͻ���IP��ַ
	wcscpy(szAddress,pTcpCustom->m_RemoteHost);

	CAnsleServerDlg *pThis = (CAnsleServerDlg*)pOwner;
	
	//�����첽��Ϣ����ʾ�пͻ������ӣ���Ϣ���������Ҫ�ͷ��ڴ�
	pThis->PostMessage(WM_TCP_CLIENT_CONNECT,1,LPARAM(szAddress));

}

//���������յ����Կͻ��˵�����
void CALLBACK CAnsleServerDlg::OnClientRead(void* pOwner,CTCPCustom_CE* pTcpCustom,const char * buf,DWORD dwBufLen )
{
	DATA_BUF *pGenBuf = new DATA_BUF;
	char *pRecvBuf = NULL; //���ջ�����
	//�õ�������ָ��
	CAnsleServerDlg* pThis = (CAnsleServerDlg*)pOwner;
	//�����յĻ�����������pRecvBuf��
	pRecvBuf = new char[dwBufLen];
	MoveMemory(pRecvBuf,buf,dwBufLen);

	ZeroMemory(pGenBuf,sizeof(DATA_BUF));
	pGenBuf->dwBufLen = dwBufLen;
	pGenBuf->sBuf = pRecvBuf;
	
	//
	wcscpy(pGenBuf->szAddress,pTcpCustom->m_RemoteHost);

	//�����첽��Ϣ����ʾ�յ�TCP���ݣ���Ϣ�����꣬Ӧ�ͷ��ڴ�
	pThis->PostMessage(WM_RECV_TCP_DATA,WPARAM(pGenBuf),LPARAM(pTcpCustom));

   
}

//�ͻ���Socket�����¼�������
void CALLBACK CAnsleServerDlg::OnClientError(void* pOwner,CTCPCustom_CE* pTcpCustom,int nErrorCode)
{	
}

//��������Socket�����¼�������
void CALLBACK CAnsleServerDlg::OnServerError(void* pOwner,CTCPServer_CE* pTcpServer_CE,int nErrorCode)
{	
}

//TCP�������ݴ�����
LONG CAnsleServerDlg::OnRecvTCPData(WPARAM wParam,LPARAM lParam)
{
	DATA_BUF *pGenBuf = (DATA_BUF*)wParam; //ͨ�û�����
	CTCPCustom_CE* pTcpCustom= (CTCPCustom_CE* )lParam; //TCP�ͻ��˶���
	//������ʾ�б�
	CListBox * pLstRecv = (CListBox*)GetDlgItem(RECEIVELIST);
	ASSERT(pLstRecv != NULL);
	//���յ�������
	CString strRecv;
	CString strLen;
	strLen.Format(L"%d",pGenBuf->dwBufLen);
	strRecv = CString(pGenBuf->sBuf);

	pLstRecv->AddString(_T("*************************************"));
	pLstRecv->AddString(_T("������: ") + CString(pGenBuf->szAddress) );
	pLstRecv->AddString(_T("���ݳ���Ϊ:")+strLen);
	pLstRecv->AddString(strRecv);
	///////////////////////////////////////////////
	int flag_1;
	int flag_2;
	int pos = 0;
	CString temp = L"";
	CString num = L"";

	while (!strRecv.IsEmpty())
	{
		int args = 0;
		CString arg = L"";
		flag_1 = strRecv.Find('$');
		arg = strRecv.Left(flag_1);
		////////////////////////////////////
		if(arg.Compare(L"ECG") == 0)
		{
			arg.Replace(L"ECG" ,L"1");
			args = _ttoi(arg);
		}
		else if(arg.Compare(L"SPO2") == 0)
		{
			arg.Replace(L"SPO2" ,L"2");
			args = _ttoi(arg);
		}
		else if(arg.Compare(L"BP") == 0)
		{
			arg.Replace(L"BP" ,L"3");
			args = _ttoi(arg);
		}
		else if(arg.Compare(L"Pulse") == 0)
		{
			arg.Replace(L"Pulse" ,L"4");
			args = _ttoi(arg);
		}
        /////////////////////////////////////
		flag_2 = strRecv.Find('#');
		temp = strRecv.Mid(flag_1+1,flag_2-flag_1-1);
		strRecv.Delete(0,flag_2+1);
		switch(args)
		{
			case 1:
				{
					int i = 0;
					while(!temp.IsEmpty())
					{
						pos = temp.Find(';');
						num = temp.Left(pos);
						Ptr_next->ECG[i] =  _ttoi(num);
						i++;
						temp.Delete(0,pos+1);
					}
					break;
				}
			case 2:
				{
					while(!temp.IsEmpty())
					{
						pos = temp.Find(';');
						num = temp.Left(pos);
						Ptr_next->SPO2 =  _ttoi(num);
						temp.Delete(0,pos+1);
					}
					break;
				}
			case 3:
				{
					int i=0;
					while(!temp.IsEmpty())
					{
						pos = temp.Find(';');
						num = temp.Left(pos);
						Ptr_next->BP[i] =  _ttoi(num);
						i++;
						temp.Delete(0,pos+1);
					}
					break;
				}
			case 4:
				{
					while(!temp.IsEmpty())
					{
						pos = temp.Find(';');
						num = temp.Left(pos);
						Ptr_next->Pulse =  _ttoi(num);
						temp.Delete(0,pos+1);
					}
					break;
				}
		}
	}
	Ptr_next->next = InitDataNode();
	Ptr_next = Ptr_next->next;
	//������ӵ��ʱ��
//	SetTimer(1,20,NULL);
	///////////////////////////////////////////////    
	
	//���ͻ�Ӧ����
	char * sendBuf = "���ͳɹ�!";
	int sendLen=0;
	sendLen = strlen(sendBuf)+1;
	if (!m_tcpServer.SendData(pTcpCustom,sendBuf,sendLen))
	{
		AfxMessageBox(_T("����ʧ��"));
	}

	//�ͷ��ڴ�
	delete[] pGenBuf->sBuf;
	pGenBuf->sBuf = NULL;
	delete pGenBuf;
	pGenBuf = NULL;
	return 0;
}

//�ͻ������ӶϿ���Ϣ����
LONG CAnsleServerDlg::OnClientConnect(WPARAM wParam,LPARAM lParam)
{
	int iIndex = 0;
	TCHAR *szAddress = (TCHAR*)lParam;
	CString strAddrss = szAddress;
	
	CListBox * pLstConn = (CListBox*)GetDlgItem(CONNECTLIST);
	ASSERT(pLstConn != NULL);

	if (wParam == 0)
	{
		pLstConn->AddString(strAddrss + _T("��������"));
	}
	else
	{
		iIndex = pLstConn->FindString(iIndex,strAddrss + _T("��������"));
		if (iIndex != LB_ERR)
		{
			pLstConn->DeleteString(iIndex); 
		}
	}

	//�ͷ��ڴ�
	delete[] szAddress;
	szAddress = NULL;
	return 0;
}

void CAnsleServerDlg::OnBnClickedMonitor()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	IS_LISTEN = TRUE;
	UpdateData(TRUE);
	//����m_tcpServer����
   	m_tcpServer.m_LocalPort = m_localPort;
	m_tcpServer.m_pOwner = this;
	m_tcpServer.OnClientConnect = OnClientConnect;
	m_tcpServer.OnClientClose = OnClientClose;
	m_tcpServer.OnClientRead = OnClientRead;
	m_tcpServer.OnClientError = OnClientError;
	m_tcpServer.OnServerError = OnServerError;
	if (m_tcpServer.Open() <= 0)
	{
		AfxMessageBox(_T("����ʧ��"));
		return;
	}
	else
	{
		PrintStatusInfo(_T("�����ɹ�"));
	}
}

void CAnsleServerDlg::OnBnClickedClose()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CListBox * pLstConn = (CListBox*)GetDlgItem(CONNECTLIST);
	ASSERT(pLstConn != NULL);
	
	CListBox * pLstRecv = (CListBox*)GetDlgItem(RECEIVELIST);
	ASSERT(pLstRecv != NULL);
	
	//
	if (m_tcpServer.Close() <=0)
	{
		AfxMessageBox(_T("�ر�TCP������ʧ��"));
		return;
	}
	
	//����б�
	pLstConn->ResetContent();
	pLstRecv->ResetContent();
}
////////////////////////////////////////////////////////////////////////////////////////
//��������
////////////////////////////////////////////////////////////////////////////////////////
CString CAnsleServerDlg::GetLocalIP()
{
	HOSTENT *LocalAddress;
	char	*Buff;
	TCHAR	*wBuff;
	CString strReturn = _T("");
	//�����µĻ�����
	Buff = new char[256];
	wBuff = new TCHAR[256];
	//�ÿջ�����
	memset(Buff, '\0', 256);
	memset(wBuff, TEXT('\0'), 256*sizeof(TCHAR));
	//�õ����ؼ������
	if (gethostname(Buff, 256) == 0)
	{
		//ת����˫�ֽ��ַ���
		mbstowcs(wBuff, Buff, 256);
		//�õ����ص�ַ
		LocalAddress = gethostbyname(Buff);
		//�ÿ�buff
		memset(Buff, '\0', 256);
		//��ϱ���IP��ַ
		sprintf(Buff, "%d.%d.%d.%d\0", LocalAddress->h_addr_list[0][0] & 0xFF,
			LocalAddress->h_addr_list[0][1] & 0x00FF, LocalAddress->h_addr_list[0][2] & 0x0000FF, LocalAddress->h_addr_list[0][3] & 0x000000FF);
		//�ÿ�wBuff
		memset(wBuff, TEXT('\0'), 256*sizeof(TCHAR));
		//ת����˫�ֽ��ַ���
		mbstowcs(wBuff, Buff, 256);
		//���÷���ֵ
		strReturn = wBuff;
	}
	else
	{
	}
	//�ͷ�Buff������
	delete[] Buff;
	Buff = NULL;
	//�ͷ�wBuff������
	delete[] wBuff;
	wBuff = NULL;
	return strReturn;
}
void CAnsleServerDlg::PrintStatusInfo(CString strinfo)
{
	CString str;
	str.Format(_T("%s\r\n"),strinfo);
	m_status += str;
	UpdateData(FALSE);
}