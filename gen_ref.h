/*
 * gen_ref.h
 *
 */

#define LINEA_COMANDO_LARGO 500

typedef struct argumento {

   char directorio_especificaciones[DIRECTORIO_LARGO+1] ;
   char directorio_salida[DIRECTORIO_LARGO+1] ;

} argumento ;

void Uso( char * ) ;
int LeeArgumentos( int , char *[], argumento * ) ;

void GeneraArchivoReformat_h( char * ) ;
void GeneraArchivoReformat_c( entrada_salida *, char * ) ;
void GeneraArchivoDefinici_h( constante *, mensaje *, tabla *, entrada_salida *, char * ) ;
