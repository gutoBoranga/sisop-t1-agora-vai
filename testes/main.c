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
}

void teste9(void) {
  printf("entrou no teste\n");
}

void ateste(void){
  print("ateste novo");
  cyield();
  print("oq? cyied, mas já vai? ah bom");
  cyield();
  print("demorei mas voltei rápido");
  cjoin(2);
}

int main() {



  printf("ta na main\n");
  void *(*fPointer)(void *) = &teste9;
  void *x;

  void *(*aPointer)(void*) = &ateste;
  void *y;

  ccreate(fPointer, x, 0);
  ccreate(aPointer, y, 0);
  cjoin(1);
  printf("dale\n");
  return 0;
}
