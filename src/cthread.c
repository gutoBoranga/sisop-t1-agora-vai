#define _XOPEN_SOURCE 600 // essa linha é só pra funcionar no mac


#include <stdio.h>
#include <ucontext.h>
#include <support.h>
#include <cthread.h>

#define NAMES "Augusto Boranga\nLucas Assis\nOctávio Arruda\n"
#define NAMES_SIZE 45
#define ARGC 1

#define MAIN_THREAD_TID 0
#define CREATION_STATE 0
#define ABLE_STATE 1
#define EXECUTING_STATE 2
#define BLOCKED_STATE 3
#define DONE_STATE 4
#define PRIORITY 0

typedef struct s_TCB {  // isso deve ser definido aqui????
  int tid;
  int state;
  int prio;
  ucontext_t context;
} TCB_t;


int cidentify (char *name, int size) {
  
  int i = 0;
  
  printf("Esse trabalho foi desenvolvido pelos alunos: \n");
  while (i < size) {
    
    if (name+i != NULL)
      printf("%c", name[i]);
    
    else
      return -1;
    
    i++;
  }
  
  return 0;
}


int ccreate (void* (*start)(void*), void *arg, int prio) {
  
  
  if (0) { // só pra ja deixar identado. aqui é se for a main
    ucontext_t *newContext;
    getcontext(newContext);
    
    TCB_t *mainThread;
    mainThread->tid = MAIN_THREAD_TID;
    mainThread->state = CREATION_STATE;
    mainThread->prio = PRIORITY;
    mainThread->context = *newContext;
  }
  
  else {  // aqui é se for outra thread
    ucontext_t *newContext;
    makecontext(newContext, (void (*)(void))start, ARGC, arg);  // cast pra funçao void* sem argumento
    
    TCB_t *newThread;
    //newThread->tid = ultimo tid atribuido + 1
    newThread->state = CREATION_STATE;
    newThread->prio = PRIORITY;
    newThread->context = *newContext;
  }
  
  // coloca na fila de aptos

  
  return 0;
}




/*int cyield(void);
int cjoin(int tid);
int csem_init(csem_t *sem, int count);
int cwait(csem_t *sem);
int csignal(csem_t *sem);
*/


int main() {
  
  //cidentify(NAMES, NAMES_SIZE);
  
  return 0;
}
