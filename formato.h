/*
 * formato.h
 *
 */
 
typedef struct campo {

   char  nombre[NOMBRE_CAMPO_LARGO+1] ;
   int   inicio ;
   int   largo ;
   char  tipo[NOMBRE_TIPO_LARGO+1] ;   
   char  justificacion[JUSTIFICACION_LARGO+1] ;
   char  relleno[RELLENO_LARGO+1] ;
   char  propiedad[PROPIEDAD_LARGO+1];
   char  separador ;
   struct campo *siguiente ;

} campo ;

typedef struct formato {

   char    nombre[NOMBRE_FORMATO_LARGO+1] ;
   int     largo_registro ;
   campo   *campos ;
   struct formato *siguiente ;

} formato ;

void ValidaTipo( char *, char *, long, char * ) ;
void ImprimeFormatos( formato * ) ;
int EsFormatoValido( char *, formato * ) ;
formato *LeeEspecFormato( FILE * ) ;
void GeneraArchivoformato_h( formato *, char * ) ;
void GeneraArchivoformato_c( formato *, char * ) ;

