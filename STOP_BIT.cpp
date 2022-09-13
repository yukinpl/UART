#include "pch.h"
#include "STOP_BIT.h"
#include <algorithm>


STOP_BIT StringToSTOP_BIT( std::string str )
{
	std::transform( str.begin() , str.end() , str.begin() , ::tolower ) ;

	if( "1" == str )
	{
		return STOP_BIT::BIT_1 ;
	}
	else if( "1.5" == str )
	{
		return STOP_BIT::BIT_15 ;
	}
	else if( "2" == str )
	{
		return STOP_BIT::BIT_2 ;
	}
	else
	{
		return STOP_BIT::Error ;
	}
}