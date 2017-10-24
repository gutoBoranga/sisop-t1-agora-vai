#include <stdio.h>
#include <cthread.h>
#include <cdata.h>


void teste(void) {

  printf("printou do teste!!!\n");
  cyield();
}

int main() {


  void *(*fPointer)(void *) = &teste;
  void *x;
  ccreate(fPointer, x, 0);
  // ccreate(fPointer, x, 0);
  
  // printf("ccreate ok\n");
  //list_threads(ABLE_QUEUE);
  // printf("list_able ok\n");
  cyield();
  // printf("voltou pra main!!!\n");
  // cyield();
  printf("\n\n> Até mais e obrigado pelos peixes\n\n\n");
  
  return 0;
}
