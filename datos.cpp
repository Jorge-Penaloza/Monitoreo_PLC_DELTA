#include <iostream>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <windows.h>
#include <fstream>

using namespace std;
// Tipo de datos:
typedef struct
{
    char Puerto[5];
    int Baudios;
    int BitsDatos;
    int BitsStop;
    char Paridad[25];
} tipoOpciones;

// Variables globales:
tipoOpciones Ops;           // Opciones
HGLOBAL hCadena, hSalida;   // Buffers
char *cad, *sal;
DCB dcb;                    // Puerto Serie
HANDLE idComDev;
bool Comunicacion;
HANDLE hHilo;               // Hilo del puerto serie

// Prototipos:
void IniciarBuffers();
void LiberarBuffers();
bool InicioComunicacion(void);
bool FinComunicacion(void);
DWORD Hilo(LPDWORD lpdwParam);
void LeeSerie();
void EscribirSerie (char *buf);

ofstream file;
// Programa:
int main (int argc, char *argv[])
{
  
  	for(int i=0;i<argc;i++)
  	{
  		//cout<<argc<<endl;
  		cout<<argv[i]<<endl;
    }
    //file.open("C:/prueba/archivo.csv");
    file.open(argv[1]);
    bool salir=false;
    DWORD param, id;
   
    //Inicializar opciones del puerto serie:
    //strcpy (Ops.Puerto, "COM2");
    strcpy (Ops.Puerto, argv[2]);
    Ops.Baudios = 9600;
    //Ops.Baudios = 115200;
    Ops.BitsDatos = 8;
    Ops.BitsStop = 1;
    strcpy (Ops.Paridad, "Sin paridad");
   
    // Inicialización Buffers:
    IniciarBuffers();
    // No se ha establecido comunicación:
    
    Comunicacion = false;
    if(!InicioComunicacion())
    {
        cout << "Inicializacion puerto serie" << endl;
        cout << "ERROR: No se puede acceder al puerto serie." << endl;
        LiberarBuffers();
        system("PAUSE");
        return 1;
    }
    else cout <<"Inicializacion puerto serie.....Correcto" << endl << endl;
   
    // Lanzar hilo de lectura del puerto serie:
    hHilo = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)Hilo, &param, 0, &id);
    if(!hHilo) cout << "ERROR" << endl;
    else cout << "- Hilo de lectura lanzado." << endl << endl;
           
    // Bucle principal:
    unsigned int contador = 0;
    while (!salir)
    {
    	 // Leer un comando:
    	 
    	        cin.getline (cad, 80);
    	        //cout<<cad;
    	        // Si es "salir", abandonar el bucle:
    	        if(!strcmp (cad, "salir")) 
				{
					salir = true;
					
				}
    	        else
    	        {
    	            // Si no, enviar cadena por el puerto serie:
    	            strcat (cad, "\r");
    	            EscribirSerie (cad);
    	        }
    	 
    }
    file.close();  
    // Liberar Hilo:
    CloseHandle (hHilo);
   
    // Liberar puerto serie:
    FinComunicacion();
    LiberarBuffers();
    system("PAUSE");
    return 0;
    }
	
// Reservar memoria global para buffers del puerto serie:
void IniciarBuffers()
{
    hCadena = GlobalAlloc (GMEM_MOVEABLE, 4096);
    hSalida = GlobalAlloc (GMEM_MOVEABLE, 4096);
    cad = (char *) GlobalLock (hCadena);
    sal = (char *) GlobalLock (hSalida);
}

// Liberar memoria global de buffers del puerto serie:
void LiberarBuffers()
{
    GlobalUnlock (hCadena);
    GlobalUnlock (hSalida);
    GlobalFree (hCadena);
    GlobalFree (hSalida);
}

// Iniciar el puerto serie:
bool InicioComunicacion(void)
{
    bool fSuccess;
   
    // Abrir el fichero asociado al puerto:
    idComDev = CreateFile (Ops.Puerto, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if(idComDev == INVALID_HANDLE_VALUE)
    {
        cout << "ERROR: CreateFile. Inicializacion puerto serie" << endl;
        system("PAUSE");
        return false;
    }
    else cout << "CreateFile.....Correcto" << endl << endl;
    	
    // Leer estructura de control del puerto serie, cdb:
    fSuccess = GetCommState (idComDev, &dcb);
    if(!fSuccess)
    {
        cout << "ERROR: GetCommState. Inicializacion puerto serie" << endl;
        system("PAUSE");
        return false;
    }
    else cout << "GetCommState.....Correcto" << endl << endl;
   
    // Modificar el dcb según las opciones definidas:
    dcb.BaudRate = Ops.Baudios;
    dcb.ByteSize = Ops.BitsDatos;
   
    if(!strcmp(Ops.Paridad, "Sin paridad")) dcb.Parity = NOPARITY;
    if(!strcmp(Ops.Paridad, "Paridad par")) dcb.Parity = EVENPARITY;
    if(!strcmp(Ops.Paridad, "Paridad impar")) dcb.Parity = ODDPARITY;
   
    switch (Ops.BitsStop)
    {
        case (1):
            dcb.StopBits = ONESTOPBIT;
            break;
        case (2):
            dcb.StopBits = TWOSTOPBITS;
            break;
    }
   
    // Modificar la estructura de control del puerto serie:
    fSuccess = SetCommState (idComDev, &dcb);
    if(!fSuccess)
    {
        cout << "ERROR: SetCommStatus. Inicializacion puerto serie" << endl;
        system("PAUSE");
        return false;
    }
    else cout << "SetCommStatus.....Correcto" << endl << endl;
       
    // Especifica que queremos monitorizar la recepción de datos:
    if(!SetCommMask (idComDev, EV_RXCHAR))
    {
        cout << "No se pudo activar el evento de lectura." << endl;
        system("PAUSE");
        return false;
    }
    else cout << "- Se ha activado el evento de lectura." << endl << endl;
   
    // Comunicación establecida:
    Comunicacion = true;
    return true;
}
// Finalizar comunicación por puerto serie:
bool FinComunicacion (void)
{
    // Liberar máscara de eventos del puerto serie:
    SetCommMask (idComDev, 0);
   
    // Cerrar el puerto serie:
    CloseHandle (idComDev);
    // Comunicación interrumpida:
    Comunicacion = false;
    return true;
}

// Hilo de escucha del puerto serie:
DWORD Hilo (LPDWORD lpdwParam)
{
    DWORD dwEvtMask;
    do
    {
        if(WaitCommEvent(idComDev, &dwEvtMask, NULL))
            if(dwEvtMask & EV_RXCHAR) LeeSerie();
    }
    while (true);
    return 0;
}

// Leer datos del puerto serie:
void LeeSerie()
{
    DWORD x;
    COMSTAT cs;
   
    // Actualizar COMSTAT; sirve para averiguar el número de bytes en el buffer de entrada:
    ClearCommError (idComDev, &x, &cs);
    // Leer cs.cbInQue caracteres:
    ReadFile (idComDev, cad, cs.cbInQue, &x, NULL);
    // Actualizar el fin de cadena:
    cad[x] = 0;
    // Mostrar caracteres leídos:
    //cout << cad;
	file << cad;
	cout << cad;
    return;
}

void EscribirSerie (char *buf)
{
    char Buffer[1024];
    DWORD n, l, p, err;
   
    strcpy (Buffer, buf);
    l = strlen (Buffer);
    p = 0;
 
    while (1)
    {
    	//file<<&Buffer[p];
    	
        if (!WriteFile (idComDev, &Buffer[p], l-p, &n, NULL))
        {
        	
            err = GetLastError();
            if(err != ERROR_IO_PENDING)
            {
                Comunicacion = false;
                
                return;
            }
        }
        l -= n;
        p += n;
    }
}
