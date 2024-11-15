#pragma once
#include "Server.h"
#include "ServerService.h"
#include "Session.h"

DWORD ProcessIoCompletion(LPVOID param);
void DoAccept(Session* session, DWORD dwBytes);
void DoReceive(Session* session, DWORD dwBytes);
void DoSend(Session* session, DWORD dwBytes);
void InitThreads(LPVOID params);