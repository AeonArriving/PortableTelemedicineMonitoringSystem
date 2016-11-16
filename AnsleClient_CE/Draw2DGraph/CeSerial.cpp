#include "stdafx.h"
#include "CeSerial.h"

CCeSerial::CCeSerial()
{
	m_hComm = INVALID_HANDLE_VALUE;		/* ���ڲ��������Ч */
	hRecvThread = NULL;
	m_ExitThreadEvent = NULL;			/* ���ڽ����߳��˳��¼���Ч */
}

CCeSerial::~CCeSerial()
{
	ClosePort();
	if (hRecvThread != NULL)
	{
		CloseHandle(hRecvThread);
		SetEvent(m_ExitThreadEvent);				/* ֪ͨ���ڽ����߳��˳� */
		Sleep(200);
		CloseHandle(m_ExitThreadEvent);				/* �ر��߳��˳��¼� */
	}
}


/*******************************************************************************************
��������: CCeSerial::CommRecvTread
��    ��: ���ڽ����߳�
�������: LPVOID lparam: �̲߳���,�����߳�ʱ����
�������: ��
��    ��: 0: �߳��˳�, ����ֵû���⺬��
********************************************************************************************/
DWORD CCeSerial::CommRecvTread(LPVOID lparam)
{
	DWORD dwLength;
	BYTE *recvBuf = new BYTE[1024];
	CCeSerial *pCeSerial = (CCeSerial *)lparam;

	while(TRUE)
	{																/* �ȴ��߳��˳��¼� */
		if (WaitForSingleObject(pCeSerial->m_ExitThreadEvent, 0) == WAIT_OBJECT_0)
			break;	

		if (pCeSerial->m_hComm != INVALID_HANDLE_VALUE)
		{															/* �Ӵ��ڶ�ȡ���� */
			BOOL fReadState = ReadFile(pCeSerial->m_hComm, recvBuf, 1024, &dwLength, NULL);
			if(!fReadState)
			{
				//MessageBox(_T("�޷��Ӵ��ڶ�ȡ����!"));
			}
			else
			{
				if(dwLength != 0)									/* ���ճɹ����ûص����� */
					pCeSerial->OnCommRecv(pCeSerial->m_UserParam, recvBuf, dwLength);		
			}
		}
	}		

	delete[] recvBuf;
	return 0;
}



/*******************************************************************************************
��������: CCeSerial::OpenPort
��    ��: �򿪴���
�������: LPCTSTR Port: ������,��"COM0:","COM1:"
		  int BaudRate: ������
		  int DataBits: ����λ, ȡֵΪ7��8
		  int StopBits: ֹͣλ
		  int Parity  : ��żУ��λ
�������: ��
��    ��: FALSE: ʧ��;    TRUE: �ɹ�
********************************************************************************************/
BOOL CCeSerial::OpenPort(LPCTSTR Port, int BaudRate, int DataBits, int StopBits, int Parity,
						 BOOL bDTRShake, BOOL bRTSShake, DWORD UserParam)
{
	COMMTIMEOUTS CommTimeOuts;

	// �򿪴���
	m_hComm = CreateFile(Port, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if(m_hComm == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("�޷��򿪶˿ڻ�˿��Ѵ�!�����Ƿ��ѱ�ռ��."));
		return FALSE;
	}

	GetCommState(m_hComm, &dcb);						/* ��ȡ���ڵ�DCB */
	dcb.BaudRate = BaudRate;			
	dcb.ByteSize = DataBits;
	dcb.Parity = Parity;
	dcb.StopBits = StopBits;	
	dcb.fParity = FALSE;								/* ��ֹ��żУ�� */
	dcb.fBinary = TRUE;

	if (bDTRShake == FALSE)
		dcb.fDtrControl = 0;							/* ��ֹ�������� */
	else
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;

	if (bRTSShake == FALSE)
		dcb.fRtsControl = 0;
	else
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;

	dcb.fOutX = 0;
	dcb.fInX = 0;
	dcb.fTXContinueOnXoff = 0;
	
	//����״̬����
	SetCommMask(m_hComm, EV_RXCHAR);					/* �����¼�:���յ�һ���ַ� */	
	SetupComm(m_hComm, 16384, 16384);					/* ���ý����뷢�͵Ļ�������С */
	if(!SetCommState(m_hComm, &dcb))					/* ���ô��ڵ�DCB */
	{
		AfxMessageBox(_T("�޷�����ǰ�������ö˿ڣ��������!"));
		ClosePort();
		return FALSE;
	}
		
	//���ó�ʱ����
	GetCommTimeouts(m_hComm, &CommTimeOuts);		
	CommTimeOuts.ReadIntervalTimeout = 100;				/* �����ַ������ʱ���� */
	CommTimeOuts.ReadTotalTimeoutMultiplier = 1;		
	CommTimeOuts.ReadTotalTimeoutConstant = 100;		/* �������ܳ�ʱ���� */
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 0;		
	if(!SetCommTimeouts(m_hComm, &CommTimeOuts))
	{
		AfxMessageBox(_T("�޷����ó�ʱ����!"));
		ClosePort();
		return FALSE;
	}
		
	PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);	 /* �����/�������� */	
	
	m_UserParam = UserParam;							 /* �����û����� */

	// �����̼߳����
	DWORD IDThread;

	/* �������ڽ����߳��˳��¼�*/
	m_ExitThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);		

	// �������ڽ����߳�
	hRecvThread = CreateThread(0, 0, CommRecvTread, this, 0, &IDThread);
	if (hRecvThread == NULL) 
	{
		ClosePort();
		AfxMessageBox(_T("���������߳�ʧ��!"));
		return FALSE;
	}	
	
	return TRUE;		
}



BOOL CCeSerial::IsPortOpen(void)
{
	if(m_hComm != INVALID_HANDLE_VALUE) 
		return TRUE;

	return FALSE;
}


/*******************************************************************************************
��������: CCeSerial::ClosePort
��    ��: �رմ���
�������: ��
�������: ��
��    ��: FALSE: ʧ��;    TRUE: �ɹ�
********************************************************************************************/
BOOL CCeSerial::ClosePort(void)
{
	if(m_hComm != INVALID_HANDLE_VALUE)
	{
		SetCommMask(m_hComm, 0);		
		PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);	/* �����/������ */
		CloseHandle(m_hComm);								/* �رմ��ڲ������ */
		m_hComm = INVALID_HANDLE_VALUE;
		return TRUE;
	}

	return FALSE;
}


/*******************************************************************************************
��������: CCeSerial::OnOpenCom
��    ��: "�رն˿�" ����
�������: ��
�������: ��
��    ��: ��
********************************************************************************************/
void CCeSerial::Senddata(BYTE *psendbuf, DWORD length) 
{
	DWORD dwactlen;

	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("����δ��!"));
		return;
	}

	WriteFile(m_hComm, psendbuf, length, &dwactlen, NULL);	 /* �Ӵ��ڷ������� */
}




