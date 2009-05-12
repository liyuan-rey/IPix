// IPixCOM.h : Declaration of the CIPixCOM

#ifndef __IPIXCOM_H_
#define __IPIXCOM_H_

#include "resource.h"       // main symbols

#include <Docobj.h>			// IOleCommandTarget define

#include <fstream.h>

/////////////////////////////////////////////////////////////////////////////
// CIPixCOM
class ATL_NO_VTABLE CIPixCOM : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CIPixCOM, &CLSID_IPixCOM>,
	public IOleCommandTarget,
	public IObjectWithSiteImpl<CIPixCOM>,
	public IDispatchImpl<IIPixCOM, &IID_IIPixCOM, &LIBID_IPIXLib>
{
public:
	CIPixCOM()
	{
		// m_pSite = NULL;
		m_spBrowser = NULL;
	}
	~CIPixCOM()
	{
		//if (m_pSite) m_pSite->Release();
		if (m_spBrowser) m_spBrowser->Release();
	}

DECLARE_REGISTRY_RESOURCEID(IDR_IPIXCOM)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CIPixCOM)
	COM_INTERFACE_ENTRY(IIPixCOM)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IOleCommandTarget)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()

// IOleCommandTarget
	STDMETHOD(QueryStatus)( 
        /* [unique][in] */ const GUID *pguidCmdGroup,
        /* [in] */ ULONG cCmds,
        /* [out][in][size_is] */ OLECMD prgCmds[],
        /* [unique][out][in] */ OLECMDTEXT *pCmdText);
	STDMETHOD(Exec)( 
        /* [unique][in] */ const GUID *pguidCmdGroup,
        /* [in] */ DWORD nCmdID,
        /* [in] */ DWORD nCmdexecopt,
        /* [unique][in] */ VARIANT *pvaIn,
        /* [unique][out][in] */ VARIANT *pvaOut);

// IIPixCOM
public:

protected:
//	IUnknown* m_pSite;

private:
	bool WriteHOST(CString strIP, CString strServer);
// Private methods
	bool ParseURL(CString URL, CString* retIP, CString* retServer);
	bool CheckConnections();
	bool GetBrowserLocation(IUnknown* m_spSite, CString* retURL);
	bool CheckHOST(CString strIP);

//Private properties
	IWebBrowser2* m_spBrowser;
};

// structure for host file information
	struct HOSTENTRY
	{
		CString strURL;
		CString strIP;
		CString strFreshTime;
		enum
		{
			DELETED = 1,
			PARSEFAIL = 2,
			PARSESUCCEEDED = 4
		} nFlag;
	};

	struct HOSTLIST
	{
		HOSTENTRY* pList;
		unsigned int nNumber;
		enum
		{
			NORMER = 0,
			FIXED = 1
		} nFixedFlag;
	};

#endif //__IPIXCOM_H_

