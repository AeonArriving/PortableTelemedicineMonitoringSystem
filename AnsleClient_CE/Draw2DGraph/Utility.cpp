#include "stdafx.h"
#include "Utility.h"

CUtility::CUtility()
{

}

CUtility::~CUtility()
{

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ���ݱ���ת��
//

void CUtility::UnsignedShortToCharp(unsigned short *srcstr,char *deststr)
{ 
	UINT  nStrULength=WideCharToMultiByte(CP_ACP,0,(LPCWSTR)srcstr,-1,NULL,NULL,NULL,NULL);	 
	WideCharToMultiByte(CP_ACP,0,(LPCWSTR)srcstr,-1,deststr,nStrULength,NULL,NULL);   
}

void CUtility::CharpToUnsignedShort(const char *srcstr, unsigned short *deststr)
{    
	
	int nStatus=MultiByteToWideChar(CP_ACP,0,srcstr,-1,(LPWSTR)deststr,255); 
}
