// FileRecovery.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFileRecoveryApp:
// �йش����ʵ�֣������ FileRecovery.cpp
//

class CFileRecoveryApp : public CWinApp
{
public:
	CFileRecoveryApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFileRecoveryApp theApp;