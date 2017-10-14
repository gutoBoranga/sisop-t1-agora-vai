#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <support.h>
#include <cthread.h>
#include <cdata.h>


int dispatcherContextSet = 0;
SCHEDULER_t *scheduler; // acho que isso nao precisa ser um ponteiro, mas depois a gente se preocupa com isso
int voltandopramain = 0;

void list_threads(int queue) {

  PFILA2 q;
  PNODE2 currentNode;
  TCB_t *currentThread;

  if (queue == ABLE_QUEUE) {
    q = scheduler->able;
  }

  else {
    q = scheduler->blocked;
  }

  if (FirstFila2(q) == 0) {

    while (GetAtIteratorFila2(q) != NULL) { // iterador no primeiro elemento
      currentNode = (PNODE2) GetAtIteratorFila2(q);
      currentThread = (TCB_t*) currentNode->node;
      printf("tid: %d\n", currentThread->tid);
      NextFila2(q);
    }
  }

  else {
    printf("lista vazia ou invalida\n");
  }

  FirstFila2(q);  // volta pro inicio da fila
}


int dispatcher() {

  printf("entrando no dispatcher\n");

  if (!dispatcherContextSet) {

    printf("vai pegar o contexto do dispatcher!\n");
    ucontext_t dispatcherContext;
    getcontext(&dispatcherContext);

    // o contexto do dispatcher sempre vai apontar pra ca, entao
    // temos que verificar de novo pra só sair fora do dispatcher na 1a vez
    // if (!dispatcherContextSet) {

      printf("pegou o contexto do dispatcher\n");
      dispatcherContextSet = 1; // nao entra mais nesses lugares!!!
      scheduler->dispatcherContext = dispatcherContext;
      setcontext(&(scheduler->mainContext)); // volta pro contexto da main
      // }

      // else
      printf("voltou pro dispatcher mas ja tinha o contexto\n");
  }

  printf("começando a atividade normal do dispatcher\n");
  // para o contador de tempo // _______ CHAMANDO ATENÇAO PRA COISAS FALTANDO

  // estamos fingindo que a fila ja esta ordenada
  
  PFILA2 q = scheduler->able;
  PNODE2 queueNode;
  TCB_t *first, *previous;

  if (FirstFila2(q) == 0) { // conseguiu por o iterador no primeiro elemento da fila

    if (GetAtIteratorFila2(q) != NULL) { // tem alguem na fila pra pegar a cpu
      //NextFila2(q); // ==========> essa linha faz com que funcione (pq aí ele nao escalona pra main)
      queueNode = (PNODE2)GetAtIteratorFila2(q);

      previous = scheduler->executing; // thread que tava executando

      first = queueNode->node;
      scheduler->executing = first; // o em execuçao agora é o primeiro da fila
      DeleteAtIteratorFila2(q); // tira ele da lista de aptos, pq ta executando
      // começa o contador de tempo!!!!!!!!
      printf("o tid do first eh %d\n", first->tid);




      if (first->tid == 0) {
	printf("TA VOLTANDO PRA MAIN\n");
	voltandopramain = 1;
      }





      //setcontext(&(first->context)); // poe pra executar o malandro
      swapcontext(&(previous->context), &(first->context));
      return 0;
    }

    /*    else { // nao tem ningeum pra pegar a cpu -> só segue o baile mas reseta o tempo pq ele tentou
      // PARA O CONTADOR DE TEMPO
      // COMEÇA O CONTADOR DE TEMPO
      return 0;
      }*/ // ---------------------------> ACHO QUE ESSE ELSE NAO PRECISA MAS VOU DEIXAR
  }

  else { // nao conseguiu por o iterador no primeiro elemento da fila (?)
    return -1; // aí ta feio mesmo
  }
}

int csched_init() {


  printf("iniciando o scheduler!\n");

  scheduler = malloc(sizeof(SCHEDULER_t));
  scheduler->able = malloc(sizeof(PFILA2));
  scheduler->blocked = malloc(sizeof(PFILA2));

  if (scheduler) {
    CreateFila2(scheduler->able); // filas vazias
    CreateFila2(scheduler->blocked);
    scheduler->count = 0; // ninguem escalonado ainda

    PNODE2 threadNode = malloc(sizeof(PNODE2));
    TCB_t *mainThread = malloc(sizeof(TCB_t));
    ucontext_t mainContext;

    getcontext(&mainContext); // pega o contexto atual (que vai ser o da main)
    printf("veio pro contexto da main\n");

    if (!dispatcherContextSet) { // se ainda nao pegou o contexto do dispatcher vem pra ca

      printf("o contexto do dispatcher ainda nao foi colocado, entao bora\n");
      
      scheduler->mainContext = mainContext; // define ele como contexto principal no scheduler

      mainThread->state = PROCST_CRIACAO; // preenche as parada da thread 
      mainThread->prio = PRIORITY;
      mainThread->tid = 0;
      mainThread->context = mainContext; 
    
      threadNode->node = mainThread; // coloca a thread no PNODE
      //    AppendFila2(scheduler->able, threadNode); // vai pra fila de aptos
      scheduler->executing = mainThread; // main que ta executando
      scheduler->count++;

      dispatcher(); // chama o dispatcher dizendo que é a primeira vez que ele é executado
    }

    else
      printf("veio pra csched de volta mas ja tinha colocado o contexto do disp\n");

    
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
    printf("scheduler nulo: bora inicializa essa merda\n");
    csched_init();
    printf("ta no ccreate\n");
  }

  // aqui é a nova thread
  printf("criando nova thread\n");
  TCB_t *newThread = malloc(sizeof(TCB_t));  
  ucontext_t *newContext = malloc(sizeof(ucontext_t));
  char threadStack[SIGSTKSZ];

  getcontext(newContext);

  if (voltandopramain) {
    printf("veio pra ccreate mesmo!!!\n");
    return 0;
  }

  newContext->uc_link = &(scheduler->dispatcherContext); // contexto a executar no término
  newContext->uc_stack.ss_sp = threadStack; // endereço de início da pilha
  newContext->uc_stack.ss_size = sizeof(threadStack); // tamanho da pilha
  makecontext(newContext, (void (*)(void))start, ARGC, arg);  // cast pra funçao void* sem argumento

  newThread->state = PROCST_CRIACAO;
  newThread->prio = PRIORITY;
  newThread->tid = scheduler->count;
  newThread->context = *newContext;


  PNODE2 newThreadNode = malloc(sizeof(PNODE2));
  newThreadNode->node = newThread;
  AppendFila2(scheduler->able, newThreadNode); // nova thread ta apta
  scheduler->count++; // tem mais uma thread no scheduler
 
  return 0;
}


int cyield(void) {

  // PARA O CONTADOR DE TEMPO

  PFILA2 q = scheduler->able;

  PNODE2 executing = malloc(sizeof(PNODE2));
  executing->node = scheduler->executing;
  AppendFila2(q, executing); // coloca na fila de aptos quem ta pronto
  //scheduler->executing = NULL; // nao tem ninguem executando

  printf("o tid %d ta liberando e vai chamar o dispatcher\n", scheduler->executing->tid);
  dispatcher(); // chama o dispatcher pra ver quem vai
}


/*
int cjoin(int tid);
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
