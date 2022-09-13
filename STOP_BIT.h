#pragma once

#include <stdint.h>
#include <afxmt.h>
#include <string>

enum class STOP_BIT : uint8_t
{
	BIT_1  = ONESTOPBIT   ,
	BIT_15 = ONE5STOPBITS ,
	BIT_2  = TWOSTOPBITS  ,
	Error  = MAXUINT8
};

STOP_BIT StringToSTOP_BIT( std::string str ) ;