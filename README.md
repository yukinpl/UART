# UART
UART on Windows 10. Written by C++
    
```cpp
BEGIN_MESSAGE_MAP( CUartDlg , CDialogEx )
	ON_MESSAGE( WM_UART_CLOSE , CUartDlg::OnUartThreadClose )
END_MESSAGE_MAP()
```

```cpp
LRESULT CUartDlg::OnUartThreadClose( WPARAM length , LPARAM lparam )
{
	( ( UART * ) lparam )->CloseHandle() ;

	delete ( UART * ) lparam ;

	return 0 ;
}
```
