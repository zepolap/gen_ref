/*
 *   reformat.c
 *
 */

#include "definici.h"
#include "bibliote.h"
#include "formato.h"
#include "funcion.h"
#include "ent_sal.h"

void GeneraArchivoReformat_h( char *directorio_salida )
{
   char nombre_archivo[NOMBRE_ARCHIVO_LARGO+1] ;
   FILE *reformat_h ;

   fprintf( stdout, "Generando archivo 'reformat.h'...\n\n" ) ;

#ifdef MSDOS
	sprintf( nombre_archivo, "%s\\reformat.h", directorio_salida ) ;
#else
	sprintf( nombre_archivo, "%s/reformat.h", directorio_salida ) ;
#endif
	CreaArchivo( &reformat_h, nombre_archivo ) ;

	fprintf( reformat_h, "/*\n *   reformat.h\n *\n */\n\n" ) ;

	fprintf( reformat_h, "typedef struct archivo {\n\n" ) ;
	fprintf( reformat_h, "\tFILE *archivo_entrada ;\n" ) ;
	fprintf( reformat_h, "\tFILE *archivo_estadisticas ;\n" ) ;
	fprintf( reformat_h, "\tFILE *archivo_error ;\n" ) ;
	fprintf( reformat_h, "\tFILE *archivo_error_msg ;\n" ) ;	 
	fprintf( reformat_h, "\tFILE *archivo_sobrante ;\n" ) ;
	fprintf( reformat_h, "\tFILE *archivo_sobrante_msg ;\n" ) ;
	fprintf( reformat_h, "} archivo ;\n\n" ) ;

	fprintf( reformat_h, "typedef struct argumento {\n\n" ) ;
	fprintf( reformat_h, "\tint  entrada ;\n" ) ;
	fprintf( reformat_h, "\tchar nombre_entrada[NOMBRE_ENTRADA_LARGO+1] ;\n" ) ;
	fprintf( reformat_h, "\tchar nombre_archivo_entrada[NOMBRE_ARCHIVO_LARGO+1] ;\n" ) ;
	fprintf( reformat_h, "\tchar directorio_parametros[NOMBRE_DIRECTORIO_LARGO+1] ;\n" ) ;
	fprintf( reformat_h, "\tchar directorio_temporal[NOMBRE_DIRECTORIO_LARGO+1] ;\n" ) ;
	fprintf( reformat_h, "\tint proceso_concurrente ;\n" ) ;
	fprintf( reformat_h, "\tint carga_tablas ;\n\n" ) ;
	fprintf( reformat_h, "} argumento ;\n" ) ;

}

void GeneraArchivoReformat_c( entrada_salida *lista_entrada_salida, char *directorio_salida )
{
	char nombre_archivo[NOMBRE_ARCHIVO_LARGO+1] ;
	char comando[BUFFER_LARGO+1] ;
	int  numero_entrada ;
	FILE *reformat ;

	entrada_salida *e ;

	fprintf( stdout, "Generando archivo 'reformat.c'...\n\n" ) ;

#ifdef MSDOS
	sprintf( nombre_archivo, "%s\\reformat.c", directorio_salida ) ;
#else
	sprintf( nombre_archivo, "%s/reformat.c", directorio_salida ) ;
#endif
	CreaArchivo( &reformat, nombre_archivo ) ;

	fprintf( reformat, "/*\n *   reformat.c\n *\n */\n\n" ) ;
	fprintf( reformat, "#include \"definici.h\"\n" ) ;
	fprintf( reformat, "#include \"bibliote.h\"\n" ) ;
	fprintf( reformat, "#include \"reformat.h\"\n\n" ) ;

	fprintf( reformat, "void Uso( char *nombre_programa )\n{\n" ) ;
	fprintf( reformat, "\tfprintf( stdout, \"CTC S.A.\\nArea Facturacion Filiales.\\n\" ) ;\n" ) ;
	fprintf( reformat, "\tfprintf( stdout, \"reformateo version 3.0 Copyright septiembre 1996 Pablo Lopez\\n\\n\" ) ;\n" ) ;
	fprintf( reformat, "\tfprintf( stdout, \"Uso: %%s -e <entrada> [-T] [-t <directorio_temporal>] [-p <directorio_parametros>] [-c <proceso_concurrente>] [-a <archivo_entrada>]\\n\", nombre_programa ) ;\n\texit( 1 ) ;\n}\n\n" ) ;

	fprintf( reformat, "static int ObtenerEntrada( char *nombre_entrada )\n{\n" ) ;
	fprintf( reformat, "\tMinuscula( nombre_entrada ) ;\n\n" ) ;

	numero_entrada = 0 ;
	e = lista_entrada_salida ;
	while ( e != NULL )
	{
		if ( (e->tipo == _E) || (e->tipo ==_E_S) )
		{
			numero_entrada++ ;
			if ( numero_entrada == 1 )
				fprintf( reformat, "\tif ( strcmp(nombre_entrada,\"%s\") == 0 )\n", e->nombre ) ;
			else
				fprintf( reformat, "\telse if ( strcmp(nombre_entrada,\"%s\") == 0 )\n", e->nombre ) ;
			fprintf( reformat, "\t\treturn( ID_%s ) ;\n", e->nombre ) ;
		}

		e = (entrada_salida *)e->siguiente ;
	}

	fprintf( reformat, "\telse\n" ) ;
	fprintf( reformat, "\t\treturn( -1 ) ;\n}\n\n" ) ;

	fprintf( reformat, "static int LeeArgumentos( int argc, char *argv[], argumento *argumentos )\n{\n" ) ;
	fprintf( reformat, "\tint _a=FALSE, _c=FALSE, _e=FALSE, _p=FALSE, _t=FALSE, _T=FALSE ;\n" ) ;
	fprintf( reformat, "\tint comando ;\n\n" ) ;

	fprintf( reformat, "\tif ( argc < 2 )\n\t{\n\t\treturn( FALSE ) ;\n\t}\n\n" ) ;

	fprintf( reformat, "\twhile( ( comando = getopt( argc , argv , \"a:c:e:p:t:T\")) != EOF )\n\t{\n" ) ;
	fprintf( reformat, "\t\tswitch( comando )\n\t\t{\n" ) ;

	fprintf( reformat, "\t\t\tcase 'a': /* archivo */\n" ) ;
	fprintf( reformat, "\t\t\t\tstrcpy( argumentos->nombre_archivo_entrada, optarg ) ;\n\t\t\t\t_a = TRUE ;\n\t\t\t\tbreak;\n\n" ) ;

	fprintf( reformat, "\t\t\tcase 'c': /* proceso concurrente */\n" ) ;
	fprintf( reformat, "\t\t\t\targumentos->proceso_concurrente = atoi( optarg ) ;\n\t\t\t\t_c = TRUE ;\n\t\t\t\tbreak;\n\n" ) ;

	fprintf( reformat, "\t\t\tcase 'e': /* entrada */\n" ) ;
	fprintf( reformat, "\t\t\t\tstrcpy( argumentos->nombre_entrada, optarg ) ;\n" ) ;
	fprintf( reformat, "\t\t\t\targumentos->entrada = ObtenerEntrada( argumentos->nombre_entrada ) ;\n\t\t\t\t_e = TRUE ;\n\t\t\t\tbreak;\n\n" ) ;


	fprintf( reformat, "\t\t\tcase 'p': /* directorio parametros */\n" ) ;
	fprintf( reformat, "\t\t\t\tstrcpy( argumentos->directorio_parametros, optarg ) ;\n" ) ;
	fprintf( reformat, "\t\t\t\tif ( argumentos->directorio_parametros[strlen(argumentos->directorio_parametros)-1] == '/' )\n\t\t\t\t{\n" ) ;
	fprintf( reformat, "\t\t\t\t\targumentos->directorio_parametros[strlen(argumentos->directorio_parametros)-1] = '\\0' ;\n\t\t\t\t}\n\t\t\t\t_p = TRUE ;\n\t\t\t\tbreak;\n\n" ) ;

	fprintf( reformat, "\t\t\tcase 't': /* directorio temporal */\n" ) ;
	fprintf( reformat, "\t\t\t\tstrcpy( argumentos->directorio_temporal, optarg ) ;\n" ) ;
	fprintf( reformat, "\t\t\t\tif ( argumentos->directorio_temporal[strlen(argumentos->directorio_temporal)-1] == '/' )\n\t\t\t\t{\n" ) ;
	fprintf( reformat, "\t\t\t\t\targumentos->directorio_temporal[strlen(argumentos->directorio_temporal)-1] = '\\0' ;\n\t\t\t\t}\n\t\t\t\t_t = TRUE ;\n\t\t\t\tbreak;\n\n" ) ;

	fprintf( reformat, "\t\t\tcase 'T': /* carga todas las tablas de una vez */\n" ) ;
	fprintf( reformat, "\t\t\t\targumentos->carga_tablas = TRUE ;\n" ) ;
	fprintf( reformat, "\t\t\t\t_T = TRUE ;\n\t\t\t\tbreak;\n\n" ) ;

	fprintf( reformat, "\t\t\tdefault:\n\t\t\t\treturn( FALSE ) ;\n\t\t}\n\t}\n\n" ) ;

	fprintf( reformat, "\tif ( !_a )\n\t{\n" ) ;
	fprintf( reformat, "\t\tstrcpy( argumentos->nombre_archivo_entrada, \"\" ) ;\n\t}\n\n" ) ;

	fprintf( reformat, "\tif ( !_c )\n\t{\n" ) ;
	fprintf( reformat, "\t\targumentos->proceso_concurrente = 0 ;\n\t}\n\n" ) ;

	fprintf( reformat, "\tif ( !_e || (argumentos->entrada < 0) )\n\t{\n" ) ;
	fprintf( reformat, "\t\tfprintf( stderr, \"Error. LeeArgumentos(...) Falta definir entrada.\\n\" ) ;\n\t\texit( 1 ) ;\n\t}\n\n" ) ;

	fprintf( reformat, "\tif ( !_p )\n\t{\n" ) ;
	fprintf( reformat, "\t\tstrcpy( argumentos->directorio_parametros, \"parametros\" ) ;\n\t}\n\n" ) ;

	fprintf( reformat, "\tif ( !_t )\n\t{\n" ) ;
	fprintf( reformat, "\t\tstrcpy( argumentos->directorio_temporal, \"temp\" ) ;\n\t}\n\n" ) ;

	fprintf( reformat, "\tif ( !_T )\n\t{\n" ) ;
	fprintf( reformat, "\t\targumentos->carga_tablas = FALSE ;\n\t}\n\n" ) ;

	fprintf( reformat, "\treturn( TRUE ) ;\n}\n\n" ) ;

	fprintf( reformat, "int main( int argc, char *argv[] )\n{\n" ) ;
	fprintf( reformat, "\tint error=FALSE ;\n" ) ;
	fprintf( reformat, "\tchar nombre_archivo[NOMBRE_ARCHIVO_LARGO+1] ;\n\n" ) ;

	fprintf( reformat, "\tarchivo archivos ;\n" ) ;
	fprintf( reformat, "\targumento argumentos ;\n\n" ) ;

	fprintf( reformat, "\terror = !LeeArgumentos( argc, argv, &argumentos ) ;\n" ) ;
	fprintf( reformat, "\tif ( error )\n\t{\n\t\tUso( argv[0] ) ;\n\t}\n\n" ) ;

	fprintf( reformat, "\tfprintf( stdout, \"reformateo version 3.0 Copyright septiembre 1996\\n\\n\" ) ;\n" ) ;
	fprintf( reformat, "\tfprintf( stdout, \"Inicializando tablas de parametros...\\n\\n\" ) ;\n" ) ;
	fprintf( reformat, "\tInicializaTablas() ;\n\n" ) ;

	fprintf( reformat, "\tif ( argumentos.carga_tablas )\n\t{\n" ) ;
	fprintf( reformat, "\t\tfprintf( stdout, \"Cargando tablas de parametros...\\n\\n\" ) ;\n" ) ;
	fprintf( reformat, "\t\tleeTrdString() ;\n\t}\n\n" ) ;

	fprintf( reformat, "\tInicDirecParametros( argumentos.directorio_parametros ) ;\n\n" ) ;

	fprintf( reformat, "\t/* archivo de entrada */\n" ) ;
	fprintf( reformat, "\tstrcpy( nombre_archivo, argumentos.nombre_archivo_entrada ) ;\n" ) ;
	fprintf( reformat, "\tLeeArchivo( &(archivos.archivo_entrada), nombre_archivo ) ;\n\n" ) ;

	fprintf( reformat, "\t/* archivo de estadisticas */\n" ) ;
	fprintf( reformat, "\tif ( argumentos.proceso_concurrente > 0 )\n\t{\n" ) ;
	fprintf( reformat, "\t\tsprintf( nombre_archivo, \"%%s/estadist.dat.%%d\", argumentos.directorio_temporal, argumentos.proceso_concurrente ) ;\n\t}\n" ) ;
	fprintf( reformat, "\telse\n\t{\n" ) ;
	fprintf( reformat, "\t\tsprintf( nombre_archivo, \"%%s/estadist.dat\", argumentos.directorio_temporal ) ;\n\t}\n" ) ;
	fprintf( reformat, "\tCreaArchivo( &(archivos.archivo_estadisticas), nombre_archivo ) ;\n\n" ) ;

	fprintf( reformat, "\t/* archivo de errores */\n" ) ;
	fprintf( reformat, "\tif ( argumentos.proceso_concurrente > 0 )\n\t{\n" ) ;
	fprintf( reformat, "\t\tsprintf( nombre_archivo, \"%%s/error.dat.%%d\", argumentos.directorio_temporal, argumentos.proceso_concurrente ) ;\n\t}\n" ) ;
	fprintf( reformat, "\telse\n\t{\n" ) ;
	fprintf( reformat, "\t\tsprintf( nombre_archivo, \"%%s/error.dat\", argumentos.directorio_temporal ) ;\n\t}\n" ) ;
	fprintf( reformat, "\tCreaArchivo( &(archivos.archivo_error), nombre_archivo ) ;\n\n" ) ;

	fprintf( reformat, "\t/* archivo de mensajes error */\n" ) ;
	fprintf( reformat, "\tif ( argumentos.proceso_concurrente > 0 )\n\t{\n" ) ;
	fprintf( reformat, "\t\tsprintf( nombre_archivo, \"%%s/error.msg.%%d\", argumentos.directorio_temporal, argumentos.proceso_concurrente ) ;\n\t}\n" ) ;
	fprintf( reformat, "\telse\n\t{\n" ) ;
	fprintf( reformat, "\t\tsprintf( nombre_archivo, \"%%s/error.msg\", argumentos.directorio_temporal ) ;\n\t}\n" ) ;
	fprintf( reformat, "\tCreaArchivo( &(archivos.archivo_error_msg), nombre_archivo ) ;\n\n" ) ;

	fprintf( reformat, "\t/* archivo de sobrantes */\n" ) ;
	fprintf( reformat, "\tif ( argumentos.proceso_concurrente > 0 )\n\t{\n" ) ;
	fprintf( reformat, "\t\tsprintf( nombre_archivo, \"%%s/sobrante.dat.%%d\", argumentos.directorio_temporal, argumentos.proceso_concurrente ) ;\n\t}\n" ) ;
	fprintf( reformat, "\telse\n\t{\n" ) ;
	fprintf( reformat, "\t\tsprintf( nombre_archivo, \"%%s/sobrante.dat\", argumentos.directorio_temporal ) ;\n\t}\n" ) ;
	fprintf( reformat, "\tCreaArchivo( &(archivos.archivo_sobrante), nombre_archivo ) ;\n\n" ) ;

	fprintf( reformat, "\t/* archivo de mensajes sobrante */\n" ) ;
	fprintf( reformat, "\tif ( argumentos.proceso_concurrente > 0 )\n\t{\n" ) ;
	fprintf( reformat, "\t\tsprintf( nombre_archivo, \"%%s/sobrante.msg.%%d\", argumentos.directorio_temporal, argumentos.proceso_concurrente ) ;\n\t}\n" ) ;
	fprintf( reformat, "\telse\n\t{\n" ) ;
	fprintf( reformat, "\t\tsprintf( nombre_archivo, \"%%s/sobrante.msg\", argumentos.directorio_temporal ) ;\n\t}\n" ) ;
	fprintf( reformat, "\tCreaArchivo( &(archivos.archivo_sobrante_msg), nombre_archivo ) ;\n\n" ) ;

	fprintf( reformat, "\tfprintf( stdout, \"reformateando...\\n\\n\" ) ;\n\n" ) ;

	fprintf( reformat, "#ifdef DEBUG\n" ) ;
	fprintf( reformat, "\tfprintf( stderr, \"INICIO DEPURACION\\n\" ) ;\n" ) ;
	fprintf( reformat, "\tfprintf( stderr, \"Proceso de depuracion reformateo entrada '%%s'.\\n\", argumentos.nombre_entrada ) ;\n" ) ;
	fprintf( reformat, "#endif\n\n" ) ;

	fprintf( reformat, "\tswitch( argumentos.entrada )\n\t{\n" ) ;

	e = lista_entrada_salida ;
	while ( e != NULL )
	{
		if ( (e->tipo == _E) || (e->tipo ==_E_S) )
		{
			fprintf( reformat, "\t\tcase ID_%s:\n", e->nombre ) ;
			fprintf( reformat, "\t\t\tReformatea_%s( &argumentos, &archivos ) ;\n\t\t\tbreak;\n\n", e->nombre ) ;
		}

		e = (entrada_salida *)e->siguiente ;
	}

	fprintf( reformat, "\t\tdefault:\n" ) ;
	fprintf( reformat, "\t\t\tfprintf( stderr, \"Error. main(...) Entrada '%%d' no implementada.\\n\", argumentos.entrada ) ;\n " ) ;
	fprintf( reformat, "\t\t\texit( 1 ) ;\n\t}\n\n" ) ;

	fprintf( reformat, "\tfprintf( stdout, \"\\n\" ) ;\n\n" ) ;

	fprintf( reformat, "#ifdef DEBUG\n" ) ;
	fprintf( reformat, "\tfprintf( stderr, \"FIN DEPURACION\\n\" ) ;\n" ) ;
	fprintf( reformat, "#endif\n\n" ) ;

	fprintf( reformat, "\treturn( 0 ) ;\n}\n" ) ;

}
