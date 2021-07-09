/*
 * funcion.c
 *
 */

#include "definici.h"
#include "bibliote.h"
#include "formato.h"
#include "funcion.h"

void EliminaUltimoArgumento( argumento_funcion **lista_argumento )
{
   argumento_funcion *p, *q ;

   p = *lista_argumento ;

   while ( p->siguiente != NULL )
   {
      q = p ;
      p = (argumento_funcion *)p->siguiente ;
   }

   if ( (*lista_argumento)->siguiente == NULL )
      *lista_argumento = NULL ;
   else
      q->siguiente = NULL ;

   free( p ) ;
}

void EliminaUltimaImplementacion( implementacion **lista_implementacion )
{
   implementacion *p, *q ;

   p = *lista_implementacion ;

   while ( p->siguiente != NULL )
   {
      q = p ;
      p = (implementacion *)p->siguiente ;
   }

   if ( (*lista_implementacion)->siguiente == NULL )
      *lista_implementacion = NULL ;
   else
      q->siguiente = NULL ;

   free( p ) ;
}

void EliminaUltimaFuncion( funcion **lista_funcion )
{
   funcion *p, *q ;

   p = *lista_funcion ;

   while ( p->siguiente != NULL )
   {
      q = p ;
      p = (funcion *)p->siguiente ;
   }

   if ( (*lista_funcion)->siguiente == NULL )
      *lista_funcion = NULL ;
   else
      q->siguiente = NULL ;


   while ( p->lista_argumento != NULL )
      EliminaUltimoArgumento( &(p->lista_argumento) ) ;

   while ( p->lista_implementacion != NULL )
      EliminaUltimaImplementacion( &(p->lista_implementacion) ) ;

   free( p ) ;
}

void AgregaFuncion( char *nombre_archivo, funcion **lista_funcion, char *nombre_identificador, char *nombre_tipo, long numero_registro )
{
   funcion *p, *q ;

   q = (funcion *)malloc( sizeof(funcion) ) ;
   if ( q == NULL )
   {
      fprintf( stderr, "Error: AgregaFuncion(...) Insuficiente memoria para cargar informacion.\n" ) ;
      exit( 1 ) ;
   }

   strcpy( q->nombre, nombre_identificador ) ;
   strcpy( q->tipo, nombre_tipo ) ;
   q->lista_argumento = NULL ;
   q->lista_implementacion = NULL ;
   q->siguiente = NULL ;

   p = *lista_funcion ;

   if ( p == NULL ) /* lista nueva de funciones */
      *lista_funcion = q ;
   else
   {
      if ( strcmp(p->nombre,nombre_identificador) == 0 )
      {
	  fprintf( stderr, "Error: '%s.esp', linea %ld. AgregaFuncion(...) Doble especificacion de salida '%s'.\n", nombre_archivo, numero_registro, nombre_identificador ) ;
	  free( q ) ;
	  exit( 1 ) ;
      }

      while ( p->siguiente != NULL )
	 p = (funcion *)p->siguiente ;

      p->siguiente = (struct funcion *)q ;
   }
}

void AgregaArgumentoFuncion( char *nombre_archivo, funcion *lista_funcion, char *nombre_identificador, char *nombre_tipo, char *nombre_funcion_actual, long numero_registro )
{
   argumento_funcion *p, *q ;
   funcion *s ;

   s = lista_funcion ;

   while ( (s != NULL) && (strcmp(s->nombre,nombre_funcion_actual) != 0) )
      s = (funcion *)s->siguiente ;

   if ( s == NULL )
   {
      fprintf( stderr, "Error: '%s.esp', linea %ld. AgregaArgumentoFuncion(...) Funcion '%s' no encontrada.\n", nombre_archivo, numero_registro, nombre_funcion_actual ) ;
      exit( 1 ) ;
   }
   else
   {
      q = (argumento_funcion *)malloc( sizeof(argumento_funcion) ) ;
      if ( q == NULL )
      {
	 fprintf( stderr, "Error: AgregaArgumentoFuncion(...) Insuficiente memoria para cargar informacion.\n" ) ;
	 exit( 1 ) ;
      }

      strcpy( q->nombre, nombre_identificador ) ;
      strcpy( q->tipo, nombre_tipo ) ;
      q->siguiente = NULL ;

      p = s->lista_argumento ;

      if ( p == NULL ) /* lista nueva de argumentos */
	 s->lista_argumento = q ;
      else
      {
	 if ( strcmp(p->nombre,nombre_identificador) == 0 )
	 {
	    fprintf( stderr, "Error: '%s.esp', linea %ld. AgregaArgumentoFuncion(...) Doble especificacion de argumento '%s' para la funcion '%s'.\n", nombre_archivo, numero_registro, nombre_identificador, nombre_funcion_actual ) ;
	    free( q ) ;
	    exit( 1 ) ;
	 }

	 while ( p->siguiente != NULL )
	    p = (argumento_funcion *)p->siguiente ;

	 p->siguiente = (struct argumento_funcion *)q ;
      }
   }
}

void AgregaImplFuncion( char *nombre_archivo, funcion *lista_funcion, char *registro, char *nombre_funcion_actual, long numero_registro )
{
   implementacion *p, *q ;
   funcion *s ;

   s = lista_funcion ;

   while ( (s != NULL) && (strcmp(s->nombre,nombre_funcion_actual) != 0) )
      s = (funcion *)s->siguiente ;

   if ( s == NULL )
   {
      fprintf( stderr, "Error: '%s.esp', linea %ld. AgregaImplFuncion(...) Funcion '%s' no encontrada.\n", nombre_archivo, numero_registro, nombre_funcion_actual ) ;
      exit( 1 ) ;
   }
   else
   {
      q = (implementacion *)malloc( sizeof(implementacion) ) ;
      if ( q == NULL )
      {
	 fprintf( stderr, "Error: AgregaImplFuncion(...) Insuficiente memoria para cargar informacion.\n" ) ;
	 exit( 1 ) ;
      }

      strcpy( q->registro, registro ) ;
      q->siguiente = NULL ;

      p = s->lista_implementacion ;

      if ( p == NULL ) /* lista nueva de implementaciones */
	 s->lista_implementacion = q ;
      else
      {
	 while ( p->siguiente != NULL )
	    p = (implementacion *)p->siguiente ;

	 p->siguiente = (struct implementacion *)q ;
      }
   }
}

funcion *LeeEspecFuncionesGlobales( FILE *arch_esp_funciones_globales )
{
   char registro[BUFFER_LARGO] ;
   char nombre_identificador[NOMBRE_IDENTIFICADOR_LARGO+1] ;
   char tipo_identificador[NOMBRE_TIPO_LARGO+1] ;
   char nombre_tipo[NOMBRE_TIPO_LARGO+1] ;
   char nombre_funcion_actual[NOMBRE_FUNCION_LARGO+1] ;
   int  en_funcion=FALSE ;
   int  en_esp_funcion=FALSE ;
   int  en_esp_arg_funcion=FALSE ;
   int  en_esp_impl_funcion=FALSE ;
   long numero_registro ;

   funcion *lista_funciones_globales=NULL ;

   numero_registro = 0L ;
   while ( LeeRegistro(arch_esp_funciones_globales,registro) )
   {
      numero_registro++ ;

      switch( registro[0] )
      {
	 case 'f':
	 case 'F':
	    if ( en_esp_funcion     ||
		 en_esp_arg_funcion ||
		 (registro[1] != ' ' && registro[1] != '\t') )
	    {
	       fprintf( stderr, "Error: 'funcion.esp', linea %ld. LeeEspecFuncionesGlobales(...) Sintaxis incorrecta.\n", numero_registro ) ;
	       exit( 1 ) ;
	    }
	    else
	    {
	       en_funcion = TRUE ;
	       en_esp_funcion = TRUE ;
	       en_esp_impl_funcion = FALSE ;
	    }

	    if ( NumeroPalabras(registro+2) < 2 )
	    {
	       fprintf( stderr, "Error: 'funcion.esp', linea %ld. LeeEspecFuncionesGlobales(...) Se esperan a lo menos dos argumentos.\n", numero_registro ) ;
	       exit( 1 ) ;
	    }

	    sscanf( registro+2, "%s %s \n", nombre_identificador, tipo_identificador ) ;
	    ValidaTipo( "funcion", tipo_identificador, numero_registro, nombre_tipo ) ;

	    AgregaFuncion( "funcion", &lista_funciones_globales, nombre_identificador, nombre_tipo, numero_registro ) ;

	    strcpy( nombre_funcion_actual, nombre_identificador ) ;

	 break;
	 case 'a':
	 case 'A':
	    if ( !en_funcion          ||
		  en_esp_impl_funcion ||
		  (registro[1] != ' ' && registro[1] != '\t') )
	    {
	       fprintf( stderr, "Error: 'funcion.esp', linea %ld. LeeEspecFuncionesGlobales(...) Sintaxis incorrecta.\n", numero_registro  ) ;
	       exit( 1 ) ;
	    }
	    else
	    {
	       en_esp_arg_funcion = TRUE ;
	       en_esp_funcion = FALSE ;
	    }

	    if ( NumeroPalabras(registro+2) < 2 )
	    {
	       fprintf( stderr, "Error: 'funcion.esp', linea %ld. LeeEspecFuncionesGlobales(...) Se esperan a lo menos dos argumentos.\n", numero_registro  ) ;
	       exit( 1 ) ;
	    }

	    sscanf( registro+2, "%s %s \n", nombre_identificador, tipo_identificador ) ;

	    ValidaTipo( "funcion", tipo_identificador, numero_registro, nombre_tipo ) ;

	    AgregaArgumentoFuncion( "funcion", lista_funciones_globales, nombre_identificador, nombre_tipo, nombre_funcion_actual, numero_registro ) ;

	 break;
	 case 'i':
	 case 'I':
	    if ( !en_funcion )
	    {
	       fprintf( stderr, "Error: 'funcion.esp', linea %ld. LeeEspecFuncionesGlobales(...) Sintaxis incorrecta.\n", numero_registro  ) ;
	       exit( 1 ) ;
	    }
	    else
	    {
	       en_esp_impl_funcion = TRUE ;
	       en_esp_funcion = FALSE ;
	       en_esp_arg_funcion = FALSE ;
	    }

	    AgregaImplFuncion( "funcion", lista_funciones_globales, registro+1, nombre_funcion_actual, numero_registro ) ;

	 break;
	 case '\n':
	 case '#':
	    /* comentarios */
	 break;
	 default:
	    fprintf( stderr, "Error: 'funcion.esp', linea %ld. LeeEspecFormato(...) Sintaxis incorrecta.\n", numero_registro  ) ;
	    exit( 1 ) ;

      } /* switch */
   } /* while */

   return( lista_funciones_globales ) ;
}


void GeneraArchivofuncion_h( funcion *lista_funciones_globales, char *directorio_salida )
{
  char nom_arch_funcion_h[NOMBRE_ARCHIVO_LARGO+1] ;
  int numero_argumento ;
  FILE *arch_funcion_h ;

  funcion *f ;
  argumento_funcion *a ;

  fprintf( stdout, "Generando archivo 'funcion.h'...\n\n" ) ;

#ifdef MSDOS
  sprintf( nom_arch_funcion_h, "%s\\funcion.h", directorio_salida ) ;
#else
  sprintf( nom_arch_funcion_h, "%s/funcion.h", directorio_salida ) ;
#endif
  CreaArchivo( &arch_funcion_h, nom_arch_funcion_h ) ;

  fprintf( arch_funcion_h, "/*\n *   funcion.h\n *\n */\n\n" ) ;

  f = lista_funciones_globales ;
  while ( f != NULL )
  {
     numero_argumento = 0 ;

     fprintf( arch_funcion_h, "%s%s(", f->tipo, f->nombre ) ;

     a = f->lista_argumento ;
     while ( a != NULL )
     {
	numero_argumento++ ;
	if ( a->siguiente != NULL )
	   fprintf( arch_funcion_h, " %s%s,", a->tipo, a->nombre ) ;
	else
	   fprintf( arch_funcion_h, " %s%s ) ;\n", a->tipo, a->nombre ) ;

	a = (argumento_funcion *)a->siguiente ;
     }

     if ( numero_argumento == 0 )
	fprintf( arch_funcion_h, " void ) ;\n" ) ;

     f = (funcion *)f->siguiente ;
  }
}


void GeneraArchivofuncion_c( funcion *lista_funciones_globales, char *directorio_salida )
{
  char nom_arch_funcion_c[NOMBRE_ARCHIVO_LARGO+1] ;
  int numero_argumento ;
  FILE *arch_funcion_c ;

  funcion *f ;
  argumento_funcion *a ;
  implementacion *i ;

  fprintf( stdout, "Generando archivo 'funcion.c'...\n\n" ) ;

#ifdef MSDOS
  sprintf( nom_arch_funcion_c, "%s\\funcion.c", directorio_salida ) ;
#else
  sprintf( nom_arch_funcion_c, "%s/funcion.c", directorio_salida ) ;
#endif
  CreaArchivo( &arch_funcion_c, nom_arch_funcion_c ) ;

  fprintf( arch_funcion_c, "/*\n *   funcion.c\n *\n */\n\n" ) ;
  fprintf( arch_funcion_c, "#include \"definici.h\"\n" ) ;
  fprintf( arch_funcion_c, "#include \"bibliote.h\"\n\n" ) ;

  /* funciones */
  f = lista_funciones_globales ;
  while ( f != NULL )
  {
     numero_argumento = 0 ;

     fprintf( arch_funcion_c, "%s%s(", f->tipo, f->nombre ) ;

     /* argumentos */
     a = f->lista_argumento ;
     while ( a != NULL )
     {
	numero_argumento++ ;
	if ( a->siguiente != NULL )
	   fprintf( arch_funcion_c, " %s%s,", a->tipo, a->nombre ) ;
	else
	   fprintf( arch_funcion_c, " %s%s )\n{\n", a->tipo, a->nombre ) ;

	a = (argumento_funcion *)a->siguiente ;
     }

     if ( numero_argumento == 0 )
	fprintf( arch_funcion_c, " void )\n{\n" ) ;

     /* implementaciones */
     i = f->lista_implementacion ;
     while ( i != NULL )
     {
	fprintf( arch_funcion_c, "%s", i->registro ) ;

	i = (implementacion *)i->siguiente ;
     }

     fprintf( arch_funcion_c, "}\n\n" ) ;

     f = (funcion *)f->siguiente ;
  }
}
