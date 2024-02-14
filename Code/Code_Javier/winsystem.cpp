	
#include <windows.h>
#include "system.h"

namespace PROGRAMACION_GRAFICA
{

///////////////////////////////////////////////////////////////////////////////
///     GetMilliseconds: Calcula los milisegundos transcurridos desde que se 
///						arranca la aplicación.
///
///     @param  nada
///
///     @return  long: milisegundos transcurridos.
///////////////////////////////////////////////////////////////////////////////
long System::GetMilliseconds()
{
	static DWORD msStarting= GetTickCount();
	return (long)(GetTickCount()-msStarting);
}

///////////////////////////////////////////////////////////////////////////////
///     GetSystemError: Devuelve el código del último error producido.
///
///     @param   nada.
///
///     @return  int: código de error.
///////////////////////////////////////////////////////////////////////////////
int System::GetSystemError()
{
	return (int)GetLastError();
}

///////////////////////////////////////////////////////////////////////////////
///     ShowError: Muestra un mensaje de error de Windows.
///
///     @param  int errorCode: Código de error cuyo mensaje mostraremos.
///
///     @return  nada
///////////////////////////////////////////////////////////////////////////////
void System::ShowError(int errorCode)
{
	LPTSTR msgError;
	DWORD  dwBufferLength;

	if ( errorCode != 0 )
	{
		dwBufferLength= FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
									   FORMAT_MESSAGE_FROM_SYSTEM,
									   NULL, errorCode,
									   GetSystemDefaultLangID(),
									   (LPTSTR) &msgError,
									   0, NULL );
		if (dwBufferLength)
		{
			MessageBox(NULL,msgError,"Error:",MB_ICONERROR);
			LocalFree(msgError);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///     ShowMessage: Muestra un mensaje en pantalla o consola.
///
///     @param  char *title: Título del mensaje.
///     @param  char *message: Texto del mensaje.
///     @param  bool bErrorMsg:
///						- true:  es un mensaje de tipo error.
///						- false: es un mensaje informativo.
///
///     @return  nada
///////////////////////////////////////////////////////////////////////////////
void System::ShowMessage(char *title, char *message, bool bErrorMsg)
{
	if ( bErrorMsg )
		MessageBox(NULL,message,title,MB_ICONERROR);
	else
		MessageBox(NULL,message,title,MB_ICONEXCLAMATION);
}

}; // end namespace
