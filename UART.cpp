#include "pch.h"
#include "UART.h"

#include <memory>


UINT UARTThread( LPVOID lpData )
{
	char buf[ UART::MaxBufferSize ] ;

	UART * uart = ( UART * ) lpData ;

	while( uart->IsOpen() )
	{
		DWORD      eventMask = 0 ;
		DWORD      length = 0 ;
		DWORD      errors = 0 ;
		COMSTAT    status ;

		int inSize = 0 ;

		memset( buf , '\0' , UART::MaxBufferSize ) ;

		WaitCommEvent( uart->GetHandle() , &eventMask , &( uart->readOverlapped ) ) ;
		ClearCommError( uart->GetHandle() , &errors , &status ) ;

		if( /* eventMask & EV_RXCHAR &&*/ status.cbInQue )
		{
			int size = status.cbInQue ;
			if( status.cbInQue > UART::MaxBufferSize )
			{
				size = UART::MaxBufferSize ;
			}

			do
			{
				ClearCommError( uart->GetHandle() , &errors , &status ) ;

				if( !ReadFile( uart->GetHandle() , 
					buf + inSize , size , &length , &( uart->GetReadOverlapped() ) ) )
				{
					TRACE( " ERROR IN ReadFile() \n " ) ;
					if( ERROR_IO_PENDING == GetLastError() )
					{
						if( WAIT_OBJECT_0 == WaitForSingleObject( uart->GetReadOverlapped().hEvent , 1000 ) )
						{
							GetOverlappedResult( 
								uart->GetHandle() , &( uart->GetReadOverlapped() ) , &length , FALSE ) ;
						}
						else
						{
							length = 0 ;
						}
					}
					else
					{
						length = 0 ;
					}
				}

				inSize = length ;

			} while( ( 0 != length ) && ( inSize < size ) ) ;

			ClearCommError( uart->GetHandle() , &errors , &status ) ;

			if( uart->GetLength() + inSize > UART::MaxBufferSize )
			{
				inSize = ( uart->GetLength() + inSize ) - UART::MaxBufferSize ;
			}

			uart->GetEvent()->ResetEvent() ;
			memcpy( uart->recvBuf + uart->GetLength() , buf , inSize ) ;
			uart->SetLength( uart->GetLength() + inSize ) ;
			uart->GetEvent()->SetEvent() ;

			SendMessage( uart->GetHwnd() , WM_UART_RECEIVE , uart->GetLength() , ( LPARAM ) uart ) ;
		}
	}

	PurgeComm( uart->GetHandle() , PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

	SendMessage( uart->GetHwnd() , WM_UART_CLOSE , 0 , ( LPARAM ) uart ) ;

	return 0 ;
}


UART::UART( std::string port , std::string baud , PARITY_BIT parity , DATA_BIT databit , STOP_BIT stopbit )
{
	this->port = port ;
	this->baud = baud ;

	this->parity = parity ;
	this->databit = databit ;
	this->stopbit = stopbit ;

	this->length = 0 ;
	this->flowCheck = 0 ;

	this->isOpen = false ;

	this->pEvent = new CEvent( FALSE , TRUE ) ;

	memset( this->recvBuf , 0 , MaxBufferSize ) ;

	SetBaudRateMap() ;
}


UART::~UART()
{
	if( true == isOpen )
	{
		Close() ;
	}

	baudRateMap.clear() ;

	delete pEvent ;
}


void UART::SetBaudRateMap()
{
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(       "300" , CBR_300    ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(       "600" , CBR_600    ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(      "1200" , CBR_1200   ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(      "2400" , CBR_2400   ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(      "4800" , CBR_4800   ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(      "9600" , CBR_9600   ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(     "14400" , CBR_14400  ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(     "19200" , CBR_19200  ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(     "28800" , CBR_38400  ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(     "33600" , CBR_38400  ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(     "38400" , CBR_38400  ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(     "56000" , CBR_56000  ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(     "57600" , CBR_57600  ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(    "115200" , CBR_115200 ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(    "128000" , CBR_128000 ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >(    "256000" , CBR_256000 ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >( "PCI_9600"  , 1075       ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >( "PCI_19200" , 2212       ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >( "PIC_38400" , 4300       ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >( "PCI_57600" , 6450       ) ) ;
	baudRateMap.emplace( std::pair< std::string , std::int32_t >( "PIC_500K"  , 56000      ) ) ;
}


void UART::Close()
{
	if( false == isOpen )
	{
		return ;
	}

	isOpen = false ;
	SetCommMask( handle , 0 ) ;
	EscapeCommFunction( handle , CLRDTR ) ;

	PurgeComm( handle , PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

	Sleep( 500 ) ;
}


void UART::CloseHandle()
{
	::CloseHandle( handle ) ;
	::CloseHandle( readOverlapped.hEvent  ) ;
	::CloseHandle( writeOverlapped.hEvent ) ;
}


void UART::Clear()
{
	PurgeComm( handle , PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	memset( recvBuf , 0 , MaxBufferSize ) ;
}


bool UART::Create( HWND hwnd )
{
	this->hwnd = hwnd ; // for message

	CString czPort( port.c_str() ) ;

	// open uart
	handle = CreateFile(
		czPort , GENERIC_READ | GENERIC_WRITE , 0 , NULL , OPEN_EXISTING , 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED , NULL ) ;

	// is open ? -- return value is INVALID_HANDLE_VALUE if fail
	if( INVALID_HANDLE_VALUE == handle )
	{
		isOpen = false ;
		return false ;
	}

	Reset() ;

	isOpen = true ;

	readOverlapped.Offset = 0 ;
	readOverlapped.OffsetHigh = 0 ;

	writeOverlapped.Offset = 0 ;
	writeOverlapped.OffsetHigh = 0 ;

	readOverlapped.hEvent  = CreateEvent( NULL , TRUE , FALSE , NULL ) ;
	writeOverlapped.hEvent = CreateEvent( NULL , TRUE , FALSE , NULL ) ;

	AfxBeginThread( UARTThread , ( LPVOID ) this ) ;

	EscapeCommFunction( handle , SETDTR ) ;

	return true ;
}


void UART::Reset()
{
	if( false == isOpen )
	{
		return ;
	}

	DCB dcb ;
	DWORD error ;

	COMMTIMEOUTS timeOuts ;

	umap::const_iterator it = baudRateMap.find( baud ) ;
	if( baudRateMap.end() == it )
	{
		return ;
	}
	
	int32_t baudRate = it->second ;

	memset( &dcb , 0 , sizeof( DCB ) ) ;
	dcb.DCBlength = sizeof( DCB ) ;

	timeOuts.ReadIntervalTimeout = MAXWORD ;
	timeOuts.ReadTotalTimeoutConstant   = 0 ;
	timeOuts.ReadTotalTimeoutMultiplier = 0 ;

	timeOuts.WriteTotalTimeoutConstant   = 1000 ;
	timeOuts.WriteTotalTimeoutMultiplier =    0 ;

	ClearCommError( handle , &error , NULL ) ;             // reset all errors on UART Ports
	SetupComm( handle , RxBufferSize , TxBufferSize ) ;    // set buffer size
	PurgeComm( handle , PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ; // No permit Rx/Tx and bottom buffer up
	SetCommTimeouts( handle , &timeOuts ) ;

	GetCommState( handle , &dcb ) ;
	dcb.fBinary = TRUE ;
	dcb.fParity = TRUE ;
	dcb.BaudRate = baudRate ;
	dcb.Parity = ToUnderlying( parity ) ;
	dcb.ByteSize = ToUnderlying( databit ) ;
	dcb.StopBits = ToUnderlying( stopbit ) ;
	dcb.fRtsControl = RTS_CONTROL_ENABLE ;
	dcb.fDtrControl = DTR_CONTROL_ENABLE ;
	dcb.fOutxDsrFlow = FALSE ;

	if( true == flowCheck )
	{
		dcb.fOutX  = FALSE ;
		dcb.fInX   = FALSE ;
		dcb.XonLim  = 2048 ;
		dcb.XoffLim = 1024 ;
	}
	else
	{
		dcb.fOutxCtsFlow = TRUE ;
		dcb.fRtsControl  = RTS_CONTROL_HANDSHAKE ;
	}

	SetCommState( handle , &dcb ) ;
	SetCommMask ( handle , EV_RXCHAR ) ;
}


bool UART::Send( uint8_t * pBuf , int32_t length )
{
	bool retVal = true ;

	DWORD error ;
	COMSTAT status ; 
	DWORD writtenBytes = 0 ;
	DWORD sentBytes = 0 ;

	ClearCommError( handle , &error , &status ) ;
	if( !WriteFile( handle , pBuf , length , &writtenBytes , &writeOverlapped ) )
	{
		if( ERROR_IO_PENDING != GetLastError() )
		{
			retVal = false ;
		}
		else
		{
			if( WAIT_OBJECT_0 == WaitForSingleObject( writeOverlapped.hEvent , 1000 ) )
			{
				GetOverlappedResult( handle , &writeOverlapped , &writtenBytes , FALSE ) ;
			}
			else
			{
				retVal = false ;
			}
		}
	}

	ClearCommError( handle , &error , &status ) ;

	return retVal ;
}


int32_t UART::Receive( char * pBuf , int32_t length )
{
	CSingleLock lockObj( ( CSyncObject * ) pEvent , FALSE ) ;

	if( length < 1 || MaxBufferSize < length )
	{
		return -1 ;
	}

	if( 0 == this->length )
	{
		pBuf[ 0 ] = '\0' ;
		return 0 ;
	}

	int readBytes = 0 ;

	if( this->length <= length )
	{
		lockObj.Lock() ;
		memcpy( pBuf , & recvBuf , this->length ) ;
		memset( recvBuf , 0 , MaxBufferSize ) ;
		readBytes = this->length ;

		this->length = 0 ;
		lockObj.Unlock() ;
	}
	else
	{
		lockObj.Lock() ;
		memcpy( pBuf , &recvBuf , this->length ) ;
		memmove( recvBuf , recvBuf + length , MaxBufferSize - length ) ;
		this->length = this->length - length ;
		lockObj.Unlock() ;

		readBytes = length ;
	}

	return readBytes ;
}


bool UART::IsOpen()
{
	return isOpen ;
}


HANDLE & UART::GetHandle()
{
	return handle ;
}


HWND & UART::GetHwnd()
{
	return hwnd ;
}


OVERLAPPED & UART::GetReadOverlapped()
{
	return readOverlapped ;
}


int32_t UART::GetLength()
{
	return length ;
}


void UART::SetLength( int32_t length )
{
	this->length = length ;
}


CEvent *& UART::GetEvent()
{
	return pEvent ;
}

/*
char *& UART::GetRecvBuf()
{
	return recvBuf ;
}
*/