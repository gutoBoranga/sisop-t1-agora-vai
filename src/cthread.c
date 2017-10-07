#define _XOPEN_SOURCE 600 // essa linha é só pra funcionar no mac


#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <support.h>
#include <cthread.h>
#include <cdata.h>

#define NAMES "Augusto Boranga\nLucas Assis\nOctávio Arruda\n"
#define NAMES_SIZE 45
#define ARGC 1

#define MAIN_THREAD_TID 0
#define PRIORITY 0

SCHEDULER_t *scheduler; // acho que isso nao precisa ser um ponteiro, mas depois a gente se preocupa com isso

void list_able() {

  FirstFila2(scheduler->able); // iterador no primeiro elemento
	TCB_t *thread = (TCB_t*) GetAtIteratorFila2(scheduler->able);
  printf("tid: %d\n", thread->tid);
}
	 
  

int csched_init() {

  scheduler = malloc(sizeof(SCHEDULER_t));
  scheduler->able = malloc(sizeof(PFILA2));
  scheduler->blocked = malloc(sizeof(PFILA2));
  
  if (scheduler) {
    scheduler->executing = -1; // ninguem executando
    CreateFila2(scheduler->able); // filas vazias
    CreateFila2(scheduler->blocked); 
    scheduler->count = 0; // ninguem escalonado ainda

	TCB_t *thread;
	thread = malloc(sizeof(TCB_t));
	thread->tid = 99;
	AppendFila2(scheduler->able, thread);
    return 0;    
  }

  else {
    return 1;
  }
}


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

  TCB_t *newThread;
  newThread = malloc(sizeof(TCB_t));

  newThread->state = PROCST_CRIACAO;
  newThread->prio = PRIORITY;

  int tid = scheduler->count;


  if (scheduler->count == 0) {

    ucontext_t *newContext;
    getcontext(newContext);

    newThread->tid = MAIN_THREAD_TID;
    newThread->context = *newContext;
  }

  else {  // aqui é se for outra thread
    ucontext_t *newContext;
    makecontext(newContext, (void (*)(void))start, ARGC, arg);  // cast pra funçao void* sem argumento
    newThread->tid = tid;
    newThread->context = *newContext;
  }

  scheduler->count++;
	printf("vai bugar no append\n");
	AppendFila2(scheduler->able, newThread);  
list_able();
  // coloca na fila de aptos


  return tid;
}




/*int cyield(void);
int cjoin(int tid);
int csem_init(csem_t *sem, int count);
int cwait(csem_t *sem);
int csignal(csem_t *sem);
*/

/* Editado por Octavio Arruda
  *** Inicialização do semáforo ***
*/

/*int csem_init(csem_t *sem, int count){

  PFILA2 pFilaSem;
  CreateFila2 (pFilaSem); // Cria a fila para o semáforo

  sem = malloc(sizeof(csem_t));

  sem->count = 1; //semáforo começa livre
  sem->fila = pFilaSem;

  if(check_malloc(sizeof(sem)) == NULL){ // retorna null quando houve erro
    return -1;
  } // se não houve erro, a região foi alocada corretamente
  else return 0;

}

/* Fim da edição */

