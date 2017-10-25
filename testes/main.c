#include <stdio.h>
#include <cthread.h>
#include <cdata.h>


void teste(void) {
  printf("\n\n> printou do teste!!!\n");
  int i = 0;
  cyield();
  printf("voltou pro teste\n");
  cyield();
  printf("ultima vez no teste\n");
  cjoin(1);
  printf("sera que faz um cjoin na ateste dos parceiro?");
}

void teste9(void) {
  printf("entrou no teste\n");
}

void ateste(void){
  /* testezinho pra fazer 2 yield e um cjoin em ninguém... */
  printf("se o cara fica dando cyield eu tenho que executar");
  cyield();
  int i = 0;
  for (i = 0; i< 1000000000000000; i++);

  printf("um loopzinho pra ele ficar serenity");
}

void terceira(void){
  cyield();
  printf(" terceira funcao teste ");
}


int main() {



  printf("ta na main\n");
  void *(*fPointer)(void *) = &teste9;
  void *x;

  void *(*aPointer)(void*) = &ateste; // tid = 1
  void *y;

  void *(*tPointer)(void*) = &terceira;
  void *z;

  ccreate(fPointer, x, 0);
  ccreate(aPointer, y, 0);
  ccreate(tPointer, z, 0);

  teste();


  cjoin(1);
  printf("dale\n");
  return 0;
}
