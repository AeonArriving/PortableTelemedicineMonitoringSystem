// Draw2DGraphDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "resourceppc.h"
#include "Draw2DGraph.h"
#include "Draw2DGraphDlg.h"
#include "DataStructures.h"

#define BUFFER_LENGTH 32
#define MAX_SIZE  1000
#define MIN_SIZE  500
#define N 10
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
///////////////////////////////////////////////////
ECGDataNode *ECG = InitECGDataNode();
SPO2DataNode *SPO2 = InitSPO2DataNode();
BPDataNode *BP = InitBPDataNode();

ECGDataNode *Ptr_ECG = ECG;
ECGDataNode *Ptr_Next_ECG = ECG;
ECGDataNode *Ptr_Send_ECG = ECG;

SPO2DataNode *Ptr_SPO2 = SPO2;
SPO2DataNode *Ptr_Next_SPO2 = SPO2;
SPO2DataNode *Ptr_Send_SPO2 = SPO2;

BPDataNode *Ptr_BP = BP;
BPDataNode *Ptr_Next_BP = BP;
BPDataNode *Ptr_Send_BP = BP;

int ecg = 0;

SendDataNode *SendDate = InitSendDataNode();
SendDataNode *Ptr_SendDate = SendDate;
SendDataNode *Ptr_Next_Send = SendDate;
//////////////////////////////////////////////////

// CDraw2DGraphDlg �Ի���
//�Ի����캯��
CDraw2DGraphDlg::CDraw2DGraphDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDraw2DGraphDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pSerial1 = NULL;//��ʼ�����ڶ���ָ��Ϊ��
	Spo2 = FALSE;
	Pulse = FALSE;
}

void CDraw2DGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LED, m_led);
	DDX_Control(pDX, IDC_LED1, m_led1);
	DDX_Control(pDX, IDC_LED2, m_led2);
	DDX_Control(pDX, IDC_LED3, m_led3);
}

BEGIN_MESSAGE_MAP(CDraw2DGraphDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_MESSAGE(WM_RECV_SERIAL_DATA,OnRecvSerialData)
	ON_MESSAGE(WM_COMM_MESSAGE,&OnCommMessage)
	ON_MESSAGE(WM_RECV_TCP_DATA,OnRecvTCPData)
	ON_MESSAGE(WM_TCP_CLIENT_DISCONNECT,OnClientDisconnect)
	ON_BN_CLICKED(IDC_START, &CDraw2DGraphDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_REMOTEMODE, &CDraw2DGraphDlg::OnBnClickedRemotemode)
	ON_BN_CLICKED(IDC_ECG, &CDraw2DGraphDlg::OnBnClickedEcg)
	ON_BN_CLICKED(IDC_BOOLPRESSURE, &CDraw2DGraphDlg::OnBnClickedBoolpressure)
	ON_BN_CLICKED(IDC_SPO2, &CDraw2DGraphDlg::OnBnClickedSpo2)
	ON_BN_CLICKED(IDC_PULSE, &CDraw2DGraphDlg::OnBnClickedPulse)
END_MESSAGE_MAP()

// CDraw2DGraphDlg ��Ϣ�������
//��ʼ���Ի�����
BOOL CDraw2DGraphDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//��ʼ����ʾ�ؼ����������С
	CRect rect;
	rect.left = 10;
	rect.top = 10;
	rect.right = 475;
	rect.bottom = 140;
	//�������߿ؼ�ʵ��
	m_2DGraph.Create(_T(""),_T(""),WS_VISIBLE | WS_CHILD, rect, this,0,NULL) ; 
///////////////////////////////////////////////////////
	remotemode = new GPRSCONTDlg(); 
	ISOpenRemoteMode = FALSE;//�ر�Զ�̼��ģʽ
	IsClicked = 0;
	m_remotehost = _T("192.168.16.1");
	m_remoteport = 5000;
///////////////////////////////////////////////////////
//	m_pointCount = 0;
//	ecgpoint = 0;
	ecgsend = 0;
	number = 1;
//////////////////////////////////////////////////////
	//���ñ���ɫ
	m_led.SetColorBackGround(RGB(0,0,0));
	//����ǰ��ɫ
	m_led.SetColorForeGround(RGB(0,255,255));
/////////////////////////////////////////////////////////////////////
	//���ñ���ɫ
	m_led1.SetColorBackGround(RGB(0,0,0));
	//����ǰ��ɫ
	m_led1.SetColorForeGround(RGB(0,255,255));
/////////////////////////////////////////////////////////////////
	//���ñ���ɫ
	m_led2.SetColorBackGround(RGB(0,0,0));
	//����ǰ��ɫ
	m_led2.SetColorForeGround(RGB(0,255,255));
/////////////////////////////////////////////////////////////////
	//���ñ���ɫ
	m_led3.SetColorBackGround(RGB(0,0,0));
	//����ǰ��ɫ
	m_led3.SetColorForeGround(RGB(0,255,255));
/////////////////////////////////////////////////////////////////
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CDraw2DGraphDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_DRAW2DGRAPH_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_DRAW2DGRAPH_DIALOG));
	}
}
#endif
///////////////////////////////////////////////////////////////////////////
//�̺߳���
/*DWORD CDraw2DGraphDlg::FilterThread(PVOID pArg)
{
	int ecgfilter = 0;
	while(1)
	{
		{//ƽ���˲�		
			int ECGtemp = 0;
			for(int i=0;i<N;i++)
			{
				ECGtemp += Ptr_ECG->ECG[(ecgfilter+i)%ECG_SIZE];
			}
			Ptr_ECG->ECG[ecgfilter] = (int)(ECGtemp/N);
		}
		Ptr_ECG->ECG[ecgfilter] = (Ptr_ECG->ECG[ecgfilter]-500)*5;
		ecgfilter++;
		if(ecgfilter == ECG_SIZE)
		{
			if(Ptr_ECG->next != NULL)
			{
				ecgfilter = 0;
				Ptr_Show_ECG = Ptr_ECG->next;
			}
		}
	Sleep(20);
	}
	return 1;
}*/
DWORD CDraw2DGraphDlg::ECGThread(PVOID pArg)
{
	
	while(1)
	{
		int nRandomY;//Ҫ��ʾ�ĵ��Y����
		if(Ptr_ECG->next == NULL)
		{
			nRandomY = rand()%(900-200+1)+200;
		}
		else
		{
			{//ƽ���˲�		
				int ECGtemp = 0;
				for(int i=0;i<N;i++)
				{
					ECGtemp += Ptr_ECG->ECG[(ecgpoint+i)%ECG_SIZE];
				}
				Ptr_ECG->ECG[ecgpoint] = (int)(ECGtemp/N);
			}
//			Ptr_Show_ECG->ECG[ecgpoint] = (Ptr_Show_ECG->ECG[ecgpoint]-500)*6;
//			nRandomY = ((Ptr_ECG->ECG[ecgpoint])*5/1024)*1000;
			nRandomY = (Ptr_ECG->ECG[ecgpoint]-600)*7;
			////////////////////////
			ecgpoint++;
			if(ecgpoint == ECG_SIZE)
			{
				if(Ptr_ECG->next != NULL)
				{
					ecgpoint=0;
					Ptr_ECG = Ptr_ECG->next;
				}
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
		m_pointCount++;					
		//////////////////////////////////
		Sleep(100);
	}
	return 1;
}

DWORD CDraw2DGraphDlg::HBPThread(PVOID pArg)
{
	//////////////////////////////////
	//����LED��ʾ����
	while(1)
	{
		int led = rand()%(130-110+1)+110;
	    m_led.Display(led); 
		Sleep(500);
		int led1 = rand()%(85-70+1)+70;
		m_led1.Display(led1);
		Sleep(500);
	} 	
	return 1;
}
/*
DWORD CDraw2DGraphDlg::LBPThread(PVOID pArg)
{ 	
	//����LED��ʾ����
	while(1)
	{
		int led = rand()%(85-70+1)+70;
		m_led1.Display(led);
		Sleep(800);
	}		
	return 1;
}
DWORD CDraw2DGraphDlg::SPO2Thread(PVOID pArg)
{
	//����LED��ʾ����
	while(1)
	{
		int led = rand()%(98-93+1)+93;
		m_led2.Display(led);
		Sleep(800);
	}
	return 1;
}
DWORD CDraw2DGraphDlg::PulseThread(PVOID pArg)
{
	//����LED��ʾ����
	while(1)
	{
		int led = rand()%(78-70+1)+70;
		m_led3.Display(led);
		Sleep(800);
	} 
	return 1;
}
*/
DWORD CDraw2DGraphDlg::SendThread(PVOID pArg)
{
	while(1)
	{
		/////////////////////////////////////////////////
		//ͨ��TCP��������
		CString strSendData = L"";
		CString ECGSend = L"ECG$#";
		CString SPO2Send = L"SPO2$#";
		CString BPSend = L"BP$#";
		CString PulseSend = L"Pulse$#";
		//////////////////////////////////////////
		//ECG
		{
			for(int i=0;i<ECG_SIZE;i++)
			{
				int pos = 0;
				pos = ECGSend.Find(L"#",0);
				CString temp = L"";
				temp.Format(L"%d;",Ptr_Send_ECG->ECG[i]);
				ECGSend.Insert(pos,temp);
			}
			if(Ptr_Send_ECG->next != NULL)
			{
				Ptr_Send_ECG = Ptr_Send_ECG->next;
			}
			else
			{
				int pos = 0;
				pos = ECGSend.Find(L"#");
				CString temp = L"";
				temp.Format(L"%s;",L"0");
				ECGSend.Insert(pos,temp);
			}
		}
		//////////////////////////////////////////
		//SPO2
		{
			int pos = 0;
			pos = SPO2Send.Find(L"#",0);
			CString temp = L"";
			temp.Format(L"%s;",L"0");
			SPO2Send.Insert(pos,temp);
		}
		//////////////////////////////////////////
		//BP
		{
			int pos = 0;
			pos = BPSend.Find(L"#",0);
			CString temp = L"";
			temp.Format(L"%s;",L"0");
			BPSend.Insert(pos,temp);
			int pos1 = 0;
			pos1 = BPSend.Find(L"#",0);
			CString temp1 = L"";
			temp1.Format(L"%s;",L"0");
			BPSend.Insert(pos,temp);
		}
		//////////////////////////////////////////
		//Pulse
		{
			int pos = 0;
			pos = PulseSend.Find(L"#",0);
			CString temp = L"";
			temp.Format(L"%s;",L"0		");
			PulseSend.Insert(pos,temp);
		}
		///////////////////////////////////////////////////
		//��ɷ�������
		strSendData = ECGSend+SPO2Send+BPSend+PulseSend;
		char * sendBuf = NULL;
		int sendLen=0;
		//���÷��ͻ�����
		sendLen = strSendData.GetLength()*2 + 2;
		sendBuf = new char[sendLen];
		ZeroMemory(sendBuf,sendLen);
		wcstombs(sendBuf,strSendData,sendLen);
		sendLen = strlen(sendBuf)+1;
//		Ptr_Next_Send->SendDate = sendBuf;
//		Ptr_Next_Send->next = InitSendDataNode();
//		Ptr_Next_Send = Ptr_Next_Send->next;
		///////////////////////////////////////////////////////////
//		int sendLen=0;
//		sendLen = strlen(Ptr_SendDate->SendDate)+1;
		//��������
		if (!m_tcpClient.SendData(sendBuf,sendLen))
		{
			AfxMessageBox(_T("����ʧ��"));
						
		}
//		int led = rand()%(78-70+1)+70;
//		m_led3.Display(led);
		Sleep(600);
	}
	return 1;
}
///////////////////////////////////////////////////////////////////////////
//��ʱ����Ϣ������
void CDraw2DGraphDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
//		switch(nIDEvent)
//		{
/*		case 1:
			{
				int nRandomY;//Ҫ��ʾ�ĵ��Y����

				if(Ptr_Show_ECG->next == NULL)
				{
					nRandomY = rand()%(900-200+1)+200;
				}
				else
				{
					//����ĸ�ֵ���˲�����
					{//�˲�
						int ECGtemp = 0;
						for(int i=0;i<N;i++)
						{
							ECGtemp += Ptr_Show_ECG->ECG[(ecgpoint+i)%ECG_SIZE];
						}
						Ptr_Show_ECG->ECG[ecgpoint] = (int)(ECGtemp/N);
					}
					nRandomY =(int)((Ptr_Show_ECG->ECG[ecgpoint]*5)/1024);
					nRandomY = Ptr_Show_ECG->ECG[ecgpoint];
					////////////////////////
					ecgpoint++;
					if(ecgpoint == ECG_SIZE)
					{
						if(Ptr_Show_ECG->next != NULL)
						{
							ecgpoint=0;
							Ptr_Show_ECG = Ptr_Show_ECG->next;
						}
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
				//////////////////////////////////
				CDialog::OnTimer(nIDEvent);
				break;
			}*/
//		case 2:
			{
				//����LED��ʾ����
				int led = rand()%(130-110+1)+110;
				m_led.Display(led);  
				//����LED��ʾ����
				int led1 = rand()%(85-70+1)+70;
				m_led1.Display(led1);  
				if(Spo2 == TRUE)
				{
					int led2 = rand()%(98-93+1)+93;
					m_led2.Display(led2);
				}
				if(Pulse == TRUE)
				{
					int led3 = rand()%(78-70+1)+70;
					m_led3.Display(led3);
				}
				CDialog::OnTimer(nIDEvent);
//				break;
			}
/*		case 3:
			{
				//����LED��ʾ����
				int led1 = rand()%(85-70+1)+70;
				m_led1.Display(led1);  	
				CDialog::OnTimer(nIDEvent);
				break;
			}*/
/*		case 4:
			{
				//����LED��ʾ����
				int led2 = rand()%(98-93+1)+93;
				m_led2.Display(led2);  	
				CDialog::OnTimer(nIDEvent);
				break;
			}
		case 5:
			{
				//����LED��ʾ����
				int led3 = rand()%(78-70+1)+70;
				m_led3.Display(led3);  	
				CDialog::OnTimer(nIDEvent);
				break;
			}*/
/*		case 6:
			{
					/////////////////////////////////////////////////
					//ͨ��TCP��������
					CString strSendData = L"";
					CString ECGSend = L"ECG$#";
					CString SPO2Send = L"SPO2$#";
					CString BPSend = L"BP$#";
					CString PulseSend = L"Pulse$#";
					//////////////////////////////////////////
					//ECG
					{
						for(int i=0;i<ECG_SIZE;i++)
						{
							int pos = 0;
							pos = ECGSend.Find(L"#",0);
							CString temp = L"";
							temp.Format(L"%d;",Ptr_Send_ECG->ECG[i]);
							ECGSend.Insert(pos,temp);
						}
						if(Ptr_Send_ECG->next != NULL)
						{
							Ptr_Send_ECG = Ptr_Send_ECG->next;
						}
						else
						{
							int pos = 0;
							pos = ECGSend.Find(L"#");
							CString temp = L"";
							temp.Format(L"%s;",L"0");
							ECGSend.Insert(pos,temp);
						}
					}
					//////////////////////////////////////////
					//SPO2
					{
						int pos = 0;
						pos = SPO2Send.Find(L"#",0);
						CString temp = L"";
						temp.Format(L"%s;",L"0");
						SPO2Send.Insert(pos,temp);
					}
					//////////////////////////////////////////
					//BP
					{
							int pos = 0;
							pos = BPSend.Find(L"#",0);
							CString temp = L"";
							temp.Format(L"%s;",L"0");
							BPSend.Insert(pos,temp);
							int pos1 = 0;
							pos1 = BPSend.Find(L"#",0);
							CString temp1 = L"";
							temp1.Format(L"%s;",L"0");
							BPSend.Insert(pos,temp);
					}
					//////////////////////////////////////////
					//Pulse
					{
						int pos = 0;
						pos = PulseSend.Find(L"#",0);
						CString temp = L"";
						temp.Format(L"%s;",L"0		");
						PulseSend.Insert(pos,temp);
					}
					///////////////////////////////////////////////////
					//��ɷ�������
					strSendData = ECGSend+SPO2Send+BPSend+PulseSend;
					///////////////////////////////////////////////////
					/////////////////////////////////////////////////
					//ͨ��TCP��������
					CString strSendData = L"";
					strSendData = Ptr_SendDate->SendDate;
					char * sendBuf = NULL;
					int sendLen=0;
					//���÷��ͻ�����
					sendLen = strSendData.GetLength()*2 + 2;
					sendBuf = new char[sendLen];
					ZeroMemory(sendBuf,sendLen);
					wcstombs(sendBuf,strSendData,sendLen);
					sendLen = strlen(sendBuf)+1;
					int sendLen=0;
					sendLen = strlen(Ptr_SendDate->SendDate)+1;
					//��������
					if (!m_tcpClient.SendData(Ptr_SendDate->SendDate,sendLen))
					{
						KillTimer(6);
						AfxMessageBox(_T("����ʧ��"));
						
					}
					else
					{
						if(Ptr_SendDate->next == NULL)
						{
							KillTimer(6);
						}
						Ptr_SendDate = Ptr_SendDate->next;
					}
					//�ͷ��ڴ�
					CDialog::OnTimer(nIDEvent);
				break;
			}*/
//		default:
//			break;
//	}	
}
///////////////////////////////////////////////////////////
//���ڴ�������
///////////////////////////////////////////////////////////
//���崮�ڽ������ݺ�������
void CALLBACK CDraw2DGraphDlg::OnSerialRead(void * pOwner,BYTE* buf,DWORD bufLen)
{
	BYTE *pRecvBuf = NULL; //���ջ�����
	//�õ�������ָ��
	CDraw2DGraphDlg* pThis = (CDraw2DGraphDlg*)pOwner;
	//�����յĻ�����������pRecvBuf��
	pRecvBuf = new BYTE[bufLen];
	CopyMemory(pRecvBuf,buf,bufLen);

	//�����첽��Ϣ����ʾ�յ��������ݣ���Ϣ�����꣬Ӧ�ͷ��ڴ�
	pThis->PostMessage(WM_RECV_SERIAL_DATA,WPARAM(pRecvBuf),bufLen);

}

// ���ڽ������ݴ�����
LONG CDraw2DGraphDlg::OnRecvSerialData(WPARAM wParam,LPARAM lParam)
{
	CString strRecv = L"";
	//���ڽ��յ���BUF
	CHAR *pBuf = (CHAR*)wParam;
	//���ڽ��յ���BUF����
	DWORD dwBufLen = lParam;
    strRecv = CString(pBuf);
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
		if(pos<0)
		{//ֻʣһ����
			num = strRecv;
			strRecv.Empty();
			Ptr_Next_ECG->ECG[ecg] = _ttoi(num);
			if(Ptr_Next_ECG->ECG[ecg]<MIN_SIZE||Ptr_Next_ECG->ECG[ecg]>MAX_SIZE)
				Ptr_Next_ECG->ECG[ecg] = Ptr_Next_ECG->ECG[ecg-1];
			ecg++;
			if(ecg ==ECG_SIZE)
			{
				ecg = 0;
				Ptr_Next_ECG->next = InitECGDataNode();
				Ptr_Next_ECG = Ptr_Next_ECG->next;
			}
		}
		else
		{
			num = strRecv.Left(pos);
			int len = num.GetLength();
			strRecv.Delete(0,pos+1);
			Ptr_Next_ECG->ECG[ecg] = _ttoi(num);
			if(Ptr_Next_ECG->ECG[ecg]<MIN_SIZE||Ptr_Next_ECG->ECG[ecg]>MAX_SIZE)
				Ptr_Next_ECG->ECG[ecg] = Ptr_Next_ECG->ECG[ecg-1];
			ecg++;
			if(ecg ==ECG_SIZE)
			{
				ecg = 0;
				Ptr_Next_ECG->next = InitECGDataNode();
				Ptr_Next_ECG = Ptr_Next_ECG->next;
			}
		}
	}
	//////////////////////////////////////
	//�ͷ��ڴ�
	delete[] pBuf;
	pBuf = NULL;
	return 0;
}
void CDraw2DGraphDlg::OnSeriesRead(CWnd* pWnd,BYTE* buf,int bufLen)
{
	CDraw2DGraphDlg *pDlg = (CDraw2DGraphDlg *)pWnd;
	::PostMessage(pWnd->m_hWnd,WM_COMM_MESSAGE,(WPARAM)buf,(LPARAM)bufLen);
}

LRESULT CDraw2DGraphDlg::OnCommMessage(WPARAM pbuf,LPARAM lbufLen)
{
	CString strRecv = L"";
	//���ڽ��յ���BUF
	CHAR *pBuf = (CHAR*)pbuf;
	//���ڽ��յ���BUF����
	DWORD dwBufLen = lbufLen;
    strRecv = CString(pBuf);
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
		if(pos<0)
		{//ֻʣһ����
			num = strRecv;
			strRecv.Empty();
			Ptr_Next_ECG->ECG[ecg] = _ttoi(num);
			if(Ptr_Next_ECG->ECG[ecg]<MIN_SIZE||Ptr_Next_ECG->ECG[ecg]>MAX_SIZE)
				Ptr_Next_ECG->ECG[ecg] = Ptr_Next_ECG->ECG[ecg-10];
			ecg++;
			if(ecg == ECG_SIZE)
			{
				ecg = 0;
				Ptr_Next_ECG->next = InitECGDataNode();
				Ptr_Next_ECG = Ptr_Next_ECG->next;
			}
		}
		else
		{
			num = strRecv.Left(pos);
			int len = num.GetLength();
			strRecv.Delete(0,pos+1);
			Ptr_Next_ECG->ECG[ecg] = _ttoi(num);
			if(Ptr_Next_ECG->ECG[ecg]<MIN_SIZE||Ptr_Next_ECG->ECG[ecg]>MAX_SIZE)
				Ptr_Next_ECG->ECG[ecg] = Ptr_Next_ECG->ECG[ecg-1];
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
	//�ͷ��ڴ�
	delete[] pBuf;
	pBuf = NULL;
	return 0;
}
//���ݽ����¼�
void CALLBACK CDraw2DGraphDlg::OnRead(CWnd* pWnd,const char * buf,int len )
{

	CDraw2DGraphDlg * pDlg = (CDraw2DGraphDlg*)pWnd;
	TCHAR *wbuf = NULL;
	wbuf = new TCHAR[len];
	if (wbuf == NULL)
	{
		return;
	}
	CUtility::CharpToUnsignedShort(buf,(unsigned short*)wbuf);	
	CString strRecv;
	strRecv.Format(_T("%s"),wbuf);	
	delete [] wbuf;
}
//////////////////////////////////////////////////////////////////////
//TCP��������
//////////////////////////////////////////////////////////////////////
//���ӶϿ��¼�
void CALLBACK CDraw2DGraphDlg::OnDisConnect(void* pOwner)
{
	//�õ�������ָ��
	CDraw2DGraphDlg* pThis = (CDraw2DGraphDlg*)pOwner;
	//������Ϣ��ʾ�ͻ������ӶϿ�
	pThis->PostMessage(WM_TCP_CLIENT_DISCONNECT,0,0);
}
//���ݽ����¼�
void CALLBACK CDraw2DGraphDlg::OnTcpRead(void* pOwner,const char * buf,DWORD dwBufLen )
{
	BYTE *pRecvBuf = NULL; //���ջ�����
	//�õ�������ָ��
	CDraw2DGraphDlg* pThis = (CDraw2DGraphDlg*)pOwner;
	//�����յĻ�����������pRecvBuf��
	pRecvBuf = new BYTE[dwBufLen];
	CopyMemory(pRecvBuf,buf,dwBufLen);
	//�����첽��Ϣ����ʾ�յ�TCP���ݣ���Ϣ�����꣬Ӧ�ͷ��ڴ�
	pThis->PostMessage(WM_RECV_TCP_DATA,WPARAM(pRecvBuf),dwBufLen);
}

//Socket�����¼�
void CALLBACK CDraw2DGraphDlg::OnTcpError(void* pOwner,int nErrorCode)
{
	TRACE(L"�ͻ���socket��������");
}

//TCP�������ݴ�����
LONG CDraw2DGraphDlg::OnRecvTCPData(WPARAM wParam,LPARAM lParam)
{
	CString strOldRecv = L"";
	CString strRecv = L"";
	//���յ���BUF
	CHAR *pBuf = (CHAR*)wParam;
	//���յ���BUF����
	DWORD dwBufLen = lParam;
	strRecv = CString(pBuf);
    ///////////////////////////////////////////////
	//TCP���ݽ��մ���

	///////////////////////////////////////////////
	//�ͷ��ڴ�
	delete[] pBuf;
	pBuf = NULL;
	return 0;
}

//�ͻ������ӶϿ���Ϣ����
LONG CDraw2DGraphDlg::OnClientDisconnect(WPARAM wParam,LPARAM lParam)
{
	AfxMessageBox(_T("���ӶϿ�"));
	return 0;
}
/////////////////////////////////////////////////////////////
//����ť�¼�
/////////////////////////////////////////////////////////////
//��ʼ���
void CDraw2DGraphDlg::OnBnClickedStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	///////////////////////////////////////////////////////////
	//��COM3��COM4�ֱ����������ĵ����ݺ�Ѫѹ-����Ѫ������
	//�жϴ����Ƿ��Ѿ���
	int flag = 0;
	if (m_pSerial1 != NULL)
	{
		m_pSerial1->ClosePort();

		delete m_pSerial1;
		m_pSerial1 = NULL;
	}
	//�½�����ͨѶ����
	m_pSerial1 = new CCESerials();
	m_pSerial1-> m_OnSerialsRead = OnSerialRead; //

	//�򿪴���
	if (m_pSerial1->OpenPort(this,3,9600,NOPARITY,8,0))
	{
		flag++;
//		AfxMessageBox(L"COM3�򿪳ɹ�");
	}
	else
	{
		m_pSerial1->ClosePort();
		AfxMessageBox(L"COM3��ʧ��");
		flag++;
	}
	///////////////////////////////////////////////////////////////
	//COM4
/*	if (m_pSerial2 != NULL)
	{
		m_pSerial2->ClosePort();

//		delete m_pSerial1;
//		m_pSerial1 = NULL;
	}
	m_pSerial2 = new CCESeries();
	m_pSerial2->m_OnSeriesRead = OnSeriesRead;
	if (m_pSerial2->OpenPort(this,4,9600))
	{
		flag++;
//		AfxMessageBox(_T("COM4�򿪳ɹ�"));
		
	}
	else  // �򿪴��ڳɹ�
	{
		m_pSerial2->ClosePort();
		AfxMessageBox(_T("COM4��ʧ��"));
		flag++;
		
	}*/
//	Sleep(50);
	///////////////////////////////////////////////////////////
	//�����̣߳������˲�
/*	if(flag == 2)
	{
		DWORD FilterThreadID;
		HANDLE filterhandle;
		///////////////////////////////////////////////////////////
		//�����̣߳����ڷ������ݵĴ���
//		DWORD SendThreadID;
//		HANDLE Sendhandle;
		filterhandle = CreateThread(NULL,0,FilterThread,NULL,0,&FilterThreadID);
//		Sleep(5);		
//		Sendhandle = CreateThread(NULL,0,SendThread,NULL,0,&SendThreadID);
//		Sleep(5);
//		if(!Sendhandle)
//		{
//			AfxMessageBox(_T("�̴߳���ʧ��"));
//		}
		if(!filterhandle)
		{
			AfxMessageBox(_T("�˲����̴߳���ʧ��"));
		}
	}*/
}
//Զ��ģʽ
void CDraw2DGraphDlg::OnBnClickedRemotemode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	///////////////////////////////////////////////////////////////
	//��÷�����IP��ַ�Ͷ˿ں�
	if (remotemode->DoModal() == IDOK)
	{
		m_remotehost = remotemode-> m_remotehost;
		m_remotehost = _T("192.168.16.1");
		remotemode->m_GPRS.GPRS_ClosePort();
	}
	Sleep(50);
	///////////////////////////////////////////////////////////////
	// ��ʼ�� GPRS ģ��
//	BOOL ret = m_GPRS.GPRS_Init(_T("COM2:"), 115200, (DWORD)this);
//	if (ret == FALSE)
//	{
//		m_GPRS.GPRS_ClosePort();
//		MessageBox(_T("GPRS��ʼ��ʧ��"));
//	}
//	else
//	{//����TCP����
//		m_GPRS.GPRS_SetUpPPPLink();
//		m_remotehost = m_GPRS.GPRS_AskForPPPState();
//		AfxMessageBox(m_remotehost);
//		BOOL ret = m_GPRS.GPRS_SetUpTCPLink(m_remotehost);
//		if(ret == FALSE)
//		{
//			AfxMessageBox(_T("TCP���ӽ���ʧ��"));
//		}
//		else
//		{
//			AfxMessageBox(_T("TCP�����Ѿ�����"));
//		}
		///////////////////////////////////////////////////////
		//��������
//		SetTimer(6,200,NULL);
//	}
//	///////////////////////////////////////////////////////////
//	//�����̣߳����ڷ������ݵĴ���
	DWORD SendThreadID;
	HANDLE Sendhandle;
	//////////////////////////////////////////////////////////
	//��TCP����
	//����m_tcpClient����
	m_tcpClient.m_remoteHost = m_remotehost;
	m_tcpClient.m_port = m_remoteport;
	m_tcpClient.OnDisConnect = OnDisConnect;
	m_tcpClient.OnRead = OnTcpRead;
	m_tcpClient.OnError = OnTcpError;
	//�򿪿ͻ���socket
	m_tcpClient.Open(this);
	//�����������������
	if (m_tcpClient.Connect())
	{
		///////////////////////////////////////////////////////
		//��������
//		SetTimer(6,300,NULL);
		AfxMessageBox(L"��������");
		
		Sendhandle = CreateThread(NULL,0,SendThread,NULL,0,&SendThreadID);
//		Sleep(1000);
		if(!Sendhandle)
		{
			AfxMessageBox(_T("�̴߳���ʧ��"));
		}
	}
	else
	{
		AfxMessageBox(_T("��������ʧ��"));
//		Sendhandle = CreateThread(NULL,0,SendThread,NULL,0,&SendThreadID);
//		Sleep(1000);
//		if(!Sendhandle)
//		{
//		AfxMessageBox(_T("�̴߳���ʧ��"));
//		}
//		AfxMessageBox(_T("�̴߳���"));
		return;
	}
}
//�ĵ���ʾ�봦��
void CDraw2DGraphDlg::OnBnClickedEcg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//������ӵ��ʱ��
//	Sleep(1000);
//	SetTimer(1,500,NULL);
	DWORD ECGThreadID;
	HANDLE ECGThreadhandle;
	ECGThreadhandle = CreateThread(NULL,0,ECGThread,NULL,0,&ECGThreadID);
//	Sleep(1000);
	if(!ECGThreadhandle)
	{
		AfxMessageBox(_T("�̴߳���ʧ��"));
	}
}
//Ѫѹ��ʾ�봦��
void CDraw2DGraphDlg::OnBnClickedBoolpressure()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���ö�ʱ��
//	SetTimer(2,TIMER_ARGS,NULL);
//	SetTimer(3,TIMER_ARGS,NULL);
	///////////////////////////////////////////////////////////
	//�����̣߳�����Ѫѹ��ʾ
	DWORD HBPThreadID;
	HANDLE HBPThreadhandle;
	HBPThreadhandle = CreateThread(NULL,0,HBPThread,NULL,0,&HBPThreadID);
//	Sleep(1000);
	if(!HBPThreadhandle)
	{
		AfxMessageBox(_T("�̴߳���ʧ��"));
	}
	///////////////////////////////////////////////////////////
	//�����̣߳�����Ѫѹ��ʾ
//	DWORD LBPThreadID;
//	HANDLE LBPThreadhandle;
//	LBPThreadhandle = CreateThread(NULL,0,LBPThread,NULL,0,&LBPThreadID);
//	Sleep(1000);
//	if(!LBPThreadhandle)
//	{
//		AfxMessageBox(_T("�̴߳���ʧ��"));
	//}
}
//Ѫ�����Ͷ���ʾ�봦��
void CDraw2DGraphDlg::OnBnClickedSpo2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���ö�ʱ��
//	SetTimer(4,TIMER_ARGS,NULL);
//	Spo2 = TRUE;
	///////////////////////////////////////////////////////////
	//�����̣߳�����Ѫѹ��ʾ
//	DWORD SPO2ThreadID;
//	HANDLE SPO2Threadhandle;
//	SPO2Threadhandle = CreateThread(NULL,0,SPO2Thread,NULL,0,&SPO2ThreadID);
//	Sleep(1000);
//	if(!SPO2Threadhandle)
//	{
//		AfxMessageBox(_T("�̴߳���ʧ��"));
//	}
}
//������ʾ�봦��
void CDraw2DGraphDlg::OnBnClickedPulse()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���ö�ʱ��
//	SetTimer(5,TIMER_ARGS,NULL);
//	Pulse = TRUE;
	///////////////////////////////////////////////////////////
	//�����̣߳�����Ѫѹ��ʾ
//	DWORD PulseThreadID;
//	HANDLE PulseThreadhandle;
//	PulseThreadhandle = CreateThread(NULL,0,PulseThread,NULL,0,&PulseThreadID);
//	Sleep(1000);
//	if(!PulseThreadhandle)
//	{
//		AfxMessageBox(_T("�̴߳���ʧ��"));
//	}
}