
// DSPDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDSPDemoApp:
// �йش����ʵ�֣������ DSPDemo.cpp
//

class CDSPDemoApp : public CWinAppEx
{
public:
	CDSPDemoApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDSPDemoApp theApp;