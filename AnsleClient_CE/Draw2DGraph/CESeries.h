#pragma once

//CE����ͨѶ��
//ר���ڲ������ӵĽ���
class CCESeries
{

public:
	CCESeries();
	virtual ~CCESeries();
public:
	//�򿪴���
	BOOL OpenPort(CWnd* pPortOwner,			/*ʹ�ô����࣬������*/
				  UINT portNo	= 1,		/*���ں�*/
				  UINT baud		= 9600,	/*������*/
				  UINT parity	= NOPARITY, /*��żУ��*/
				  UINT databits	= 8,		/*����λ*/
				  UINT stopbits	= 0        /*ֹͣλ*/
				  );
	//�رմ���
	void ClosePort();
	//���ô��ڶ�ȡ��д�볬ʱ
	BOOL SetSeriesTimeouts(COMMTIMEOUTS CommTimeOuts);
	//�򴮿�д������
	BOOL WritePort(const BYTE *buf,DWORD bufLen);
	//���崮�ڽ������ݺ�������
typedef void (CALLBACK* ONSERIESREAD)(CWnd*,BYTE* buf,int bufLen);
private:
    //���ڶ��̺߳���
    static  DWORD WINAPI ReadThreadFunc(LPVOID lparam);
	//����д�̺߳���
    static DWORD WINAPI WriteThreadFunc(LPVOID lparam);

	//�򴮿�д������
	static BOOL WritePort(HANDLE hComm,const BYTE *buf,DWORD bufLen);

	//�رն��߳�
	void CloseReadThread();
	//�ر�д�߳�
	void CloseWriteThread();
private:
    //�Ѵ򿪵Ĵ��ھ��
	HANDLE	m_hComm;
	CWnd* m_pPortOwner;

	//��д�߳̾��
	HANDLE m_hReadThread;
	HANDLE m_hWriteThread;

	//��д�߳�ID��ʶ
	DWORD m_dwReadThreadID;
	DWORD m_dwWriteThreadID;

	//���߳��˳��¼�
	HANDLE m_hReadCloseEvent;
	//д�߳��˳��¼�
	HANDLE m_hWriteCloseEvent;
public:
	ONSERIESREAD m_OnSeriesRead; //���ڶ�ȡ�ص�����
};
