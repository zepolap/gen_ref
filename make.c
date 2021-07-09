/*
 * make.c
 *
 */

#include "definici.h"
#include "bibliote.h"
#include "formato.h"
#include "funcion.h"
#include "ent_sal.h"
#include "make.h"

void GeneraArchivoMakefile( entrada_salida *lista_entrada_salida, char *directorio_salida )
{
   char nombre_archivo[NOMBRE_ARCHIVO_LARGO+1] ;
   int  numero_entrada ;
   FILE *makefile ;

   entrada_salida *e ;

   fprintf( stdout, "\nGenerando archivo 'Makefile'...\n\n" ) ;

#ifdef MSDOS
   sprintf( nombre_archivo, "%s\\makefile", directorio_salida ) ;
#else
   sprintf( nombre_archivo, "%s/Makefile", directorio_salida ) ;
#endif
   CreaArchivo( &makefile, nombre_archivo ) ;

   fprintf( makefile, "#\n# CTC S.A.\n# Area Facturacion Filiales\n") ;
   fprintf( makefile, "# SPTT Sistema de Procesamiento de Trafico Telefonico.\n" ) ;
   fprintf( makefile, "# reformat version 1.0 Copyright septiembre 1996 Pablo Lopez\n" ) ;
   fprintf( makefile, "#\n# Makefile\n#\n\n" ) ;

#ifdef MSDOS
   fprintf( makefile, "CC=TCC\n\n" ) ;
   fprintf( makefile, "INCLUDE=c:\\tc\\include\\\n\n" ) ;
   fprintf( makefile, "LIB=c:\\tc\\lib\\\n\n" ) ;
#endif

   fprintf( makefile, "FUENTES= reformat.c funcion.c formato.c mensaje.c\\\n%8c", ' ' ) ;

   numero_entrada = 0 ;
   e = lista_entrada_salida ;
   while ( e != NULL )
   {
      numero_entrada++ ;

      if ( (e->tipo == _E) || (e->tipo == _E_S) )
	 fprintf( makefile, " %s.c", e->nombre ) ;

      if ( (numero_entrada == 5) &&
           ((e->siguiente != NULL) &&
            (((e->siguiente)->tipo == _E) || ((e->siguiente)->tipo == _E_S))) ) 
      {
	 fprintf( makefile, "\\\n%8c", ' ' ) ;
	 numero_entrada = 0 ;
      }

      e = (entrada_salida *)e->siguiente ;
   }

   fprintf( makefile, "\n\n" ) ;

#ifdef MSDOS
   fprintf( makefile, "OBJETOS= reformat.obj bibliote.obj funcion.obj formato.obj mensaje.obj\\\n%8c", ' ' ) ;
#else
   fprintf( makefile, "OBJETOS= reformat.o funcion.o formato.o mensaje.o\\\n%8c", ' ' ) ;
#endif

   numero_entrada = 0 ;
   e = lista_entrada_salida ;
   while ( e != NULL )
   {
      numero_entrada++ ;

      if ( (e->tipo == _E) || (e->tipo ==_E_S) )
#ifdef MSDOS
	 fprintf( makefile, " %s.obj", e->nombre ) ;
#else
	 fprintf( makefile, " %s.o", e->nombre ) ;
#endif

      if ( (numero_entrada == 5) &&
           ((e->siguiente != NULL) &&
            (((e->siguiente)->tipo == _E) || ((e->siguiente)->tipo == _E_S))) ) 
      {
	 fprintf( makefile, "\\\n%8c", ' ' ) ;
	 numero_entrada = 0 ;
      }

      e = (entrada_salida *)e->siguiente ;
   }

   fprintf( makefile, "\n\nEJECUTABLE= reformat\n\n" ) ;

   fprintf( makefile, "DEBUG=NODEBUG\n" ) ;
   fprintf( makefile, "\n\n" ) ;

#ifdef MSDOS
   fprintf( makefile, "reformat.exe: reformat.obj\n" ) ;
   fprintf( makefile, "\t$(CC) -o reformat -L$(LIB) $(OBJETOS)\n" ) ;

   fprintf( makefile, "reformat.obj:\n" ) ;
   fprintf( makefile, "\t$(CC) -c -I$(INCLUDE) -DMSDOS $(FUENTES)\n\n" ) ;
#else
   fprintf( makefile, "all: reformat\n\n" ) ;

   fprintf( makefile, "clean:\n" ) ;
   fprintf( makefile, "\t@rm -f $(EJECUTABLE) $(OBJETOS) > /dev/null\n\n" ) ;

   fprintf( makefile, "reformat.o: $(FUENTES)\n" ) ;
   fprintf( makefile, "\t$(CC) -c -D $(DEBUG) $(FUENTES)\n\n" ) ;

   fprintf( makefile, "reformat: $(OBJETOS) bibliote.o\n" ) ;
   fprintf( makefile, "\t$(CC) -o $(EJECUTABLE) $(OBJETOS) bibliote.o\n" ) ;
   fprintf( makefile, "\tcp $(EJECUTABLE) /facfi/corresp/bin\n" ) ;
#endif
}
