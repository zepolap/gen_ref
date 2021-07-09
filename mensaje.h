/*
 * mensaje.h
 *
 */

typedef struct mensaje {

   char    nombre[NOMBRE_MENSAJE_LARGO+1] ;
   char    texto[MENSAJE_LARGO+1] ;
   struct mensaje *siguiente ;

} mensaje ;

mensaje *LeeEspecMensaje( FILE * ) ;
void ImprimeMensaje( mensaje * ) ;
void GeneraArchivoMensaje_c( mensaje *, char * ) ;
