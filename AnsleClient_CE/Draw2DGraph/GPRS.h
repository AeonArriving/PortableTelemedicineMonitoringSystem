#pragma once

#include "CeSerial.h"

class CGPRS  
{
public:
	CGPRS();
	virtual ~CGPRS();
	BOOL GPRS_Init(LPCTSTR Port, int BaudRate, DWORD UserParam);
	// GPRS �ص�����
	typedef void (CALLBACK *ONGPRSRECV)(DWORD UserParam, DWORD Status, CString strData);
	ONGPRSRECV OnGPRSRecv;
	BOOL GPRS_DialUp(CString strTelNum);	
	BOOL GPRS_DialDown(void);
	BOOL GPRS_TakeTheCall(void);
	BOOL GPRS_HangUpTheCall(void);
	BOOL GPRS_DeleteShortMsg(DWORD num);
	BOOL GPRS_ReadShortMessage(DWORD num, CString *strTelCode, CString *Msg);
	BOOL GPRS_SendShortMessage(CString strTelNum, CString Msg);
	BOOL GPRS_SetShortMSGCenterTel(CString strTelNum);
	BOOL GPRS_CheckMsgNum(CString *strNum, CString *strTotal);
	BOOL GPRS_SetUpPPPLink(void);
	BOOL GPRS_SetUpTCPLink(CString remoteIP);
	BOOL GPRS_SendData(CString senddata,CString num,CString len);
	BOOL GPRS_CloseTCPLink(void);
	CString GPRS_AskForPPPState(void);
	void GPRS_ClosePort(void);
private:
	CCeSerial *pCeSerial;
	DWORD m_UserParam;							/* ʹ�ñ�ģ���û� */
	HANDLE m_ATCmdRespEvent;					/* AT ����ظ��¼� */	
	CString m_strRespBuf;						/* �ظ��������ַ��� */
	DWORD m_RespCnt;
	BOOL bSendATCmd;							/* �Ƿ��������� */
	void ResetGlobalVarial(void);
	BOOL GPRS_SendATCmd(CString strCmd);		/* ���� AT ���� */
	BOOL GPRS_WriteMsg(CString Msg);			/* ���Ͷ������� */
	BOOL GPRS_WaitResponseOK(void);				/* �ȴ� AT �������Ӧ */
	CString GPRS_GetTheCallNum(CString str);	/* ��ȡ������ĵ绰�ĺ��� */
	int FindMsgPos(int *posnum, int *numlen, int *posmsg, int *msglen);
	static void CALLBACK OnCommRecv(DWORD UserParam, BYTE *buf, DWORD buflen);
};

// GPRS ״̬��
#define GPRS_STATUS_RECEIVE_CALL			0x01		/* �ӵ����� */
#define GPRS_STATUS_OTHER_SIDE_HANDUP		0x02		/* �Է��һ� */