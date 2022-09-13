#include "pch.h"
#include "PARITY_BIT.h"
#include <algorithm>

PARITY_BIT StringToPARITY_BIT( std::string str )
{
	std::transform( str.begin() , str.end() , str.begin() , ::tolower ) ;

	if( "none" == str )
	{
		return PARITY_BIT::None ;
	}
	else if( "even" == str )
	{
		return PARITY_BIT::Even ;
	}
	else if( "odd" == str )
	{
		return PARITY_BIT::Odd ;
	}
	else
	{
		return PARITY_BIT::Error ;
	}
}