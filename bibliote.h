/*
 *	  biblioteca.h
 *
 */

#define NULO  0

/* codigos de justificacion */
#define JUST_IZQ 1
#define JUST_CEN 2
#define JUST_DER 3

/* codigos de relleno */
#define RELL_BLANCO 1
#define RELL_CERO	  2
#define RELL_EFE	  3

/* codigos propiedad */
/* decimal */
#define PROP_0			 11
#define PROP_00		 12
#define PROP_000		 13
#define PROP_0000		 14
#define PROP_00000	 15
/* fecha */
#define PROP_AAAAMMDD    21
#define PROP_DDMMAAAA	 22
#define PROP_AAMMDD	 23
#define PROP_DDMMAA	 24
#define PROP_MMDDAA	 25
#define PROP_MM_DD	 26
#define PROP_MMDD        27
/* hora */
#define PROP_HHMMSS	 31
#define PROP_HHMM		 32
#define PROP_SS		 33
#define PROP_MM		 34
#define PROP_MMMMSS	 35
#define PROP_HH_MM	 36  
#define PROP_MMMSS	 37

/* macros */
#define ComienzaCon( texto, patron ) ( strncmp(texto,patron,strlen(patron)) == 0 )
#define EsIgual( texto1, texto2 )	( strcmp(texto1,texto2) == 0 )
#define EsMayor( texto1, texto2 )	( strcmp(texto1,texto2) > 0 )
#define EsMenor( texto1, texto2 )	( strcmp(texto1,texto2) < 0 )

/* funciones */
void leeTrdString( void ) ;
void InicDirecParametros( char * ) ;
char *ObtDirecParametros( void ) ;
void InicializaTablas( void ) ;
int CargaTabla( int ) ;
int BuscaEnTablaPatron( int , char * ) ;
char *ObtenerPatron( void ) ;
int ObtenerNumReemp( void ) ;
char *ObtenerReemp( void ) ;
char *ObtenerReempN( int ) ;
char *ObtenerResto( void ) ;
char *ObtenerDefecto( void ) ;
char *ObtenerSgteReemp( void ) ;

void InicializaSalida( void ) ;
void GeneraSalida( int ) ;
void AgregaSalida( int ) ;
int ObtieneSalida( int * ) ;
int HaySalida( void ) ;
void EliminaSalida( int	 ) ;

void InicializaMultiRegistro( void ) ;
void EsMultiRegistro( int ) ;
void MultiRegistroActual( int ) ;
int ObtenerMultiRegistroActual( void ) ;
int ObtenerMultiRegistros( void ) ;
int HayMultiRegistro( void ) ;

int HaySalidaConSob( void ) ;
void SeProdujoSalidaConSob( void ) ;
void SalidaCampoProcesado ( char *, char * ) ;

int HayCondSobrante( void ) ;
void SeProdujoCondSobrante( void ) ;

void InicializaError( void ) ;
void GeneraError( char * ) ;
int HayError( void ) ;
void EscribirMensajeError( FILE * ) ;

void InicializaSobrante( void ) ;
void GeneraSobrante( char * ) ;
int HaySobrante( void ) ;
void EscribirMensajeSobrante( FILE * ) ;

int HayPreCondicion( void ) ;
int HayPreCondicionSalida( int ) ;
void SeProdujoPreCondConError( void ) ;
int HayPreCondConError( void ) ;

void EsCondicionTerminal( void ) ;
int HayCondicionTerminal( void ) ;

void AjustarPropiedad( char * , int , char ) ;
void AjustarTexto( char *, int , int , int , char ) ;

int NumeroPalabras( char * ) ;
char *Minuscula( char * ) ;
char *Mayuscula( char * ) ;
char *ltrim ( char * ) ;
char *rtrim( char * ) ;
char *lztrim ( char * ) ;
char *rztrim( char * ) ;
char *lftrim ( char * ) ;
char *rftrim( char * ) ;
char *latrim ( char * ) ;
char *ratrim( char * ) ; 
char *ObtenerItem( int , char * ) ;
char *itoa( int ) ;
char atoc( char * ) ;
int EsNumerico( char * ) ;
int EsLineaEnBlanco( char * ) ;

int LeeRegistro( FILE *, char * ) ;

int CreaArchivo( FILE **, char * ) ;
int LeeArchivo( FILE **, char * ) ;

long JulDay( long ) ;
int DiaSemana( long ) ;
int FechaValida( long ) ;
int HoraValida( char * ) ;
void ObtAgnoMesDia( long, int *, int *, int * ) ;
long ObtFecha( int, int, int ) ;
void ObtHorMinSeg( long, int *, int *, int * ) ;
long ObtHora( int, int, int ) ;
long ObtHHMMSS( long ) ;
char *ftoa( long ) ;

/* EOF bibliote.h */
