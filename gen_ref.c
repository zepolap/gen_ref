/*
 *   gen_ref.c
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
#include "make.h"
#include "gen_ref.h"

void Uso( char *nombre_programa )
{
   fprintf( stdout, "CTC S.A.\nArea Facturacion Filiales.\n" ) ;
   fprintf( stdout, "gen_ref version 2.1 Copyright octubre 1998 Pablo Lopez\n\n" ) ;
   fprintf( stdout, "Uso: %s -e <directorio_especificaciones> -s <directorio_salida>\n", nombre_programa ) ;
   exit( 1 ) ;
}

#ifndef MSDOS
int LeeArgumentos( int argc, char *argv[], argumento *argumentos )
{
  int _e=FALSE ;
  int _s=FALSE ;
  int comando ;

  if ( argc < 3 )
     return( FALSE ) ;

  while( (comando = getopt(argc,argv,"e:s:")) != EOF )
  {
    switch( comando )
    {
       case 'e':
	  strcpy( argumentos->directorio_especificaciones, optarg ) ;
	  _e = TRUE ;
       break;
       case 's':
	  strcpy( argumentos->directorio_salida, optarg ) ;
	  _s = TRUE ;
       break;
       default:
	  return( FALSE ) ;
    }
  }

  if ( !_e )
  {
     fprintf( stderr, "Error. LeeArgumentos(...) Falta definir directorio de especificaciones.\n" ) ;
     exit( 1 ) ;
  }

  if ( !_s )
  {
     fprintf( stderr, "Error. LeeArgumentos(...) Falta definir directorio de salida.\n" ) ;
     exit( 1 ) ;
  }

  return( TRUE ) ;
}
#endif

int main( int argc, char *argv[] )
{
   int error=FALSE ;
   int numero_entrada=0 ;
   char comando[BUFFER_LARGO+1] ;
   char nombre_archivo_espec[NOMBRE_ARCHIVO_LARGO+1] ;
   FILE *archivo_espec ;

   argumento argumentos ;
   constante *lista_constante ;
   mensaje   *lista_mensaje ;
   tabla     *lista_tabla ;
   funcion   *lista_funciones_globales ;
   formato   *lista_formatos ;
   entrada_salida *p, *lista_entrada_salida ;
   reformateo reformateos ;

#ifndef MSDOS
   error = !LeeArgumentos( argc, argv, &argumentos ) ;
#else
   if ( argc == 1 )
   {
      strcpy( argumentos.directorio_especificaciones, "c:\\temp" ) ;
      strcpy( argumentos.directorio_salida, "..\\temp" ) ;
      error = FALSE ;
   }
   else
      Uso( argv[0] ) ;
#endif

   if ( error )
      Uso( argv[0] ) ;

   fprintf( stdout, "gen_ref version 2.1 Copyright octubre 1998.\n\n" ) ;

   fprintf( stdout, "Leyendo especificacion de formatos...\n\n" ) ;
#ifdef MSDOS
   sprintf( nombre_archivo_espec, "%s\\formato.esp", argumentos.directorio_especificaciones ) ;
#else
   sprintf( nombre_archivo_espec, "%s/formato.esp", argumentos.directorio_especificaciones ) ;
#endif
   LeeArchivo( &archivo_espec, nombre_archivo_espec ) ;
   lista_formatos = LeeEspecFormato( archivo_espec ) ;

   fclose( archivo_espec ) ;

/*   ImprimeFormatos( lista_formatos ) ; */

   GeneraArchivoformato_h( lista_formatos, argumentos.directorio_salida ) ;
   GeneraArchivoformato_c( lista_formatos, argumentos.directorio_salida ) ;

   fprintf( stdout, "Leyendo especificacion de funciones globales...\n\n" ) ;
#ifdef MSDOS
   sprintf( nombre_archivo_espec, "%s\\funcion.esp", argumentos.directorio_especificaciones ) ;
#else
   sprintf( nombre_archivo_espec, "%s/funcion.esp", argumentos.directorio_especificaciones ) ;
#endif
   LeeArchivo( &archivo_espec, nombre_archivo_espec ) ;
   lista_funciones_globales = LeeEspecFuncionesGlobales( archivo_espec ) ;

   fclose( archivo_espec ) ;

/*   ImprimeFuncionesGlobales( lista_funciones_globales ) ; */

   GeneraArchivofuncion_h( lista_funciones_globales, argumentos.directorio_salida ) ;
   GeneraArchivofuncion_c( lista_funciones_globales, argumentos.directorio_salida ) ;

   fprintf( stdout, "Leyendo especificacion de entradas y salidas...\n\n" ) ;
#ifdef MSDOS
   sprintf( nombre_archivo_espec, "%s\\ent_sal.esp", argumentos.directorio_especificaciones ) ;
#else
   sprintf( nombre_archivo_espec, "%s/ent_sal.esp", argumentos.directorio_especificaciones ) ;
#endif
   LeeArchivo( &archivo_espec, nombre_archivo_espec ) ;
   lista_entrada_salida = LeeEspecEntradaSalida( archivo_espec, lista_formatos ) ;

   fclose( archivo_espec ) ;

/*  ImprimeEntrada_Salida( lista_entrada_salida ) ; */

   fprintf( stdout, "Leyendo especificacion de constantes...\n\n" ) ;
#ifdef MSDOS
   sprintf( nombre_archivo_espec, "%s\\constant.esp", argumentos.directorio_especificaciones ) ;
#else
   sprintf( nombre_archivo_espec, "%s/constant.esp", argumentos.directorio_especificaciones ) ;
#endif
   LeeArchivo( &archivo_espec, nombre_archivo_espec ) ;

   lista_constante = LeeEspecConstante( archivo_espec ) ;

   fclose( archivo_espec ) ;

/*   ImprimeMensaje( lista_constante ) ; */


   fprintf( stdout, "Leyendo especificacion de mensajes...\n\n" ) ;
#ifdef MSDOS
   sprintf( nombre_archivo_espec, "%s\\mensaje.esp", argumentos.directorio_especificaciones ) ;
#else
   sprintf( nombre_archivo_espec, "%s/mensaje.esp", argumentos.directorio_especificaciones ) ;
#endif
   LeeArchivo( &archivo_espec, nombre_archivo_espec ) ;

   lista_mensaje = LeeEspecMensaje( archivo_espec ) ;

   fclose( archivo_espec ) ;

/*   ImprimeMensaje( lista_mensaje ) ; */

   fprintf( stdout, "Leyendo especificacion de tablas...\n\n" ) ;
#ifdef MSDOS
   sprintf( nombre_archivo_espec, "%s\\tabla.esp", argumentos.directorio_especificaciones ) ;
#else
   sprintf( nombre_archivo_espec, "%s/tabla.esp", argumentos.directorio_especificaciones ) ;
#endif
   LeeArchivo( &archivo_espec, nombre_archivo_espec ) ;
   lista_tabla = LeeEspecTabla( archivo_espec ) ;

   fclose( archivo_espec ) ;

/*   ImprimeTabla( lista_tabla ) ; */

   GeneraArchivoMensaje_c( lista_mensaje, argumentos.directorio_salida ) ;
   GeneraArchivoDefinici_h( lista_constante, lista_mensaje, lista_tabla, lista_entrada_salida, argumentos.directorio_salida ) ;

   p = lista_entrada_salida ;

   while ( p != NULL )
   {
      if ( (p->tipo == _E) || (p->tipo == _E_S) )
      {
	 numero_entrada++ ;
	 InicializaReformateos( &reformateos, numero_entrada ) ;

	 strcpy( reformateos.nombre_entrada, p->nombre ) ;
	 strcpy( reformateos.nombre_formato, p->formato ) ;

	 fprintf( stdout, "Leyendo especificacion de reformateos '%s'...\n\n", p->nombre ) ;
#ifdef MSDOS
	 sprintf( nombre_archivo_espec, "%s\\%s.esp", argumentos.directorio_especificaciones, p->nombre ) ;
#else
	 sprintf( nombre_archivo_espec, "%s/%s.esp", argumentos.directorio_especificaciones, p->nombre ) ;
#endif
	 LeeArchivo( &archivo_espec, nombre_archivo_espec ) ;

	 LeeEspecReformateos( archivo_espec, &reformateos, lista_entrada_salida, lista_formatos ) ;

	 fclose( archivo_espec ) ;

	 GeneraArchivoReformateos_h( reformateos, lista_entrada_salida, argumentos.directorio_salida ) ;
	 GeneraArchivoReformateos_c( reformateos, argumentos.directorio_salida ) ;
      }

      p = (entrada_salida *)p->siguiente ;
   }

   GeneraArchivoReformat_h( argumentos.directorio_salida ) ;
   GeneraArchivoReformat_c( lista_entrada_salida, argumentos.directorio_salida ) ;

   fprintf( stdout, "Copiando biblioteca de rutinas...\n\n", comando ) ;
#ifdef MSDOS
   sprintf( comando, "copy bibliote.h %s", argumentos.directorio_salida ) ;
   system( comando ) ;
   sprintf( comando, "copy bibliote.obj %s", argumentos.directorio_salida ) ;
#else
   sprintf( comando, "cp /facfi/corresp/fuentes/gen_ref/bibliote.h /facfi/corresp/fuentes/gen_ref/bibliote.o %s", argumentos.directorio_salida ) ;
#endif
   system( comando ) ;

   GeneraArchivoMakefile( lista_entrada_salida, argumentos.directorio_salida ) ;

   return( 0 ) ;
}
