/*
 * mensaje.c
 *
 */

#include "definici.h"
#include "bibliote.h"
#include "formato.h"
#include "funcion.h"
#include "ent_sal.h"
#include "mensaje.h"

static void AgregaMensaje( mensaje **lista_mensaje, char *nombre_mensaje, char *texto_mensaje, long numero_registro )
{
   mensaje *p, *q ;

   q = (mensaje *)malloc( sizeof(mensaje) ) ;
   if ( q == NULL )
   {
      fprintf( stderr, "Error: AgregaMensaje(...) Insuficiente memoria para cargar informacion.\n" ) ;
      exit( 1 ) ;
   }

   strcpy( q->nombre, nombre_mensaje ) ;
   strcpy( q->texto, texto_mensaje ) ;
   q->siguiente = NULL ;

   p = *lista_mensaje ;

   if ( p == NULL ) /* lista nueva de mensajes */
      *lista_mensaje = q ;
   else
   {
      if ( strcmp(p->nombre,nombre_mensaje) == 0 )
      {
	  fprintf( stderr, "Error: 'mensaje.esp', linea %ld. AgregaMensaje(...) Doble especificacion de mensaje '%s'.\n", numero_registro, nombre_mensaje ) ;
	  free( q ) ;
	  exit( 1 ) ;
      }

      while ( p->siguiente != NULL )
	 p = (mensaje *)p->siguiente ;

      p->siguiente = (struct mensaje *)q ;
   }
}


void ImprimeMensaje( mensaje *lista_mensaje )
{
   mensaje *p ;

   p = lista_mensaje ;

   while ( p != NULL )
   {
      fprintf( stdout, "%s %s\n", p->nombre, p->texto ) ;

      p = (mensaje *)p->siguiente ;
   }
}


mensaje *LeeEspecMensaje( FILE *arch_esp_mensaje )
{
   char registro[BUFFER_LARGO] ;
   char nombre_mensaje[NOMBRE_MENSAJE_LARGO+1] ;
   char *t, texto_mensaje[MENSAJE_LARGO+1] ;
   long numero_registro ;

   mensaje *lista_mensaje=NULL ;

   numero_registro = 0L ;
   while ( LeeRegistro(arch_esp_mensaje,registro) )
   {
      numero_registro++ ;

      switch( registro[0] )
      {
	 case 'm':
	 case 'M':
	    if ( NumeroPalabras(registro+2) < 2 )
	    {
	       fprintf( stderr, "Error: 'mensaje.esp', linea %ld. LeeEspecMensaje(...) Se esperan a lo menos dos argumentos.\n", numero_registro ) ;
	       exit( 1 ) ;
	    }

	    t = strtok( registro+2, "\t " ) ;
	    strcpy( nombre_mensaje, t ) ;

	    t = strtok( NULL, "\n" ) ;
	    strcpy( texto_mensaje, t ) ;

	    Mayuscula( nombre_mensaje ) ;

	    AgregaMensaje( &lista_mensaje, nombre_mensaje, texto_mensaje, numero_registro ) ;

	 break;
	 case '\n':
	 case '#':
	    /* comentarios */
	 break;
	 default:
	    fprintf( stderr, "Error: 'mensaje.esp', linea %ld. LeeEspecMensaje(...) Sintaxis incorrecta.\n", numero_registro  ) ;
	    exit( 1 ) ;

      } /* switch */
   } /* while */

   return( lista_mensaje ) ;
}

void GeneraArchivoMensaje_c( mensaje *lista_mensaje, char *directorio_salida )
{
   FILE *mensaje_c ;
   char nombre_archivo[NOMBRE_ARCHIVO_LARGO+1] ;

   mensaje *m ;

   fprintf( stdout, "Generando archivo 'mensaje.c'...\n\n" ) ;

#ifdef MSDOS
   sprintf( nombre_archivo, "%s\\mensaje.c", directorio_salida ) ;
#else
   sprintf( nombre_archivo, "%s/mensaje.c", directorio_salida ) ;
#endif
   CreaArchivo( &mensaje_c, nombre_archivo ) ;

   fprintf( mensaje_c, "/*\n *   mensaje.c\n *\n */\n\n" ) ;
   fprintf( mensaje_c, "/* Mensajes definidos por el sistema */\n" ) ;
   fprintf( mensaje_c, "char *MSG_CONDICION_ERROR=    \"Cumple condicion error.\" ;\n" ) ;
   fprintf( mensaje_c, "char *MSG_CONDICION_SOBRANTE=    \"Cumple condicion sobrante.\" ;\n" ) ;
   fprintf( mensaje_c, "char *MSG_NO_SALIDA=             \"No hay salida.\" ;\n" ) ;
   fprintf( mensaje_c, "char *MSG_NO_SE_GENERO_REGISTRO= \"No se pudo generar registro de entrada.\" ;\n\n" ) ;

   fprintf( mensaje_c, "\n/* Mensajes definidos por el usuario */\n" ) ;

   m = lista_mensaje ;
   while ( m != NULL )
   {
      fprintf( mensaje_c, "char *MSG_%s=%s ;\n", m->nombre, m->texto ) ;

      m = (mensaje *)m->siguiente  ;
   }
}
