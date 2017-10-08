#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <support.h>
#include <cthread.h>
#include <cdata.h>

SCHEDULER_t *scheduler; // acho que isso nao precisa ser um ponteiro, mas depois a gente se preocupa com isso

void list_able() {

  TCB_t *current;

  if (FirstFila2(scheduler->able) == 0) {

    while (GetAtIteratorFila2(scheduler->able) != NULL) { // iterador no primeiro elemento
      current = (TCB_t*) GetAtIteratorFila2(scheduler->able);
      printf("tid: %d\n", current->tid);
      NextFila2(scheduler->able);
    }
  }

  else {
    printf("lista vazia ou invalida\n");
  }
}

void list_threads(int queue) {
  
  PFILA2 q;
  TCB_t *current;
  
  if (queue == ABLE_QUEUE) {
    q = scheduler->able;
  }
  
  else {
    q = scheduler->blocked;
  }
  
  if (FirstFila2(q) == 0) {
    
    while (GetAtIteratorFila2(q) != NULL) { // iterador no primeiro elemento
      current = (TCB_t*) GetAtIteratorFila2(q);
      printf("tid: %d\n", current->tid);
      NextFila2(q);
    }
  }
  
  else {
    printf("lista vazia ou invalida\n");
  }

  FirstFila2(q);  // volta pro inicio da fila
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
  
  if (scheduler == NULL) {  // se ainda nao inicializou o scheduler, manda bala
    csched_init();
  }
  
  if (scheduler->count == 0) { // aqui cria a thread da main

    ucontext_t *mainContext = malloc(sizeof(ucontext_t));
    TCB_t *mainThread;
    
    mainThread = malloc(sizeof(TCB_t));
    
    getcontext(mainContext);
    scheduler->mainContext = mainContext;

    mainThread->state = PROCST_CRIACAO;
    mainThread->prio = PRIORITY;
    mainThread->tid = MAIN_THREAD_TID;
    mainThread->context = *mainContext;

    
    AppendFila2(scheduler->able, mainThread); // isso aqui tem que virar uma chamada pro dispatcher!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! (eu acho kkkkk)
    scheduler->count++;
  }

  // aqui é a nova thread

  TCB_t *newThread = malloc(sizeof(TCB_t));
  ucontext_t *newContext = malloc(sizeof(ucontext_t));
  ucontext_t *currentContext = malloc(sizeof(ucontext_t));  // esse é o contexto atual
  
  getcontext(currentContext);
  getcontext(newContext);
  
  newContext->uc_link = scheduler->currentContext; // contexto a executar no término
  newContext->uc_stack.ss_sp = malloc(STACK); // endereço de início da pilha
  newContext->uc_stack.ss_size = STACK; // tamanho da pilha

  makecontext(newContext, (void (*)(void))start, ARGC, arg);  // cast pra funçao void* sem argumento

  newThread->state = PROCST_CRIACAO;
  newThread->prio = PRIORITY;
  newThread->tid = scheduler->count;
  newThread->context = *newContext;

  AppendFila2(scheduler->able, newThread); // isso aqui tem que virar uma chamada pro dispatcher!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  scheduler->count++;
  
  return newThread->tid;
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

int csem_init(csem_t *sem, int count){

  PFILA2 pFilaSem = malloc(sizeof(PFILA2)); // PFILA é um ponteiro, entao tu tem que alocar o espaço dele antes
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

