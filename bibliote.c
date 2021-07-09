/*
 * bibliote.c
 * version 2.0 Octubre 1998. Copyright Pablo Lopez A.
 *
 */

#include "definici.h"
#include "bibliote.h"

#define MENSAJE_LARGO  80
#define SALIDAS_MAXIMO 25

static int indice_salida ;
static int salidas_generadas[SALIDAS_MAXIMO] ;
static int condicion_terminal ;
static int condicion_sobrante ;
static int error ;
static int sobrante ;
static int salida_con_sob ;
static int pre_condicion_con_sob ;
static int id_salida ;
static long registros_error=1L ;
static long registros_sobrante=1L ;
static char mensaje_error[MENSAJE_LARGO+1] ;
static char mensaje_sobrante[MENSAJE_LARGO+1] ;
static char salida_procesada[NOMBRE_ENTRADA_SALIDA_LARGO+1] ;
static char campo_procesado[NOMBRE_CAMPO_LARGO+1] ;
static char *directorio_parametros="parametros" ;
static int multiregistros, multiregistro_actual ;

/*******/

#define L_PATRON         50
#define L_REEMP          50
#define MAX_REEMP        25
#define MAX_TABLATRAD  1000

static struct sNodoArbol {

        char reemp[L_REEMP + 1] ;
        struct sNodoArbol *pNext[10] ;

} sNodoArbol ;

static struct sNodoLista {

        char patron[L_PATRON + 1] ;
        char reemp[L_REEMP + 1] ;

} sNodoLista ;

static struct dtablaTrad {

        int  formato ; /* 1: numerico, 2: alfanumerico */
        int  num_elementos_informados ;
        int  num_elementos_leidos ;
        char defecto[L_REEMP+1] ;
        union u {
                struct sNodoArbol *pRaiz ;
                struct sNodoLista *pTop ;
        } u ;

} dtablaTrad ;

static struct dtablaTrad *tablaTrad[MAX_TABLATRAD+1] ;

static struct reemplazo {

        char reemp[L_REEMP+1] ;

} reemplazo ;

static char patron_[L_PATRON+1] ;
static char reemp[L_REEMP+1] ;
static char resto[L_PATRON+1] ;
static char defecto[L_REEMP+1] ;

static struct reemplazo reemplazos[MAX_REEMP] ;
static int reemp_sgte, num_reemp ;

/******************
*
* newNodoArbol
*
*******************/
static struct sNodoArbol *newNodoArbol(void)
/* retorna una estructura de tipo sNodoArbol */
{
  int i ;
  char *mensaje ;
  struct sNodoArbol *pNodoArbol ;

  pNodoArbol = (struct sNodoArbol *)malloc( sizeof(struct sNodoArbol) ) ;
  if ( pNodoArbol == NULL )
  {
          mensaje = "MakeNodoArbol: No existe memoria para crear un nodo" ;
          fprintf(stderr,"**** ERROR: %s\n", mensaje);
          fflush(stdout);
          exit( 1 ) ;
  }

  for ( i=0; i < L_REEMP+1; i++ )
         pNodoArbol->reemp[i] = '\0';

  for(i=0; i < 10; i++)
         pNodoArbol->pNext[i] = NULL;

  return( pNodoArbol ) ;
}

/******************************
*
* insertArbol
*
*******************************/
static int insertArbol( struct sNodoArbol *pNodoArbol, char *patron, char *reemp )
/* agrega un patron de reconocimiento y si reemplazo al arbol   */
/* de patrones                                                                                                                                  */
{
  char *mensaje ;
  int pos;

  if ( pNodoArbol == NULL )
  {
          mensaje = "insertArbol: No se puede insertar en un arbol nulo" ;
          fprintf(stderr,"**** ERROR: %s\n", mensaje);
          fflush(stdout);
          exit( 1 ) ;
  }

  if ( patron[0] == ' ' || patron[0] == '\0' )
  {
          /* se acabo la secuencia, inserto el reemplazo */
          if ( pNodoArbol->reemp[0] != '\0' )
          {
             if ( strcmp(pNodoArbol->reemp,reemp) == 0 ) /* existe reemplazo y es igual */
             {
                  mensaje = "insertArbol: Secuencia duplicada" ;
                  fprintf( stderr,"**** ADVERTENCIA: %s. actual reemp(%s)\n", mensaje, pNodoArbol->reemp ) ;
                  return( TRUE ) ;
             }
             else  /* existe reemplazo y es distinto */
             {
                  mensaje = "insertArbol: Secuencia duplicada" ;
                  fprintf( stderr,"**** ERROR: %s. actual reemp(%s)\n", mensaje, pNodoArbol->reemp ) ;
                  return( FALSE ) ;
             } 
          }

          strcpy( pNodoArbol->reemp, reemp ) ;
          return( TRUE );
  }

  pos = patron[0] - '0' ;
  if (pos < 0 || pos > 9)
  {
          sprintf(mensaje,"insertArbol: caracter (%c) no numerico en patron", patron[0]);
          fprintf(stderr,"**** ERROR: %s\n", mensaje);
          fflush(stdout);
          exit( 1 ) ;
  }

  if ( pNodoArbol->pNext[pos] == NULL )
          pNodoArbol->pNext[pos] = newNodoArbol();

  return( insertArbol(pNodoArbol->pNext[pos],&patron[1],reemp) ) ;
}

/******************************
*
* insertLista
*
*******************************/
static int insertLista( struct dtablaTrad *pNodo, char *patron, char *reemp )
{
   struct sNodoLista *p, *q ;

   if ( pNodo->num_elementos_leidos < pNodo->num_elementos_informados )
   {
      p = pNodo->u.pTop ;
      q = p + pNodo->num_elementos_leidos ;

      strcpy( q->patron, patron ) ;
      strcpy( q->reemp, reemp ) ;

      return( TRUE ) ;
   }
   else
      return( FALSE ) ;
}

/******************************
*
* buscaArbol
*
*******************************/
static int buscaArbol(struct sNodoArbol *pNodoArbol, char *patron, char *reemp, char *resto)
/* busca en el arbol indicado por 'pNodoArbol' el prefijo ma's largo de   */
/* 'patron'. Si lo encuentra deja en 'reemp' el reemplazo y en            */
/* 'resto' la parte no utilizada de 'patron'.                             */
{
   int pos;

   if ( patron[0] == '\0' )
   {
      if ( pNodoArbol->reemp[0] != '\0' )
      {
         strcpy( resto, patron ) ;
         strcpy( reemp, pNodoArbol->reemp ) ;

         return( TRUE ) ;
      }

      return( FALSE ) ;

   }

   pos = patron[0] - '0';
   if ( (pos < 0) || (pos > 9) )
      return(FALSE);

   if ( pNodoArbol->pNext[pos] != NULL )
      if ( buscaArbol(pNodoArbol->pNext[pos], &patron[1], reemp, resto) )
         return(TRUE);

    if ( pNodoArbol->reemp[0] != '\0' )
    {
       strcpy( resto, patron ) ;
       strcpy( reemp, pNodoArbol->reemp ) ;

       return( TRUE ) ;
    }

    return( FALSE ) ;

}

/******************************
*
*  ComparaNodoLista
*
*******************************/
/*static int ComparaNodoLista( const void *i, const void *j )*/
static int ComparaNodoLista( struct sNodoLista  *i, struct sNodoLista *j )
{

   return( strcmp( ((struct sNodoLista *)i)->patron, ((struct sNodoLista *)j)->patron ) ) ;
}

/******************************
*
*  ComparaNodoLista2
*
*******************************/
/*static int ComparaNodoLista2( const void *i, const void *j )*/
static int ComparaNodoLista2( struct sNodoLista  *i, struct sNodoLista *j )
{

   return( strcmp(ObtenerItem(1,((struct sNodoLista *)i)->patron),ObtenerItem(1,((struct sNodoLista *)j)->patron)) ) ;
}

/******************
*
* leeTrdString
*
*******************/
void leeTrdString( void )
{
        char patron[L_PATRON] ;
        char reemp[L_REEMP];
        char nombre_archivo[NOMBRE_ARCHIVO_LARGO+1] ;
        char buffer[BUFFER_LARGO+1] ;
        char *ptr ;
        int  tabla ;
        FILE *archivo ;

#ifdef MSDOS
        sprintf( nombre_archivo, "%s\\tablas\\traducio.des", ObtDirecParametros() ) ;
#else
        sprintf( nombre_archivo, "%s/tablas/traducio.des", ObtDirecParametros() ) ;
#endif

        LeeArchivo( &archivo, nombre_archivo ) ;

        while ( fgets(buffer,BUFFER_LARGO,archivo) != NULL )
        {
                /* codigo tabla */
                ptr = strtok( buffer, "," ) ;
                tabla = atoi( ptr ) ;

                if ( tablaTrad[tabla] != NULL )
                {
                   fprintf( stderr, "Error: leeTrdString(...) archivo 'traducio.des'. Tabla doblemente inicializada.\n" ) ;
                   exit( -1 ) ;
                }

                tablaTrad[tabla] = (struct dtablaTrad *)malloc( sizeof(dtablaTrad) ) ;
                if ( tablaTrad[tabla] == NULL )
                {
                   fprintf( stderr, "Error: leeTrdString(...) Insuficiente memoria para cargar informacion.\n" ) ;
                   exit( -1 ) ;
                }

                /* codigo formato 1: numerico, 2: alfanumerico, 3: alfanumpref, 4: rango, 5: multivalor */
                ptr = strtok( NULL, "," ) ;
                tablaTrad[tabla]->formato = atoi( ptr ) ;

                /* numero de elementos informados */
                ptr = strtok( NULL, "," ) ;
                tablaTrad[tabla]->num_elementos_informados = atoi( ptr ) ;

                if ( tablaTrad[tabla]->num_elementos_informados <= 0 )
                {
                   fprintf( stderr, "Error: leeTrdString(...) archivo 'traducio.des'. Numero de elementos invalido.\n" ) ;
                   exit( -1 ) ;
                }

                /* numero de elementos leidos */
                tablaTrad[tabla]->num_elementos_leidos = 0 ;

                /* valor por defecto */
                ptr = strtok( NULL, "," ) ;
                strcpy( tablaTrad[tabla]->defecto, ptr ) ;

                /* inicializacion estructuras */
                if ( tablaTrad[tabla]->formato == 1 ) /* numerico */
                {
                        tablaTrad[tabla]->u.pRaiz = newNodoArbol() ;
                }
                else /* alfanumerico - alfanumpref - rango - multivalor */
                {
                   tablaTrad[tabla]->u.pTop = (struct sNodoLista *)malloc( sizeof(sNodoLista) * tablaTrad[tabla]->num_elementos_informados ) ;
                   if ( tablaTrad[tabla]->u.pTop == NULL )
                   {
                      fprintf( stderr, "Error: leeTrdString(...) Insuficiente memoria para cargar informacion.\n" ) ;
                      exit( -1 ) ;
                   }
                }
        }

        fclose( archivo ) ;

#ifdef MSDOS
        sprintf( nombre_archivo, "%s\\tablas\\traducio.txt", ObtDirecParametros() ) ;
#else
        sprintf( nombre_archivo, "%s/tablas/traducio.txt", ObtDirecParametros() ) ;
#endif

        LeeArchivo( &archivo, nombre_archivo ) ;

        while ( fgets(buffer,BUFFER_LARGO,archivo) != NULL )
        {
                /* indice */
                ptr = strtok( buffer, "," ) ;
                tabla = atoi( ptr ) ;

                /* subindice */
                ptr = strtok( NULL, "," ) ;

                /* patron */
                ptr = strtok( NULL, "," ) ;
                strcpy( patron, ptr ) ;

                /* reempl */
                ptr = strtok( NULL, "," ) ;
                strcpy( reemp, ptr ) ;

                if ( tablaTrad[tabla] == NULL )
                {
                        fprintf( stderr,"Error: leeTrdString(...) tabla '%d' no inicializada.\n", tabla ) ;
                        exit( -1 ) ;
                }

                if ( tablaTrad[tabla]->formato == 1 ) /* numerico */
                {
                        if ( !insertArbol(tablaTrad[tabla]->u.pRaiz,patron,reemp) )
                        {
                                fprintf( stderr, "Error: leeTrdString(...) insertArbol tabla(%d) patron(%s) reemp(%s)\n", tabla, patron, reemp ) ;
                                exit( -1 ) ;
                        }
                }
                else    /* alfanumerico - alfanumpref - rango - multivalor */
                {
                        if ( !insertLista(tablaTrad[tabla],patron,reemp) )
                        {
                                fprintf( stderr, "Error: leeTrdString(...) insertLista tabla(%d) patron(%s) reemp(%s)\n", tabla, patron, reemp ) ;
                                exit( -1 ) ;
                        }
                }

                tablaTrad[tabla]->num_elementos_leidos++ ;
        }

        fclose( archivo ) ;

        for ( tabla=0; tabla<MAX_TABLATRAD; tabla++ )
        {
           if ( tablaTrad[tabla] != NULL )
           {
              if ( tablaTrad[tabla]->num_elementos_leidos != tablaTrad[tabla]->num_elementos_informados )
              {
                 fprintf( stderr, "Error: leeTrdString(...) elementos leidos (%d) != elementos informados (%d)\n", tablaTrad[tabla]->num_elementos_leidos, tablaTrad[tabla]->num_elementos_informados  ) ;
                 exit( -1 ) ;
              }

              /* alfanumericos - alfanumpref - rango - multivalor */
              if ( (tablaTrad[tabla]->formato == 2) ||
                   (tablaTrad[tabla]->formato == 3) ||
                   (tablaTrad[tabla]->formato == 4) ||
                   (tablaTrad[tabla]->formato == 5) )
              {
                 qsort( (struct sNodoLista *)tablaTrad[tabla]->u.pTop, tablaTrad[tabla]->num_elementos_informados, sizeof(struct sNodoLista), ComparaNodoLista ) ;
              }
           }
        }
}

/******************
*
* InicDirecParametros
*
*******************/
void InicDirecParametros( char *directorio )
{
   directorio_parametros = directorio ;
}


/******************
*
* ObtDirecParametros
*
*******************/
char *ObtDirecParametros( void )
{
   return( directorio_parametros ) ;
}

/******************
*
* InicializaTablas
*
*******************/
void InicializaTablas( void )
{
   int tabla ;

   for ( tabla=0; tabla<MAX_TABLATRAD; tabla++ )
      tablaTrad[ tabla ] = NULL ;
}

/******************
*
* CargaTabla
*
*******************/
int CargaTabla( int tabla )
{
        char patron[L_PATRON] ;
        char reemp[L_REEMP];
        char nombre_archivo[NOMBRE_ARCHIVO_LARGO+1] ;
        char buffer[BUFFER_LARGO+1] ;
        char *ptr ;
        int  tbl ;
        FILE *archivo ;

        fputc( 'T', stdout ) ; 
	fprintf( stdout, "( %d )", tabla ) ;

        fflush( stdout ) ; 

#ifdef MSDOS
        sprintf( nombre_archivo, "%s\\tablas\\traducio.des", ObtDirecParametros() ) ;
#else
        sprintf( nombre_archivo, "%s/tablas/traducio.des", ObtDirecParametros() ) ;
#endif

        LeeArchivo( &archivo, nombre_archivo ) ;

        tbl = 0 ;
        while ( (tbl != tabla) &&
                (fgets(buffer,BUFFER_LARGO,archivo) != NULL) )
        {
                /* codigo tabla */
                ptr = strtok( buffer, "," ) ;
                tbl = atoi( ptr ) ;

                if ( tbl < tabla )
                   continue ;
                else if ( tbl > tabla )
                   return( FALSE ) ;

                if ( tablaTrad[tabla] != NULL )
                {
                   fprintf( stderr, "Error: CargaTabla(...) archivo 'traducio.des'. Tabla doblemente inicializada.\n" ) ;
                   exit( -1 ) ;
                }

                tablaTrad[tabla] = (struct dtablaTrad *)malloc( sizeof(dtablaTrad) ) ;
                if ( tablaTrad[tabla] == NULL )
                {
                   fprintf( stderr, "Error: CargaTabla(...) Insuficiente memoria para cargar informacion.\n" ) ;
                   exit( -1 ) ;
                }

                /* codigo formato 1: numerico, 2: alfanumerico, 3: alfanumpref, 4: rango, 5: multivalor */
                ptr = strtok( NULL, "," ) ;
                tablaTrad[tabla]->formato = atoi( ptr ) ;

                /* numero de elementos informados */
                ptr = strtok( NULL, "," ) ;
                tablaTrad[tabla]->num_elementos_informados = atoi( ptr ) ;

                if ( tablaTrad[tabla]->num_elementos_informados <= 0 )
                {
                   fprintf( stderr, "Error: CargaTabla(...) archivo 'traducio.des'. Numero de elementos invalido.\n" ) ;
                   exit( -1 ) ;
                }

                /* numero de elementos leidos */
                tablaTrad[tabla]->num_elementos_leidos = 0 ;

                /* valor por defecto */
                ptr = strtok( NULL, "," ) ;
                strcpy( tablaTrad[tabla]->defecto, ptr ) ;

                /* inicializacion estructuras */
                if ( tablaTrad[tabla]->formato == 1 ) /* numerico */
                {
                        tablaTrad[tabla]->u.pRaiz = newNodoArbol() ;
                }
                else /* alfanumerico - alfanumpref - rango - multivalor */
                {
                   tablaTrad[tabla]->u.pTop = (struct sNodoLista *)malloc( sizeof(sNodoLista) * tablaTrad[tabla]->num_elementos_informados ) ;
                   if ( tablaTrad[tabla]->u.pTop == NULL )
                   {
                      fprintf( stderr, "Error: CargaTabla(...) Insuficiente memoria para cargar informacion.\n" ) ;
                      exit( -1 ) ;
                   }
                }
        }

        fclose( archivo ) ;

#ifdef MSDOS
        sprintf( nombre_archivo, "%s\\tablas\\traducio.txt", ObtDirecParametros() ) ;
#else
        sprintf( nombre_archivo, "%s/tablas/traducio.txt", ObtDirecParametros() ) ;
#endif

        LeeArchivo( &archivo, nombre_archivo ) ;

        tbl = 0 ;
        while ( fgets(buffer,BUFFER_LARGO,archivo) != NULL )
        {
                /* indice */
                ptr = strtok( buffer, "," ) ;
                tbl = atoi( ptr ) ;

                if ( tbl < tabla )
                   continue ;
                else if ( tbl > tabla )
                   break ;

                /* subindice */
                ptr = strtok( NULL, "," ) ;

                /* patron */
                ptr = strtok( NULL, "," ) ;
                strcpy( patron, ptr ) ;

                /* reempl */
                ptr = strtok( NULL, "," ) ;
                strcpy( reemp, ptr ) ;

                if ( tablaTrad[tabla] == NULL )
                {
                        fprintf( stderr,"Error: CargaTabla(...) tabla '%d' no inicializada.\n", tabla ) ;
                        exit( -1 ) ;
                }

                if ( tablaTrad[tabla]->formato == 1 ) /* numerico */
                {
                        if ( !insertArbol(tablaTrad[tabla]->u.pRaiz,patron,reemp) )
                        {
                                fprintf( stderr, "Error: CargaTabla(...) insertArbol tabla(%d) patron(%s) reemp(%s)\n", tabla, patron, reemp ) ;
                                exit( -1 ) ;
                        }
                }
                else    /* alfanumerico, alfanumpref, rango, multivalor */
                {
                        if ( !insertLista(tablaTrad[tabla],patron,reemp) )
                        {
                                fprintf( stderr, "Error: CargaTabla(...) insertLista tabla(%d) patron(%s) reemp(%s)\n", tabla, patron, reemp ) ;
                                exit( -1 ) ;
                        }
                }

                tablaTrad[tabla]->num_elementos_leidos++ ;
        }

        fclose( archivo ) ;

        if ( tablaTrad[tabla] != NULL )
        {
           if ( tablaTrad[tabla]->num_elementos_leidos != tablaTrad[tabla]->num_elementos_informados )
           {
              fprintf( stderr, "Error: CargaTabla(...) elementos leidos (%d) != elementos informados (%d)\n", tablaTrad[tabla]->num_elementos_leidos, tablaTrad[tabla]->num_elementos_informados  ) ;
              exit( -1 ) ;
           }

           /* alfanumericos - alfanumpref - rango - multivalor */
           if ( (tablaTrad[tabla]->formato == 2) ||
                (tablaTrad[tabla]->formato == 3) ||
                (tablaTrad[tabla]->formato == 4) ||                
                (tablaTrad[tabla]->formato == 5) )
           {
              qsort( (struct sNodoLista *)tablaTrad[tabla]->u.pTop, tablaTrad[tabla]->num_elementos_informados, sizeof(struct sNodoLista), ComparaNodoLista ) ;
           }
        }
        else
        {
           return( FALSE ) ;
        }

        return( TRUE ) ;
}

int BuscaEnTablaPatron( int tabla, char *patron )
{
  int i, largo_patron_=0 ;
  long r ;
  char ptrn[L_PATRON + 1] ;
  struct sNodoLista *p, *pp, q ;

  if ( tablaTrad[tabla] == NULL )
  {
          if ( !CargaTabla(tabla) )
          {
             fprintf( stderr, "Error: BuscaEnTablaPatron. Tabla '%d' invalida!!!\n", tabla ) ;

             exit( 1 ) ;
          }
  }

  if ( tablaTrad[tabla]->formato == 1 ) /* numerico */
  {
          if ( buscaArbol(tablaTrad[tabla]->u.pRaiz,patron,reemp,resto) )
          {
                  largo_patron_ = strstr(patron,resto) - patron ;
                  strncpy( patron_, patron, largo_patron_ ) ;
                  patron_[largo_patron_] = '\0' ;
                  strcpy( defecto, tablaTrad[tabla]->defecto ) ;
                  
                  num_reemp = 1 ;
                  strcpy( reemplazos[0].reemp, reemp ) ;

                  return( TRUE ) ;
          }
          else
          {
                  strcpy( patron_, patron ) ;
                  reemp[0] = '\0' ;
                  strcpy( defecto, tablaTrad[tabla]->defecto ) ;
                  
                  num_reemp = 0 ;
                  strcpy( reemplazos[0].reemp, "\0" ) ;
                                  
                  return( FALSE ) ;
          }
  }
  else if ( tablaTrad[tabla]->formato == 2 ) /* alfanumerico */
  {
          strcpy( q.patron, patron ) ;
          p = bsearch( &q, tablaTrad[tabla]->u.pTop, tablaTrad[tabla]->num_elementos_informados, sizeof(struct sNodoLista), ComparaNodoLista ) ;

          if ( p != NULL )
          {
                  strcpy( patron_, patron ) ;
                  strcpy( reemp, p->reemp ) ;
                  strcpy( defecto, tablaTrad[tabla]->defecto ) ;
                  resto[0] = '\0' ;

                  num_reemp = 1 ;
                  strcpy( reemplazos[0].reemp, reemp ) ;
                  
                  return( TRUE ) ;
          }
          else
          {
                  strcpy( patron_, patron ) ;
                  reemp[0] = '\0' ;
                  strcpy( defecto, tablaTrad[tabla]->defecto ) ;
                  resto[0] = '\0' ;                  

                  num_reemp = 0 ;
                  strcpy( reemplazos[0].reemp, "\0" ) ;
                  
                  return( FALSE ) ;
          }
  }
  else if ( tablaTrad[tabla]->formato == 3 ) /* alfanumpref */
  {
  
     strcpy( ptrn, patron ) ;
     
     while ( strlen(ptrn) > 0 )
     {
       
          strcpy( q.patron, ptrn ) ;
          
          p = bsearch( &q, tablaTrad[tabla]->u.pTop, tablaTrad[tabla]->num_elementos_informados, sizeof(struct sNodoLista), ComparaNodoLista ) ;

          if ( p != NULL )
          {
                  strcpy( patron_, ptrn ) ;
                  strcpy( reemp, p->reemp ) ;
                  strcpy( defecto, tablaTrad[tabla]->defecto ) ;
                  resto[0] = '\0' ;

                  num_reemp = 1 ;
                  strcpy( reemplazos[0].reemp, reemp ) ;

                  return( TRUE ) ;
          }
          
          ptrn[strlen(ptrn)-1] = '\0' ;
     }
          
     strcpy( patron_, patron ) ;
     reemp[0] = '\0' ;
     strcpy( defecto, tablaTrad[tabla]->defecto ) ;
     resto[0] = '\0' ;     

     num_reemp = 0 ;
     strcpy( reemplazos[0].reemp, "\0" ) ;
     
     return( FALSE ) ;
     
  }
  else if ( tablaTrad[tabla]->formato == 4 ) /* rango */
  {
 
     strcpy( ptrn, ObtenerItem(1,patron) ) ;
     r = atol( ObtenerItem(2,patron) ) ;

     p  = NULL ;
     pp = tablaTrad[tabla]->u.pTop ;
     
     for ( i=0; (i < tablaTrad[tabla]->num_elementos_informados) && !EsMayor(ObtenerItem(1,pp->patron),ptrn); i++ )
     {
          if ( EsIgual(ObtenerItem(1,pp->patron),ptrn) )
          {
              if ( (r >= atol(ObtenerItem(2,pp->patron))) &&
                   (r <= atol(ObtenerItem(3,pp->patron))) )
              {
                 p = pp ;
                 break ;
              }
          }
        
          pp++ ;
     }
               
     if ( p != NULL )
     {
        strcpy( patron_, ptrn ) ;
        strcpy( reemp, p->reemp ) ;
        strcpy( defecto, tablaTrad[tabla]->defecto ) ;
        resto[0] = '\0' ;
        
        num_reemp = 1 ;
        strcpy( reemplazos[0].reemp, reemp ) ;

        return( TRUE ) ;
     }    
     else
     {         
        strcpy( patron_, patron ) ;
        reemp[0] = '\0' ;
        strcpy( defecto, tablaTrad[tabla]->defecto ) ;
        resto[0] = '\0' ;        

        num_reemp = 0 ;
        strcpy( reemplazos[0].reemp, "\0" ) ;
        
        return( FALSE ) ;
     }     
  }  
  else if ( tablaTrad[tabla]->formato == 5 ) /* multivalor */
  {
     p = tablaTrad[tabla]->u.pTop ;
     num_reemp = 0 ;
     
     for ( i=0; (i < tablaTrad[tabla]->num_elementos_informados) && !EsMayor(p->patron,patron); i++ )
     {
          if ( EsIgual(p->patron,patron) )
          {
             strcpy( reemp, p->reemp ) ;

             if ( num_reemp < MAX_REEMP )
             {
                strcpy( reemplazos[num_reemp++].reemp, reemp ) ;             
             }
             else
             {
                fprintf( stderr, "Error: BuscaEnTablaPatron. Tabla '%d' excedio maximo multivalor (%d)!!!\n", tabla, MAX_REEMP ) ;
                exit( -1 ) ;             
             }
          }
        
          p++ ;
     }
               
     if ( num_reemp > 0 )
     {
        strcpy( patron_, patron ) ;
        strcpy( defecto, tablaTrad[tabla]->defecto ) ;
        resto[0] = '\0' ;
        reemp_sgte = 1 ;
        
        return( TRUE ) ;
     }    
     else
     {         
        strcpy( patron_, patron ) ;
        reemp[0] = '\0' ;
        strcpy( defecto, tablaTrad[tabla]->defecto ) ;
        resto[0] = '\0' ;        

        strcpy( reemplazos[0].reemp, "\0" ) ;
        
        return( FALSE ) ;
     }     
  }
  else
  {
     fprintf( stderr, "Error: BuscaEnTablaPatron. Formato '%d' invalido para Tabla '%d'!!!\n", tablaTrad[tabla]->formato, tabla ) ;
     exit( 1 ) ;
  }

  return( FALSE ) ;
}

static int ImprimeListaTabla( int tabla )
{
   int i ;
   struct sNodoLista *p ;

   fprintf( stderr, "Lista\n" ) ;

   p = tablaTrad[tabla]->u.pTop ;
   for ( i = 0; i < tablaTrad[tabla]->num_elementos_informados; i++ )
   {
      fprintf( stderr, "NodoLista(%d): Patron= '%s', Reemp= '%s'\n", i+1,  p->patron, p->reemp ) ;
      p++  ;
   }

   return( 0 ) ;
}

char *ObtenerPatron( void )
{
        return( patron_ ) ;
}

int ObtenerNumReemp( void )
{
        return( num_reemp ) ;
}

char *ObtenerReemp( void )
{
        reemp_sgte = 1 ; /* inicializa multivalor */
        
        return( reemplazos[0].reemp ) ;
}

char *ObtenerReempN( int valor )
{
        if ( valor-1 < num_reemp )
        {
           return( reemplazos[valor-1].reemp ) ;
        }
        else
        {
           return( NULL ) ;
        }
}

char *ObtenerResto( void )
{
        return( resto ) ;
}

char *ObtenerDefecto( void )
{
        return( defecto ) ;
}

char *ObtenerSgteReemp( void )
{        
        if ( (num_reemp > 1) && (reemp_sgte < num_reemp) )        
        {
           return( reemplazos[reemp_sgte++].reemp ) ;
        }
        else
        {
           return( NULL ) ;        
        }
}

/*******/

void InicializaSalida( void )
{
        condicion_sobrante = FALSE ;
        pre_condicion_con_sob = FALSE ;
        condicion_terminal = FALSE ;
        salida_con_sob = FALSE ;
        indice_salida = -1 ;
        id_salida = -1 ;
        strcpy( salida_procesada, "sobrante" ) ;
        strcpy( campo_procesado, "null" ) ;
}

void GeneraSalida( int id_sal )
{
        id_salida = id_sal ;
}

void AgregaSalida( int salida )
{
        if ( indice_salida == -1 )
                indice_salida = 0 ;

        salidas_generadas[indice_salida++] = salida ;
}

int ObtieneSalida( int *salida )
{
        if ( --indice_salida >= 0 )
        {
                *salida = salidas_generadas[indice_salida] ;
                return( TRUE ) ;
        }
        else
                return( FALSE ) ;
}

int HaySalida( void )
{
        if ( indice_salida >= 0 )
                return( TRUE ) ;
        else
        {
                GeneraError( "No hay salida." ) ;
                return( FALSE ) ;
        }
}

void EliminaSalida( int salida )
{
        while ( (indice_salida > 0) && (salidas_generadas[indice_salida-1] == salida) )
        {        
           indice_salida-- ;
        }
}

void InicializaMultiRegistro( void )
{
   multiregistros = 1 ;
   multiregistro_actual = 1 ;
}

void EsMultiRegistro( int registros )
{
   multiregistros = registros ;
}

void MultiRegistroActual( int multiregistro )
{
   multiregistro_actual = multiregistro ;
}

int ObtenerMultiRegistroActual( void )
{
   return( multiregistro_actual ) ;
}

int ObtenerMultiRegistros( void )
{
   return( multiregistros ) ;
}

int HayMultiRegistro( void )
{
   return( (multiregistros > 1 ? TRUE : FALSE) ) ;
}

int HaySalidaConSob( void )
{
        return( salida_con_sob ) ;
}

void SeProdujoSalidaConSob( void )
{
        salida_con_sob = TRUE ;
}

void SalidaCampoProcesado ( char *salida, char *campo )
{
        strcpy( salida_procesada, salida ) ;
        strcpy( campo_procesado, campo ) ;
}

int HayCondSobrante( void )
{
        return( condicion_sobrante ) ;
}

void SeProdujoCondSobrante( void )
{
        condicion_sobrante = TRUE ;
}

void InicializaError( void )
{
        error = FALSE ;
        mensaje_error[0] = '\0' ;
}

void GeneraError( char *mensaje )
{
        error = TRUE ;
        strcpy( mensaje_error, mensaje ) ;
}

int HayError( void )
{
        return( error ) ;
}

void EscribirMensajeError( FILE *archivo_error_msg )
{
        fprintf( archivo_error_msg, "%08ld\t%-40s\n", registros_error++, mensaje_error ) ;
}

void InicializaSobrante( void )
{
        sobrante = FALSE ;
        mensaje_sobrante[0] = '\0' ;
}

void GeneraSobrante( char *mensaje )
{
        sobrante = TRUE ;
        strcpy( mensaje_sobrante, mensaje ) ;
}

int HaySobrante( void )
{
        return( sobrante ) ;
}

void EscribirMensajeSobrante( FILE *archivo_sobrante_msg )
{
        fprintf( archivo_sobrante_msg, "%08ld\t%-12s\t%-12s\t%-40s\n", registros_sobrante++, salida_procesada, campo_procesado, mensaje_sobrante ) ;
}

int HayPreCondicion( void )
{
        if ( id_salida == -1 )
                return( FALSE ) ;
        else
                return( TRUE ) ;
}

int HayPreCondicionSalida( int id_sal )
{
        if ( (id_salida == -1) || (id_sal != id_salida) )
                return( FALSE ) ;
        else        
                return( TRUE ) ;
}

void SeProdujoPreCondConSob( void )
{
        pre_condicion_con_sob = TRUE ;
}

int HayPreCondConSob( void ) 
{
        return( pre_condicion_con_sob ) ;
}

void EsCondicionTerminal( void )
{
        condicion_terminal = TRUE ;
}

int HayCondicionTerminal( void )
{
        return( condicion_terminal ) ;
}


void AjustarPropiedad( char *texto, int propiedad, char separador )
{
   char buffer_temp[BUFFER_LARGO+1] ;
   char pe[BUFFER_LARGO+1], pd[BUFFER_LARGO+1] ;
   char dia[2+1], mes[2+1], ano[2+1], siglo[4+1] ;
   char hh[2+1], mm[2+1], ss[2+1] ;
   char AA[2+1] ;
   int nd ;
   int nAgno, nMes, nDia ;
   long segundos=0L ;
   time_t t ;
   struct tm *st ;


   memset( buffer_temp, ' ', strlen(texto) ) ;
   buffer_temp[strlen(texto)] = '\0' ;

   switch ( propiedad )
   {
      case NULO:
      case PROP_AAAAMMDD:
         if ( !FechaValida(atol(texto)) )
         {
            strcpy( texto, "-1" ) ; /* fecha invalida */
         }
         break ;

      case PROP_AAMMDD:
         time(&t);
         st = localtime(&t) ;
         
         sprintf( buffer_temp, "%d", 1900 + st->tm_year ) ; /* AAAA fecha act */
         memcpy( AA, buffer_temp, 2 ) ;
         AA[2] = '\0' ;
         
         sprintf( buffer_temp, "%s%s", AA, texto ) ;
         memcpy( texto, buffer_temp, 8 ) ;
         texto[8] = '\0' ;
         
         if ( !FechaValida(atol(texto)) )
         {
            strcpy( texto, "-1" ) ; /* fecha invalida */
         }
         
         break;

      case PROP_DDMMAA:
      case PROP_MMDDAA:
      case PROP_MM_DD: 
         
         if ( propiedad == PROP_DDMMAA )
                 memcpy( dia, texto + (1 - 1), 2 ) ;
         else if ( propiedad == PROP_MM_DD )
                 memcpy( dia, texto + (6 - 1), 2 ) ;
         else
                 memcpy( dia, texto + (4 - 1), 2 ) ;
         dia[2] = '\0' ;
         
         if ( propiedad == PROP_DDMMAA )
                 memcpy( mes, texto + (4 - 1), 2 ) ;
         else if ( propiedad == PROP_MM_DD )
                 memcpy( mes, texto + (1 - 1), 2 ) ;
         else
                 memcpy( mes, texto + (1 - 1), 2 ) ;
         
         mes[2] = '\0' ;
         
         time(&t);
         st = localtime(&t) ;
         
         if ( propiedad == PROP_MM_DD )
         {
            sprintf( buffer_temp, "%d", 1900 + st->tm_year ) ; /* AAAA fecha act */
            memcpy( ano, buffer_temp+2, 2 ) ;
            ano[2] = '\0' ;

            nMes  = atoi( mes ) ;
            nAgno = atoi( ano ) ;
            /* tratamiento llamadas agno anterior */
            if ( (nMes - (st->tm_mon+1)) > 0 )
            {
               nAgno -= 1 ;
               sprintf( ano, "%2d", nAgno ) ;
            }
         }
         else
         {
            memcpy( ano, texto + (7 - 1), 2 ) ;
            ano[2] = '\0' ;
         }
         
         sprintf( buffer_temp, "%d", 1900 + st->tm_year ) ; /* AAAA fecha act */
         memcpy( AA, buffer_temp, 2 ) ;
         AA[2] = '\0' ;
         
         sprintf( buffer_temp, "%s%s%s%s", AA, ano, mes, dia ) ;
         memcpy( texto, buffer_temp, 8 ) ;
         texto[8] = '\0' ;
         
         if ( !FechaValida(atol(texto)) )
         {
            strcpy( texto, "-1" ) ; /* fecha invalida */
         }
         
         break ;

      case PROP_HHMMSS:
      
         switch( separador )
         {
            case '~':
            
               if ( !HoraValida(texto) )
               {
                  strcpy( texto, "-1" ) ; /* hora invalida */
                  break ;
               }
        
               memcpy( hh, texto + (1 - 1), 2 ) ;
               hh[2] = '\0' ;
        
               memcpy( mm, texto + (3 - 1), 2 ) ;
               mm[2] = '\0' ;
        
               memcpy( ss, texto + (5 - 1), 2 ) ;
               ss[2] = '\0' ;
        
               segundos = (atol(hh) * 3600L) + (atol(mm) * 60L) + atol(ss) ;
               sprintf( texto, "%ld", segundos ) ;
        
               break ;
        
            case ':':
            
               memcpy( hh, texto + (1 - 1), 2 ) ;
               hh[2] = '\0' ;
        
               memcpy( mm, texto + (4 - 1), 2 ) ;
               mm[2] = '\0' ;
        
               memcpy( ss, texto + (7 - 1), 2 ) ;
               ss[2] = '\0' ;
        
               sprintf( buffer_temp, "%s%s%s", hh, mm, ss ) ;
               if ( !HoraValida(buffer_temp) )
               {
                  strcpy( texto, "-1" ) ; /* hora invalida */
                  break ;
               }
        
               segundos = (atol(hh) * 3600L) + (atol(mm) * 60L) + atol(ss) ;
               sprintf( texto, "%ld", segundos ) ;
        
               break ;
        
            default:
                    fprintf( stderr, "Error: AjustarPropiedad(...) Separador de propiedad no implementado\n" ) ;
                    exit( 1 ) ;
         }
         break ;

      case PROP_HHMM:
      case PROP_HH_MM:
      
         switch( separador )
         {
            case '~':
        
               memcpy( hh, texto + (1 - 1), 2 ) ;
               hh[2] = '\0' ;
        
               if ( propiedad == PROP_HH_MM )
                  memcpy( mm, texto + (6 - 1), 2 ) ;
               else
                  memcpy( mm, texto + (3 - 1), 2 ) ;
               mm[2] = '\0' ;
        
               sprintf( buffer_temp, "%s%s00", hh, mm ) ;
               if ( !HoraValida(buffer_temp) )
               {
                  strcpy( texto, "-1" ) ; /* hora invalida */
                  break ;
               }
        
               segundos = (atol(hh) * 3600L) + (atol(mm) * 60L) ;
               sprintf( texto, "%ld", segundos ) ;        
               break ;        
        
            case ':':
            
               memcpy( hh, texto + (1 - 1), 2 ) ;
               hh[2] = '\0' ;
        
               memcpy( mm, texto + (4 - 1), 2 ) ;
               mm[2] = '\0' ;
        
               sprintf( buffer_temp, "%s%s00", hh, mm ) ;
               if ( !HoraValida(buffer_temp) )
               {
                  strcpy( texto, "-1" ) ; /* hora invalida */
                  break ;
               }
        
               segundos = (atol(hh) * 3600L) + (atol(mm) * 60L) ;
               sprintf( texto, "%ld", segundos ) ;
        
               break ;
        
            default:
            
               fprintf( stderr, "Error: AjustarPropiedad(...) Separador de propiedad no implementado\n" ) ;
               exit( 1 ) ;
         }
         break ;
        
      case PROP_SS:
      
         if ( !EsNumerico(ltrim(rtrim(texto))) )
         {
            strcpy( texto, "-1" ) ; /* hora invalida */
         }
         break ;

      case PROP_MM:
      
         if ( !EsNumerico(ltrim(rtrim(texto))) )
         {
            strcpy( texto, "-1" ) ; /* hora invalida */
            break ;
         }

         if ( (atol(texto)/60L) != 0L ) 
         {
            strcpy( texto, "-1" ) ; /* hora invalida */
            break ;
         }

         segundos = atol(texto) * 60L ;
         sprintf( texto, "%ld", segundos ) ;
         
         break ;

      case PROP_MMMMSS:
      
         if ( !EsNumerico(ltrim(rtrim(texto))) )
         {
            strcpy( texto, "-1" ) ; /* hora invalida */
            break ;
         }

         memcpy( buffer_temp, texto, 4 ) ;
         buffer_temp[4] = '\0' ;

         segundos = atol(buffer_temp) * 60L + atol( texto+4 ) ;
         sprintf( texto, "%ld", segundos ) ;

         break ;
         
      case PROP_0:
      case PROP_00:   
      case PROP_000:  
      case PROP_0000: 
      case PROP_00000:
      
         if ( !EsNumerico(ltrim(rtrim(texto))) )
         {
            strcpy( texto, "-1" ) ; /* decimal invalido */
            break ;
         }
         
         if ( propiedad == PROP_0 )
         {
            nd = 1 ;
         }
         else if ( propiedad == PROP_00 )
         {
            nd = 2 ;         
         }
         else if ( propiedad == PROP_000 )
         {
            nd = 3 ;         
         }
         else if ( propiedad == PROP_0000 )
         {
            nd = 4 ;         
         }
         else if ( propiedad == PROP_00000 )
         {
            nd = 5 ;         
         }
         
         if ( strlen(ltrim(rtrim(texto))) <= nd )
         {
            fprintf( stderr, "Error: AjustarPropiedad(...) Decimal mal definido\n" ) ;
            exit( 1 ) ;                  
         } 

         strcpy( buffer_temp, ltrim(rtrim(texto)) ) ;
                  
         memcpy( pe, buffer_temp, strlen(buffer_temp) - nd ) ;
         pe[strlen(buffer_temp) - nd] = '\0' ;
         
         memcpy( pd, buffer_temp + strlen(buffer_temp) - nd , nd ) ;         
         pd[nd] = '\0' ;

         sprintf( buffer_temp, "%d.%s", atoi(pe), rztrim(pd) ) ;
         strcpy( texto, buffer_temp ) ;
            
         break ;
         
      default:     
           
         fprintf( stderr, "Error: AjustarPropiedad(...) Propiedad no implementada\n" ) ;
         exit( 1 ) ;
   }
}

void AjustarTexto( char *texto, int justif, int relleno, int propiedad, char separador )
{
   char c, *p, *q ;
   char dia[2+1], mes[2+1], ano[2+1], siglo[4+1] ;
   char hh[2+1], mm[2+1], mmmm[4+1], ss[2+1] ;
   char buffer_temp[BUFFER_LARGO+1] ;
   char pe[BUFFER_LARGO+1], pd[BUFFER_LARGO+1] ;
   int  espacios, blancos_izq=0, blancos_der=0 ;
   int  i, nd ;
   int  largo_texto, largo ;

   largo_texto = strlen( texto ) ;

   memset( buffer_temp, ' ', largo_texto ) ;
   buffer_temp[largo_texto] = '\0' ;

   /* Propiedad */
   switch ( propiedad )
   {
      case PROP_AAMMDD:
      case PROP_DDMMAA:
      case PROP_MMDDAA:
      
         /* Fecha */
         memcpy( dia, texto + (7 - 1), 2 ) ;
         dia[2] = '\0' ;
         
         memcpy( mes, texto + (5 - 1), 2 ) ;
         mes[2] = '\0' ;
         
         memcpy( ano, texto + (3 - 1), 2 ) ;
         ano[2] = '\0' ;
         
         memcpy( siglo, texto + (1 - 1), 4 ) ;
         siglo[4] = '\0' ;
         break ;
         
      case PROP_HHMMSS:
      case PROP_HHMM:
      case PROP_SS:
      case PROP_MM:
      case PROP_MMMMSS:
      
         /* Hora */
         
         sprintf( hh, "%02ld", (atol(texto) / 3600L) ) ;
         sprintf( mm, "%02ld", ((atol(texto)%3600L) / 60L) ) ;
         sprintf( mmmm, "%04ld", (atol(hh) * 60L) + atol(mm) ) ;
         sprintf( ss, "%02ld", ((atol(texto)%3600L) % 60L) ) ;         
         break ;
         
      case PROP_0:
      case PROP_00:   
      case PROP_000:  
      case PROP_0000: 
      case PROP_00000:

         largo = (int)strstr(texto,".") - (int)texto ;
         memcpy( pe, texto, largo ) ;
         pe[largo] = '\0' ;
         
         strcpy( pd, strstr(texto,".") + 1 ) ;
         break ;         
   }

   switch ( propiedad )
   {
      case NULO:
      case PROP_AAAAMMDD:
      
         strcpy( buffer_temp, texto ) ;
         break ;

      case PROP_AAMMDD:
      
         sprintf( buffer_temp, "%s%s%s", ano, mes, dia ) ;
         break ;

      case PROP_DDMMAA:
      
         if ( separador == '~' )
         {
            sprintf( buffer_temp, "%s%s%s", dia, mes, ano ) ;
         }
         else
         {
            sprintf( buffer_temp, "%s%c%s%c%s", dia, separador, mes, separador, ano ) ;
         }
         break ;

      case PROP_MMDDAA:
      
         if ( separador == '~' )
         {
            sprintf( buffer_temp, "%s%s%s", mes, dia, ano ) ;
         }
         else
         {
            sprintf( buffer_temp, "%s%c%s%c%s", mes, separador, dia, separador, ano ) ;
         }         
         break ;

      case PROP_HHMMSS:
      
         switch( separador )
         {
            case '~':
            
               sprintf( buffer_temp, "%s%s%s", hh, mm, ss ) ;         
               break;
         
            case ':':
            
               sprintf( buffer_temp, "%s%c%s%c%s", hh, separador, mm, separador, ss ) ;         
               break ;
         
            default:
            
               fprintf( stderr, "Error: AjustarTexto(...) Separardor propiedad no para tipo 'hora' no implementada\n" ) ;
               exit( 1 ) ;
         }
         break ;

      case PROP_HHMM:
      
         switch( separador )
         {
            case '~':
            
               sprintf( buffer_temp, "%s%s", hh, mm ) ;
               break;
         
            case ':':
            
               sprintf( buffer_temp, "%s%c%s", hh, separador, mm ) ;         
               break ;
         
            default:
            
               fprintf( stderr, "Error: AjustarTexto(...) Separardor propiedad no para tipo 'hora' no implementada\n" ) ;
               exit( 1 ) ;
         }
         break ;

      case PROP_SS:
      
         strcpy( buffer_temp, texto ) ;
         break ;
           
      case PROP_MM:
      
         sprintf( buffer_temp, "%ld", atol(texto) / 60L )  ;
         break ;

      case PROP_MMMMSS:
      
         sprintf( buffer_temp, "%s%s", mmmm, ss ) ;
         break;

      case PROP_0:
      case PROP_00:   
      case PROP_000:  
      case PROP_0000: 
      case PROP_00000:

         if ( propiedad == PROP_0 )
         {
            nd = 1 ;
         }
         else if ( propiedad == PROP_00 )
         {
            nd = 2 ;         
         }
         else if ( propiedad == PROP_000 )
         {
            nd = 3 ;         
         }
         else if ( propiedad == PROP_0000 )
         {
            nd = 4 ;         
         }
         else if ( propiedad == PROP_00000 )
         {
            nd = 5 ;         
         }
         
         sprintf( buffer_temp, "%d%-*s", atoi(pe), nd, rztrim(rtrim(pd)) ) ;
         for ( i=0; buffer_temp[i] != '\0'; i++ )
         {
            if ( buffer_temp[i] == ' ' )
            {
               buffer_temp[i] = '0' ;
            }
         }         
         break ;

      default:
      
         fprintf( stderr, "Error: AjustarTexto(...) Propiedad no implementada.\n" ) ;
         exit( 1 ) ;
   }

   /* Relleno */
   switch ( relleno )
   {
      case NULO:
      
         c = ' ' ;
         break ;
         
      case RELL_BLANCO:
      
         c = ' ' ;
         break ;
         
      case RELL_CERO:
      
         c = '0' ;
         break ;
         
      case RELL_EFE:
      
         c = 'F' ;
         break ;
         
      default:
      
         fprintf( stderr, "Error: AjustarTexto(...) Relleno no implementado.\n" ) ;
         exit( 1 ) ;
   }

   memset( texto, ' ', largo_texto ) ;
   buffer_temp[largo_texto] = '\0' ;

   memcpy( texto, buffer_temp, strlen(buffer_temp) ) ;

   memset( buffer_temp, ' ', largo_texto ) ;
   buffer_temp[largo_texto] = '\0' ;

   /* Justificacion */
   switch ( justif )
   {
      case NULO:
      
         strcpy( buffer_temp, texto ) ;
         break ;
         
      case JUST_IZQ:
      
         p = texto ;
         q = buffer_temp ;
         
         while ( (*p != '\0') && (*p == ' ') )
                 p++ ;
         
         while ( *p != '\0' )
                 *q++ = *p++ ;
         
         q-- ;
         
         while ( (q != buffer_temp) && (*q == ' ') )
                 *q-- = c ;
         
         break ;
         
      case JUST_CEN:
      
         p = texto ;
         
         while ( (*p != '\0') && (*p == ' ') )
         {
                 blancos_izq++ ;
                 p++ ;
         }
         
         q = texto + strlen(texto) - 1 ;
         
         while ( (q != texto) && (*q == ' ') )
         {
                 blancos_der++ ;
                 q-- ;
         }
         
         espacios = (blancos_izq+blancos_der) / 2 ;
         
         q = buffer_temp ;
         
         while ( q < buffer_temp+espacios )
                 *q++ = c ;
         
         p = texto + blancos_izq ;
         
         while ( p < texto+strlen(texto)-blancos_der )
                 *q++ = *p++ ;
         
         while ( *q != '\0' )
                 *q++ = c ;
         
         break ;
    
      case JUST_DER:
      
         p = texto + strlen(texto) - 1 ;
         q = buffer_temp + strlen(texto) - 1 ;
         
         while ( (p != texto) && (*p == ' ') )
                 p-- ;
         
         while ( p >= texto )
                 *q-- = *p-- ;
         
         q = buffer_temp ;
         
         while ( (*q != '\0') && (*q == ' ') )
                 *q++ = c ;
         
         break ;

      default:
      
         fprintf( stderr, "Error: AjustarTexto(...) Tipo no implementado.\n" ) ;
         exit( 1 ) ;
   }

   strcpy( texto, buffer_temp ) ;
   
}

int NumeroPalabras( char *texto )
{
        char *p=texto ;
        int numero_palabras=0 ;
        int estado ;
        int dentro=1, fuera=0 ; /* dentro y fuera de una palabra */

        estado = fuera ;
        while ( *p != '\0')
        {
                if ( (*p == ' ') || (*p == '\t') || (*p == '\n') )
         estado = fuera ;
                else if ( estado == fuera )
                {
         estado = dentro ;
         numero_palabras++ ;
                }

                p++ ;
        }

        return( numero_palabras ) ;
}
 

char *Minuscula( char *texto ) 
{
        char *p=texto ;

        while ( *p != '\0' )
                *p++ = tolower( *p ) ;

        return( texto ) ;
}

char *Mayuscula( char *texto ) 
{
        char *p=texto ;

        while ( *p != '\0' )
                *p++ = toupper( *p ) ;

        return( texto ) ;
}

/*
 *       ltrim
 */
char *ltrim (char *texto )
{
        static char str[255+1] ;
        char *p ;

        strcpy( str, texto ) ;

        p = str ;
        while( (*p != '\0') && isspace(*p) )
                p++ ;

        return( p ) ;
}

/*
 *       rtrim
 */
char *rtrim( char *texto )
{
        static char str[255+1] ;
        char *p;

        strcpy( str, texto ) ;

        p = str + strlen(str) - 1 ;

        while( (p >= str) && isspace(*p) )
                p-- ;

        *++p = '\0' ;

        return( str ) ;
}

/*
 *       lztrim
 */
char *lztrim( char *texto )
{
        static char str[255+1] ;
        char *p;

        strcpy( str, texto ) ;

        p = str ;
        while( (*p != '\0') && (*p == '0') )
                p++ ;

        return( p ) ;
}

/*
 *       rztrim
 */
char *rztrim( char *texto )
{
        static char str[255+1] ;
        char *p;

        strcpy( str, texto ) ;

        p = str + strlen(str) - 1 ;

        while( (p > str) && (*p == '0') )
                p-- ;

        *++p = '\0' ;

        return( str ) ;
}

/*
 *       lftrim
 */
char *lftrim( char *texto )
{
        static char str[255+1] ;
        char *p;

        strcpy( str, texto ) ;

        p = str ;
        while( (*p != '\0') && (*p == 'F') )
                p++ ;

        return( p ) ;
}

/*
 *       rftrim
 */
char *rftrim( char *texto )
{
        static char str[255+1] ;
        char *p;

        strcpy( str, texto ) ;

        p = str + strlen(str) - 1 ;

        while( (p > str) && (*p == 'F') )
                p-- ;

        *++p = '\0' ;

        return( str ) ;
}

/*
 *       latrim
 */
char *latrim( char *texto )
{
        static char str[255+1] ;
        char *p;

        strcpy( str, texto ) ;

        p = str ;
        while( (*p != '\0') && !isdigit(*p) )
                p++ ;

        return( p ) ;
}

/*
 *       ratrim
 */
char *ratrim( char *texto )
{
        static char str[255+1] ;
        char *p;

        strcpy( str, texto ) ;

        p = str + strlen(str) - 1 ;

        while( (p > str) && !isdigit(*p) )
                p-- ;

        *++p = '\0' ;

        return( str ) ;
}

/*
 * ObtenerItem
 * 
 * Se asume texto de la forma: 
 *
 * # texto=> texto1.texto2,texto3:texto4;texto5
 * texto=> texto1.texto2:texto3;texto4
 */
char *ObtenerItem( int item, char *texto )
{
        static char str[255+1] ;
        int  i=1 ;
        char *p, *q ;

        if ( item > 4 )
        {
           return( NULL ) ;
        }

        *str = '\0' ;

        p = texto ;
        q = str ;

        while( *p != '\0' )
        {
           if ( (*p == '.') ||
                (*p == ':') ||
                (*p == ';') )
           {
              i++ ;
              p++ ;
              continue ;
           }

           if ( i > item )
              break ;
           else if ( i == item )
              *q++ = *p++ ;
           else
              p++ ;
        }

        *q = '\0' ;

        if ( *str == '\0' )
           return( NULL ) ;
        else
           return( str ) ;
}

/*
 *       itoa
 */
char *itoa( int numero )
{
        static char str[255+1] ;

        sprintf( str, "%d", numero ) ;

        return( str ) ;
}

/*
 * atoc
 */
char atoc( char *texto )
{
   return( texto[0] ) ;
}

int EsNumerico( char *texto )
{
    int i, largo ;

    largo = strlen(texto) ;

    if ( largo == 0 )
       return( FALSE ) ;
    
    for ( i=0; i < largo; i++ )
    {
       if ( !isdigit(texto[i]) )
          return( FALSE ) ;
    }

    return( TRUE ) ;
}

int EsLineaEnBlanco( char *texto )
{
        char *p=texto ;

        while ( (*p != '\0') && (*p == ' ' || *p == '\t' || *p == '\n') )
                p++ ;

        if ( *p == '\0' )
                return( TRUE ) ;
        else
                return( FALSE ) ;
}

int LeeRegistro( FILE *archivo, char *registro )
{
        if ( fgets(registro,BUFFER_LARGO,archivo) == NULL )
                return( FALSE ) ;
        else
                return( TRUE ) ;
}

int CreaArchivo( FILE **archivo, char *nombre_archivo )
{
        *archivo = fopen( nombre_archivo, "w" ) ;
        if ( *archivo == NULL )
        {
                fprintf( stderr, "Error: CrearArchivo(...) No se pudo crear archivo '%s'.\n", nombre_archivo ) ;
                exit( 1 ) ;
        }

        return( 0 ) ;
}

int LeeArchivo( FILE **archivo, char *nombre_archivo )
{
        if ( strcmp("",nombre_archivo) == 0 )
                *archivo = stdin ;
        else
                *archivo = fopen( nombre_archivo, "r" ) ;

        if ( *archivo == NULL )
        {
                fprintf( stderr, "Error: LeeArchivo(...) No se pudo leer archivo '%s'.\n", nombre_archivo ) ;
                exit( 1 ) ;
        }

        return( 0 ) ;
}

/* Funciones de manejo de fecha cortesia de Francois Bertrand
 *
 *  Rutinas     :
 *
 *      ObtAgnoMesDia
 *          Obtiene agno, mes y dia de una fecha
 *
 *      JulDay
 *          calcula el dia juliano
 *
 *      DiaSemana
 *          determina el dia de la semana: lunes = 0, martes = 1 ....
 *
 *      EsBisiesto
 *          determina si un ano es bisiesto
 *
 *      FechaValida
 *          chequea validez de una fecha
 *
 *      HoraValida
 *          chequea validez de una hora
 *  ----------------------------------------------------------------------------
 */

/*
 * ObtAgnoMesDia
 */ 
void ObtAgnoMesDia( long fecha, int *nAgno, int *nMes, int *nDia )
{
   char tmp[4+1] ;
   char _fecha[8+1] ;

   sprintf( _fecha, "%ld", fecha ) ;

   memcpy( tmp, _fecha, 4 ) ;
   tmp[4] = '\0' ;
   *nAgno = atoi( tmp ) ;

   memcpy( tmp, _fecha+4, 2 ) ;
   tmp[2] = '\0' ;
   *nMes = atoi( tmp ) ;

   memcpy( tmp, _fecha+6, 2 ) ;
   tmp[2] = '\0' ;
   *nDia = atoi( tmp ) ;
}

/*
 *  ObtFecha
 */
long ObtFecha( int nAgno, int nMes, int nDia )
{
   char fecha[8+1] ;
   time_t t ;
   struct tm *st ;

   if ( nAgno == 0 )
   {
      time( &t ) ;
      st = localtime(&t) ; /* fecha actual del sistema */

      nAgno = 1900 + st->tm_year ;

      /* tratamiento llamadas agno anterior */
      if ( (nMes - (st->tm_mon+1)) > 0 )  
         nAgno -= 1 ;
   }
      
   sprintf( fecha, "%04d%02d%02d", nAgno, nMes, nDia ) ;

   return( atol(fecha) ) ;
}

void ObtHorMinSeg( long hora, int *hh, int *mm, int *ss )
{
/* fprintf( stdout, "bibliote: hora= '%ld'\n", hora ) ; */
   *hh = hora / 3600L ;
   *mm = (hora%3600L) / 60L ;
   *ss = (hora%3600L) % 60L ;
/* fprintf( stdout, "bibliote: hh= '%d', mm= '%d', ss= '%d'\n", *hh, *mm, *ss ) ; */

}

/*
 *  ObtHora
 */
long ObtHora( int hh, int mm, int ss )
{
   return( (hh * 3600L) + (mm * 60L) + ss ) ;
}

/*
 *  ObtHHMMSS
 */
long ObtHHMMSS( long hora )
{
   char texto[6+1] ;
   int hh, mm, ss ;

   ObtHorMinSeg( hora, &hh, &mm, &ss ) ;
   sprintf( texto, "%02d%02d%02d", hh, mm, ss ) ;

   return( atol(texto) ) ; 
}

/*
 *  JulDay
 */
long JulDay( long fecha )
{
    int  nAgno, nMes, nDia ;
    long lf, lg ;
    long l1, l2 ;
    long lJulDia ;

    ObtAgnoMesDia( fecha, &nAgno, &nMes, &nDia ) ; 

    if ( nMes > 2 )
    {
        lf = (long) nMes + 1 ;
        lg = (long) nAgno ;
    }

    else
    {
        lf = (long) nMes + 13L ;
        lg = (long) nAgno - 1L ;
    }

    l1 =  (long) (365.25 * (double) lg) ;
    l2 =  (long) (30.6 * (double) lf) ;

    lJulDia = l1 + l2 + (long) nDia - 621049L ;

    return( lJulDia ) ;
}

/*
 *  DiaSemana
 */
int DiaSemana( long fecha )
{
    long lJul ;

    lJul = JulDay( fecha ) ;

    /* el 15 de Junio de 1914 es un dia Lunes */
    lJul -= JulDay( 19140615L ) ;

    lJul %= 7L ;

    return( (int)lJul ) ;
}

/*
 *  EsBisiesto
 */
int EsBisiesto( int nAgno )
{
    if ( (nAgno % 400) == 0 )
        return( TRUE ) ;

    if ( ((nAgno % 4) == 0) && ((nAgno % 100) != 0) )
        return( TRUE ) ;

    return( FALSE ) ;
}

static int anDiasXMes [] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} ;
static int anDiasJuli [] = {31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365} ;

#define ENERO       0
#define FEBRERO     1
#define MARZO       2
#define ABRIL       3
#define MAYO        4
#define JUNIO       5
#define JULIO       6
#define AGOSTO      7
#define SEPTIEMBRE  8
#define OCTUBRE     9
#define NOVIEMBRE   10
#define DICIEMBRE   11 

/*
 *  SeteaFebrero
 */

static void SeteaFebrero( int nAgno )
{
    if ( EsBisiesto(nAgno) )
        anDiasXMes[FEBRERO] = 29 ;

    else
        anDiasXMes[FEBRERO] = 28 ;
}

/*
 *  FechaValida
 */
int FechaValida( long fecha )
{
    int  nAgno, nMes, nDia ;

    ObtAgnoMesDia( fecha, &nAgno, &nMes, &nDia ) ;

    if ( (nMes >= 1) && (nMes <= 12) )
    {
       SeteaFebrero( nAgno ) ;

       if ( (nDia >= 1) && (nDia <= anDiasXMes[nMes - 1]) )
          return( TRUE ) ;
    }

    /* por defecto */
    return( FALSE ) ;
}

/*
 *  HoraValida
 */
int HoraValida( char *hora )
{
    int  nHora, nMinuto, nSegundo ;
    char hh[2+1], mm[2+1], ss[2+1] ;

    memcpy( hh, hora + (1 - 1), 2 ) ;
    hh[2] = '\0' ;

    memcpy( mm, hora + (3 - 1), 2 ) ;
    mm[2] = '\0' ;

    memcpy( ss, hora + (5 - 1), 2 ) ;
    ss[2] = '\0' ;

    nHora    = atoi( hh ) ;
    nMinuto  = atoi( mm ) ;
    nSegundo = atoi( ss ) ;

    if ( (nHora >= 0) && (nHora <= 23) )
    {
       if ( (nMinuto >= 0) && (nMinuto <= 59) )
       {
          if ( (nSegundo >= 0) && (nSegundo <= 59) )
          {
             return( TRUE ) ;
          }
       }
    }

    /* por defecto */
    return( FALSE ) ;
}

/*
 * ftoa
 */
char *ftoa( long fecha )
{
   static char _fecha[8+1] ;

   sprintf( _fecha, "%l", fecha ) ;

   return( _fecha ) ;
}

/* EOF bibliote.c */
