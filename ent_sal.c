/*
 *
 * ent_sal.c
 *
 */
#include "definici.h"
#include "bibliote.h"
#include "formato.h"
#include "funcion.h"
#include "ent_sal.h"

static void EliminaUltimoRequisito( requisito **lista_requisito )
{
	requisito *p, *q ;

	p = *lista_requisito ;

	while ( p->siguiente != NULL )
	{
		q = p ;
		p = (requisito *)p->siguiente ;
	}

	if ( (*lista_requisito)->siguiente == NULL )
		*lista_requisito = NULL ;
	else
		q->siguiente = NULL ;

	free( p ) ;
}

static void EliminaUltimoCampo( campo_salida **lista_campo )
{
	campo_salida *p, *q ;

	p = *lista_campo ;

	while ( p->siguiente != NULL )
	{
		q = p ;
		p = (campo_salida *)p->siguiente ;
	}

	if ( (*lista_campo)->siguiente == NULL )
		*lista_campo = NULL ;
	else
		q->siguiente = NULL ;


	while ( p->lista_implementacion != NULL )
		EliminaUltimaImplementacion( &(p->lista_implementacion) ) ;

	free( p ) ;
}

static void EliminaUltimaSalida( salida **lista_salida )
{
	salida *p, *q ;

	p = *lista_salida ;

	while ( p->siguiente != NULL )
	{
		q = p ;
		p = (salida *)p->siguiente ;
	}

	if ( (*lista_salida)->siguiente == NULL )
		*lista_salida = NULL ;
	else
		q->siguiente = NULL ;


	while ( p->lista_requisito != NULL )
		EliminaUltimoRequisito( &(p->lista_requisito) ) ;

	while ( p->lista_campo != NULL )
		EliminaUltimoCampo( &(p->lista_campo) ) ;

	while ( p->lista_inicializacion != NULL )
		EliminaUltimaImplementacion( &(p->lista_inicializacion) ) ;

	free( p ) ;
}

static void EliminaUltimaPreCondicion( pre_condicion_salida **pre_condicion )
{
	pre_condicion_salida *p ;

	p = *pre_condicion ;

	while ( p->lista_implementacion != NULL )
			EliminaUltimaImplementacion( &(p->lista_implementacion) ) ;
}

void EliminaUltimaVariableLocal( variable **lista_variable_local )
{
	variable *p, *q ;

	p = *lista_variable_local ;

	while ( p->siguiente != NULL )
	{
		q = p ;
		p = (variable *)p->siguiente ;
	}

	if ( (*lista_variable_local)->siguiente == NULL )
		*lista_variable_local = NULL ;
	else
		q->siguiente = NULL ;

	free( p ) ;
}

void InicializaReformateos( reformateo *reformateos, int numero_entrada )
{
	pre_condicion_salida *pas ;

	reformateos->nombre_entrada[0] = '\0' ;
	reformateos->nombre_formato[0] = '\0' ;

	if ( numero_entrada == 1 ) /* primera vez */
	{
		reformateos->lista_error = NULL ;
		reformateos->lista_requisito = NULL ;
		reformateos->lista_funcion = NULL ;
		reformateos->lista_salida = NULL ;
		reformateos->lista_inicializacion = NULL ;

		pas = (pre_condicion_salida *)malloc( sizeof(pre_condicion_salida) ) ;
		pas->lista_implementacion = NULL ;
		reformateos->pre_condicion = pas ;

		reformateos->lista_variable_local = NULL ;
	}
	else
	{
		while ( reformateos->lista_error != NULL )
			EliminaUltimoRequisito( &(reformateos->lista_error) ) ;
  
		while ( reformateos->lista_requisito != NULL )
			EliminaUltimoRequisito( &(reformateos->lista_requisito) ) ;

		while ( reformateos->lista_funcion != NULL )
			EliminaUltimaFuncion( &(reformateos->lista_funcion) ) ;

		while ( reformateos->lista_salida != NULL )
			EliminaUltimaSalida( &(reformateos->lista_salida) ) ;

	  if ( (reformateos->pre_condicion)->lista_implementacion != NULL )
			EliminaUltimaPreCondicion( &(reformateos->pre_condicion) ) ;

	  while ( reformateos->lista_variable_local != NULL )
			EliminaUltimaVariableLocal( &(reformateos->lista_variable_local) ) ;

	  while ( reformateos->lista_inicializacion != NULL )
			EliminaUltimaImplementacion( &(reformateos->lista_inicializacion) ) ;
	}
}

static void ValidaSalida( entrada_salida *lista_entrada_salida, char *nombre_salida, char *nombre_formato, reformateo *reformateos, long numero_registro	 )
{
	entrada_salida *p ;

	Minuscula( nombre_salida ) ;
	p = lista_entrada_salida ;

	while ( p != NULL )
	{
		if ( (strcmp(nombre_salida,p->nombre) == 0) &&
		((p->tipo == _S) || (p->tipo == _E_S)) )
		{
	 strcpy( nombre_formato, p->formato ) ;
	 break ;
		}

		p = (entrada_salida *)p->siguiente ;
	}

	if ( p == NULL )
	{
		fprintf( stderr, "Error: '%s.esp', linea %ld. ValidaSalida(...) Salida '%s' no valida.\n", reformateos->nombre_entrada, numero_registro, nombre_salida ) ;
		exit( 1 ) ;
	}
}

static void ValidaCampoSalida( entrada_salida *lista_entrada_salida, formato *lista_formatos, char *nombre_salida_actual, char *nombre_campo, char *nombre_tipo, reformateo *reformateos, long numero_registro )
{
	formato *f ;
	campo	  *c ;
	entrada_salida *s ;

	s = lista_entrada_salida ;

	while ( s != NULL )
	{
		if ( strcmp(nombre_salida_actual,s->nombre) == 0 )
	  break ;
		else
	  s = (entrada_salida *)s->siguiente ;
	}

	if ( s == NULL )
	{
		fprintf( stderr, "Error: '%s.esp', linea %ld. ValidaCampoSalida(...) Salida '%s' no encontrada.\n", reformateos->nombre_entrada, numero_registro, nombre_salida_actual ) ;
		exit( 1 ) ;
	}

	f = lista_formatos ;
	while ( f != NULL )
	{
		if ( strcmp(s->formato,f->nombre) == 0 )
	 break ;
		else
	 f = (formato *)f->siguiente ;
	}

	if ( f == NULL )
	{
		fprintf( stderr, "Error: '%s.esp', linea %ld. ValidaCampoSalida(...) Formato '%s' no encontrado.\n", reformateos->nombre_entrada, numero_registro, s->formato ) ;
		exit( 1 ) ;
	}

	c = f->campos ;

	while ( c != NULL )
	{
		if ( strcmp(nombre_campo,c->nombre) == 0 )
	 break ;
		else
	 c = (campo *)c->siguiente ;
	}

	if ( c == NULL )
	{
		fprintf( stderr, "Error: '%s.esp', linea %ld. ValidaCampoSalida(...) Campo '%s' no valido para formato '%s'.\n", reformateos->nombre_entrada, numero_registro, nombre_campo, s->formato ) ;
		exit( 1 ) ;
	}
	else
		strcpy( nombre_tipo, c->tipo ) ;
}

static void AgregaVariableLocal( char *nombre_archivo, variable **lista_variable_local, char *nombre_identificador, char *nombre_tipo, long numero_registro )
{
	variable *p, *q ;

	q = (variable *)malloc( sizeof(variable) ) ;
	if ( q == NULL )
	{
		fprintf( stderr, "Error: AgregaVariableLocal(...) Insuficiente memoria para cargar informacion.\n" ) ;
		exit( -1 ) ;
	}

	strcpy( q->nombre, nombre_identificador ) ;
	strcpy( q->tipo, nombre_tipo ) ;
	q->siguiente = NULL ;

	p = *lista_variable_local ;

	if ( p == NULL ) /* lista nueva de variables */
		*lista_variable_local = q ;
	else
	{
		if ( strcmp(p->nombre,nombre_identificador) == 0 )
		{
	  fprintf( stderr, "Error: '%s.esp', linea %ld. AgregaVariableLocal(...) Doble especificacion de variable '%s'.\n", nombre_archivo, numero_registro, nombre_identificador ) ;
			 free( q ) ;
			 exit( -1 ) ;
		}

		while ( p->siguiente != NULL )
		{
			p = (variable *)p->siguiente ;
			if ( strcmp(p->nombre,nombre_identificador) == 0 )
			{
		 fprintf( stderr, "Error: '%s.esp', linea %ld. AgregaVariableLocal(...) Doble especificacion de variable '%s'.\n", nombre_archivo, numero_registro, nombre_identificador ) ;
				free( q ) ;
				exit( -1 ) ;
			}
		}

		p->siguiente = (struct variable *)q ;
	}
}

static void AgregaImplPreCondicion( reformateo *reformateos, char *registro )
{
	 pre_condicion_salida *pas ; 
	 implementacion *p, *q ;

	 pas = reformateos->pre_condicion ;

	 q = (implementacion *)malloc( sizeof(implementacion) ) ;
	 if ( q == NULL )
	 {
		 fprintf( stderr, "Error: AgregaImplPreCondicion(...) Insuficiente memoria para cargar informacion.\n" ) ;
		 exit( 1 ) ;
	 }

	 strcpy( q->registro, registro ) ;
	 q->siguiente = NULL ;

	 if ( pas->lista_implementacion == NULL ) /* lista nueva de implementaciones */
		 pas->lista_implementacion = q ;
	 else
	 {
		 p = pas->lista_implementacion ;

		 while ( p->siguiente != NULL )
	  p = (implementacion *)p->siguiente ;

		 p->siguiente = (struct implementacion *)q ;
	 }
}

static void AgregaImplCampo( reformateo *reformateos, char *registro, char *nombre_salida_actual, char *nombre_campo_actual, long numero_registro )
{
	implementacion *p, *q ;
	campo_salida *cs ;
	salida *s ;

	s = reformateos->lista_salida ;

	while ( (s != NULL) && (strcmp(nombre_salida_actual,s->nombre) != 0) )
		s = (salida *)s->siguiente ;

	if ( s == NULL )
	{
		fprintf( stderr, "Error: '%s.esp', linea %ld. AgregaImplCampo(...) Salida '%s' no encontrada.\n", reformateos->nombre_entrada, numero_registro, nombre_salida_actual ) ;
		exit( 1 ) ;
	}
	else
	{
		cs = s->lista_campo ;

		while ( (cs != NULL) && (strcmp(nombre_campo_actual,cs->nombre) != 0) )
	 cs = (campo_salida *)cs->siguiente ;

		if ( cs == NULL )
		{
	 fprintf( stderr, "Error: '%s.esp', linea %ld. AgregaImplCampo(...) Campo '%s' no encontrado.\n", reformateos->nombre_entrada, numero_registro, nombre_campo_actual ) ;
	 exit( 1 ) ;
		}
		else
		{
	 q = (implementacion *)malloc( sizeof(implementacion) ) ;
	 if ( q == NULL )
	 {
		 fprintf( stderr, "Error: AgregaArgumentoFuncion(...) Insuficiente memoria para cargar informacion.\n" ) ;
		 exit( 1 ) ;
	 }

	 strcpy( q->registro, registro ) ;
	 q->siguiente = NULL ;

	 if ( cs->lista_implementacion == NULL ) /* lista nueva de implementaciones */
		 cs->lista_implementacion = q ;
	 else
	 {
		 p = cs->lista_implementacion ;

		 while ( p->siguiente != NULL )
	  p = (implementacion *)p->siguiente ;

		 p->siguiente = (struct implementacion *)q ;
	 }
		}
	}
}

static void AgregaCondError( reformateo *reformateos, char *registro )
{
	requisito *p, *q ;

	q = (requisito *)malloc( sizeof(requisito) ) ;
	if ( q == NULL )
	{
		fprintf( stderr, "Error: AgregaCondError(...) Insuficiente memoria para cargar informacion.\n" ) ;
		exit( 1 ) ;
	}

	strcpy( q->registro, registro ) ;
	q->siguiente = NULL ;

	p = reformateos->lista_error ;

	if ( p == NULL ) /* lista nueva de requisitos */
		reformateos->lista_error = q ;
	else
	{
		while ( p->siguiente != NULL )
	 p = (requisito *)p->siguiente ;

		p->siguiente = (struct requisito *)q ;
	}
}

static void AgregaCondSobrante( reformateo *reformateos, char *registro )
{
	requisito *p, *q ;

	q = (requisito *)malloc( sizeof(requisito) ) ;
	if ( q == NULL )
	{
		fprintf( stderr, "Error: AgregaCondSobrante(...) Insuficiente memoria para cargar informacion.\n" ) ;
		exit( 1 ) ;
	}

	strcpy( q->registro, registro ) ;
	q->siguiente = NULL ;

	p = reformateos->lista_requisito ;

	if ( p == NULL ) /* lista nueva de requisitos */
		reformateos->lista_requisito = q ;
	else
	{
		while ( p->siguiente != NULL )
	 p = (requisito *)p->siguiente ;

		p->siguiente = (struct requisito *)q ;
	}
}

static void AgregaCondSalida( reformateo *reformateos, char *registro, char *nombre_salida_actual, long numero_registro )
{
	requisito *p, *q ;
	salida *s ;

	s = reformateos->lista_salida ;

	while ( (s != NULL) && (strcmp(s->nombre,nombre_salida_actual) != 0) )
		s = (salida *)s->siguiente ;

	if ( s == NULL )
	{
		fprintf( stderr, "Error: '%s.esp', linea %ld. AgregaCondSalida(...) Salida '%s' no encontrada.\n", reformateos->nombre_entrada, numero_registro, nombre_salida_actual ) ;
		exit( 1 ) ;
	}
	else
	{
		q = (requisito *)malloc( sizeof(requisito) ) ;
		if ( q == NULL )
		{
	 fprintf( stderr, "Error: AgregaCondSalida(...) Insuficiente memoria para cargar informacion.\n" ) ;
	 exit( 1 ) ;
		}

		strcpy( q->registro, registro ) ;
		q->siguiente = NULL ;

		p = s->lista_requisito ;

		if ( p == NULL ) /* lista nueva de requisitos */
	 s->lista_requisito = q ;
		else
		{
	 while ( p->siguiente != NULL )
		 p = (requisito *)p->siguiente ;

	 p->siguiente = (struct requisito *)q ;
		}
	}
}

static void AgregaSalidaReformateo( reformateo *reformateos, char *nombre_identificador, char *nombre_formato, int condicion_terminal, long numero_registro )
{
	salida *p, *q ;

	q = (salida *)malloc( sizeof(salida) ) ;
	if ( q == NULL )
	{
		fprintf( stderr, "Error: AgregaSalidaReformateo(...) Insuficiente memoria para cargar informacion.\n" ) ;
		exit( 1 ) ;
	}

	strcpy( q->nombre, nombre_identificador ) ;
	strcpy( q->nombre_formato, nombre_formato ) ;
	q->lista_requisito = NULL ;
	q->condicion_terminal = condicion_terminal ;
	q->lista_campo = NULL ;
	q->lista_inicializacion = NULL ;
	q->siguiente = NULL ;

	p = reformateos->lista_salida ;

	if ( p == NULL ) /* lista nueva de salidas */
		reformateos->lista_salida = q ;
	else
	{
		if ( strcmp(p->nombre,nombre_identificador) == 0 )
		{
	 fprintf( stderr, "Error: '%s.esp', linea %ld. AgregaSalidaReformateo(...) Doble especificacion de salida '%s'.\n", reformateos->nombre_entrada, numero_registro, nombre_identificador ) ;
	 free( q ) ;
	 exit( 1 ) ;
		}

		while ( p->siguiente != NULL )
	p = (salida *)p->siguiente ;

		p->siguiente = (struct salida *)q ;
	}
}

static void AgregaCampoSalida( reformateo *reformateos, char *nombre_identificador, char *nombre_tipo, char *nombre_salida_actual, long numero_registro )
{
	campo_salida *p, *q ;
	salida *s ;

	s = reformateos->lista_salida ;

	while ( (s != NULL) && (strcmp(s->nombre,nombre_salida_actual) != 0) )
		s = (salida *)s->siguiente ;

	if ( s == NULL )
	{
		fprintf( stderr, "Error: '%s.esp', linea %ld. AgregaCampoSalida(...) Salida '%s' no encontrada.\n", reformateos->nombre_entrada, numero_registro, nombre_salida_actual ) ;
		exit( 1 ) ;
	}
	else
	{
		q = (campo_salida *)malloc( sizeof(campo_salida) ) ;
		if ( q == NULL )
		{
	 fprintf( stderr, "Error: AgregaCampoSalida(...) Insuficiente memoria para cargar informacion.\n" ) ;
	 exit( 1 ) ;
		}

		strcpy( q->nombre, nombre_identificador ) ;
		strcpy( q->tipo, nombre_tipo ) ;
		q->lista_implementacion = NULL ;
		q->siguiente = NULL ;

		p = s->lista_campo ;

		if ( p == NULL ) /* lista nueva de campos */
	 s->lista_campo = q ;
		else
		{
	 if ( strcmp(p->nombre,nombre_identificador) == 0 )
	 {
		 fprintf( stderr, "Error: '%s.esp', linea %ld. AgregaCampoSalida(...) Doble especificacion de campo '%s' para salida '%s'.\n", reformateos->nombre_entrada, numero_registro, nombre_identificador, nombre_salida_actual ) ;
		 free( q ) ;
		 exit( 1 ) ;
	 }

	 while ( p->siguiente != NULL )
		 p = (campo_salida *)p->siguiente ;

	 p->siguiente = (struct campo_salida *)q ;
		}
	}
}

static void AgregaInic( reformateo *reformateos, char *registro )
{
	implementacion *p, *q ;

	q = (implementacion *)malloc( sizeof(implementacion) ) ;
	if ( q == NULL )
	{
		fprintf( stderr, "Error: AgregaInic(...) Insuficiente memoria para cargar informacion.\n" ) ;
		exit( 1 ) ;
	}

	strcpy( q->registro, registro ) ;
	q->siguiente = NULL ;

	if ( reformateos->lista_inicializacion == NULL ) /* lista nueva de inicializaciones */
		reformateos->lista_inicializacion = q ;
	else
	{
		p = reformateos->lista_inicializacion ;

		while ( p->siguiente != NULL )
	 p = (implementacion *)p->siguiente ;

		p->siguiente = (struct implementacion *)q ;
	}
}

static void AgregaInicSalida( reformateo *reformateos, char *registro, char *nombre_salida_actual, long numero_registro )
{
	implementacion *p, *q ;
	salida *s ;

	s = reformateos->lista_salida ;

	while ( (s != NULL) && (strcmp(nombre_salida_actual,s->nombre) != 0) )
		s = (salida *)s->siguiente ;

	if ( s == NULL )
	{
		fprintf( stderr, "Error: '%s.esp', linea %ld. AgregaInicSalida(...) Salida '%s' no encontrada.\n", reformateos->nombre_entrada, numero_registro, nombre_salida_actual ) ;
		exit( 1 ) ;
	}
	else
	{
		q = (implementacion *)malloc( sizeof(implementacion) ) ;
		if ( q == NULL )
		{
	 fprintf( stderr, "Error: AgregaInicSalida(...) Insuficiente memoria para cargar informacion.\n" ) ;
	 exit( 1 ) ;
		}

		strcpy( q->registro, registro ) ;
		q->siguiente = NULL ;

		if ( s->lista_inicializacion == NULL ) /* lista nueva de inicializaciones */
		{
	 s->lista_inicializacion = q ;
		}
		else
		{
	 p = s->lista_inicializacion ;

	 while ( p->siguiente != NULL )
		 p = (implementacion *)p->siguiente ;

	 p->siguiente = (struct implementacion *)q ;
		}
	}
}

void ImprimeEntrada_Salida( entrada_salida *lista_entrada_salida )
{
	entrada_salida *p ;

	p = lista_entrada_salida ;

	while ( p != NULL )
	{
		fprintf( stdout, "EntradaSalida->nombre= '%s'\n", p->nombre ) ;
		fprintf( stdout, "             ->formato= '%s'\n", p->formato ) ;
		fprintf( stdout, "             ->tipo= %d\n", p->tipo ) ;
		p = (entrada_salida *)p->siguiente ;
	}
}

entrada_salida *LeeEspecEntradaSalida( FILE *arch_esp_entrada_salida, formato *lista_formatos )
{
	long numero_registro=0L ;
	entrada_salida *p, *q, *lista_entrada_salida ;
	int tipo ;
	char registro[BUFFER_LARGO] ;

	lista_entrada_salida = NULL ;

	while ( LeeRegistro(arch_esp_entrada_salida,registro) )
	{
		numero_registro++ ;

		tipo = 0 ;
		switch( registro[0] )
		{
	 case 'e':
	 case 'E':

		 if ( (registro[1] == '/') || (registro[1] == '-') || (registro[1] == '_'))
		 {
			 if ( (registro[2] != 's') && (registro[2] != 'S') )
			 {
		  fprintf( stderr, "Error: 'ent_sal.esp', linea %ld. LeeEspecEntradaSalida(...) Sintaxis incorrecta.\n", numero_registro  ) ;
		  exit( 1 ) ;
			 }
			 else
		  tipo = _E_S ; /* E/S */
		 }
		 else if ( (registro[1] == ' ') || (registro[1] == '\t') )
			 tipo = _E ; /* E */
		 else
		 {
			 fprintf( stderr, "Error: 'ent_sal.esp', linea %ld. LeeEspecEntradaSalida(...) Sintaxis incorrecta.\n", numero_registro	 ) ;
			 exit( 1 ) ;
		 }

	 break ;
	 case 's':
	 case 'S':

		 if ( (registro[1] == ' ') || (registro[1] == '\t') )
			 tipo = _S ; /* S */
		 else
		 {
			 fprintf( stderr, "Error: 'ent_sal.esp', linea %ld. LeeEspecEntradaSalida(...) Sintaxis incorrecta.\n", numero_registro	 ) ;
			 exit( 1 ) ;
		 }
	 case '\n':
	 case '#':
		 /* comentarios */
	 break;
	 default:
		 fprintf( stderr, "Error: 'ent_sal.esp', linea %ld. LeeEspecEntradaSalida(...) Sintaxis incorrecta.\n", numero_registro	 ) ;
		 exit( 1 ) ;
		} /* switch */

		if ( tipo != 0 )
		{
	 q = (entrada_salida *)malloc( sizeof(entrada_salida) ) ;
	 if ( tipo == _E_S )
	 {
		 sscanf( registro+3, "%s %s \n", q->nombre,
						 q->formato ) ;
	 }
	 else
	 {
		 sscanf( registro+2, "%s %s \n", q->nombre,
						 q->formato ) ;
	 }

	 Minuscula( q->nombre ) ;
	 Mayuscula( q->formato ) ;
	 q->tipo = tipo ;

	 if ( !EsFormatoValido(q->formato,lista_formatos) )
	 {
		 fprintf( stderr, "Error: 'ent_sal.esp', linea %ld. LeeEspecEntradaSalida(...) Formato '%s' no definido.\n", numero_registro, q->formato ) ;
		 exit( 1 ) ;
	 }

	 q->siguiente = NULL ;

	 if ( lista_entrada_salida == NULL ) /* lista nueva de entrada salida */
		 lista_entrada_salida = q ;
	 else
	 {
		 p = lista_entrada_salida ;
		 while ( (p->siguiente != NULL) && (strcmp(q->nombre,p->nombre) != 0) )
			 p = (entrada_salida *)p->siguiente ;

		 if ( strcmp(q->nombre,p->nombre) == 0 )
		 {
	  fprintf( stderr, "Error: 'ent_sal.esp', linea %ld. LeeEspecEntradaSalida(...) Doble especificacion de entrada salida.\n", numero_registro	) ;
			 exit( 1 ) ;
		 }

		 p->siguiente = (struct entrada_salida *)q ;
	 }
		}
	} /* while */

	return( lista_entrada_salida ) ;
}

void LeeEspecReformateos( FILE *arch_esp_ref_entrada, reformateo *reformateos, entrada_salida *lista_entrada_salida, formato *lista_formatos )
{
	char registro[BUFFER_LARGO] ;
	char nombre_identificador[NOMBRE_IDENTIFICADOR_LARGO+1] ;
	char tipo_identificador[NOMBRE_TIPO_LARGO+1] ;
	char nombre_tipo[NOMBRE_TIPO_LARGO+1] ;
	char nombre_formato[NOMBRE_FORMATO_LARGO+1] ;
	char nombre_funcion_actual[NOMBRE_FUNCION_LARGO+1] ;
	char nombre_salida_actual[NOMBRE_ENTRADA_SALIDA_LARGO+1] ;
	char nombre_campo_actual[NOMBRE_CAMPO_LARGO+1] ;
	int  condicion_terminal ;
	int  en_funcion=FALSE ;
	int  en_entrada=FALSE ;
	int  en_salida=FALSE ;
	int  en_campo=FALSE ;
	int  en_esp_funcion=FALSE ;
	int  en_esp_arg_funcion=FALSE ;
	int  en_esp_impl_funcion=FALSE ;
	int  en_esp_cond_error=FALSE ;
	int  en_esp_cond_sobrante=FALSE ;
	int  en_esp_salida=FALSE ;
	int  en_esp_cond_salida=FALSE ;
	int  en_esp_campo=FALSE ;
	int  en_esp_pre_condicion=FALSE ;
	long numero_registro ;

	numero_registro = 0L ;
	while ( LeeRegistro(arch_esp_ref_entrada,registro) )
	{
		numero_registro++ ;

		switch( registro[0] )
		{
	 case 'v':
	 case 'V':
		 if ( en_funcion				||
		 en_esp_pre_condicion ||
		 en_entrada				 ||
		 (registro[1] != ' ' && registro[1] != '\t') )
		 {
			 fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Sintaxis incorrecta.\n", reformateos->nombre_entrada, numero_registro  ) ;
			 exit( -1 ) ;
		 }

		 if ( NumeroPalabras(registro+2) < 2 )
		 {
			 fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Se esperan a lo menos dos argumentos.\n", reformateos->nombre_entrada, numero_registro	 ) ;
			 exit( -1 ) ;
		 }

		 sscanf( registro+2, "%s %s \n", nombre_identificador, tipo_identificador ) ;
		 ValidaTipo( reformateos->nombre_entrada, tipo_identificador, numero_registro, nombre_tipo ) ;

		 AgregaVariableLocal( reformateos->nombre_entrada, &(reformateos->lista_variable_local), nombre_identificador, nombre_tipo, numero_registro ) ;
	 break;
	case 'f':
	case 'F':
		 if ( en_esp_funcion			||
		 en_esp_arg_funcion	 ||
		 en_esp_pre_condicion ||
		 en_entrada				 ||
		 (registro[1] != ' ' && registro[1] != '\t') )
		 {
	  fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Sintaxis incorrecta.\n", reformateos->nombre_entrada, numero_registro	) ;
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
	  fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Se esperan a lo menos dos argumentos.\n", reformateos->nombre_entrada, numero_registro  ) ;
	  exit( 1 ) ;
		 }

		 sscanf( registro+2, "%s %s \n", nombre_identificador, tipo_identificador ) ;
		 ValidaTipo( reformateos->nombre_entrada, tipo_identificador, numero_registro, nombre_tipo ) ;

		 AgregaFuncion( reformateos->nombre_entrada, &(reformateos->lista_funcion), nombre_identificador, nombre_tipo, numero_registro ) ;

		 strcpy( nombre_funcion_actual, nombre_identificador ) ;

	 break;
	 case 'a':
	 case 'A':
		 if ( !en_funcion			  ||
		 en_esp_impl_funcion ||
		 (registro[1] != ' ' && registro[1] != '\t') )
		 {
	  fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Sintaxis incorrecta.\n", reformateos->nombre_entrada, numero_registro	) ;
	  exit( 1 ) ;
		 }
		 else
		 {
	  en_esp_arg_funcion = TRUE ;
	  en_esp_funcion = FALSE ;
		 }

		 if ( NumeroPalabras(registro+2) < 2 )
		 {
	  fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Se esperan a lo menos dos argumentos.\n", reformateos->nombre_entrada, numero_registro  ) ;
	  exit( 1 ) ;
		 }

		 sscanf( registro+2, "%s %s \n", nombre_identificador, tipo_identificador ) ;

		 ValidaTipo( reformateos->nombre_entrada, tipo_identificador, numero_registro, nombre_tipo ) ;

		 AgregaArgumentoFuncion( reformateos->nombre_entrada, reformateos->lista_funcion, nombre_identificador, nombre_tipo, nombre_funcion_actual, numero_registro ) ;

	 break;
	 case 'i':
	 case 'I':
	if ( (registro[1] == 'n') ||
		  (registro[1] == 'N') )
	{
	  if (  !en_entrada ||
		en_esp_cond_error ||
		en_esp_cond_sobrante ||
                en_campo ||
                (registro[2] != ' ' && registro[2] != '\t') )
          {
fprintf( stderr, "en_entrada= '%d'.\n", en_entrada ) ;
fprintf( stderr, "en_esp_cond_error= '%d'.\n", en_esp_cond_error ) ;
fprintf( stderr, "en_esp_cond_sobrante= '%d'.\n", en_esp_cond_sobrante ) ;
fprintf( stderr, "en_esp_cond_salida= '%d'.\n", en_esp_cond_salida ) ;
fprintf( stderr, "en_campo= '%d'.\n", en_campo ) ;
		  fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Sintaxis incorrecta.\n", reformateos->nombre_entrada, numero_registro	) ;
		  exit( 1 ) ;
	  }
	  else
	  {
		  if ( en_salida ) /* inicializacion salida */
		  {
		en_esp_salida = FALSE ;
		AgregaInicSalida( reformateos, registro+2, nombre_salida_actual, numero_registro ) ;
		  }
		  else /* inicializacion reformateo */
		  {
		AgregaInic( reformateos, registro+2 ) ;
		  }
	  }
		 }
		 else
		 {
	  if ( !en_funcion &&
			 !en_campo )
	  {
		  fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Sintaxis incorrecta.\n", reformateos->nombre_entrada, numero_registro	) ;
		  exit( 1 ) ;
	  }
	  else
	  {
		  if ( en_funcion ) /* implementacion funcion */
		  {
		en_esp_funcion = FALSE ;
		en_esp_arg_funcion = FALSE ;
		en_esp_impl_funcion = TRUE ;
		  }
		  else /* implementacion campo */
		  {
		en_esp_campo = FALSE ;
		  }
	  }

	  if ( en_esp_impl_funcion )
		  AgregaImplFuncion( reformateos->nombre_entrada, reformateos->lista_funcion, registro+1, nombre_funcion_actual, numero_registro ) ;
	  else
		  AgregaImplCampo( reformateos, registro+1, nombre_salida_actual, nombre_campo_actual, numero_registro ) ;
		 }

	 break;
	 case 'p':
	 case 'P':
		if (	 en_esp_funcion		 ||
		  en_esp_arg_funcion	  ||
		  en_entrada			  ||
		  (registro[1] != ' ' && registro[1] != '\t') )
		 {
	  fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Sintaxis incorrecta.\n", reformateos->nombre_entrada, numero_registro	) ;
	  exit( 1 ) ;
		 }
		 else
		 {
	  en_funcion = FALSE ;
	  en_esp_impl_funcion = FALSE ;
	  en_esp_pre_condicion = TRUE ;
		 }

		 AgregaImplPreCondicion( reformateos, registro+1 ) ;

	break ;
	 case 'e':
	 case 'E':
		 if ( en_esp_funcion			||
		 en_esp_arg_funcion	 ||
		 en_entrada				 ||
		 (registro[1] != ' ' && registro[1] != '\t') )
		 {
	  fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Sintaxis incorrecta.\n", reformateos->nombre_entrada, numero_registro	) ;
			 exit( 1 ) ;
		 }
		 else
		 {
	  en_funcion = FALSE ;
	  en_esp_impl_funcion = FALSE ;
	  en_esp_pre_condicion = FALSE ;
	  en_entrada = TRUE ;
		 }

		 if ( NumeroPalabras(registro+2) < 1 )
		 {
	  fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Se espera a lo menos un argumentos.\n", reformateos->nombre_entrada, numero_registro	) ;
			 exit( 1 ) ;
		 }

		 sscanf( registro+2, "%s \n", nombre_identificador ) ;

		 if( strcmp( Minuscula(nombre_identificador), reformateos->nombre_entrada) != 0	)
		 {
	  fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Nombre de entrada '%s' no coincide con archivo de especificación.\n", reformateos->nombre_entrada, numero_registro, nombre_identificador ) ;
			 exit( 1 ) ;
		 }

	 break ;

	 case 'b':
	 case 'B':
		 if (	 !en_entrada  ||
		  en_esp_cond_sobrante ||
		  en_salida ||
		  (registro[1] != ' ' && registro[1] != '\t') )
		 {
	  fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Sintaxis incorrecta.\n", reformateos->nombre_entrada, numero_registro	) ;
	  exit( 1 ) ;
		 }
		 else
		 {
	  en_esp_cond_error = TRUE ;
		 }

		 AgregaCondError( reformateos, registro+1 ) ;
		 break ;

	 case 'r':
	 case 'R':
		 if ( !en_entrada	 ||
		 en_campo	||
		 (registro[1] != ' ' && registro[1] != '\t') )
		 {
	  fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Sintaxis incorrecta.\n", reformateos->nombre_entrada, numero_registro	) ;
	  exit( 1 ) ;
		 }
		 else
		 {
	  if ( en_salida) /* Requisito Salida */
	  {
		  en_esp_salida = FALSE ;
		  en_esp_cond_salida = TRUE ;
	  }
	  else  /* Requisito Sobrante */
	  {
			en_esp_cond_error = FALSE ;
			en_esp_cond_sobrante = TRUE ;
	  }
		 }

		 if ( en_esp_cond_salida )
	  AgregaCondSalida( reformateos, registro+1, nombre_salida_actual, numero_registro ) ;
		 else
	  AgregaCondSobrante( reformateos, registro+1 ) ;

	 break ;
	 case 's':
	 case 'S':
		 if (	 !en_entrada			 ||
		  en_esp_salida		  ||
		  en_esp_campo			  ||
		 (registro[1] != ' ' && registro[1] != '\t') )
		 {
			 fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Sintaxis incorrecta.\n", reformateos->nombre_entrada, numero_registro  ) ;
			 exit( 1 ) ;
		 }
		 else
		 {
			 en_esp_cond_error = FALSE ;
			 en_esp_cond_sobrante = FALSE ;
			 en_campo = FALSE ;
			 en_salida = TRUE ;
			 en_esp_salida = TRUE ;
		 }

		 if ( NumeroPalabras(registro+2) < 2 )
		 {
			 fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Se esperan a lo menos dos parametros.\n", reformateos->nombre_entrada, numero_registro	 ) ;
			 exit( 1 ) ;
		 }

		 sscanf( registro+2, "%s %d \n", nombre_identificador, &condicion_terminal ) ;

		 ValidaSalida( lista_entrada_salida, nombre_identificador, nombre_formato, reformateos, numero_registro ) ;

		 AgregaSalidaReformateo( reformateos, nombre_identificador, nombre_formato, condicion_terminal, numero_registro ) ;

		 strcpy( nombre_salida_actual, nombre_identificador ) ;

		 break ;
	 case 'c':
	 case 'C':
		 if ( !en_salida				 ||
		  en_esp_campo			  ||
		 (registro[1] != ' ' && registro[1] != '\t') )
		 {
	  fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Sintaxis incorrecta.\n", reformateos->nombre_entrada, numero_registro	) ;
	  exit( 1 ) ;
		 }
		 else
		 {
	  en_esp_salida = FALSE ;
          en_esp_cond_salida = FALSE ;
	  en_campo = TRUE ;
	  en_esp_campo = TRUE ;
		 }

		 sscanf( registro+2, "%s \n", nombre_identificador ) ;

		 ValidaCampoSalida( lista_entrada_salida, lista_formatos, nombre_salida_actual, nombre_identificador, nombre_tipo, reformateos, numero_registro ) ;

		 AgregaCampoSalida( reformateos, nombre_identificador, nombre_tipo, nombre_salida_actual, numero_registro ) ;

		 strcpy( nombre_campo_actual, nombre_identificador ) ;

	 break ;
	 case '\n':
	 case '#':
		 /* comentarios */
	 break;
	 default:
		 fprintf( stderr, "Error: '%s.esp', linea %ld. LeeEspecReformateos(...) Sintaxis incorrecta.\n", reformateos->nombre_entrada, numero_registro  ) ;
		 exit( 1 ) ;

		} /* switch */
	} /* while */
}

void GeneraArchivoReformateos_h( reformateo reformateos, entrada_salida *lista_entrada_salida, char *directorio_salida )
{
	char nom_arch_entrada_h[NOMBRE_ARCHIVO_LARGO+1] ;
	FILE *arch_entrada_h ;

	salida *p ;
	entrada_salida *e ;

	fprintf( stdout, "Generando archivo '%s.h'...\n\n", reformateos.nombre_entrada ) ;

#ifdef MSDOS
	sprintf( nom_arch_entrada_h, "%s\\%s.h", directorio_salida, reformateos.nombre_entrada ) ;
#else
	sprintf( nom_arch_entrada_h, "%s/%s.h", directorio_salida, reformateos.nombre_entrada ) ;
#endif
	CreaArchivo( &arch_entrada_h, nom_arch_entrada_h ) ;

	fprintf( arch_entrada_h, "/*\n *   %s.h\n *\n */\n\n", reformateos.nombre_entrada ) ;
	fprintf( arch_entrada_h, "typedef struct contador {\n\n\tlong registros_%s ;\n\tlong registros_error ;\n\tlong registros_sobrante ;\n", reformateos.nombre_entrada ) ;
	fprintf( arch_entrada_h, "\tlong registros_cumple_cond_sob ;\n\tlong registros_error_pre_cond ;\n\tlong registros_sin_salida ;\n" ) ;

	p = reformateos.lista_salida ;
	while ( p != NULL )
	{
		fprintf( arch_entrada_h, "\tlong registros_%s_OK ;\n\tlong registros_%s_NOK ;\n", p->nombre, p->nombre ) ;
		p = (salida *)p->siguiente ;
	}

	fprintf( arch_entrada_h, "\n} contador ;\n\n" ) ;

	e = lista_entrada_salida ;
	while ( e != NULL )
	{
		if ( (e->tipo == _E) || (e->tipo ==_E_S) )
	 fprintf( arch_entrada_h, "int Reformatea_%s( argumento *, archivo * ) ;\n", e->nombre ) ;

		e = (entrada_salida *)e->siguiente ;
	}
}

void GeneraArchivoReformateos_c( reformateo reformateos, char *directorio_salida )
{
	char nom_arch_entrada_c[NOMBRE_ARCHIVO_LARGO+1] ;
	char registro_actual[BUFFER_LARGO] ;
	int genera_error ;
	int numero_argumento ;
	FILE *arch_entrada_c ;

	funcion *f ;
	argumento_funcion *a ;
	implementacion *i ;
	requisito *r ;
	salida *s ;
	campo_salida *c ;
	pre_condicion_salida *pas ;
	variable *v ;

	fprintf( stdout, "Generando archivo '%s.c'...\n\n", reformateos.nombre_entrada ) ;

#ifdef MSDOS
	sprintf( nom_arch_entrada_c, "%s\\%s.c", directorio_salida, reformateos.nombre_entrada ) ;
#else
	sprintf( nom_arch_entrada_c, "%s/%s.c", directorio_salida, reformateos.nombre_entrada ) ;
#endif
	CreaArchivo( &arch_entrada_c, nom_arch_entrada_c ) ;

	fprintf( arch_entrada_c, "/*\n *   %s.c\n *\n */\n\n", reformateos.nombre_entrada ) ;
	fprintf( arch_entrada_c, "#include \"definici.h\"\n" ) ;
	fprintf( arch_entrada_c, "#include \"bibliote.h\"\n" ) ;
	fprintf( arch_entrada_c, "#include \"funcion.h\"\n" ) ;
	fprintf( arch_entrada_c, "#include \"formato.h\"\n" ) ;
	fprintf( arch_entrada_c, "#include \"reformat.h\"\n" ) ;
	fprintf( arch_entrada_c, "#include \"%s.h\"\n\n", reformateos.nombre_entrada ) ;

	/* variables locales */
	if ( reformateos.lista_variable_local != NULL ) 
	{
		fprintf( arch_entrada_c, "static struct variable_local {\n\n" ) ;

		v = reformateos.lista_variable_local ;
		while ( v != NULL )
		{

			if ( EsIgual(v->tipo,"char *") )
				fprintf( arch_entrada_c, "\tchar %s[VARIABLE_LOCAL_LARGO+1] ;\n", v->nombre ) ;
			else
				fprintf( arch_entrada_c, "\t%s%s ;\n", v->tipo, v->nombre ) ;

			v = (variable *)v->siguiente ;
		}

		fprintf( arch_entrada_c, "\n} variable_local ;\n\n" ) ;
	}

	/* funciones */
	f = reformateos.lista_funcion ;
	while ( f != NULL )
	{
		numero_argumento = 0 ;

		fprintf( arch_entrada_c, "static %s%s(", f->tipo, f->nombre ) ;

		/* argumentos */
		a = f->lista_argumento ;
		while ( a != NULL )
		{
	 numero_argumento++ ;
	 if ( a->siguiente != NULL )
		 fprintf( arch_entrada_c, " %s%s,", a->tipo, a->nombre ) ;
	 else
		 fprintf( arch_entrada_c, " %s%s )\n{\n", a->tipo, a->nombre ) ;

	 a = (argumento_funcion *)a->siguiente ;
		}

		if ( numero_argumento == 0 )
	 fprintf( arch_entrada_c, " void )\n{\n" ) ;

		/* implementaciones */
		i = f->lista_implementacion ;
		while ( i != NULL )
		{
	 fprintf( arch_entrada_c, "%s", i->registro ) ;

	 i = (implementacion *)i->siguiente ;
		}

		fprintf( arch_entrada_c, "}\n\n" ) ;

		f = (funcion *)f->siguiente ;
	}

	fprintf( arch_entrada_c, "static void InicializaContadores( contador *contadores )\n{\n" ) ;
	fprintf( arch_entrada_c, "\tcontadores->registros_%s=0L ;\n", reformateos.nombre_entrada ) ;
	fprintf( arch_entrada_c, "\tcontadores->registros_error=0L ;\n" ) ;
	fprintf( arch_entrada_c, "\tcontadores->registros_sobrante=0L ;\n" ) ;
	fprintf( arch_entrada_c, "\tcontadores->registros_cumple_cond_sob=0L ;\n" ) ;
	fprintf( arch_entrada_c, "\tcontadores->registros_error_pre_cond=0L ;\n" ) ;
	fprintf( arch_entrada_c, "\tcontadores->registros_sin_salida=0L ;\n" ) ;

	s = reformateos.lista_salida ;
	while ( s != NULL )
	{
		fprintf( arch_entrada_c, "\tcontadores->registros_%s_OK=0L ;\n\tcontadores->registros_%s_NOK=0L ;\n", s->nombre, s->nombre ) ;

		s = (salida *)s->siguiente ;
	}
	fprintf( arch_entrada_c, "}\n\n" ) ;

	fprintf( arch_entrada_c, "static void GeneraEstadisticas( FILE *archivo_estadisticas, contador contadores )\n{\n" ) ;
	fprintf( arch_entrada_c, "\tfprintf( archivo_estadisticas, \"%%-18s\\t%%8ld\\t%%8ld\\n\", \"%s\", contadores.registros_%s, 0L ) ;\n", reformateos.nombre_entrada, reformateos.nombre_entrada	 ) ;
	fprintf( arch_entrada_c, "\tfprintf( archivo_estadisticas, \"%%-18s\\t%%8ld\\t%%8ld\\n\", \"error\", contadores.registros_error, 0L ) ;\n" ) ;
	fprintf( arch_entrada_c, "\tfprintf( archivo_estadisticas, \"%%-18s\\t%%8ld\\t%%8ld\\n\", \"sobrante\", contadores.registros_sobrante, 0L ) ;\n" ) ;
	fprintf( arch_entrada_c, "\tfprintf( archivo_estadisticas, \"%%-18s\\t%%8ld\\t%%8ld\\n\", \"cumple_cond_sob\", 0L, contadores.registros_cumple_cond_sob ) ;\n" ) ;
	fprintf( arch_entrada_c, "\tfprintf( archivo_estadisticas, \"%%-18s\\t%%8ld\\t%%8ld\\n\", \"error_pre_cond\", 0L, contadores.registros_error_pre_cond ) ;\n" ) ;
	fprintf( arch_entrada_c, "\tfprintf( archivo_estadisticas, \"%%-18s\\t%%8ld\\t%%8ld\\n\", \"sin_salida\", 0L, contadores.registros_sin_salida ) ;\n" ) ;

	s = reformateos.lista_salida ;
	while ( s != NULL )
	{
		fprintf( arch_entrada_c, "\tfprintf( archivo_estadisticas, \"%%-18s\\t%%8ld\\t%%8ld\\n\", \"%s\", contadores.registros_%s_OK, contadores.registros_%s_NOK ) ;\n", s->nombre, s->nombre, s->nombre ) ;

		s = (salida *)s->siguiente ;
	}
	fprintf( arch_entrada_c, "}\n\n" ) ;

	fprintf( arch_entrada_c, "static int CumpleCondicion_error( %s *registro_entrada )\n{\n", reformateos.nombre_formato ) ;
	registro_actual[0] = '\0' ;
	r = reformateos.lista_error ;
	while ( r != NULL )
	{
		fprintf( arch_entrada_c, "%s", r->registro ) ;
		if ( !EsLineaEnBlanco(r->registro) )
			strcpy( registro_actual, r->registro ) ;

		r = (requisito *)r->siguiente ;
	}

	if ( strstr(registro_actual,"return") == NULL )
		fprintf( arch_entrada_c, "\treturn( FALSE ) ; /* valor por defecto */\n" ) ;

	fprintf( arch_entrada_c, "}\n\n" ) ;

	fprintf( arch_entrada_c, "static int CumpleCondicion_sobrante( %s *registro_entrada )\n{\n", reformateos.nombre_formato ) ;
	registro_actual[0] = '\0' ;
	r = reformateos.lista_requisito ;
	while ( r != NULL )
	{
		fprintf( arch_entrada_c, "%s", r->registro ) ;
		if ( !EsLineaEnBlanco(r->registro) )
	 strcpy( registro_actual, r->registro ) ;

		r = (requisito *)r->siguiente ;
	}

	if ( strstr(registro_actual,"return") == NULL )
		fprintf( arch_entrada_c, "\treturn( FALSE ) ; /* valor por defecto */\n" ) ;

	fprintf( arch_entrada_c, "}\n\n" ) ;

	fprintf( arch_entrada_c, "static void InicReformateo( %s *registro_entrada )\n{\n", reformateos.nombre_formato ) ;
	i = reformateos.lista_inicializacion ;
	while ( i != NULL )
	{
		fprintf( arch_entrada_c, "%s", i->registro ) ;

		i = (implementacion *)i->siguiente ;
	}
	fprintf( arch_entrada_c, "}\n\n" ) ;

	s = reformateos.lista_salida ;
	while ( s != NULL )
	{
		/* requisitos */
		fprintf( arch_entrada_c, "static int CumpleCondicion_%s( %s *registro_entrada )\n{\n", s->nombre, reformateos.nombre_formato ) ;
		registro_actual[0] = '\0' ;
		r = s->lista_requisito ;
		while ( r != NULL )
		{
	 fprintf( arch_entrada_c, "%s", r->registro ) ;
	 if ( !EsLineaEnBlanco(r->registro) )
		 strcpy( registro_actual, r->registro ) ;

	 r = (requisito *)r->siguiente ;
		}

		if ( strstr(registro_actual,"return") == NULL )
	 fprintf( arch_entrada_c, "\treturn( FALSE ) ; /* valor por defecto */\n" ) ;


		fprintf( arch_entrada_c, "}\n\n" ) ;

		/* campos */
		c = s->lista_campo ;
		while ( c != NULL )
		{
	 fprintf( arch_entrada_c, "static %sObtieneCampo_%s%s( %s *registro_entrada, %s *registro_salida )\n{\n", c->tipo, s->nombre, c->nombre, reformateos.nombre_formato, s->nombre_formato ) ;

	 /* implementacion */
	 genera_error = FALSE ;
	 i = c->lista_implementacion ;
	 while ( i != NULL )
	 {
		 fprintf( arch_entrada_c, "%s", i->registro ) ;
		 if ( strstr(i->registro,"GeneraError") != NULL )
			 genera_error = TRUE ;

		 i = (implementacion *)i->siguiente ;
	 }

	 if ( genera_error )
	 {
		 fprintf( arch_entrada_c, "\n\tif ( HayError() )\n" ) ;
		 if ( strcmp(c->tipo,"int ") == 0 )
			 fprintf( arch_entrada_c, "\t\treturn( 0 ) ; /* valor por defecto */\n" ) ;
		 else if ( strcmp(c->tipo,"long ") == 0 )
			 fprintf( arch_entrada_c, "\t\treturn( 0L ) ; /* valor por defecto */\n" ) ;
                 else if ( strcmp(c->tipo,"float ") == 0 )
                         fprintf( arch_entrada_c, "\t\treturn( 0.0 ) ; /* valor por defecto */\n" ) ;
		 else if ( strcmp(c->tipo,"char *") == 0 )
			 fprintf( arch_entrada_c, "\t\treturn( \"\" ) ; /* valor por defecto */\n" ) ;
	 }

	 fprintf( arch_entrada_c, "}\n\n" ) ;

	 c = (campo_salida *)c->siguiente ;
		}

		if ( !EsIgual(s->nombre_formato,reformateos.nombre_formato) &&
			  !EsIgual(s->nombre_formato,"FMTO_ENTRADA") )
		{
			fprintf( arch_entrada_c, "static void InicSalida_%s( %s *registro_entrada, %s *registro_salida )\n{\n", s->nombre, reformateos.nombre_formato, s->nombre_formato ) ;
			i = s->lista_inicializacion ;
			while ( i != NULL )
			{
				fprintf( arch_entrada_c, "%s", i->registro ) ;

				i = (implementacion *)i->siguiente ;
			}

			fprintf( arch_entrada_c, "\n}\n\n" ) ;

			fprintf( arch_entrada_c, "static int Genera_%s( %s *registro_entrada, %s *registro_salida )\n{\n", s->nombre, reformateos.nombre_formato, s->nombre_formato ) ;
			c = s->lista_campo ;
			while ( c != NULL )
			{
                                     if ( (strcmp(c->tipo,"int ") == 0)   ||
					  (strcmp(c->tipo,"long ") == 0)  ||
                                          (strcmp(c->tipo,"float ") == 0) ||
					  (strcmp(c->tipo,"char ") == 0) )
					fprintf( arch_entrada_c, "\tregistro_salida->%s = ObtieneCampo_%s%s(registro_entrada,registro_salida) ;\n", c->nombre, s->nombre, c->nombre ) ;
				else if ( strcmp(c->tipo,"char *") == 0 )
					fprintf( arch_entrada_c, "\tstrcpy( registro_salida->%s, ObtieneCampo_%s%s(registro_entrada,registro_salida) ) ;\n", c->nombre, s->nombre, c->nombre ) ;

				fprintf( arch_entrada_c, "\n\tif ( HayError() || HaySobrante() )\n\t{\n" ) ;
				fprintf( arch_entrada_c, "\t\tSalidaCampoProcesado( \"%s\", \"%s\" ) ;\n\t\treturn( FALSE ) ;\n\t}\n\n", s->nombre, c->nombre ) ;

				fprintf( arch_entrada_c, "#ifdef DEBUG\n" ) ;

					if ( strcmp(c->tipo,"int ") == 0 )
						fprintf( arch_entrada_c, "\tfprintf( stderr, \"CAMPO: %s= '%%d'\\n\", registro_salida->%s ) ;\n", c->nombre, c->nombre ) ;
					else if ( strcmp(c->tipo,"long ") == 0 )
						fprintf( arch_entrada_c, "\tfprintf( stderr, \"CAMPO: %s= '%%ld'\\n\", registro_salida->%s ) ;\n", c->nombre, c->nombre ) ;
                                        else if ( strcmp(c->tipo,"float ") == 0 )
                                                fprintf( arch_entrada_c, "\tfprintf( stderr, \"CAMPO: %s= '%%f'\\n\", registro_salida->%s ) ;\n", c->nombre, c->nombre ) ;
					else if ( strcmp(c->tipo,"char ") == 0 )
						fprintf( arch_entrada_c, "\tfprintf( stderr, \"CAMPO: %s= '%%c'\\n\", registro_salida->%s ) ;\n", c->nombre, c->nombre ) ;
					else if ( strcmp(c->tipo,"char *") == 0 )
						fprintf( arch_entrada_c, "\tfprintf( stderr, \"CAMPO: %s= '%%s'\\n\", registro_salida->%s ) ;\n", c->nombre, c->nombre ) ;

				fprintf( arch_entrada_c, "#endif\n\n" ) ;

				c = (campo_salida *)c->siguiente ;
			}

			fprintf( arch_entrada_c, "\treturn( TRUE ) ;\n}\n\n" ) ;
		}

		s = (salida *)s->siguiente ;
	}

	fprintf( arch_entrada_c, "static void PreCondicionSalida( %s *registro_entrada )\n{\n", reformateos.nombre_formato ) ;
	pas = reformateos.pre_condicion ;

	/* implementacion */
	i = pas->lista_implementacion ;
	while ( i != NULL )
	{
		fprintf( arch_entrada_c, "%s", i->registro ) ;

		i = (implementacion *)i->siguiente ;
	}

	if ( reformateos.pre_condicion == NULL )
		fprintf( arch_entrada_c, "/* defecto */\n\t;\n" ) ;

	fprintf( arch_entrada_c, "}\n\n" ) ;

	fprintf( arch_entrada_c, "int Reformatea_%s( argumento *argumentos, archivo *archivos )\n{\n", reformateos.nombre_entrada ) ;

	fprintf( arch_entrada_c, "\t%s registro_entrada ;\n", reformateos.nombre_formato ) ;
        fprintf( arch_entrada_c, "\tint i, multiregistros, salida_ant, salida ;\n\tchar nombre_archivo[NOMBRE_ARCHIVO_LARGO+1] ;\n" ) ;

	s = reformateos.lista_salida ;
	while ( s != NULL )
	{
		fprintf( arch_entrada_c, "\tFILE *archivo_%s ;\n", s->nombre ) ;

		s = (salida *)s->siguiente ;
	}
	fprintf( arch_entrada_c, "\n" ) ;

	s = reformateos.lista_salida ;
	while ( s != NULL )
	{
		if ( !EsIgual(s->nombre_formato,reformateos.nombre_formato) &&
			  !EsIgual(s->nombre_formato,"FMTO_ENTRADA") )
                {
                   fprintf( arch_entrada_c, "\tint mmr_registro_%s=0 ;\n", s->nombre ) ;
                }

		s = (salida *)s->siguiente ;
	}
	fprintf( arch_entrada_c, "\n" ) ;	

	s = reformateos.lista_salida ;
	while ( s != NULL )
	{
		if ( !EsIgual(s->nombre_formato,reformateos.nombre_formato) &&
			  !EsIgual(s->nombre_formato,"FMTO_ENTRADA") )
		{
			fprintf( arch_entrada_c, "\t%s *pmem_registro_%s=NULL, *ptr_registro_%s, registro_%s ;\n", s->nombre_formato, s->nombre, s->nombre, s->nombre ) ;
		}

		s = (salida *)s->siguiente ;
	}
	fprintf( arch_entrada_c, "\n" ) ;
	
	fprintf( arch_entrada_c, "\tcontador contadores ;\n\n" ) ;

	s = reformateos.lista_salida ;
	while ( s != NULL )
	{
		fprintf( arch_entrada_c, "\t/* archivo de salida %s */\n", s->nombre ) ;
		fprintf( arch_entrada_c, "\tif ( argumentos->proceso_concurrente > 0 )\n\t{\n" ) ;
		fprintf( arch_entrada_c, "\t\tsprintf( nombre_archivo, \"%%s/%s.dat.%%d\", argumentos->directorio_temporal, argumentos->proceso_concurrente ) ;\n\t}\n", s->nombre ) ;
		fprintf( arch_entrada_c, "\telse\n\t{\n" ) ;
		fprintf( arch_entrada_c, "\t\tsprintf( nombre_archivo, \"%%s/%s.dat\", argumentos->directorio_temporal ) ;\n\t}\n", s->nombre ) ;
		fprintf( arch_entrada_c, "\tCreaArchivo( &archivo_%s, nombre_archivo ) ;\n\n", s->nombre ) ;

		s = (salida *)s->siguiente ;
	}

	fprintf( arch_entrada_c, "\tInicializaContadores( &contadores ) ;\n\n" ) ;
	fprintf( arch_entrada_c, "\twhile ( LeeForm%s(archivos->archivo_entrada,&registro_entrada) )\n\t{\n", reformateos.nombre_formato ) ;
	fprintf( arch_entrada_c, "\t\tcontadores.registros_%s++ ;\n\t\tmPrintPuntos( contadores.registros_%s, stdout ) ;\n", reformateos.nombre_entrada, reformateos.nombre_entrada ) ;

	fprintf( arch_entrada_c, "\n\t\tInicializaError() ;\n\t\tInicializaSobrante() ;\n\t\tInicializaSalida() ;\n\n" ) ;

	fprintf( arch_entrada_c, "\t\t/* error al leer registro entrada */\n" ) ;
	fprintf( arch_entrada_c, "\t\tif ( !SeGeneroRegistroEntrada() )\n\t\t{\n\t\t\tGeneraError( MSG_NO_SE_GENERO_REGISTRO ) ;\n\t\t}\n\n" ) ;

	fprintf( arch_entrada_c, "#ifdef DEBUG\n" ) ;
	fprintf( arch_entrada_c, "\tfprintf( stderr, \"\\n\\n>>>>>>>>>>>>>>>>>>>>\\n\" ) ;\n" ) ;
	fprintf( arch_entrada_c, "\tfprintf( stderr, \"numero de registro:\\n%%07ld\\n\\n\", contadores.registros_%s ) ;\n", reformateos.nombre_entrada ) ;
	fprintf( arch_entrada_c, "\tfprintf( stderr, \"registro de entrada:\\n\" ) ;\n" ) ;
	fprintf( arch_entrada_c, "\tEscFormENTRADA( stderr ) ;\n" ) ;
	fprintf( arch_entrada_c, "\tfprintf( stderr, \"\\nsalidas...\\n\" ) ;\n" ) ;
	fprintf( arch_entrada_c, "#endif\n\n" ) ;

	fprintf( arch_entrada_c, "\t\t/* error */\n" ) ;
	fprintf( arch_entrada_c, "\t\tif ( !HayError() )\n\t\t{\n\t\t\tif ( CumpleCondicion_error(&registro_entrada) )\n\t\t\t{\n\n" ) ;

	fprintf( arch_entrada_c, "#ifdef DEBUG\n" ) ;
	fprintf( arch_entrada_c, "\tfprintf( stderr, \"SALIDA: 'error', FORMATO: '%s'.\\n\\n\" ) ;\n", reformateos.nombre_formato ) ;
	fprintf( arch_entrada_c, "#endif\n\n" ) ;

	fprintf( arch_entrada_c, "\t\t\t\tif ( !HayError() )\n\t\t\t\t{\n" ) ;
	fprintf( arch_entrada_c, "\t\t\t\t\tGeneraError( MSG_CONDICION_ERROR ) ;\n\t\t\t\t}\n\t\t\t}\n\t\t}\n\n" ) ;

	fprintf( arch_entrada_c, "\t\t/* sobrante */\n" ) ;
	fprintf( arch_entrada_c, "\t\tif ( !HayError() )\n\t\t{\n\t\t\tif ( CumpleCondicion_sobrante(&registro_entrada) )\n\t\t\t{\n\n" ) ;

	fprintf( arch_entrada_c, "#ifdef DEBUG\n" ) ;
	fprintf( arch_entrada_c, "\tfprintf( stderr, \"SALIDA: 'sobrante', FORMATO: '%s'.\\n\\n\" ) ;\n", reformateos.nombre_formato ) ;
	fprintf( arch_entrada_c, "#endif\n\n" ) ;

	fprintf( arch_entrada_c, "\t\t\t\tif ( !HaySobrante() )\n\t\t\t\t{\n" ) ;
	fprintf( arch_entrada_c, "\t\t\t\t\tGeneraSobrante( MSG_CONDICION_SOBRANTE ) ;\n\t\t\t\t}\n\n" ) ;

	fprintf( arch_entrada_c, "\t\t\t\tSalidaCampoProcesado( \"cond_sobrante\", \"null\" ) ;\n" ) ;
	fprintf( arch_entrada_c, "\t\t\t\tcontadores.registros_cumple_cond_sob++ ;\n" ) ;
	fprintf( arch_entrada_c, "\t\t\t\tSeProdujoCondSobrante() ;\n\t\t\t}\n\t\t}\n\n" ) ;

	fprintf( arch_entrada_c, "\t\t/* inicializacion */\n" ) ;
	fprintf( arch_entrada_c, "\t\tif ( !HayError() && !HaySobrante() )\n\t\t{\n\t\t\tInicReformateo( &registro_entrada ) ;\n\t\t}\n\n" ) ;

	fprintf( arch_entrada_c, "\t\t/* salidas no-terminales */\n\n" ) ;

	s = reformateos.lista_salida ;
	while ( s != NULL )
	{
	
		if ( !(s->condicion_terminal) )
		{
		
			fprintf( arch_entrada_c, "\t\t/* %s */\n", s->nombre ) ;
			fprintf( arch_entrada_c, "\t\tif ( !HayError() && !HaySobrante() )\n\t\t{\n" ) ;
			fprintf( arch_entrada_c, "\t\t\tInicializaMultiRegistro() ;\n" ) ;
			fprintf( arch_entrada_c, "\t\t\tif ( CumpleCondicion_%s(&registro_entrada) && !HaySobrante() )\n\t\t\t{\n\n", s->nombre ) ;

			fprintf( arch_entrada_c, "#ifdef DEBUG\n" ) ;
			fprintf( arch_entrada_c, "\tfprintf( stderr, \"SALIDA: '%s', FORMATO: '%s'.\\n\\n\" ) ;\n", s->nombre, s->nombre_formato ) ;
			fprintf( arch_entrada_c, "#endif\n\n" ) ;

			if ( EsIgual(s->nombre_formato,reformateos.nombre_formato) ||
		 	     EsIgual(s->nombre_formato,"FMTO_ENTRADA") )
			{
				fprintf( arch_entrada_c, "\t\t\t\tAgregaSalida( ID_%s ) ;\n\t\t\t}\n", s->nombre ) ;
			}
			else
			{
			
				fprintf( arch_entrada_c, "\t\t\t\tif ( HayMultiRegistro() )\n\t\t\t\t{\n" ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\tmultiregistros = ObtenerMultiRegistros() ;\n" ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\tif ( mmr_registro_%s < multiregistros )\n\t\t\t\t\t{\n", s->nombre ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\tmmr_registro_%s = multiregistros ;\n\n", s->nombre ) ;
				
				fprintf( arch_entrada_c, "\t\t\t\t\t\tif ( pmem_registro_%s != NULL )\n\t\t\t\t\t\t{\n", s->nombre ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\tfree( pmem_registro_%s ) ; /* se libera memoria previamente asignada */\n\t\t\t\t\t\t}\n\n", s->nombre ) ;

				fprintf( arch_entrada_c, "\t\t\t\t\t\tpmem_registro_%s = (%s *)malloc( multiregistros * sizeof(%s) ) ;\n", s->nombre, s->nombre_formato, s->nombre_formato ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\tif ( pmem_registro_%s == NULL )\n\t\t\t\t\t\t{\n", s->nombre ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\tfprintf( stderr, \"Error: Reformatea_%s(...) Insuficiente memoria para procesar multiregistros (%%d).\\n\", multiregistros ) ;\n", reformateos.nombre_entrada ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\texit( -1 ) ;\n\t\t\t\t\t\t}\n" ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t}\n\t\t\t\t\tptr_registro_%s = pmem_registro_%s ;\n", s->nombre, s->nombre ) ;
            fprintf( arch_entrada_c, "\n\t\t\t\t}\n\t\t\t\telse\n\t\t\t\t{\n" ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\tmultiregistros = 1 ;\n" ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\tptr_registro_%s = &registro_%s ;\n\t\t\t\t}\n\n", s->nombre, s->nombre ) ;
				
				fprintf( arch_entrada_c, "\t\t\t\tfor ( i=0; i < multiregistros; i++ )\n\t\t\t\t{\n\n" ) ;
				fprintf( arch_entrada_c, "#ifdef DEBUG\n" ) ;
				fprintf( arch_entrada_c, "\tfprintf( stderr, \"Multiregistros: %%d/%%d.\\n\", i + 1, multiregistros ) ;\n" ) ;
				fprintf( arch_entrada_c, "#endif\n\n" ) ;				  

				fprintf( arch_entrada_c, "\t\t\t\t\tInicializa%s( ptr_registro_%s + i ) ;\n", s->nombre_formato, s->nombre ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\tMultiRegistroActual( i + 1 ) ;\n\n" ) ;

				fprintf( arch_entrada_c, "\t\t\t\t\tInicSalida_%s( &registro_entrada, ptr_registro_%s + i ) ;\n", s->nombre, s->nombre ) ;				 
				fprintf( arch_entrada_c, "\t\t\t\t\tGenera_%s( &registro_entrada, ptr_registro_%s + i ) ;\n\n", s->nombre, s->nombre ) ;
				
				fprintf( arch_entrada_c, "\t\t\t\t\tif ( !HaySobrante() )\n\t\t\t\t\t{\n" ) ;
                                fprintf( arch_entrada_c, "\t\t\t\t\t\tAgregaSalida( ID_%s ) ;\n\t\t\t\t\t}\n", s->nombre ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\telse\n\t\t\t\t\t{\n" ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\tif ( HayMultiRegistro() )\n\t\t\t\t\t\t{\n" ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\tEliminaSalida( ID_%s ) ;\n\t\t\t\t\t\t}\n\n", s->nombre ) ;
				
				fprintf( arch_entrada_c, "\t\t\t\t\t\tcontadores.registros_%s_NOK++ ;\n", s->nombre ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\tSeProdujoSalidaConSob() ;\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t}\n" ) ;
				
			}

			fprintf( arch_entrada_c, "\t\t\telse if ( HaySobrante() )\n\t\t\t{\n" ) ;
			fprintf( arch_entrada_c, "\t\t\t\tSalidaCampoProcesado( \"%s\", \"condicion\" ) ;\n", s->nombre ) ;
			fprintf( arch_entrada_c, "\t\t\t\tcontadores.registros_%s_NOK++ ;\n", s->nombre ) ;
			fprintf( arch_entrada_c, "\t\t\t\tSeProdujoSalidaConSob() ;\n\t\t\t}\n\t\t}\n\n" ) ;

		}

		s = (salida *)s->siguiente ;

	}
	
	fprintf( arch_entrada_c, "\t\t/* pre-condicion y salidas terminales */\n\n" ) ;	 

	fprintf( arch_entrada_c, "\t\tif ( !HayError() && !HaySobrante() )\n\t\t{\n" ) ;

	fprintf( arch_entrada_c, "\t\t\t/* pre-condicion */\n" ) ;
	fprintf( arch_entrada_c, "\t\t\tPreCondicionSalida( &registro_entrada ) ;\n\n" ) ;

	fprintf( arch_entrada_c, "\t\t\tif ( HaySobrante() )\n\t\t\t{\n" ) ;
	fprintf( arch_entrada_c, "\t\t\t\tSalidaCampoProcesado( \"pre_condicion\", \"null\" ) ;\n" ) ;
	fprintf( arch_entrada_c, "\t\t\t\tcontadores.registros_error_pre_cond++ ;\n" ) ;
	fprintf( arch_entrada_c, "\t\t\t\tSeProdujoPreCondConSob() ;\n\t\t\t}\n" ) ;

	fprintf( arch_entrada_c, "\t\t\telse /* salidas terminales */\n\t\t\t{\n" ) ;

	s = reformateos.lista_salida ;
	while ( s != NULL )
	{
	
		if ( s->condicion_terminal )
		{
		
			fprintf( arch_entrada_c, "\t\t\t\t/* %s */\n", s->nombre ) ;
			fprintf( arch_entrada_c, "\t\t\t\tif ( !HaySobrante() && !HayCondicionTerminal() )\n\t\t\t\t{\n" ) ;
			fprintf( arch_entrada_c, "\t\t\t\t\tInicializaMultiRegistro() ;\n" ) ;
			fprintf( arch_entrada_c, "\t\t\t\t\tif ( HayPreCondicionSalida(ID_%s) || (!HayPreCondicion() && CumpleCondicion_%s(&registro_entrada) && !HaySobrante()) )\n\t\t\t\t\t{\n\n", s->nombre, s->nombre, s->nombre ) ;

			fprintf( arch_entrada_c, "#ifdef DEBUG\n" ) ;
			fprintf( arch_entrada_c, "\tfprintf( stderr, \"SALIDA: '%s', FORMATO: '%s'.\\n\\n\" ) ;\n", s->nombre, s->nombre_formato ) ;
			fprintf( arch_entrada_c, "#endif\n\n" ) ;

			fprintf( arch_entrada_c, "\t\t\t\t\t\tEsCondicionTerminal() ;\n" ) ;

			if ( EsIgual(s->nombre_formato,reformateos.nombre_formato) ||
				  EsIgual(s->nombre_formato,"FMTO_ENTRADA") )
			{			 
				fprintf( arch_entrada_c, "\t\t\t\t\t\tAgregaSalida( ID_%s ) ;\n\t\t\t\t\t}\n", s->nombre ) ;				  
			}
			else
			{
			
				fprintf( arch_entrada_c, "\t\t\t\t\t\tif ( HayMultiRegistro() )\n\t\t\t\t\t\t{\n" ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\tmultiregistros = ObtenerMultiRegistros() ;\n" ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\tif ( mmr_registro_%s < multiregistros )\n\t\t\t\t\t\t\t{\n", s->nombre ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\t\tmmr_registro_%s = multiregistros ;\n\n", s->nombre ) ;
				
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\t\tif ( pmem_registro_%s != NULL )\n\t\t\t\t\t\t\t\t{\n", s->nombre ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\t\t\tfree( pmem_registro_%s ) ; /* se libera memoria previamente asignada */\n\t\t\t\t\t\t\t\t}\n\n", s->nombre ) ;

				fprintf( arch_entrada_c, "\t\t\t\t\t\t\t\tpmem_registro_%s = (%s *)malloc( multiregistros * sizeof(%s) ) ;\n", s->nombre, s->nombre_formato, s->nombre_formato ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\t\tif ( pmem_registro_%s == NULL )\n\t\t\t\t\t\t\t\t{\n", s->nombre ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\t\t\tfprintf( stderr, \"Error: Reformatea_%s(...) Insuficiente memoria para procesar multiregistros (%%d).\\n\", multiregistros ) ;\n", reformateos.nombre_entrada ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\t\t\texit( -1 ) ;\n\t\t\t\t\t\t\t\t}\n" ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\t}\n\t\t\t\t\t\t\tptr_registro_%s = pmem_registro_%s ;\n", s->nombre, s->nombre ) ;
            fprintf( arch_entrada_c, "\t\t\t\t\t\t}\n\t\t\t\t\t\telse\n\t\t\t\t\t\t{\n" ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\tmultiregistros = 1 ;\n" ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\tptr_registro_%s = &registro_%s ;\n\t\t\t\t\t\t}\n\n", s->nombre, s->nombre ) ;
				
				fprintf( arch_entrada_c, "\t\t\t\t\t\tfor ( i=0; i < multiregistros; i++ )\n\t\t\t\t\t\t{\n\n" ) ;
				fprintf( arch_entrada_c, "#ifdef DEBUG\n" ) ;
				fprintf( arch_entrada_c, "\tfprintf( stderr, \"Multiregistros: %%d/%%d.\\n\", i + 1, multiregistros ) ;\n" ) ;
				fprintf( arch_entrada_c, "#endif\n\n" ) ;				  

				fprintf( arch_entrada_c, "\t\t\t\t\t\t\tInicializa%s( ptr_registro_%s + i ) ;\n", s->nombre_formato, s->nombre ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\tMultiRegistroActual( i + 1 ) ;\n\n" ) ;

				fprintf( arch_entrada_c, "\t\t\t\t\t\t\tInicSalida_%s( &registro_entrada, ptr_registro_%s + i ) ;\n", s->nombre, s->nombre ) ;			  
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\tGenera_%s( &registro_entrada, ptr_registro_%s + i ) ;\n\n", s->nombre, s->nombre ) ;
				
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\tif ( !HaySobrante() )\n\t\t\t\t\t\t\t{\n" ) ;
            fprintf( arch_entrada_c, "\t\t\t\t\t\t\t\tAgregaSalida( ID_%s ) ;\n\t\t\t\t\t\t\t}\n", s->nombre ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\telse\n\t\t\t\t\t\t\t{\n" ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\t\tif ( HayMultiRegistro() )\n\t\t\t\t\t\t\t\t{\n" ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\t\t\tEliminaSalida( ID_%s ) ;\n\t\t\t\t\t\t\t\t}\n\n", s->nombre ) ;
				
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\t\tcontadores.registros_%s_NOK++ ;\n", s->nombre ) ;
				fprintf( arch_entrada_c, "\t\t\t\t\t\t\t\tSeProdujoSalidaConSob() ;\n\t\t\t\t\t\t\t}\n\t\t\t\t\t\t}\n\t\t\t\t\t}\n" ) ;

			}

			fprintf( arch_entrada_c, "\t\t\t\t\telse if ( HaySobrante() )\n\t\t\t\t\t{\n" ) ;
			fprintf( arch_entrada_c, "\t\t\t\t\t\tSalidaCampoProcesado( \"%s\", \"condicion\" ) ;\n", s->nombre ) ;
			fprintf( arch_entrada_c, "\t\t\t\t\t\tcontadores.registros_%s_NOK++ ;\n", s->nombre ) ;
			fprintf( arch_entrada_c, "\t\t\t\t\t\tSeProdujoSalidaConSob() ;\n\t\t\t\t\t}\n\t\t\t\t}\n\n" ) ;

		}

		s = (salida *)s->siguiente ;

	}
	
	fprintf( arch_entrada_c, "\t\t\t} /* salidas terminales */\n" ) ;
	fprintf( arch_entrada_c, "\t\t}\n\n" ) ;

	fprintf( arch_entrada_c, "\t\t/* generacion de salidas */\n" ) ;
	fprintf( arch_entrada_c, "\t\tif ( HayError() )\n\t\t{\n" ) ;	 
	fprintf( arch_entrada_c, "\t\t\tEscFormENTRADA( archivos->archivo_error ) ;\n" ) ;
	fprintf( arch_entrada_c, "\t\t\tEscribirMensajeError( archivos->archivo_error_msg ) ;\n\n" ) ;
	fprintf( arch_entrada_c, "\t\t\tcontadores.registros_error++ ;\n\t\t}\n" ) ;	 
	fprintf( arch_entrada_c, "\t\telse if ( HaySobrante() || !HaySalida() )\n\t\t{\n" ) ;
	fprintf( arch_entrada_c, "\t\t\tEscFormENTRADA( archivos->archivo_sobrante ) ;\n" ) ;
	fprintf( arch_entrada_c, "\t\t\tEscribirMensajeSobrante( archivos->archivo_sobrante_msg ) ;\n\n" ) ;
	fprintf( arch_entrada_c, "\t\t\tcontadores.registros_sobrante++ ;\n\n" ) ;
	
	fprintf( arch_entrada_c, "\t\t\tif ( !HaySalida() && !HayCondSobrante() && !HayPreCondConSob() && !HaySalidaConSob() )\n\t\t\t{\n" ) ;
	fprintf( arch_entrada_c, "\t\t\t\tcontadores.registros_sin_salida++ ;\n\t\t\t}\n\t\t}\n" ) ;
	fprintf( arch_entrada_c, "\t\telse\n\t\t{\n" ) ;
	fprintf( arch_entrada_c, "\t\t\tsalida_ant = 0 ;\n" ) ;
	fprintf( arch_entrada_c, "\t\t\ti = 0 ;\n" ) ;
	fprintf( arch_entrada_c, "\t\t\twhile ( ObtieneSalida(&salida) )\n\t\t\t{\n" ) ;
	fprintf( arch_entrada_c, "\t\t\t\tif ( salida == salida_ant )\n\t\t\t\t{\n" ) ;
	fprintf( arch_entrada_c, "\t\t\t\t\ti++ ;\n\t\t\t\t}\n" ) ;
	fprintf( arch_entrada_c, "\t\t\t\telse\n\t\t\t\t{\n" ) ;	  
	fprintf( arch_entrada_c, "\t\t\t\t\tsalida_ant = salida ;\n" ) ;
	fprintf( arch_entrada_c, "\t\t\t\t\ti = 0 ;\n\t\t\t\t}\n\n" ) ;

	fprintf( arch_entrada_c, "\t\t\t\tswitch( salida )\n\t\t\t\t{\n" ) ;

	s = reformateos.lista_salida ;
	while ( s != NULL )
	{
		fprintf( arch_entrada_c, "\t\t\t\t\tcase ID_%s:\n", s->nombre ) ;
		fprintf( arch_entrada_c, "\t\t\t\t\t\tcontadores.registros_%s_OK++ ;\n", s->nombre ) ;

		if ( EsIgual(s->nombre_formato,reformateos.nombre_formato) ||
			  EsIgual(s->nombre_formato,"FMTO_ENTRADA") )
			fprintf( arch_entrada_c, "\t\t\t\t\t\tEscFormENTRADA( archivo_%s ) ;\n", s->nombre ) ;
		else
			fprintf( arch_entrada_c, "\t\t\t\t\t\tEscForm%s( archivo_%s, ptr_registro_%s + i ) ;\n", s->nombre_formato, s->nombre, s->nombre ) ;
		fprintf( arch_entrada_c, "\t\t\t\t\t\tbreak ;\n\n" ) ;

		s = (salida *)s->siguiente ;
	}

	fprintf( arch_entrada_c, "\t\t\t\t\tdefault:\n" ) ;
	fprintf( arch_entrada_c, "\t\t\t\t\t\tfprintf( stderr, \"Error: Reformatea_%s(...) Salida no implementada.\\n\" ) ;\n", reformateos.nombre_entrada ) ;
	fprintf( arch_entrada_c, "\t\t\t\t\t\texit( 1 ) ;\n" ) ;
	fprintf( arch_entrada_c, "\t\t\t\t}\n\t\t\t}\n\t\t}\n\t}\n\n" ) ;
	fprintf( arch_entrada_c, "\tGeneraEstadisticas( archivos->archivo_estadisticas, contadores ) ;\n" ) ;
	fprintf( arch_entrada_c, "\treturn( 0 ) ;\n}\n" ) ;

}
