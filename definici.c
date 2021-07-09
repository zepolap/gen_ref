/*
 * definici.c
 *
 */

#include "definici.h"
#include "bibliote.h"
#include "formato.h"
#include "funcion.h"
#include "ent_sal.h"
#include "constant.h"
#include "mensaje.h"
#include "tabla.h"

void GeneraArchivoDefinici_h( constante *lista_constante, mensaje *lista_mensaje, tabla *lista_tabla, entrada_salida *lista_entrada_salida, char *directorio_salida )
{
   int identificador=0 ;
   FILE *definici_h ;
   char nombre_archivo[NOMBRE_ARCHIVO_LARGO+1] ;

   entrada_salida *e_s ;
   constante *c ;
   mensaje *m ;
   tabla *t ;

   fprintf( stdout, "Generando archivo 'definici.h'...\n\n" ) ;

#ifdef MSDOS
   sprintf( nombre_archivo, "%s\\definici.h", directorio_salida ) ;
#else
   sprintf( nombre_archivo, "%s/definici.h", directorio_salida ) ;
#endif
   CreaArchivo( &definici_h, nombre_archivo ) ;

   fprintf( definici_h, "/*\n *   definici.h\n *\n */\n\n" ) ;

   fprintf( definici_h, "#include <stdio.h>\n" ) ;
   fprintf( definici_h, "#include <stdlib.h>\n\n" ) ;

   fprintf( definici_h, "#define FALSE 0\n" ) ;
   fprintf( definici_h, "#define TRUE  1\n\n" ) ;

   fprintf( definici_h, "#define NOMBRE_ARCHIVO_LARGO    50\n" ) ;
   fprintf( definici_h, "#define NOMBRE_DIRECTORIO_LARGO 50\n" ) ;
   fprintf( definici_h, "#define NOMBRE_ENTRADA_LARGO    50\n" ) ;
   fprintf( definici_h, "#define VARIABLE_LOCAL_LARGO    255\n" ) ;
   fprintf( definici_h, "#define BUFFER_LARGO            300\n\n" ) ;

   e_s = lista_entrada_salida ;

   while ( e_s != NULL )
   {
      identificador++ ;
      fprintf( definici_h, "#define ID_%s %d\n", e_s->nombre, identificador ) ;

      e_s = (entrada_salida *)e_s->siguiente ;
   }

   fprintf( definici_h, "\n/*** macro para imprimir puntos de estado de avance ***/\n" ) ;
   fprintf( definici_h, "/*** imprime un punto cada vez que 'valor' es multiplo de 1000, ***/\n" ) ;
   fprintf( definici_h, "/*** escribe 50 puntos por linea y cambia de linea ***/\n" ) ;
   fprintf( definici_h, "/*** se usa para indicar numero de registros procesados ***/\n" ) ;
   fprintf( definici_h, "#define mPrintPuntos( valor, f ) \\\n" ) ;
   fprintf( definici_h, "{ \\\n" ) ;
   fprintf( definici_h, "\tif ( valor %% 1000L == 0 ) \\\n" ) ;
   fprintf( definici_h, "\t{ \\\n" ) ;
   fprintf( definici_h, "\t\tfputc( '.', f ) ; \\\n" ) ;
   fprintf( definici_h, "\t\tif ( valor %% 10000L == 0 ) \\\n" ) ;
   fprintf( definici_h, "\t\t{ \\\n" ) ;
   fprintf( definici_h, "\t\t\tfputc( ' ', f ) ; \\\n" ) ;
   fprintf( definici_h, "\t\t\tif ( valor %% 50000L == 0 ) \\\n" ) ;
   fprintf( definici_h, "\t\t\t{ \\\n" ) ;
   fprintf( definici_h, "\t\t\t\tfputc( '\\n', f ) ; \\\n" ) ;
   fprintf( definici_h, "\t\t\t\tif ( valor %% 250000L == 0 ) \\\n" ) ;
   fprintf( definici_h, "\t\t\t\t{ \\\n" ) ;
   fprintf( definici_h, "\t\t\t\t\tfprintf( f, \"%%ld\\n\", valor ) ; \\\n" ) ;
   fprintf( definici_h, "\t\t\t\t} \\\n" ) ;
   fprintf( definici_h, "\t\t\t} \\\n" ) ;
   fprintf( definici_h, "\t\t} \\\n" ) ;
   fprintf( definici_h, "\t\tfflush (f) ; \\\n" ) ;
   fprintf( definici_h, "\t} \\\n" ) ;
   fprintf( definici_h, "}\n\n" ) ;

   fprintf( definici_h, "\n/* Constantes definidos por el usuario */\n" ) ;

   c = lista_constante ;
   while ( c != NULL )
   {
      fprintf( definici_h, "#define CTE_%s %s\n", c->nombre, c->valor ) ;

      c = (constante *)c->siguiente  ;
   }

   fprintf( definici_h, "\n/* Tablas */\n" ) ;

   t = lista_tabla ;
   while ( t != NULL )
   {
      fprintf( definici_h, "#define TBL_%s\t%d\n", t->nombre, t->codigo ) ;

      t = (tabla *)t->siguiente  ;
   }

   fprintf( definici_h, "\n/* Mensajes definidos por el sistema */\n" ) ;
   fprintf( definici_h, "extern char *MSG_CONDICION_ERROR ;\n" ) ;   
   fprintf( definici_h, "extern char *MSG_CONDICION_SOBRANTE ;\n" ) ;
   fprintf( definici_h, "extern char *MSG_NO_SALIDA ;\n" ) ;
   fprintf( definici_h, "extern char *MSG_NO_SE_GENERO_REGISTRO ;\n\n" ) ;

   fprintf( definici_h, "\n/* Mensajes definidos por el usuario */\n" ) ;

   m = lista_mensaje ;
   while ( m != NULL )
   {
      fprintf( definici_h, "extern char *MSG_%s ;\n", m->nombre ) ;

      m = (mensaje *)m->siguiente  ;
   }
}
