#
# Makefile de EXEMPLO
#
# OBRIGATÓRIO ter uma regra "all" para geração da biblioteca e de uma
# regra "clean" para remover todos os objetos gerados.
#
# É NECESSARIO ADAPTAR ESSE ARQUIVO de makefile para suas necessidades.
#  1. Cuidado com a regra "clean" para não apagar o "support.o"
#
# OBSERVAR que as variáveis de ambiente consideram que o Makefile está no diretótio "cthread"
#

all: clean compile_to_object_file generate_static_library

compile_to_object_file:
	gcc -c src/cthread.c -I include -o bin/cthread.o

generate_static_library:
	ar crs lib/libcthread.a bin/cthread.o bin/support.o

clean:
	cp bin/support.o lib/
	rm -rf bin/*
	cp lib/support.o bin/
	rm -rf lib/*

test:
	gcc -g testes/main.c -I include -L lib -lcthread -o testes/main
	./testes/main
#valgrind --leak-check=yes ./testes/main
