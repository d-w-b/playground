#include "pch.h"
#include "CSessionManager.h"
#include "CNetworkUtils.h"
#include "CThreadManager.h"
#include "501_Connector.h"
#include "SessionContext.h"
#include "Session.h"

LPFN_CONNECTEX		CNetworkUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX	CNetworkUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX		CNetworkUtils::AcceptEx = nullptr;

CNetworkUtils::CNetworkUtils()
{
}

CNetworkUtils::~CNetworkUtils()
{
}

void CNetworkUtils::Init()
{
	assert(_WINDOWS);
	WSADATA wsaData;
	assert(::WSAStartup(MAKEWORD(2, 2), OUT & wsaData) == 0);

	// WSA 확장 함수 등록
	SOCKET dummySocket = CreateOverlappedSocket();

	assert(BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	assert(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	assert(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));

	closesocket(dummySocket);
}

bool CNetworkUtils::BindWindowsFunction(SOCKET socket, GUID guid, OUT LPVOID* fn)
{
	DWORD bytes = 0;
	return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT & bytes, NULL, NULL);
}
/// <summary>
/// OVERLAPPED IO 가 적용되는 Socket 을 생성합니다.
/// </summary>
/// <returns>SOCKET s</returns>
SOCKET CNetworkUtils::CreateOverlappedSocket()
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

/// <summary>
/// 소켓을 만들고 입력된 주소의 서버에 ConnectEx를 호출합니다.
/// </summary>
/// <param name="sockaddr"> IPv4 Address </param>
/// <returns>SOCKET clientSocket</returns>
SOCKET CNetworkUtils::Connect(SOCKADDR_IN addr)
{
	SOCKET clientSocket = CreateOverlappedSocket();

	if (::CreateIoCompletionPort((HANDLE)clientSocket, GetAppInstance()->GetIocpManager()->GetHandle(), 0, 0) == false)
	{
		closesocket(clientSocket);
		return INVALID_SOCKET;
	}

	DWORD cbBytes = 0;

	SessionContext ctx(SessionCtx::Connect);
	
	if ( false == CNetworkUtils::ConnectEx( clientSocket,reinterpret_cast<sockaddr*>(&addr), sizeof(addr), nullptr,0, &cbBytes, reinterpret_cast<LPOVERLAPPED>(&ctx)))
	{
		INT errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			////TODO//////_connectEvent.owner = nullptr; //RELEASE_REF
			CString str;
			str.Format(_T("ConnectEx WSAError : %d"), errorCode);
			::AfxMessageBox(str);

			closesocket(clientSocket);
			return INVALID_SOCKET;
		}
	}

	return clientSocket;

}

void CNetworkUtils::BindClientAddress(SOCKET socket, INT port)
{
	SOCKADDR_IN myAddress;
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	myAddress.sin_port = ::htons(port);

	::bind(socket, reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(myAddress));
}

bool CNetworkUtils::SetReusableAddrSocket(SOCKET socket, bool flag)
{
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*> (&flag), sizeof(flag)) < 0) {
		return false;
	}

	return true;
}

SOCKADDR_IN CNetworkUtils::CreateAddress(TCHAR* ip, UINT port)
{
	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(7777);

	IN_ADDR address;
	::InetPton(AF_INET, ip, &address);
	addr.sin_addr = address;

	return addr;
}

void CNetworkUtils::AddrToPresentation(SOCKADDR_IN addr, TCHAR* szIpAddr)
{
	InetNtop(AF_INET, &addr.sin_addr, szIpAddr, INET_ADDRSTRLEN);
}


