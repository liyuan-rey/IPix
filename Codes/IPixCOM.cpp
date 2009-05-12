// IPixCOM.cpp : Implementation of CIPixCOM
#include "stdafx.h"
#include "IPix.h"
#include "IPixCOM.h"

/////////////////////////////////////////////////////////////////////////////
// CIPixCOM

// IOleCommandTarget
	STDMETHODIMP CIPixCOM::QueryStatus(
			const GUID *pguidCmdGroup,
			ULONG cCmds, OLECMD prgCmds[],
			OLECMDTEXT *pCmdText)
	{
		if( ! prgCmds )
			return E_POINTER;

		for(ULONG i = 0; i < cCmds;i++)
			prgCmds[i].cmdf = OLECMDF_SUPPORTED | OLECMDF_ENABLED;	return S_OK;

		return S_OK;
	}

	STDMETHODIMP CIPixCOM::Exec(
			const GUID *pguidCmdGroup, DWORD nCmdID,
			DWORD nCmdexecopt, VARIANT *pvaIn,
			VARIANT *pvaOut)
	{
		if (!CheckConnections())
		{
			MessageBox(NULL,
				_T("û�м�⵽��������!\n�����޷���������!"),
				_T("INFORMATION"), MB_OK+MB_ICONSTOP);
#ifndef DEBUG
			return S_OK;
#endif
		}

		CString URL;
		if(!GetBrowserLocation(m_spUnkSite, &URL))
		{
			MessageBox(NULL, _T("�޷��õ���ǰ�����ַ!\n�޷�����!"),
				_T("INFORMATION"), MB_OK+MB_ICONSTOP);
			return S_OK;
		}

		CString IP, Server;
		if(!ParseURL(URL, &IP, &Server))
		{
			MessageBox(NULL, _T("�޷������� URL !\n + URL"),
				_T("INFORMATION"), MB_OK+MB_ICONSTOP);

			return S_OK;
		}

		if (IP == "127.0.0.1")
			Server = "localhost";

		if (!CheckHOST(IP))
			return S_OK;

		if (!WriteHOST(IP, Server))
		{
			MessageBox(NULL,
				_T("������: " + Server + "\n���� IP Ϊ: " + IP + "\n\nд�뱾�� HOST �ļ�ʧ��!"),
				_T("INFORMATION"), MB_OK+MB_ICONSTOP);
		}
		else
		{
			//CString strTemp = "������: " + CString(Server) + "\n���� IP Ϊ: " + IP + "\n\n�ѳɹ�д�뱾�� HOST �ļ�!";
			MessageBox(NULL,
				_T("������: " + Server + "\n���� IP Ϊ: " + IP + "\n\n�ѳɹ�д�뱾�� HOST �ļ�!"),
				_T("INFORMATION"), MB_OK+MB_ICONINFORMATION);
		}

		return S_OK;
	}

bool CIPixCOM::CheckConnections()
{
	HINSTANCE m_hWinInetApi = NULL;

	typedef bool (__stdcall *PFNINTERNETGETCONNECTEDSTATE)(LPDWORD, DWORD);
	PFNINTERNETGETCONNECTEDSTATE m_pfnInternetGetConnectedState = NULL;

	DWORD dwFlags = 0;
	DWORD dwReserved = 0;

	m_hWinInetApi = LoadLibrary("WININET.DLL");
	if (m_hWinInetApi != NULL)
	{
		m_pfnInternetGetConnectedState = (PFNINTERNETGETCONNECTEDSTATE) GetProcAddress(m_hWinInetApi, "InternetGetConnectedState");
		if (!m_pfnInternetGetConnectedState)
		{
			MessageBox(NULL, 
				_T("���ʶ�̬���ӿ⺯��ʧ��!\n����û�гɹ����!"),
				_T("INFORMATION"), MB_OK+MB_ICONSTOP);

			FreeLibrary(m_hWinInetApi);
			return false;
		}

		if (m_pfnInternetGetConnectedState(&dwFlags, dwReserved))
		{
			m_pfnInternetGetConnectedState = NULL;
			FreeLibrary(m_hWinInetApi);
			return true;
		}
		else
		{
			m_pfnInternetGetConnectedState = NULL;
			FreeLibrary(m_hWinInetApi);
			return false;
		}
	}

	MessageBox(NULL, _T("���ض�̬���ӿ�ʧ��!\n\n����û�гɹ����!"),
		_T("INFORMATION"), MB_OK+MB_ICONSTOP);
	return false;
}

bool CIPixCOM::GetBrowserLocation(IUnknown* m_spSite, CString* retURL)
{
	IOleCommandTarget* pCmdTarget = NULL;
	HRESULT hr = m_spSite->QueryInterface(IID_IOleCommandTarget, (LPVOID*)&pCmdTarget);
	if (SUCCEEDED(hr))
	{
		IServiceProvider* pSP = NULL;
		hr = pCmdTarget->QueryInterface(IID_IServiceProvider, (LPVOID*)&pSP);

		pCmdTarget->Release();

		if (SUCCEEDED(hr))
		{
			if (m_spBrowser)
			{
				m_spBrowser->Release();
				m_spBrowser = NULL;
			}

			hr = pSP->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (LPVOID*)&m_spBrowser);
			if (FAILED(hr))
			{
				MessageBox(NULL, _T("IWebBrowserApp �ӿڷ��ʴ���!"),
					_T("INFORMATION"), MB_OK+MB_ICONSTOP);
				return false;
			}
			pSP->Release();
		}
		else
		{
			MessageBox(NULL, _T("IServiceProvider �ӿڷ��ʴ���!"),
				_T("INFORMATION"), MB_OK+MB_ICONSTOP);
			return false;
		}
	}
	else
	{
		MessageBox(NULL, _T("IOleCommandTarget �ӿڷ��ʴ���!"),
			_T("INFORMATION"), MB_OK+MB_ICONSTOP);
		return false;
	}

	BSTR bstrURL = NULL;
	bstrURL = SysAllocString(bstrURL);
	hr = m_spBrowser->get_LocationURL(&bstrURL);
	if (SUCCEEDED(hr))
	{
		*retURL = CString(bstrURL);
		SysFreeString(bstrURL);
		return true;
	}

	retURL = NULL;
	SysFreeString(bstrURL);
	return false;
}

bool CIPixCOM::ParseURL(CString URL, CString* retIP, CString* retServer)
{
	DWORD dwServiceType;
	CString strServer,strObject;
	unsigned short nPort;
	if (!AfxParseURL(URL, dwServiceType, strServer, strObject, nPort))
	{
		MessageBox(NULL, _T("URL\n" + URL + "\n�Ƿ�!"), 
			_T("INFORMATION"), MB_OK+MB_ICONSTOP);
		return false;
	}

	HOSTENT* lpHostEnt;
	lpHostEnt = gethostbyname(strServer);
	if (!lpHostEnt)
	{
		//���gethostbyname��������ʧ�ܵĴ���
		return false;
	}
	*retIP = (CString)
		inet_ntoa(*(LPIN_ADDR)*(lpHostEnt->h_addr_list));
	*retServer = strServer;
	return true;
}

bool CIPixCOM::CheckHOST(CString strIP)
{
	char pHostPath[_MAX_PATH];
	GetWindowsDirectory(pHostPath,_MAX_PATH);
	strcat( pHostPath, "\\hosts" );

	fstream fs(pHostPath,
			ios::ate | ios::in | ios::nocreate,
			filebuf::sh_read);
	if (!fs.is_open())
	{
		fs.open(pHostPath,
			ios::ate | ios::in | ios::out | ios::noreplace,
			filebuf::sh_read);
		if (!fs.is_open())
		{
			MessageBox(NULL,
				_T("HOST �ļ���ʧ��!"),
				_T("INFORMATION"), MB_OK+MB_ICONSTOP);

			return false;
		}
	}

	fs.seekg(0, ios::beg);
	while (fs.good())
	{
		char buf[1024];
		fs.getline(buf, 1023);
		if(strstr( buf, strIP))
		{
			if (IDYES == MessageBox(NULL,
				_T("IP�Ѵ����ڱ���DNSϵͳ!\n�Ƿ���Ҫ���½���?"),
				_T("��ʾ"),
				MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1 | MB_TASKMODAL))
			{
				//ɾ������buf
				AfxMessageBox(_T("�Բ���, ��ģ��δ���!\n���������½���!"));
			}

			fs.close();
			return false;
		}
	}

	fs.close();
	return true;
}

bool CIPixCOM::WriteHOST(CString strIP, CString strServer)
{
	char lpPath[_MAX_PATH];
	GetWindowsDirectory(lpPath,_MAX_PATH);
	strcat( lpPath, "\\hosts" );

	ofstream ofs(lpPath,
		ios::app | ios::out | ios::nocreate,
		filebuf::sh_read);
	if (!ofs.is_open())
	{
		ofs.open(lpPath,
			ios::app | ios::out | ios::noreplace,
			filebuf::sh_read);
		if (!ofs.is_open())
		{
			MessageBox(NULL,
				_T("HOST �ļ���ʧ��!"),
				_T("INFORMATION"), MB_OK+MB_ICONSTOP);

			return false;
		}
	}
	ofs << "\n" << strIP << " " << strServer;
	ofs.close();
	return true;
}
