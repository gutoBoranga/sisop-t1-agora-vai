#include <cthread.h>
#include <cdata.h>


void teste(void) {
}

int main() {


  printf("cshched_init ok\n");
  void *(*fPointer)(void *) = &teste;
  void *x;
  ccreate(fPointer, x, 0);
  ccreate(fPointer, x, 0);
  printf("ccreate ok\n");
  list_able();
  printf("list_able ok\n");

  return 0;
}

