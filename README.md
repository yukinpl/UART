# UART
UART on Windows 10. Written by C++
    
```cpp
BEGIN_MESSAGE_MAP( CUartDlg , CDialogEx )
	ON_MESSAGE( WM_UART_RECEIVE , CUartDlg::OnUartReceive )
	ON_MESSAGE( WM_UART_CLOSE , CUartDlg::OnUartThreadClose )
END_MESSAGE_MAP()
```

```cpp
LRESULT CUartDlg::OnUartThreadClose( WPARAM len , LPARAM uart )
{
	( ( UART * ) uart )->CloseHandle() ;

	delete ( UART * ) uart ;

	return 0 ;
}
```

```cpp
LRESULT CUartDlg::OnUartReceive( WPARAM len , LPARAM uart )
{
	static std::string stacked = "" ;
	
	UART * p = ( ( UART * ) uart ) ;
	uint32_t length = ( uint32_t ) len ;
	
	char data[ UART::MaxBufferSize ] = "\0" ;
	
	if( nullptr == p )
	{
		return 0 ;
	}
	
	p->Receive( data , length ) ;
	data[ length ] = '\0' ;
	
	stacked.append( data , length ) ;
	
	...
	...
	
	return 0 ;
}
```
     
```cpp
m_pUART = new UART( "COM3" , "115200" , PARITY_BIT::None , DATA_BIT::BIT_8 , STOP_BIT::BIT_1 ) ;
if( true == m_pUART->Create( GetSafeHwd() ) )
{
}
else
{
}
```
    
```cpp
m_pUART->Send( ( char * ) p , length ) ;
```
