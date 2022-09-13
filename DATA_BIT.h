#pragma once

#include <stdint.h>
#include <afxmt.h>
#include <string>

enum class DATA_BIT : uint8_t
{
	BIT_7 = 7 ,
	BIT_8 = 8 ,
	Error = UINT8_MAX
};

DATA_BIT StringToDATA_BIT( std::string str );