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

// retorna 1 se thread estiver na fila (SUCCESS)
// retorna 0 caso contrário
int threadIsInFila(int tid, PFILA2 fila) {
  TCB_t *thread;
  FirstFila2(fila);

  do {
    if(fila->it == 0) {
      break;
    }

    thread = (TCB_t *)GetAtIteratorFila2(fila);

    if(thread->tid == tid) {
      return TRUE;
    }
  } while(NextFila2(fila) == 0);

  return FALSE;
}

// ------------------- funções da api -------------------

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

  newContext->uc_link = scheduler; // contexto a executar no término
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

  PFILA2 pFilaSem = malloc(sizeof(PFILA2));
  CreateFila2(pFilaSem);

  //*sem = malloc(sizeof(csem_t));

  sem->count = 1; //semáforo começa livre
  sem->fila = pFilaSem;


  if(sem){ // não é nulo, sucesso
    return 0;
  } // erro
  else return -1;
}

  // *** Sincronização de Término ***

int cjoin(int tid){
  /* A filaWaited e a filaBloqueados(é a fila do semáforo!!!!!!!!!!!)
   deve ser criado em outro local...
  Além disso, a fila do semáforo deverá desbloquear a thread bloqueada via
  cjoin aqui mesmo. */
  PFILA2 filaWaited = malloc(sizeof(PFILA2));
  CreateFila2 (filaWaited);

  // testar se a tid passada existe(e está executando)
  if(tid < 0){
    return -1; // tid menor que zero não existe, erro
  }
  else { // caso seja >=0, tid existe

    if (FirstFila2(filaWaited) != tid){ /* testar se a thread da tid não está
      sendo esperada por nenhuma outra thread */

        while(GetAtIteratorFila2(filaWaited) != tid) { /* Iteração sobre a fila
          de "Esperados" até terminarem. */

          if (GetAtIteratorFila2(filaWaited) == tid){ /* tid já está na fila waited,  WARNING COMPRAÇÃO ENTRE POINTER E INTEGER, arrumada com o &
            quem chamou cjoin nesta tid deverá continuar executando normalmente. */
            return -1;
          }

          NextFila2(filaWaited);
        }
      }
      /* Se tid não está na fila, ele deve ser adicionado, e a thread que
      chamou a cjoin poderá ser bloqueada e esperar pelo término da thread
      passada como argumento. */
      NODE2 *pNovoTid = malloc(sizeof(NODE2));
      pNovoTid->ant = NULL;
      pNovoTid->next = NULL;
      pNovoTid->node = tid;
      AppendFila2(filaWaited, pNovoTid); /* não sei se pode ser só "tid", na support.pdf
      diz que para int AppendFila2(PFILA2 pFila, void *content), content deve ser um
      novo item e deve ser alocado dinamicamente da estrutura "sFilaNode2" */

        /* FALTA: criar um sistema que quando a thread esperada(que está na filaWaited)
        acabe, a thread bloqueada(que chamou) seja desbloqueada. Além disso, deve ser
        feito um free no nodo que estava na filaWaited, e na thread bloqueada que
        estava na fila bloqueada.
        */
      return 0;
    }
    return -1;

    /* É necessário um mecanismo para que as threads que estão sendo executadas
    e que queiram fazer cjoin sejam inseridas em uma estrutura de dado, podendo
    ser uma fila qualquer, de modo que um if's possamser checados:
    1:  se a thread que fez cjoin já não fez cjoin antes(não é permitido
     realizar duas chamadas de cjoin, nem para a mesma thread, nem para uma
     diferente (ou seja, não se pode esperar mais de uma thread, mesmo que
     a chamada de cjoin seja para a mesma thread feito antes)
        senão, pode fazer cjoin, e na própria função de cjoin será checado se
        a tid passada como argumento já não está sendo esperada ou não existe,
        caso a resposta para essas duas perguntas seja não, a thread que fez
        cjoin para a tid x, entrará para o estado bloqueado até que x conclua
        seu trabalho.
    */
  }

/* Fim da edição */

// cwait
// Parâmetro: *sem, um ponteiro para uma variável do tipo semáforo.
// Retorno: Quando executada corretamente: retorna SUCCESS (0) Caso contrário, retorna ERROR (-1).

int cwait (csem_t *sem) {
  sem->count--;

  // se sem está livre:
  if (sem->count >= 0) {
    // tem que fazer algo mais aqui?

    return SUCCESS;
  }

  // se sem está ocupado:
  else {
    // ponteiro pra thread executando
    TCB_t* thread = malloc(sizeof(TCB_t));

    // --> se tivermos uma fila com todas threads:
    // thread = getThreadFromTid(scheduler->executing);

    // --> ou, caso scheduler->executing seja um ponteiro pra um TCB_t:
    // thread = scheduler->executing;

    // adiciona thread executando na fila de threads bloqueadas do sem
    AppendFila2(sem->fila, thread);

    // adiciona thread executando na fila de blocked
    AppendFila2(scheduler->blocked, thread);

    // vai pro dispatcher pegar nova thread pra executar e segue o baile

    return SUCCESS;
  }
}
