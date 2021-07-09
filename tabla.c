/*
 * tabla.c
 *
 */

#include "definici.h"
#include "bibliote.h"
#include "formato.h"
#include "funcion.h"
#include "ent_sal.h"
#include "tabla.h"

static void AgregaTabla( tabla **lista_tabla, char *nombre_tabla, int codigo_tabla, long numero_registro )
{
   tabla *p, *q ;

   q = (tabla *)malloc( sizeof(tabla) ) ;
   if ( q == NULL )
   {
      fprintf( stderr, "Error: AgregaTabla(...) Insuficiente memoria para cargar informacion.\n" ) ;
      exit( 1 ) ;
   }

   strcpy( q->nombre, nombre_tabla ) ;
   q->codigo = codigo_tabla ;
   q->siguiente = NULL ;

   p = *lista_tabla ;

   if ( p == NULL ) /* lista nueva de tablas */
      *lista_tabla = q ;
   else
   {
      if ( strcmp(p->nombre,nombre_tabla) == 0 )
      {
	  fprintf( stderr, "Error: 'tabla.esp', linea %ld. AgregaTabla(...) Doble especificacion de tabla '%s'.\n", numero_registro, nombre_tabla ) ;
	  free( q ) ;
	  exit( 1 ) ;
      }

      while ( p->siguiente != NULL )
	 p = (tabla *)p->siguiente ;

      p->siguiente = (struct tabla *)q ;
   }
}


void ImprimeTabla( tabla *lista_tabla )
{
   tabla *p ;

   p = lista_tabla ;

   while ( p != NULL )
   {
      fprintf( stdout, "%s %d\n", p->nombre, p->codigo ) ;

      p = (tabla *)p->siguiente ;
   }
}


tabla *LeeEspecTabla( FILE *arch_esp_tabla )
{
   char registro[BUFFER_LARGO] ;
   char nombre_tabla[NOMBRE_TABLA_LARGO+1] ;
   char *t ;
   int  codigo_tabla ;
   long numero_registro ;

   tabla *lista_tabla=NULL ;

   numero_registro = 0L ;
   while ( LeeRegistro(arch_esp_tabla,registro) )
   {
      numero_registro++ ;

      switch( registro[0] )
      {
	 case 't':
	 case 'T':
	    if ( NumeroPalabras(registro+2) < 2 )
	    {
	       fprintf( stderr, "Error: 'tabla.esp', linea %ld. LeeEspecTabla(...) Se esperan a lo menos dos argumentos.\n", numero_registro ) ;
	       exit( 1 ) ;
	    }

	    t = strtok( registro+2, "\t " ) ;
	    strcpy( nombre_tabla, t ) ;

	    t = strtok( NULL, "\n" ) ;
	    codigo_tabla = atoi( t ) ;

	    AgregaTabla( &lista_tabla, nombre_tabla, codigo_tabla, numero_registro ) ;

	 break;
	 case '\n':
	 case '#':
	    /* comentarios */
	 break;
	 default:
	    fprintf( stderr, "Error: 'tabla.esp', linea %ld. LeeEspecTabla(...) Sintaxis incorrecta.\n", numero_registro  ) ;
	    exit( 1 ) ;

      } /* switch */
   } /* while */

   return( lista_tabla ) ;
}
