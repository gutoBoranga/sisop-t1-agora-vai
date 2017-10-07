#include <cthread.h>
#include <cdata.h>


void teste(void) {
}

int main() {


  csched_init();
  void *(*fPointer)(void *) = &teste;
  void *x;

  ccreate(fPointer, x, 0); 
//  list_able();

  return 0;
}

