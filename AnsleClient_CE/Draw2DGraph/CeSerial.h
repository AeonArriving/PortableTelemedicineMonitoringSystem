#pragma once
//������
//���ڷ��Ͷ��źͽ���PPP/TCP����
class CCeSerial  
{
public:
	CCeSerial();
	virtual ~CCeSerial();

	// �򿪴���
	BOOL OpenPort(LPCTSTR Port, 
				  int BaudRate, 
				  int DataBits, 
				  int StopBits, 
				  int Parity,
				  BOOL bDTRShake, 
				  BOOL bRTSShake, 
				  DWORD UserParam);

	// �����Ƿ��
	BOOL IsPortOpen();
	
	// �رմ���
	BOOL ClosePort(void);
	
	// ��������
	void Senddata(BYTE *sendbuf, DWORD length);

	// ���ڽ������ݳɹ��ص�����
	typedef void (CALLBACK *ONCOMMRECV)(DWORD UserParam, BYTE *buf, DWORD buflen);
	ONCOMMRECV OnCommRecv;

	DWORD m_UserParam;

private:

	DCB dcb;								/* ���ڲ����ṹ�� */
	HANDLE hRecvThread;						/* �����߳̾�� */
    HANDLE m_ExitThreadEvent;				/* ���ڽ����߳��˳��¼� */

	HANDLE m_hComm;							/* ���ڲ������ */

	// ���ڽ����߳�
	static DWORD CommRecvTread(LPVOID lparam);
};