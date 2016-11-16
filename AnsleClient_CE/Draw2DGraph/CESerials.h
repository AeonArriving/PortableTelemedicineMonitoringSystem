#pragma once
//������
//���ڴ����շ�����
class CCESerials
{
public:
	CCESerials(void);
	~CCESerials(void);
	//���崮�ڽ������ݺ�������
public:
	//�򿪴���
	BOOL OpenPort(void* pOwner,/*ָ��ָ��*/
				  UINT portNo	= 1,		/*���ں�*/
				  UINT baud		= 9600,	/*������*/
				  UINT parity	= NOPARITY, /*��żУ��*/
				  UINT databits	= 8,		/*����λ*/
				  UINT stopbits	= 0        /*ֹͣλ*/
				  );
	//�رմ���
	void ClosePort();
	//ͬ��д������
	BOOL WriteSyncPort(const BYTE*buf , DWORD bufLen);
	//���ô��ڶ�ȡ��д�볬ʱ
	BOOL SetSeriesTimeouts(COMMTIMEOUTS CommTimeOuts);
	//�õ������Ƿ��
	BOOL GetComOpened();
	typedef void (CALLBACK* ONSERIALSREAD)(void * pOwner /*������ָ��*/,
										   BYTE* buf  /*���յ��Ļ�����*/ ,
										   DWORD dwBufLen /*���յ��Ļ���������*/);
private:
    //���ڶ��̺߳���
    static  DWORD WINAPI ReadThreadFunc(LPVOID lparam);
private:
	//�رն��߳�
	void CloseReadThread();
private:
    //�Ѵ򿪵Ĵ��ھ��
	HANDLE	m_hComm;
	//���߳̾��
	HANDLE m_hReadThread;
	//���߳�ID��ʶ
	DWORD m_dwReadThreadID;
	//���߳��˳��¼�
	HANDLE m_hReadCloseEvent;
	BOOL m_bOpened; //�����Ƿ��
	void * m_pOwner; //ָ��������ָ��
public:
	ONSERIALSREAD m_OnSerialsRead;
};
