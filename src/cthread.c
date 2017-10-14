#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <support.h>
#include <cthread.h>
#include <cdata.h>


int dispatcherContextSet = 0;
SCHEDULER_t *scheduler; // acho que isso nao precisa ser um ponteiro, mas depois a gente se preocupa com isso

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

void firstAtBeggining() {

  printf("vai ordenar\n");
  PFILA2 q = scheduler->able;
  PFILA2 greatest;

  int i = 0, greatestTid = -1, found = 0;
  TCB_t *currentThread, *greatestThread;
  PNODE2 currentNode, greatestNode;


  FirstFila2(q);

  while (GetAtIteratorFila2(q) != NULL) { // iterador no primeiro elemento
    currentNode = (PNODE2) GetAtIteratorFila2(q);
    currentThread = (TCB_t*) currentNode->node;
    printf("vendo o tid %d\n", currentThread->tid);
    if (currentThread->tid > greatestTid) {
      greatestTid = currentThread->tid;
      greatestThread = currentThread;
      greatest = q;
    }
    NextFila2(q);
  }


  /* SE EU CONSEGUIR PEGAR O ITERADOR DA PRA USAR ISSO QUE É MAIS FACIL
  if (greatestTid != -1) {
    DeleteAtIteratorFila2(greatest); // tira da fila o maior
    greatestNode = malloc(sizeof(PNODE2));
    greatestNode->node = greatestThread;

    FirstFila2(q); // poe o iterador no inicio da fila
    InsertBeforeIteratorFila2(q, greatestNode); // poe o elemento novo no inicio
  }
  */

  FirstFila2(q);
  while (GetAtIteratorFila2(q) != NULL && !found) { // iterador no primeiro elemento
    currentNode = (PNODE2) GetAtIteratorFila2(q);
    currentThread = (TCB_t*) currentNode->node;
    printf("vendo o tid %d\n", currentThread->tid);
    if (currentThread->tid == greatestTid) {
      DeleteAtIteratorFila2(q);
      found = 1;
    }
    NextFila2(q);
  }

  FirstFila2(q);
  greatestNode = malloc(sizeof(PNODE2));
  greatestNode->node = greatestThread;
  InsertBeforeIteratorFila2(q, greatestNode); // poe o elemento novo no inicio
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

  firstAtBeggining();

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

      swapcontext(&(previous->context), &(first->context)); // poe pra executar o novato e salvar o estado anterior do antigo
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
    scheduler->ableCount = 0;

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

    if(thread->tid == tid) { // era tid_to_find(que não tava def) antes de tid
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

/* Editado por Octavio Arruda */

TCB_t *retorna_tcb(int tid, PFILA2 fila){ /* É necessário ter certeza de que a
  tid está na fila para utilização desta função. */
  TCB_t *temp;
  PNODE2 current;

  temp = FirstFila2(fila);
  if(temp->tid == tid){

    return temp; // ponteiro para TCB da tid
  }
  else while(GetAtIteratorFila2(fila) != NULL){
    current = (PNODE2)GetAtIteratorFila2(fila);
    temp =  (TCB_t*)current->node;

      if(temp->tid == tid) return temp;
    }
    if(temp->tid == tid) return temp;
}

/*
  *** Inicialização do semáforo ***
*/


int csem_init(csem_t *sem, int count){

  PFILA2 pFilaSem = malloc(sizeof(PFILA2));
  CreateFila2(pFilaSem);

  if(pFilaSem){ // não é nulo, sucesso

    sem->count = 1; //semáforo começa livre
    sem->fila = pFilaSem;

    return 0;
  } // erro
  else return -1;
}

  // *** Sincronização de Término ***

int cjoin(int tid){

  PFILA2 filathreads = scheduler->able;
  PFILA2 filabloqueados = scheduler->blocked;
  TCB_t *tcb;
  TCB_t *chamou = scheduler->executing;
  TCB_t *temp;
  PNODE2 currentNode;

  if(chamou->waiting != NULL) return -1;

  if(threadIsInFila(tid, filathreads) == TRUE){ /* Se a thread que foi passada
    como argumento pela cjoin já estiver na fila de threads, basta checar se ela
    já possui uma thread associada a ela(waited ou waiting).*/
    currentNode = FirstFila2(filathreads);
    temp = currentNode->node;

    if(temp->tid == tid){
      tcb = retorna_tcb(tid, filathreads);
      if( (tcb->waitedby = NULL) && (chamou->waiting = NULL) ){
        /* Ninguém fez cjoin nela ainda nessa thread, e a thread que chamou não
        está esperando por ninguém TAMBÉM.
        */
        tcb->waitedby = chamou;
        chamou->waiting = tcb;

        return 0; // a linkagem aconteceu e retorna 0

      } else return -1;
    }
    else{
      while(GetAtIteratorFila2(filathreads) != NULL){

        currentNode = GetAtIteratorFila2(filathreads);
        temp = currentNode->node;

        if(temp->tid == tid){
          tcb = retorna_tcb(temp->tid, filathreads);

          tcb->waitedby = chamou;
          chamou->waiting = tcb;
        }
      }
      return -1;
    }
  }
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
