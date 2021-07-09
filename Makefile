#
# CTC S.A.
# Area Facturacion Filiales
# Corresp Sistema de Saldos de Corresponsalias
# gen_ref version 2.1 Copyright octubre 1998 Pablo Lopez
#
# Makefile
#

FUENTES= gen_ref.c definici.c \
         constant.c mensaje.c tabla.c bibliote.c \
         formato.c ent_sal.c funcion.c \
         reformat.c make.c

OBJETOS= gen_ref.o definici.o \
         constant.o mensaje.o tabla.o bibliote.o \
         formato.o ent_sal.o funcion.o \
         reformat.o make.o

EJECUTABLE= gen_ref

all: gen_ref

clean:
	@rm -f $(EJECUTABLE) $(OBJETOS) > /dev/null

gen_ref.o: $(FUENTES)
	$(CC) -c $(FUENTES)

gen_ref: $(OBJETOS)
	$(CC) -o $(EJECUTABLE) $(OBJETOS)
	cp $(EJECUTABLE) $(DIR_CORRESP)/bin
