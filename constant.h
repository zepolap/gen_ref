/*
 * constante.h
 *
 */

typedef struct constante {

   char nombre[NOMBRE_CONSTANTE_LARGO+1] ;
   char valor[VALOR_LARGO+1] ;
   struct constante *siguiente ;

} constante ;

constante *LeeEspecConstante( FILE * ) ;
void ImprimeConstante( constante * ) ;
