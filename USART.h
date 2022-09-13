#pragma once

#include <afxmt.h>
#include <string>
#include <unordered_map>

#include "PARITY_BIT.h"
#include "DATA_BIT.h"
#include "STOP_BIT.h"


template <typename E>
constexpr typename std::underlying_type<E>::type ToUnderlying( E e ) noexcept
{
	return static_cast< typename std::underlying_type< E >::type >( e ) ;
}

#define WM_USART_RECEIVE ( WM_USER + 1 ) 
#define WM_USART_CLOSE   ( WM_USER + 2 ) 


class USART : public CCmdTarget
{

public:
	static const int RxBufferSize  = 8192 ;
	static const int TxBufferSize  = 8182 ;
	static const int MaxBufferSize = 8192 ;

private: 
	std::string port ;
	std::string baud ;

	PARITY_BIT parity ;
	DATA_BIT   databit ;
	STOP_BIT   stopbit ;

	int32_t length ;
	bool    flowCheck ;

public:
	bool isOpen ;

	char recvBuf[ MaxBufferSize ] ;

	CEvent * pEvent ;

	HWND hwnd ;
	HANDLE handle ; 
	OVERLAPPED writeOverlapped ;
	OVERLAPPED  readOverlapped ;

	using umap = std::unordered_map< std::string , uint32_t > ;
	umap baudRateMap ;

private:
	void SetBaudRateMap() ;

public:
	 USART( std::string port , std::string baud , PARITY_BIT parity , DATA_BIT databit , STOP_BIT stopbit ) ;
	~USART() ;

public:
	void Close() ;
	void CloseHandle() ;
	BOOL Create( HWND hwnd ) ;

	bool IsOpen() ;
	void Reset() ;
	void Clear() ;

	HANDLE & GetHandle() ;
	HWND   & GetHwnd() ;

	int32_t GetLength() ;
	void SetLength( int32_t length ) ;

	OVERLAPPED & GetReadOverlapped() ;

	CEvent * & GetEvent() ;

	//char *& GetRecvBuf() ;

	bool Send( char * pBuf , int32_t length ) ;
	int32_t Receive( char * pBuf , int32_t length ) ;
	
} ;

