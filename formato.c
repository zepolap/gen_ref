/*
 * formato.c
 *
 */

#include "definici.h"
#include "bibliote.h"
#include "formato.h"

void ValidaTipo( char *nombre_archivo, char *tipo, long numero_registro, char *nombre_tipo )
{
/*   fprintf ( stderr,"ANTES: %s\t", tipo );*/
   Minuscula( tipo ) ;
/*   fprintf ( stderr,"DESPUES: %s\n", tipo ); */

   if ( (strcmp(tipo,"entero") != 0)     &&
        (strcmp(tipo,"decimal") != 0)    &&
        (strcmp(tipo,"caracter") != 0)   &&
        (strcmp(tipo,"caracteres") != 0) &&
        (strcmp(tipo,"fecha") != 0)      &&
        (strcmp(tipo,"hora") != 0)       &&
        (strcmp(tipo,"booleano") != 0)   &&
        (strcmp(tipo,"nulo") != 0) )
   {
      fprintf( stderr, "Error: '%s.esp', linea %ld. ValidaTipo(...) Especificacion de tipo '%s' no valida.\n", nombre_archivo, numero_registro, tipo ) ;
      exit( 1 ) ;
   }
   else
   {
      if ( strcmp(tipo,"entero") == 0 )
         strcpy( nombre_tipo, "int " ) ;
      else if ( strcmp(tipo,"decimal") == 0 )
         strcpy(nombre_tipo, "float " ) ;
      else if ( strcmp(tipo,"caracter") == 0 )
         strcpy( nombre_tipo, "char " ) ;
      else if ( strcmp(tipo,"caracteres") == 0 )
         strcpy( nombre_tipo, "char *" ) ;
      else if ( strcmp(tipo,"fecha") == 0 )
         strcpy( nombre_tipo, "long " ) ;
      else if ( strcmp(tipo,"hora") == 0 )
         strcpy( nombre_tipo, "long " ) ;
      else if ( strcmp(tipo,"booleano") == 0 )
         strcpy( nombre_tipo, "int " ) ;
      else if ( strcmp(tipo,"nulo") == 0 )
         strcpy( nombre_tipo, "void " ) ;
   }
}

static void ValidaJustificacion( char *justificacion, long numero_registro )
{
   Mayuscula( justificacion ) ;

   if ( (strcmp(justificacion,"JUST_IZQ") != 0) &&
        (strcmp(justificacion,"JUST_CEN") != 0) &&
        (strcmp(justificacion,"JUST_DER") != 0) &&
        (strcmp(justificacion,"NULO") != 0) )
   {
      fprintf( stderr, "Error: 'formato.esp', linea %ld. ValidaJustificacion(...) Especificacion '%s' no valida.\n", numero_registro, justificacion ) ;
      exit( 1 ) ;
   }
}

static void ValidaRelleno( char *relleno, long numero_registro )
{
   Mayuscula( relleno ) ;

   if ( (strcmp(relleno,"RELL_BLANCO") != 0) &&
        (strcmp(relleno,"RELL_CERO") != 0)   &&
        (strcmp(relleno,"RELL_EFE") != 0)    &&
        (strcmp(relleno,"NULO") != 0) )
   {
      fprintf( stderr, "Error: 'formato.esp', linea %ld. ValidaRelleno(...) Especificacion '%s' no valida.\n", numero_registro, relleno ) ;
      exit( 1 ) ;
   }
}

static void ValidaPropiedad( char *propiedad, char *tipo, long numero_registro, char *nombre_propiedad, char *separador )
{
   int error=FALSE ;

   Mayuscula( propiedad ) ;

   if ( strcmp(tipo,"decimal") == 0 )
   {
      if ( (strcmp(propiedad,"PROP_0") == 0) )
      {
         strcpy( nombre_propiedad, propiedad ) ;
         *separador = '~' ;
         error = FALSE ;
      }
      else if ( (strcmp(propiedad,"PROP_0V0") == 0) ||
                (strcmp(propiedad,"PROP_0.0") == 0) ||
                (strcmp(propiedad,"PROP_0,0") == 0)
              )
      {
         strcpy( nombre_propiedad, "PROP_00" ) ;
         *separador = propiedad[7-1] ;
         error = FALSE ;
      }
      else if ( (strcmp(propiedad,"PROP_0V00") == 0) ||
                (strcmp(propiedad,"PROP_0.00") == 0) ||
                (strcmp(propiedad,"PROP_0,00") == 0)
              )
      {
         strcpy( nombre_propiedad, "PROP_000" ) ;
         *separador = propiedad[7-1] ;
         error = FALSE ;
      }
      else if ( (strcmp(propiedad,"PROP_0V000") == 0) ||
                (strcmp(propiedad,"PROP_0.000") == 0) ||
                (strcmp(propiedad,"PROP_0,000") == 0)
              )
      {
         strcpy( nombre_propiedad, "PROP_000" ) ;
         *separador = propiedad[7-1] ;
         error = FALSE ;
      }
      else if ( (strcmp(propiedad,"PROP_0V0000") == 0) ||
                (strcmp(propiedad,"PROP_0.0000") == 0) ||
                (strcmp(propiedad,"PROP_0,0000") == 0)
              )
      {
         strcpy( nombre_propiedad, "PROP_0000" ) ;
         *separador = propiedad[7-1] ;
         error = FALSE ;
      }
      else
         error = TRUE ;
   }
   else if ( strcmp(tipo,"fecha") == 0 )
   {
      if ( (strcmp(propiedad,"PROP_AAAAMMDD") == 0) ||
	   (strcmp(propiedad,"PROP_DDMMAAAA") == 0)   ||
           (strcmp(propiedad,"PROP_AAMMDD") == 0)   ||
           (strcmp(propiedad,"PROP_DDMMAA") == 0) ||
	   (strcmp(propiedad,"PROP_MMDD") == 0)
         )
      {
         strcpy( nombre_propiedad, propiedad ) ;
         *separador = '~' ;
         error = FALSE ;
      }
      else if ( (strcmp(propiedad,"PROP_DD/MM/AAAA") == 0) ||
                (strcmp(propiedad,"PROP_DD-MM-AAAA") == 0)
              )
      {
         strcpy( nombre_propiedad, "PROP_DDMMAAAA" ) ;
         *separador = propiedad[8-1] ;
         error = FALSE ;
      }
      else if ( (strcmp(propiedad,"PROP_DD/MM/AA") == 0) ||
                (strcmp(propiedad,"PROP_DD-MM-AA") == 0)
              )
      {
         strcpy( nombre_propiedad, "PROP_DDMMAA" ) ;
         *separador = propiedad[8-1] ;
         error = FALSE ;
      }
      else if ( (strcmp(propiedad,"PROP_MM/DD/AA") == 0) ||
                (strcmp(propiedad,"PROP_MM-DD-AA") == 0)
              )
      {
         strcpy( nombre_propiedad, "PROP_MMDDAA" ) ;
         *separador = propiedad[8-1] ;
         error = FALSE ;
      }
      else
         error = TRUE ;
   }
   else if ( strcmp(tipo,"hora") == 0 )
   {
      if ( (strcmp(propiedad,"PROP_HHMMSS") == 0) ||
           (strcmp(propiedad,"PROP_HHMM") == 0)   ||
           (strcmp(propiedad,"PROP_MMMMSS") == 0) ||
	   (strcmp(propiedad,"PROP_MMMSS") == 0)  ||
           (strcmp(propiedad,"PROP_MM") == 0)     ||
           (strcmp(propiedad,"PROP_SS") == 0)
         )
      {
         strcpy( nombre_propiedad, propiedad ) ;
         *separador = '~' ;
         error = FALSE ;
      }
      else if ( (strcmp(propiedad,"PROP_HH:MM:SS") == 0) )
      {
         strcpy( nombre_propiedad, "PROP_HHMMSS" ) ;
         *separador = propiedad[8-1] ;
         error = FALSE ;
      }
      else if ( (strcmp(propiedad,"PROP_HH:MM") == 0) )
      {
         strcpy( nombre_propiedad, "PROP_HHMM" ) ;
         *separador = propiedad[8-1] ;
         error = FALSE ;
      }
      else
         error = TRUE ;
   }
   else
   {
      if ( strcmp(propiedad,"NULO") == 0 )
      {
         strcpy( nombre_propiedad, propiedad ) ;
         *separador = '~' ;
         error = FALSE ;
      }
      else
         error = TRUE ;
   }

   if ( error )
   {
      fprintf( stderr, "Error: 'formato.esp', linea %ld. ValidaPropiedad(...) Especificacion '%s' no valida para el tipo '%s'.\n", numero_registro, propiedad, tipo  ) ;
      exit( 1 ) ;
   }
}

void ImprimeFormatos( formato *lista_formatos )
{
   formato *p ;
   campo   *q ;

   p = lista_formatos ;
   while ( p != NULL )
   {

      fprintf( stdout, "Formato->nombre= '%s'\n", p->nombre ) ;
      fprintf( stdout, "Formato->largo_registro= %d\n", p->largo_registro ) ;
      q = p->campos ;

      while ( q != NULL )
      {
         fprintf( stdout, "\tCampo->nombre= '%s'\n", q->nombre ) ;
         fprintf( stdout, "\t ... ->inicio= %d\n", q->inicio ) ;
         fprintf( stdout, "\t ... ->largo= %d\n", q->largo ) ;
         fprintf( stdout, "\t ... ->tipo= '%s'\n", q->tipo ) ;
         fprintf( stdout, "\t ... ->justificacion= '%s'\n", q->justificacion ) ;
         fprintf( stdout, "\t ... ->relleno= '%s'\n", q->relleno ) ;
         fprintf( stdout, "\t ... ->propiedad= '%s'\n", q->propiedad ) ;

         q = (campo *)q->siguiente ;
      }
      p = (formato *)p->siguiente ;
   }
}

int EsFormatoValido( char *nombre_formato, formato *lista_formatos )
{
   formato *p ;

   if ( EsIgual(nombre_formato,"FMTO_ENTRADA") )
      return( TRUE ) ;

   p = lista_formatos ;
   while ( p != NULL )
   {
      if ( strcmp(nombre_formato,p->nombre) != 0 )
         p = (formato *)p->siguiente ;
      else
         return( TRUE ) ;
   }

   return( FALSE ) ;
}

formato *LeeEspecFormato( FILE *arch_esp_formato )
{
   int  en_esp_formato=FALSE ;
   int  en_esp_campo=FALSE ;
   long numero_registro=0L ;
   formato *p, *q, *lista_formatos ;
   campo *i, *j ;
   char registro[BUFFER_LARGO] ;
   char nombre_tipo[NOMBRE_TIPO_LARGO+1] ;
   char nombre_propiedad[PROPIEDAD_LARGO+1] ;
   char separador_propiedad ;

   lista_formatos = NULL ;

   while ( LeeRegistro(arch_esp_formato,registro) )
   {
      numero_registro++ ;

      switch( registro[0] )
      {
         case 'f': /* lectura datos formato */
         case 'F':

            if ( en_esp_formato ||
                 (registro[1] != ' ' && registro[1] != '\t') )
            {
               fprintf( stderr, "Error: 'formato.esp', linea %ld. LeeEspecFormato(...) Sintaxis incorrecta.\n", numero_registro  ) ;
               exit( 1 ) ;
            }
            else
            {
               en_esp_formato = TRUE ;
               en_esp_campo = FALSE ;
            }

            q = (formato *)malloc( sizeof(formato) ) ;

            sscanf( registro+2, "%s %d \n", q->nombre,
                   &(q->largo_registro) ) ;

            Mayuscula( q->nombre ) ;

       q->campos = NULL ;
       q->siguiente = NULL ;

       if ( lista_formatos == NULL ) /* lista nueva de formatos */
          lista_formatos = q ;
       else
       {
          p = lista_formatos ;
          while ( (p->siguiente != NULL) && (strcmp(q->nombre,p->nombre) != 0) )
        p = (formato *)p->siguiente ;

          if ( strcmp(q->nombre,p->nombre) == 0 )
          {
        fprintf( stderr, "Error: 'formato.esp', linea %ld. LeeEspecFormato(...) Doble especificacion de formato.\n", numero_registro  ) ;
        exit( 1 ) ;
          }

          p->siguiente = (struct formato *)q ;
       }
    break;
    case 'c':
    case 'C':
       if ( (!en_esp_formato && !en_esp_campo) ||
       (registro[1] != ' ' && registro[1] != '\t') )
       {
          fprintf( stderr, "Error: 'formato.esp', linea %ld. LeeEspecFormato(...) Sintaxis incorrecta.\n", numero_registro  ) ;
          exit( 1 ) ;
       }
       else
       {
          en_esp_formato = FALSE ;
          en_esp_campo = TRUE ;
       }

       i = (campo *)malloc( sizeof(campo) ) ;

       sscanf( registro+2, "%s %d %d %s %s %s %s \n", i->nombre,
                        &(i->inicio),
                        &(i->largo),
                        i->tipo,
                        i->justificacion,
                        i->relleno,
                        i->propiedad ) ;

       ValidaTipo( "formato", i->tipo , numero_registro, nombre_tipo ) ;
       ValidaJustificacion( i->justificacion, numero_registro ) ;
       ValidaRelleno( i->relleno, numero_registro ) ;
       ValidaPropiedad( i->propiedad, i->tipo, numero_registro, nombre_propiedad, &separador_propiedad ) ;

       strcpy( i->tipo, nombre_tipo ) ;
       strcpy( i->propiedad, nombre_propiedad ) ;
       i->separador = separador_propiedad ;

       i->siguiente = NULL ;

       if ( q->campos == NULL ) /* lista nueva de campos del formato */
          q->campos = i ;
       else
       {
          j = q->campos ;
          while ( (j->siguiente != NULL) && (strcmp(i->nombre,j->nombre) != 0) )
        j = (campo *)j->siguiente ;

          if ( strcmp(i->nombre,j->nombre) == 0 )
          {
        fprintf( stderr, "Error: 'formato.esp', linea %ld. LeeEspecFormato(...) Doble especificacion de formato.\n", numero_registro  ) ;
        exit( 1 ) ;
          }

          j->siguiente = (struct campo *)i ;
       }

    break;
    case '\n':
    case '#':
       /* comentarios */
    break;
    default:
       fprintf( stderr, "Error: 'formato.esp', linea %ld. LeeEspecFormato(...) Sintaxis incorrecta.\n", numero_registro  ) ;
       exit( 1 ) ;
      } /* switch */
   } /* while */

   if ( en_esp_formato )
   {
      fprintf( stderr, "Error: 'formato.esp', linea %ld. LeeEspecFormato(...) Sintaxis incorrecta.\n", numero_registro  ) ;
      exit( 1 ) ;
   }

   return( lista_formatos ) ;

}

void GeneraArchivoformato_h( formato *lista_formatos, char *directorio_salida )
{
   FILE *formato_h ;
   char nombre_archivo[NOMBRE_ARCHIVO_LARGO+1] ;
   formato *p ;
   campo   *q ;

   fprintf( stdout, "Generando archivo 'formato.h'...\n\n" ) ;

#ifdef MSDOS
   sprintf( nombre_archivo, "%s\\formato.h", directorio_salida ) ;
#else
   sprintf( nombre_archivo, "%s/formato.h", directorio_salida ) ;
#endif
   CreaArchivo( &formato_h, nombre_archivo ) ;
   fprintf( formato_h, "/*\n *   formato.h\n *\n */\n" ) ;

   /* #define */
   p = lista_formatos ;
   while ( p != NULL )
   {

      fprintf( formato_h, "\n#define %s_LARGO %d\n", p->nombre, p->largo_registro ) ;
      q = p->campos ;

      while ( q != NULL )
      {
    fprintf( formato_h, "#define %s%s_INICIO (%d - 1)\n", p->nombre, q->nombre, q->inicio ) ;
    fprintf( formato_h, "#define %s%s_LARGO %d\n", p->nombre, q->nombre, q->largo ) ;

    q = (campo *)q->siguiente ;
      }
      p = (formato *)p->siguiente ;
   }

   /* typedef struct */
   p = lista_formatos ;
   while ( p != NULL )
   {

      fprintf( formato_h, "\ntypedef struct %s {\n\n", p->nombre ) ;

      q = p->campos ;

      while ( q != NULL )
      {
    if ( strcmp(q->tipo,"char *") == 0 )
       fprintf( formato_h, "\tchar %s[%s%s_LARGO+1] ;\n", q->nombre, p->nombre, q->nombre, q->largo ) ;
    else
       fprintf( formato_h, "\t%s%s ;\n", q->tipo, q->nombre ) ;

    q = (campo *)q->siguiente ;
      }

      fprintf( formato_h, "\n} %s ;\n", p->nombre ) ;

      p = (formato *)p->siguiente ;
   }

   /* funciones de formato */
   p = lista_formatos ;

   while ( p != NULL )
   {

      fprintf( formato_h, "\nint LeeForm%s( FILE *, %s * ) ;\n", p->nombre, p->nombre ) ;
      fprintf( formato_h, "int EscForm%s( FILE *, %s * ) ;\n", p->nombre, p->nombre ) ;
      fprintf( formato_h, "void Inicializa%s( %s * ) ;\n", p->nombre, p->nombre ) ;

      p = (formato *)p->siguiente ;
   }

   /* funciones */
   fprintf( formato_h, "\nint SeGeneroRegistro( void ) ;\n" ) ;
   fprintf( formato_h, "void GeneracionRegistro( int ) ;\n" ) ;
   fprintf( formato_h, "void EscFormENTRADA( FILE * ) ;\n" ) ;

   fclose( formato_h ) ;

}

void GeneraArchivoformato_c( formato *lista_formatos, char *directorio_salida )
{
   FILE *formato_c ;
   char nombre_archivo[NOMBRE_ARCHIVO_LARGO+1] ;
   formato *p ;
   campo   *q ;

   fprintf( stdout, "Generando archivo 'formato.c'...\n\n" ) ;

#ifdef MSDOS
   sprintf( nombre_archivo, "%s\\formato.c", directorio_salida ) ;
#else
   sprintf( nombre_archivo, "%s/formato.c", directorio_salida ) ;
#endif
   CreaArchivo( &formato_c, nombre_archivo ) ;

   fprintf( formato_c, "/*\n *   formato.c\n *\n */\n\n" ) ;

   fprintf( formato_c, "#include \"definici.h\"\n" ) ;
   fprintf( formato_c, "#include \"bibliote.h\"\n" ) ;
   fprintf( formato_c, "#include \"formato.h\"\n\n" ) ;

   fprintf( formato_c, "static int se_genero_registro ;\n" ) ;
   fprintf( formato_c, "static char buffer_entrada[BUFFER_LARGO] ;\n" ) ;
   fprintf( formato_c, "static char buffer_salida[BUFFER_LARGO] ;\n\n" ) ;

   fprintf( formato_c, "void EscFormENTRADA( FILE *archivo )\n{\n   fprintf( archivo, \"%%s\", buffer_entrada ) ;\n}\n\n" ) ;
   fprintf( formato_c, "int SeGeneroRegistroEntrada( void )\n{\n   return( se_genero_registro ) ;\n}\n\n" ) ;
   fprintf( formato_c, "void GeneracionRegistro( int respuesta )\n{\n   se_genero_registro = respuesta ;\n}\n\n" ) ;

   /* rutinas formatos */

   /* LeeForm */

   p = lista_formatos ;

   while ( p != NULL )
   {
      fprintf( formato_c, "int LeeForm%s( FILE *archivo, %s *registro )\n{\n", p->nombre, p->nombre ) ;
      fprintf( formato_c, "\tchar buffer_temp[BUFFER_LARGO+1] ;\n\n" ) ;

      fprintf( formato_c, "\tGeneracionRegistro( FALSE ) ;\n\n" ) ;

      fprintf( formato_c, "\tif ( fgets( buffer_entrada, BUFFER_LARGO, archivo ) != NULL )\n\t{\n" ) ;

      q = p->campos ;

      while ( q != NULL )
      {
         if ( strcmp(q->tipo,"int ") == 0 )
         {
            fprintf( formato_c, "\t\tbuffer_temp[%s%s_LARGO] = '\\0' ;\n", p->nombre, q->nombre ) ;
            fprintf( formato_c, "\t\tmemcpy( buffer_temp, buffer_entrada + %s%s_INICIO, %s%s_LARGO ) ;\n", p->nombre, q->nombre, p->nombre, q->nombre ) ;
            fprintf( formato_c, "\t\tregistro->%s = atoi( buffer_temp ) ;\n\n", q->nombre ) ;
         }
         else if ( strcmp(q->tipo,"char ") == 0 )
         {
            fprintf( formato_c, "\t\tregistro->%s = buffer_entrada[%s%s_INICIO] ;\n\n", q->nombre, p->nombre, q->nombre ) ;
         }
         else if ( strcmp(q->tipo,"char *") == 0 )
         {
            fprintf( formato_c, "\t\tregistro->%s[%s%s_LARGO] = '\\0' ;\n", q->nombre, p->nombre, q->nombre ) ;
            fprintf( formato_c, "\t\tmemcpy( registro->%s, buffer_entrada + %s%s_INICIO, %s%s_LARGO ) ;\n\n", q->nombre, p->nombre, q->nombre, p->nombre, q->nombre ) ;
         }
         else if ( strcmp(q->tipo,"long ") == 0 )
         {
            fprintf( formato_c, "\t\tbuffer_temp[%s%s_LARGO] = '\\0' ;\n", p->nombre, q->nombre ) ;
            fprintf( formato_c, "\t\tmemcpy( buffer_temp, buffer_entrada + %s%s_INICIO, %s%s_LARGO ) ;\n", p->nombre, q->nombre, p->nombre, q->nombre ) ;
            fprintf( formato_c, "\t\tAjustarPropiedad( buffer_temp, %s, '%c' ) ;\n", q->propiedad, q->separador ) ;
            fprintf( formato_c, "\t\tif ( atol(buffer_temp) == -1 )\n\t\t{\n\t\t\treturn( TRUE ) ;\n\t\t}\n" ) ;
            fprintf( formato_c, "\t\tregistro->%s = atol( buffer_temp ) ;\n\n", q->nombre ) ;
         }
         else if ( strcmp(q->tipo,"float ") == 0 )
         {
            fprintf( formato_c, "\t\tbuffer_temp[%s%s_LARGO] = '\\0' ;\n", p->nombre, q->nombre ) ;
            fprintf( formato_c, "\t\tmemcpy( buffer_temp, buffer_entrada + %s%s_INICIO, %s%s_LARGO ) ;\n", p->nombre, q->nombre, p->nombre, q->nombre ) ;
            fprintf( formato_c, "\t\tAjustarPropiedad( buffer_temp, %s, '%c' ) ;\n", q->propiedad, q->separador ) ;
            fprintf( formato_c, "\t\tif ( atoi(buffer_temp) == -1 )\n\t\t{\n" ) ;
            fprintf( formato_c, "\t\t\treturn( TRUE ) ;\n\t\t}\n" ) ;
            fprintf( formato_c, "\t\tsscanf( buffer_temp, \"%%f\", &(registro->%s) ) ;\n\n", q->nombre ) ;
         }
         else
         {
            fprintf( stderr, "Error: GeneraArchivoformato_c(...) Tipo no implementado.\n" ) ;
            exit( 1 ) ;         
         }
                  
         q = (campo *)q->siguiente ;
      }

      fprintf( formato_c, "\t\tGeneracionRegistro( TRUE ) ;\n\t\treturn( TRUE ) ;\n\t}\n\telse\n\t\treturn( FALSE ) ;\n}\n\n" ) ;

      p = (formato *)p->siguiente ;
   }

   /* EscForm */

   p = lista_formatos ;

   while ( p != NULL )
   {
      fprintf( formato_c, "int EscForm%s( FILE *archivo, %s *registro )\n{\n", p->nombre, p->nombre ) ;
      fprintf( formato_c, "\tchar buffer_temp[BUFFER_LARGO+1] ;\n\n" ) ;

      fprintf( formato_c, "\tmemset( buffer_salida, ' ', %s_LARGO ) ;\n", p->nombre ) ;
      fprintf( formato_c, "\tbuffer_salida[%s_LARGO] = '\\0' ;\n\n", p->nombre ) ;

      q = p->campos ;

      while ( q != NULL )
      {
         if ( strcmp(q->tipo,"int ") == 0 )
         {
            fprintf( formato_c, "\tsprintf( buffer_temp, \"%%-*d\", %s%s_LARGO, registro->%s ) ;\n", p->nombre, q->nombre, q->nombre ) ;
         }
         else if ( strcmp(q->tipo,"char ") == 0 )
         {
            fprintf( formato_c, "\tsprintf( buffer_temp, \"%%c\", registro->%s ) ;\n", q->nombre ) ;
         }
         else if ( strcmp(q->tipo,"char *") == 0 )
         {
            fprintf( formato_c, "\tsprintf( buffer_temp, \"%%-*s\", %s%s_LARGO, registro->%s ) ;\n", p->nombre, q->nombre, q->nombre ) ;
         }
         else if ( strcmp(q->tipo,"long ") == 0 )
         {
            fprintf( formato_c, "\tsprintf( buffer_temp, \"%%-*ld\", %s%s_LARGO, registro->%s ) ;\n", p->nombre, q->nombre, q->nombre ) ;
         }
         else if ( strcmp(q->tipo,"float ") == 0 )
         {
            fprintf( formato_c, "\tsprintf( buffer_temp, \"%%-*f\", %s%s_LARGO, registro->%s ) ;\n", p->nombre, q->nombre, q->nombre ) ;         
         }
         else
         {
            fprintf( stderr, "Error: GeneraArchivoformato_c(...) Tipo no implementado.\n" ) ;
            exit( 1 ) ;         
         }
                  
         fprintf( formato_c, "\tAjustarTexto( buffer_temp, %s, %s, %s, '%c' ) ;\n", q->justificacion, q->relleno, q->propiedad, q->separador ) ;
         fprintf( formato_c, "\tmemcpy( buffer_salida + %s%s_INICIO, buffer_temp, %s%s_LARGO ) ;\n\n", p->nombre, q->nombre, p->nombre, q->nombre ) ;
         
         q = (campo *)q->siguiente ;
      }
         
      fprintf( formato_c, "\tfprintf( archivo, \"%%s\\n\", buffer_salida ) ;\n\n\treturn( 0 ) ;\n}\n\n" ) ;
      
      p = (formato *)p->siguiente ;
   }

   /* Iniciliza */

   p = lista_formatos ;

   while ( p != NULL )
   {
      fprintf( formato_c, "void Inicializa%s( %s *registro )\n{\n", p->nombre, p->nombre ) ;

      q = p->campos ;

      while ( q != NULL )
      {
         if ( strcmp(q->tipo,"int ") == 0 )
         {
            fprintf( formato_c, "\tregistro->%s = 0 ;\n", q->nombre ) ;
         }
         else if ( strcmp(q->tipo,"char ") == 0 )
         {
            fprintf( formato_c, "\tregistro->%s = ' ' ;\n", q->nombre ) ;
         }
         else if ( strcmp(q->tipo,"char *") == 0 )
         {
            fprintf( formato_c, "\tregistro->%s[0] = '\\0' ;\n", q->nombre ) ;
         }
         else if ( strcmp(q->tipo,"long ") == 0 )
         {
            fprintf( formato_c, "\tregistro->%s = 0L ;\n", q->nombre ) ;         
         }         
         else if ( strcmp(q->tipo,"float ") == 0 )
         {
            fprintf( formato_c, "\tregistro->%s = 0.0 ;\n", q->nombre ) ;         
         }
         else
         {
            fprintf( stderr, "Error: GeneraArchivoformato_c(...) Tipo no implementado.\n" ) ;
            exit( 1 ) ;         
         }
    
         q = (campo *)q->siguiente ;
      }

      fprintf( formato_c, "}\n\n" ) ;

      p = (formato *)p->siguiente ;
   }

   fclose( formato_c ) ;
   
}
