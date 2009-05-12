// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__4BCBF644_77AA_49C4_891F_9D8E0699BB9A__INCLUDED_)
#define AFX_STDAFX_H__4BCBF644_77AA_49C4_891F_9D8E0699BB9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_DCOM
#define _WIN32_DCOM 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <afxwin.h>
#include <afxdisp.h>
#include <AFXINET.H>

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

#include <afxsock.h>		// MFC socket extensions
#include <atlhost.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__4BCBF644_77AA_49C4_891F_9D8E0699BB9A__INCLUDED)
