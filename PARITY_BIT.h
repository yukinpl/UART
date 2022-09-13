#pragma once

#include <stdint.h>
#include <afxmt.h>
#include <string>
#include <algorithm>


enum class PARITY_BIT : uint8_t
{
	None  = NOPARITY   ,
	Even  = EVENPARITY ,
	Odd   = ODDPARITY  ,
	Error = UINT8_MAX
} ;


PARITY_BIT StringToPARITY_BIT( std::string str ) ;