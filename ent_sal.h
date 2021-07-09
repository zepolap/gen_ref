/*
 * ent_sal.h
 *
 */

typedef struct entrada_salida {

   char nombre[NOMBRE_ENTRADA_SALIDA_LARGO+1] ;
   char formato[NOMBRE_FORMATO_LARGO+1] ;
   int  tipo ;
   struct entrada_salida *siguiente ;

} entrada_salida ;

typedef struct pre_condicion_salida {

   implementacion *lista_implementacion ;

} pre_condicion_salida ;

typedef struct variable {

   char nombre[NOMBRE_VARIABLE_LARGO+1] ;
   char tipo[NOMBRE_TIPO_LARGO+1] ;
   struct variable *siguiente ;

} variable ;

typedef struct requisito {

   char registro[BUFFER_LARGO] ;
   struct requisito *siguiente ;

} requisito ;

typedef struct campo_salida {

   char nombre[NOMBRE_FUNCION_LARGO+1] ;
   char tipo[NOMBRE_TIPO_LARGO+1] ;
   implementacion *lista_implementacion ;
   struct campo_salida *siguiente ;

} campo_salida ;

typedef struct salida {

   char nombre[NOMBRE_FUNCION_LARGO+1] ;
   char nombre_formato[NOMBRE_FORMATO_LARGO+1] ;
   requisito *lista_requisito ;
   implementacion *lista_inicializacion ; /* inicializacion salida */
   int condicion_terminal ;
   campo_salida *lista_campo ;
   struct salida *siguiente ;

} salida ;

typedef struct reformateo {

   char nombre_entrada[NOMBRE_ENTRADA_SALIDA_LARGO+1] ;
   char nombre_formato[NOMBRE_FORMATO_LARGO+1] ;
   requisito *lista_error ; /* condicion error */
   requisito *lista_requisito ; /* condicion sobrante */
   implementacion *lista_inicializacion ; /* inicializacion reformateo */
   funcion *lista_funcion ;
   salida *lista_salida ;
   pre_condicion_salida *pre_condicion ;
   variable *lista_variable_local ;

} reformateo ;

void ImprimeEntradaSalida( entrada_salida * ) ;
entrada_salida *LeeEspecEntradaSalida( FILE *, formato * ) ;
void InicializaReformateos( reformateo *, int ) ;
void LeeEspecReformateos( FILE *, reformateo *, entrada_salida *, formato * ) ;
void GeneraArchivoReformateos_h( reformateo, entrada_salida *, char * ) ;
void GeneraArchivoReformateos_c( reformateo, char * ) ;
