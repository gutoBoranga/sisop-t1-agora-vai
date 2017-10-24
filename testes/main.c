#include <stdio.h>
#include <cthread.h>
#include <cdata.h>


void teste(void) {
  printf("\n\n> printou do teste!!!\n");
  cyield();
  printf("voltou pro teste\n");
  cyield();
  printf("ultima vez no teste\n");
}

int main() {


  void *(*fPointer)(void *) = &teste;
  void *x;
  ccreate(fPointer, x, 0);
  ccreate(fPointer, x, 0);
  
  printf("\n\n> main depois de ccriar 2 thread maluka\n");
  cyield();
  printf("\n\n> voltou pra main!!!\n");
  cyield();
  printf("\n\n> Até mais e obrigado pelos peixes\n\n\n");
  
  return 0;
}
