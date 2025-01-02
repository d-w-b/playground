#pragma once
#include "pch.h"

class CNetworkUtils
{
public:
	CNetworkUtils();
	~CNetworkUtils();

	static void Init();
	static SOCKET CreateOverlappedSocket();
	static SOCKET Connect(SOCKADDR_IN sockaddr);
	static void BindClientAddress(SOCKET socket, INT port);
	static bool SetReusableAddrSocket(SOCKET socket, bool flag);
	static SOCKADDR_IN CreateAddress(TCHAR* ip, UINT port);
	static void AddrToPresentation(SOCKADDR_IN addr, TCHAR* szIpAddr);

private:
	static bool BindWindowsFunction(SOCKET socket, GUID guid, OUT LPVOID* fn);
	

public:
	static LPFN_CONNECTEX		ConnectEx;
	static LPFN_DISCONNECTEX	DisconnectEx;
	static LPFN_ACCEPTEX		AcceptEx;
};
