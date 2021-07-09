/*
 * constant.c
 *
 */

#include "definici.h"
#include "bibliote.h"
#include "formato.h"
#include "funcion.h"
#include "ent_sal.h"
#include "constant.h"

static void AgregaConstante( constante **lista_constante, char *nombre_constante, char *valor_constante, long numero_registro )
{
   constante *p, *q ;

   q = (constante *)malloc( sizeof(constante) ) ;
   if ( q == NULL )
   {
      fprintf( stderr, "Error: AgregaConstante(...) Insuficiente memoria para cargar informacion.\n" ) ;
      exit( 1 ) ;
   }

   strcpy( q->nombre, nombre_constante ) ;
   strcpy( q->valor, valor_constante ) ;
   q->siguiente = NULL ;

   p = *lista_constante ;

   if ( p == NULL ) /* lista nueva de constantes */
      *lista_constante = q ;
   else
   {
      if ( strcmp(p->nombre,nombre_constante) == 0 )
      {
	 fprintf( stderr, "Error: 'constant.esp', linea %ld. AgregaConstante(...) Doble especificacion de constante '%s'.\n", numero_registro, nombre_constante ) ;
	 free( q ) ;
	 exit( 1 ) ;
      }

      while ( p->siguiente != NULL )
	 p = (constante *)p->siguiente ;

      p->siguiente = (struct constante *)q ;
   }
}

void ImprimeConstante( constante *lista_constante )
{
   constante *p ;

   p = lista_constante ;

   while ( p != NULL )
   {
      fprintf( stdout, "%s %s\n", p->nombre, p->valor ) ;

      p = (constante *)p->siguiente ;
   }
}

constante *LeeEspecConstante( FILE *arch_esp_constante )
{
   char registro[BUFFER_LARGO] ;
   char nombre_constante[NOMBRE_CONSTANTE_LARGO+1] ;
   char *t, valor_constante[VALOR_LARGO+1] ;
   long numero_registro ;

   constante *lista_constante=NULL ;

   numero_registro = 0L ;
   while ( LeeRegistro(arch_esp_constante,registro) )
   {
      numero_registro++ ;

      switch( registro[0] )
      {
	 case 'c':
	 case 'C':
	    if ( NumeroPalabras(registro+2) < 2 )
	    {
	       fprintf( stderr, "Error: 'constant.esp', linea %ld. LeeEspecConstante(...) Se esperan a lo menos dos argumentos.\n", numero_registro ) ;
	       exit( 1 ) ;
	    }

	    t = strtok( registro+2, "\t " ) ;
	    strcpy( nombre_constante, t ) ;

	    t = strtok( NULL, "\n" ) ;
	    strcpy( valor_constante, t ) ;

	    Mayuscula( nombre_constante ) ;

	    AgregaConstante( &lista_constante, nombre_constante, valor_constante, numero_registro ) ;

	 break;

	 case '\n':
	 case '#':
	    /* comentarios */
	 break;
	 default:
	    fprintf( stderr, "Error: 'constant.esp', linea %ld. LeeEspecConstante(...) Sintaxis incorrecta.\n", numero_registro  ) ;
	    exit( 1 ) ;

      } /* switch */
   } /* while */

   return( lista_constante ) ;
}
