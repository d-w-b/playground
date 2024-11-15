#include "pch.h"
#include "SessionContext.h"


SessionContext::SessionContext(EventType type):_type(type)
{
	
}

SessionContext::~SessionContext()
{
	
}



void SessionContext::Init()
{
	Internal		= 0;
	InternalHigh	= 0;
	Offset			= 0;
	OffsetHigh		= 0;
}

void SessionContext::Close()
{

}
