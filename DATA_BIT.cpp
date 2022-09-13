#include "pch.h"
#include "DATA_BIT.h"

#include <algorithm>

DATA_BIT StringToDATA_BIT( std::string str )
{
	std::transform( str.begin() , str.end() , str.begin() , ::tolower ) ;

	if( "7 bit" == str )
	{
		return DATA_BIT::BIT_7 ;
	}
	else if( "8 bit" == str )
	{
		return DATA_BIT::BIT_8 ;
	}
	else
	{
		return DATA_BIT::Error ;
	}
}