#include <stdio.h>
#include <cthread.h>
#include <cdata.h>


void teste(void) {

  printf("entrou no teste\n");
  cyield();
  printf("voltou pro teste deopis do yield\n");
  cyield();
  printf("terminando o teste!!!\n");
  //cyield();
  //list_threads(ABLE_QUEUE);
}

int main() {


  void *(*fPointer)(void *) = &teste;
  void *x;
  ccreate(fPointer, x, 0);
  ccreate(fPointer, x, 0);

  // ccreate(fPointer, x, 0);
  //ccreate(fPointer, x, 0);
  /*ccreate(fPointer, x, 0);
  ccreate(fPointer, x, 0);
  ccreate(fPointer, x, 0);
  */
  // printf("ccreate ok\n");
  //list_threads(ABLE_QUEUE);
  // printf("list_able ok\n");
  cyield();
  printf("voltou pra main!!!\n");
  cyield();
  printf("voltou de novo!!\n");
  
  return 0;
}
