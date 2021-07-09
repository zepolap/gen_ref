/*
 * tabla.h
 *
 */

typedef struct tabla {

   char nombre[NOMBRE_TABLA_LARGO+1] ;
   int  codigo ;
   struct tabla *siguiente ;

} tabla ;

tabla *LeeEspecTabla( FILE * ) ;
void ImprimeTabla( tabla * ) ;
