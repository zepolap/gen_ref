/*
 * funcion.h
 *
 */

typedef struct argumento_funcion {

   char nombre[NOMBRE_ARGUMENTO_LARGO+1] ;
   char tipo[NOMBRE_TIPO_LARGO+1] ;
   struct argumento_funcion *siguiente ;

} argumento_funcion ;

typedef struct implementacion {

   char registro[BUFFER_LARGO] ;
   struct implementacion *siguiente ;

} implementacion ;

typedef struct funcion {

   char nombre[NOMBRE_FUNCION_LARGO+1] ;
   char tipo[NOMBRE_TIPO_LARGO+1] ;
   argumento_funcion *lista_argumento ;
   implementacion *lista_implementacion ;
   struct funcion *siguiente ;

} funcion ;

void EliminaUltimoArgumento( argumento_funcion ** ) ;
void EliminaUltimaImplementacion( implementacion ** ) ;
void EliminaUltimaFuncion( funcion ** ) ;
void AgregaFuncion( char *, funcion **, char *, char *, long  ) ;
void AgregaArgumentoFuncion( char *, funcion *, char *, char *, char *, long ) ;
void AgregaImplFuncion( char *, funcion *, char *, char *, long ) ;
funcion *LeeEspecFuncionesGlobales( FILE * ) ;
void GeneraArchivofuncion_h( funcion *, char * ) ;
void GeneraArchivofuncion_c( funcion *, char * ) ;
