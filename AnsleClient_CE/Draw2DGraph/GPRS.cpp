#include "stdafx.h"
#include "GPRS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGPRS::CGPRS()
{
	ResetGlobalVarial();
}

CGPRS::~CGPRS()
{
	if (pCeSerial->IsPortOpen() == TRUE)
	{
		pCeSerial->ClosePort();
	}

	if (pCeSerial)
		delete pCeSerial;
}


/*******************************************************************************************
��������: ResetGlobalVarial
��    ��: ��λһЩȫ�ֱ���ΪĬ��ֵ
�������: ��
�������: ��
��    ��: ��
********************************************************************************************/
void CGPRS::ResetGlobalVarial(void)
{
	bSendATCmd = FALSE;					/* ��ǰ�����ڷ�������״̬ */	
	m_RespCnt = 0;							
	m_strRespBuf = _T("");
	ResetEvent(m_ATCmdRespEvent);		/* ��λ GPRS ��Ӧ�¼�*/
}



/*******************************************************************************************
��������: GPRS_Init
��    ��: GPRS ģ���ʼ������
�������: LPCTSTR Port   : ���ں�, �� "COM1:"
		  int BaudRate   : ���ڲ�����, �� 115200
		  DWORD UserParam: �û����ݵ������еĲ���
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
********************************************************************************************/
BOOL CGPRS:: GPRS_Init(LPCTSTR Port, int BaudRate, DWORD UserParam)
{	
	BOOL ret;
	DWORD Trycount = 2;

	do{																/* ���Գ�ʼ�� GPRS, ��ೢ������ */
		Trycount--;

		pCeSerial = new CCeSerial;
		ret = pCeSerial->OpenPort(Port, BaudRate, 
								  8, ONESTOPBIT, 
								  NOPARITY, TRUE, 
								  TRUE, (DWORD)this
								  );
		if (ret == FALSE)											/* �򿪴���, ����λΪ8,ֹͣλΪ1,��У��λ */
		{
			delete pCeSerial;
			return FALSE;
		}

		pCeSerial->OnCommRecv = OnCommRecv;							/* ���ڽ��ճɹ��ص����� */
		m_UserParam = UserParam;									/* �����û��������Ĳ��� */
		m_ATCmdRespEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	/* ���� GPRS ��Ӧ�¼� */
		for(int i=0;i<=1;i++)
		{
			GPRS_SendATCmd(_T("AT"));									/* ����ģ���Ƿ��ڼ���״̬ */
			GPRS_WaitResponseOK();
		}
		GPRS_SendATCmd(_T("AT+IPR=115200"));							
		ret = GPRS_WaitResponseOK();
		if (ret == FALSE)
		{
			delete pCeSerial;
			if (Trycount == 0) return FALSE;
		}
		else
			Trycount = 0;

	}while(Trycount > 0);

	Sleep(100);
	for(int i=0;i<=1;i++)
	{
		GPRS_SendATCmd(_T("AT+CREG=1"));	
		ret = GPRS_WaitResponseOK();
	}
	if (ret == FALSE)
	{
//		AfxMessageBox(_T("AT+CREG=1ERROR"));
		return FALSE;
	}
	for(int i=0;i<=1;i++)
	{
		GPRS_SendATCmd(_T("AT+CLIP=1"));// ����������ʾ 
		ret = GPRS_WaitResponseOK();		
	}
	if (ret == FALSE)
	{
//		AfxMessageBox(_T("AT+CLIP=1ERROR"));
		return FALSE;
	}
	for(int i=0;i<=1;i++)
	{
		GPRS_SendATCmd(_T("AT+CMGF=1"));//����Ϊ�ı�ģʽ 
		ret = GPRS_WaitResponseOK();
	}
	if (ret == FALSE)
	{
//		AfxMessageBox(_T("AT+CMGF=1ERROR"));
		return FALSE;
	}	
	for(int i=0;i<=1;i++)
	{
		GPRS_SendATCmd(_T("AT+CSCS=\"GSM\""));//�����ַ��� 
		ret = GPRS_WaitResponseOK();
	}
	if (ret == FALSE)
	{
//		AfxMessageBox(_T("AT+CSCS=\"GSM\"ERROR"));
		return FALSE;
	}

	return TRUE;
}
void CGPRS::GPRS_ClosePort()
{
	if (pCeSerial->IsPortOpen() == TRUE)
	{
		pCeSerial->ClosePort();
	}

	if (pCeSerial)
		delete pCeSerial;
}

/*******************************************************************************************
��������: OnCommRecv
��    ��: ���ڽ��ճɹ��ص�����, ��������ȷ���յ�����ʱ, �ú�����������
�������: CString strCmd: ������
�������: DWORD UserParam : �û����ݵĲ���, ͨ��ΪCGPRS���һ�������ָ��
          BYTE *buf		  : ���ڽ������ݻ���
		  DWORD buflen	  : ���ڽ��յ������ݳ���
��    ��: ��
********************************************************************************************/
void CALLBACK CGPRS::OnCommRecv(DWORD UserParam, BYTE *buf, DWORD buflen)
{
	CGPRS *pGPRS = (CGPRS *)UserParam;	
	
	CString strTmp = _T("");
	LPTSTR pStr = strTmp.GetBuffer(buflen);	
	MultiByteToWideChar(CP_ACP, 0, (char *)buf, buflen, pStr, buflen); /* �����յ�������ת��Ϊ Unicode ���� */
	strTmp.ReleaseBuffer();

	if (pGPRS->bSendATCmd == TRUE)
	{																	/* ֮ǰ�û������� AT ָ�� */
		pGPRS->m_strRespBuf.Insert(pGPRS->m_RespCnt, strTmp);			/* �������� */	
		SetEvent(pGPRS->m_ATCmdRespEvent);
		return;
	}
	
	if ((strTmp.Find(_T("RING")) >= 0) || 
		(strTmp.Find(_T("ring")) >= 0))
	{													/* �е绰����� */			
		CString strCallNum = pGPRS->GPRS_GetTheCallNum(strTmp);
		pGPRS->OnGPRSRecv(pGPRS->m_UserParam, GPRS_STATUS_RECEIVE_CALL,strCallNum);
	}
	
	if ((strTmp.Find(_T("NO CARRIER")) >= 0) || 
		(strTmp.Find(_T("no carrier")) >= 0))				
	{													/* �����, �Է��һ� */
		pGPRS->OnGPRSRecv(pGPRS->m_UserParam, GPRS_STATUS_OTHER_SIDE_HANDUP, L"");
	}	

	pGPRS->bSendATCmd = FALSE;							/* ��λ���ȫ�ֱ��� */
	pGPRS->m_RespCnt = 0;							
	pGPRS->m_strRespBuf = _T("");
	ResetEvent(pGPRS->m_ATCmdRespEvent);
}


/*******************************************************************************************
��������: GPRS_GetTheCallNum
��    ��: ȡ������ĵ绰����
�������: CString str:  ����ʱ, GPRS ��Ӧ�ַ�
�������: ��
��    ��: ����绰����
********************************************************************************************/
CString CGPRS::GPRS_GetTheCallNum(CString str)
{
	int pos1 = str.Find(_T("+CLIP: \""), 0);
	if (pos1 < 0)
		return L"";
	else
		pos1 = pos1 + wcslen(_T("+CLIP: \""));

	int pos2 = str.Find(_T("\""), pos1);
	if (pos2 < 0)
		return L"";

	CString strNum;
	strNum = str.Mid(pos1, pos2 - pos1);				  /* ȡ�õ绰���� */

	return strNum;
}


/*******************************************************************************************
��������: GPRS_SendATCmd
��    ��: ���� AT ����
�������: CString strCmd: ������
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
********************************************************************************************/
BOOL CGPRS::GPRS_SendATCmd(CString strCmd)
{
	int len = strCmd.GetLength();
	BYTE *psendbuf = new BYTE[len + 2];
	
	m_RespCnt = 0;
	bSendATCmd = TRUE;								 /* ���뷢������״̬ */

	for(int i = 0; i < len;i++)
		psendbuf[i] = (char)strCmd.GetAt(i);		 /* ת��Ϊ���ֽ��ַ� */

	psendbuf[len] = '\r';							 /* ��������� */
	psendbuf[len + 1] = '\n';
	pCeSerial->Senddata(psendbuf, len + 2);			 /* �Ӵ��ڷ������� */

	delete[] psendbuf;
	return TRUE;
}


/*******************************************************************************************
��������: GPRS_WaitResponseOK
��    ��: �ȴ� AT ������Ӧ OK
�������: ��
�������: ��
��    ��: ��
********************************************************************************************/
BOOL CGPRS::GPRS_WaitResponseOK(void)
{
	DWORD ret;
	BOOL bOK = TRUE;

	while(1)
	{	// �ȴ�** ��, ����ò����ظ�, ��Ϊ����
		ret = WaitForSingleObject(m_ATCmdRespEvent, 1500);
		if (ret == WAIT_OBJECT_0)
		{
			if ((m_strRespBuf.Find(_T("ERROR\r\n"), 0) >= 0) ||
				(m_strRespBuf.Find(_T("error\r\n"), 0) >= 0))
			{													/* ��Ӧ����"ERROR" */
				bOK = FALSE;
				break;
			}
			if ((m_strRespBuf.Find(_T("OK\r\n"), 0) >= 0) ||
				(m_strRespBuf.Find(_T("ok\r\n"), 0) >= 0))
			{													/* ��Ӧ����"OK" */
				bOK = TRUE;
				break;
			}

			ResetEvent(m_ATCmdRespEvent);
		}
		else
		{
			bOK = FALSE;										/* ��Ӧ��ʱ */
			break;
		}
	}

	ResetGlobalVarial();								/* ��λ�õ���ȫ�ֱ��� */
	if (FALSE == bOK)
		return FALSE;

	return TRUE;
}
/*******************************************************************************************
��������: GPRS_SetUpPPPLink
��    ��: ����PPP����
�������: ��
�������: ��
��    ��: ��
********************************************************************************************/
BOOL CGPRS::GPRS_SetUpPPPLink(void)
{
	BOOL ret;
	GPRS_SendATCmd(_T("AT+CGDCONT=1,\"IP\",\"CMNET\""));
	ret = GPRS_WaitResponseOK();
	if(ret != TRUE)
	{
		return FALSE;
	}
	Sleep(200);
	GPRS_SendATCmd(_T("AT+XIIC=1"));
	ret = GPRS_WaitResponseOK();
	if(ret != TRUE)
	{
		return FALSE;
	}
/*
	GPRS_SendATCmd(_T("AT+XIIC=1"));
	ret = GPRS_WaitResponseOK();
	if(ret != TRUE)
	{
		return FALSE;
	}
	*/
	return TRUE;
	
}
/*******************************************************************************************
��������: GPRS_AskForPPPState()
��    ��: ��ѯPPP����״̬
�������: ��
�������: ��
��    ��: ��
********************************************************************************************/
CString CGPRS::GPRS_AskForPPPState()
{
	CString nationalIP;
	DWORD ret,retvalue;
	GPRS_SendATCmd(_T("AT+XIIC?"));
	while(1)
	{
		ret = WaitForSingleObject(m_ATCmdRespEvent, 2000);
		if (ret == WAIT_OBJECT_0)
		{
			if (m_strRespBuf.Find(_T("ERROR\r\n"), 0) >= 0)		    /* ��ӦΪ ERROR */
			{
				retvalue = FALSE;
				break;
			}
			else
			{
				int pos1;
				pos1 = m_strRespBuf.Find(_T(","), 0);
				int pos2;
				pos2 = m_strRespBuf.Find(_T("\r\n"), pos1+1);
				nationalIP = m_strRespBuf.Mid(pos1+1,pos2-pos1-1);
				retvalue = TRUE;
				break;
			}
			ResetEvent(m_ATCmdRespEvent);
			
		}
		else
		{
			retvalue = FALSE;
			break;
		}
	}

	ResetGlobalVarial();
	if (retvalue == FALSE)
		return _T("FALSE");

	return nationalIP;
}

BOOL CGPRS::GPRS_SetUpTCPLink(CString remoteIP)
{
	BOOL ret,retvalue;
	GPRS_SendATCmd(_T("AT+TCPSETUP=0,")+remoteIP+_T(",5000"));
	while(1)
	{
		ret = WaitForSingleObject(m_ATCmdRespEvent, 2000);
		if (ret == WAIT_OBJECT_0)
		{
			if (m_strRespBuf.Find(_T("ERROR\r\n"), 0) >=0)		    /* ��ӦΪ ERROR */
			{
				retvalue = FALSE;
				break;
			}
			else
			{
				if(m_strRespBuf.Find(_T("+TCPSETUP"), 0)>=0 && m_strRespBuf.Find(_T(",OK"), 0)>=0)
					AfxMessageBox(L"TCP���ӽ����ɹ�");
					retvalue = TRUE;
				break;
			}
			ResetEvent(m_ATCmdRespEvent);		
		}
		else
		{
			retvalue = FALSE;
			break;
		}
	}

	ResetGlobalVarial();
	if (retvalue == FALSE)
		return FALSE;
	return TRUE;
}
BOOL CGPRS::GPRS_SendData(CString senddata,CString num,CString len)
{
	BOOL ret,retvalue;
	GPRS_SendATCmd(_T("AT+TCPSEND=")+num+L","+len);
	while(1)
	{
		ret = WaitForSingleObject(m_ATCmdRespEvent, 500);
		if (ret == WAIT_OBJECT_0)
		{
			if (m_strRespBuf.Find(_T(">"), 0) >=0)
			{
				GPRS_SendATCmd(senddata);
				ret = GPRS_WaitResponseOK();
				if(ret != TRUE)
				{
					retvalue = FALSE;
					break;
				}
			}
			else
			{
				retvalue = FALSE;
				break;;
			}
			ResetEvent(m_ATCmdRespEvent);
		}
		else
		{
			retvalue = FALSE;
			break;
		}
	}
	ResetGlobalVarial();
	if (retvalue == FALSE)
		return FALSE;
	return TRUE;
}
BOOL CGPRS::GPRS_CloseTCPLink()
{
	BOOL ret,retvalue;
	GPRS_SendATCmd(_T("AT+TCPCLOSE=0"));
	while(1)
	{
		ret = WaitForSingleObject(m_ATCmdRespEvent, 2000);
		if (ret == WAIT_OBJECT_0)
		{
			if (m_strRespBuf.Find(_T("ERROR\r\n"), 0) >=0)		    /* ��ӦΪ ERROR */
			{
				retvalue = FALSE;
				break;
			}
			else
			{
				if(m_strRespBuf.Find(_T("+TCPCLOSE"), 0)>=0 && m_strRespBuf.Find(_T(",OK"), 0)>=0)
					AfxMessageBox(L"TCP���ӳɹ��ر�");
					retvalue = TRUE;
				break;
			}
			ResetEvent(m_ATCmdRespEvent);		
		}
		else
		{
			retvalue = FALSE;
			break;
		}
	}

	ResetGlobalVarial();
	if (retvalue == FALSE)
		return FALSE;
	return TRUE;
}
/*******************************************************************************************
��������: GPRS_WriteMsg
��    ��: ����������д�� GPRS ģ��
�������: CString Msg:  ��������
�������: ��
��    ��: ��
********************************************************************************************/
BOOL CGPRS::GPRS_WriteMsg(CString Msg)
{
	int len = Msg.GetLength();
	BYTE *psendbuf = new BYTE[len + 3];

	for(int i = 0; i < len;i++)
		psendbuf[i] = (char)Msg.GetAt(i);			/* ת��Ϊ���ֽ��ַ� */

	psendbuf[len] = '\r';						    /* ��������� */
	psendbuf[len + 1] = '\n';
	psendbuf[len + 2] = 0x1A;
	pCeSerial->Senddata(psendbuf, len + 3);			/* �Ӵ��ڷ������� */

	delete[] psendbuf;
	return TRUE;
}


/*******************************************************************************************
��������: GPRS_DialUp
��    ��: ����绰(�ɱ������)
�������: CString strTelNum: �绰����
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
********************************************************************************************/
BOOL CGPRS::GPRS_DialUp(CString strTelNum)
{
	CString strDialup = _T("ATD");
	strDialup += strTelNum + L";";					/* �Էֺ���Ϊ���� */
	bSendATCmd = TRUE;
	GPRS_SendATCmd(strDialup);						/* ���Ͳ������� */

	return (GPRS_WaitResponseOK());
}


/*******************************************************************************************
��������: GPRS_DialDown
��    ��: �һ�(�ɱ�������Ĺһ�)
�������: ��
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
********************************************************************************************/
BOOL CGPRS::GPRS_DialDown(void)
{
	GPRS_SendATCmd(_T("ATH"));
	bSendATCmd = TRUE;
	return (GPRS_WaitResponseOK());
}


/*******************************************************************************************
��������: GPRS_TakeTheCall
��    ��: �����绰(�е绰�����, �����õ绰)
�������: ��
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
********************************************************************************************/
BOOL CGPRS::GPRS_TakeTheCall(void)
{
	GPRS_SendATCmd(_T("ATA"));
	bSendATCmd = TRUE;
	return (GPRS_WaitResponseOK());
}


/*******************************************************************************************
��������: GPRS_TakeTheCall
��    ��: �Ҷϵ绰(�е绰�����ʱ, �Ҷϵ绰)
�������: ��
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
********************************************************************************************/
BOOL CGPRS::GPRS_HangUpTheCall(void)
{
	GPRS_SendATCmd(_T("AT+CHUP"));
	bSendATCmd = TRUE;
	return (GPRS_WaitResponseOK());
}


/*******************************************************************************************
��������: GPRS_SetShortMSGCenterTel
��    ��: ���ö������ĺ���
�������: CString strTelNum:  �������ĺ���
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
********************************************************************************************/
BOOL CGPRS::GPRS_SetShortMSGCenterTel(CString strTelNum)
{
	CString strTmp;

	strTmp = strTelNum + _T(",145");
	bSendATCmd = TRUE;
	GPRS_SendATCmd(strTelNum);

	return (GPRS_WaitResponseOK());
}


/*******************************************************************************************
��������: FindMsgPos
��    ��: ���ö������ĺ���
�������: CString strTelNum:  �������ĺ���
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
********************************************************************************************/
int CGPRS::FindMsgPos(int *posnum, int *numlen, int *posmsg, int *msglen)
{
	// ���ſ�ʼλ��
	int posStart = m_strRespBuf.Find(_T("+CMGR:"), 0);					
	if (posStart < 0)
		return -1;

	// �绰���뿪ʼλ��
	*posnum = m_strRespBuf.Find(_T("\"REC READ\",\""), posStart);	
	if (*posnum < 0)
		return -1;

	*posnum = *posnum + wcslen(_T("\"REC READ\",\""));			

	// �绰�������λ��
	int posnumend = m_strRespBuf.Find(_T("\""), *posnum + 1);
	if (posnumend < 0)
		return -1;

	// �绰���볤��
	*numlen = posnumend - *posnum;				
	
	// ���ſ�ʼλ��
	*posmsg = m_strRespBuf.Find(_T("\r\n"), posStart);				
	if (*posmsg < 0)
		return -1;

	*posmsg = *posmsg + wcslen(_T("\r\n"));	
	
	// ���Ž���λ��
	int posmsgend = m_strRespBuf.Find(_T("\r\n\r\nOK"), *posmsg);
	*msglen = posmsgend - *posmsg;
	
	return 1;
}
/*******************************************************************************************
��������: GPRS_ReadShortMessage
��    ��: ��ȡ��������
�������: DWORD num		: �������
�������: CString *Msg	: ��������
��    ��: TRUE: �ɹ�   FALSE: ʧ��
********************************************************************************************/
BOOL CGPRS::GPRS_ReadShortMessage(DWORD num, CString *strTelCode, CString *Msg)
{
	CString strReadNum;
	BOOL ret, retvalue;

	strReadNum.Format(_T("AT+CMGR=%d"), num);
	bSendATCmd = TRUE;
	GPRS_SendATCmd(strReadNum);									    /* ����: ���� num ������ */

	while(1)
	{
		ret = WaitForSingleObject(m_ATCmdRespEvent, 2000);
		if (ret == WAIT_OBJECT_0)
		{
			if (m_strRespBuf.Find(_T("ERROR\r\n"), 0) >= 0)		    /* ��ӦΪ ERROR */
			{
				//AfxMessageBox(_T("��ȡ���ŷ��ִ���"));
				retvalue = FALSE;
				break;
			}
			
			int posnum, numlen, posmsg, msglen;	
			int pos = FindMsgPos(&posnum, &numlen, &posmsg, &msglen);	/* Ѱ�Ҷ���λ�� */
			if (pos > 0)
			{
				*strTelCode = m_strRespBuf.Mid(posnum, numlen);
				*Msg = m_strRespBuf.Mid(posmsg, msglen);				/* ȡ���������� */
				retvalue = TRUE;
				break;
			}
			
			ResetEvent(m_ATCmdRespEvent);
			
		}//end of if (ret == WAIT_OBJECT_0)
		else
		{
			//AfxMessageBox(_T("��ȡ���ų�ʱ"));
			retvalue = FALSE;
			break;
		}
	}

	ResetGlobalVarial();
	if (retvalue == FALSE)
		return FALSE;

	return TRUE;
}


/*******************************************************************************************
��������: GPRS_SendShortMessage
��    ��: ���Ͷ���
�������: CString strTelNum : �Է��绰����
�������: CString *Msg		: ��������
��    ��: TRUE: �ɹ�   FALSE: ʧ��
********************************************************************************************/
BOOL CGPRS::GPRS_SendShortMessage(CString strTelNum, CString Msg)
{
	BOOL ret, retvalue;
	int pos;

	/* 
	*	1. ���͵绰����
	*/
	CString strNum;
	strNum.Format(_T("AT+CMGS=\"%s\""), strTelNum);
	bSendATCmd = TRUE;
	GPRS_SendATCmd(strNum);									/* ���͵绰���� */

	while(1)
	{
		ret = WaitForSingleObject(m_ATCmdRespEvent, 2000);
		if (ret == WAIT_OBJECT_0)	
		{	
			pos = m_strRespBuf.Find(_T('>'), 0);
			if (pos > 0)
			{												/* �յ���ʾ��, ���ڿ�����ģ��д����������� */
				retvalue = TRUE;
				break;
			}
			pos = m_strRespBuf.Find(_T("ERROR\r\n"), 0);
			if (pos > 0)
			{
				retvalue = FALSE;							/* GPRS ��Ӧ�ַ��а�����"ERROR" */
				break;
			}
		}
		else
		{
			retvalue = FALSE;
			break;
		}
	}

	ResetGlobalVarial();
	if (retvalue == FALSE) return FALSE;

	/* 
	*	2. �ȴ� '>' ��ʾ��, ���Ͷ�������
	*/
	bSendATCmd = TRUE;
	GPRS_WriteMsg(Msg);	

	while(1)
	{
		ret = WaitForSingleObject(m_ATCmdRespEvent,500);
		if (ret == WAIT_OBJECT_0)	
		{
			int pos = m_strRespBuf.Find(_T("ERROR\r\n"), 0);
			if (pos > 0)
			{
				retvalue = FALSE;
				break;
			}
			pos = m_strRespBuf.Find(_T("CMGS"), 0);
			if (pos > 0)
			{												/* ��Ӧ��ȷ, ���ͳɹ� */
				retvalue = TRUE;
				break;
			}
		}
	}
	
	ResetGlobalVarial();
	if (retvalue == FALSE)
		return FALSE;

	return TRUE;
}


/*******************************************************************************************
��������: CheckMsgNum
��    ��: ����������: SIN������Ч���������������ݵ�����������(private����)
�������: CString str	   : GPRS ��Ӧ�ַ�
�������: CString *strNum  : ��Ч��������
		  CString *strTotal: �����ݵ�����������
��    ��: TRUE: �ɹ�   FALSE: ʧ��
********************************************************************************************/
BOOL CheckMsgNum(CString str, CString *strNum, CString *strTotal)
{
	int pos1 = str.Find(_T("\"SM\","), 0);			/* ��ʶ�ַ� */
	if (pos1 < 0)
		return FALSE;

	int pos2 = str.Find(_T(','), pos1 + 5);			/* �ָ����� */
	if (pos2 < 0)
		return FALSE;

	*strNum = str.Mid(pos1 + 5, pos2 - (pos1 + 5));	/* ���ж������� */

	int pos3 = str.Find(_T(","), pos2 + 1);
	if (pos3 < 0)
		return FALSE;

	*strTotal = str.Mid(pos2 + 1, pos3 - pos2 - 1); /* �����ݵ����������� */

	return TRUE;
}


/*******************************************************************************************
��������: GPRS_CheckMsgNum
��    ��: ����������: SIN������Ч���������������ݵ�����������(public����)
�������: ��
�������: CString *strNum  : ��Ч��������
		  CString *strTotal: �����ݵ�����������
��    ��: TRUE: �ɹ�   FALSE: ʧ��
********************************************************************************************/
BOOL CGPRS::GPRS_CheckMsgNum(CString *strNum, CString *strTotal)
{	
	BOOL ret, retvalue;
	CString strTmp;

	bSendATCmd = TRUE;
	GPRS_SendATCmd(_T("AT+CPMS?"));									/* ��ѯ���� */

	while(1)
	{
		ret = WaitForSingleObject(m_ATCmdRespEvent, 500);
		if (ret == WAIT_OBJECT_0)	
		{
			int pos = m_strRespBuf.Find(_T("ERROR\r\n"), 0);
			if (pos > 0)
			{
				retvalue = FALSE;
				break;
			}

			if (CheckMsgNum(m_strRespBuf, strNum, strTotal) == TRUE)
			{														/* ��ѯ�ɹ� */
				retvalue = TRUE;
				break;
			}
			ResetEvent(m_ATCmdRespEvent);
		}
		else
		{
			retvalue = FALSE;
			break;			
		}
	}

	ResetGlobalVarial();
	if (retvalue == FALSE) 
		return FALSE;

	return TRUE;
}


/*******************************************************************************************
��������: GPRS_DeleteShortMsg
��    ��: ɾ������
�������: ��
�������: DWORD num  : �������
��    ��: TRUE: �ɹ�   FALSE: ʧ��
********************************************************************************************/
BOOL CGPRS::GPRS_DeleteShortMsg(DWORD num)
{
	CString strCmd;
	BOOL ret;

	strCmd.Format(_T("AT+CMGD=%d"), num);

	GPRS_SendATCmd(strCmd);
	ret = GPRS_WaitResponseOK();
	if (ret == FALSE)
		return FALSE;

	return TRUE;
}