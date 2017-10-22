#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <support.h>
#include <cthread.h>
#include <cdata.h>

//   ___    ______    _    _    _____    _____     ___     ___       ___
//  / _ \  |__  __|  | |  | |  |  _  \  |  ___|   /   \   |   \    / __ \
// / / \_\    | |    | |__| |  | |_| |  | |___   /  _  \  | .  \  | | _|_|
// | |        | |    |  __  |  |    /   |  ___|  | |_|  | | |\  | \__  \
// | |  _     | |    | |  | |  | |\ \   | |      |  __  | | |/  |  _  \ |
// \ \_/ /    | |    | |  | |  | | \ \  | |___   | |  | | |    /  \ \_/ /
//  \___/     |_|    |_|  |_|  |_|  \_\ |_____|  |_|  |_| |___/    \___/

int endingThread = 1;

SCHEDULER_t *scheduler; // acho que isso nao precisa ser um ponteiro, mas depois a gente se preocupa com isso

void list_threads(int queue) {
  
  printf("LISTA DAS THREADS NA FILA %i\n", queue);
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

  int i = 0, greatestTid = -1, found = 0, end = 0, greatest_tid_only_if_this_is_the_only_thread_available = -1;
  TCB_t *currentThread, *greatestThread;
  PNODE2 currentNode, greatestNode;

  
  FirstFila2(q);
  while (GetAtIteratorFila2(q) != NULL) {
    currentNode = (PNODE2) GetAtIteratorFila2(q);
    currentThread = (TCB_t*) currentNode->node;
    printf("vendo o tid %d\n", currentThread->tid);
    if (currentThread->tid > greatestTid) {
      if (currentThread->tid == scheduler->executing->tid) {
        // se o tid da currentThread for o que """"está executando""""
        // ele não pode ser escolhido a menos que seja a unica thread
        greatest_tid_only_if_this_is_the_only_thread_available = currentThread->tid;
      } else {
        // senão, o tid pode ser escolhido
        greatestTid = currentThread->tid;
        greatestThread = currentThread;
        greatest = q;
      }
    }
    
    NextFila2(q);
  }
  
  if (greatestTid = -1) {
    // se não achou nenhuma thread além da que está executando, vai essa mesmo né, paciência
    greatestTid = greatest_tid_only_if_this_is_the_only_thread_available;
  }

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
  
  greatestNode = malloc(sizeof(PNODE2));
  greatestNode->node = greatestThread;
  
  // se a fila estiver vazia
  if (FirstFila2(q)) {
    AppendFila2(q, greatestNode);
  } else {
    InsertBeforeIteratorFila2(q, greatestNode); // poe o elemento novo no inicio
  }
}


int dispatcher() {
  // printf("\n\nentrando no dispatcher (leia-se \"TÔ CHEGANDO NA COHAB\")\n");
  printf("\n[ DISPATCHER ]\n");


  unsigned int elapsedTime = stopTimer();
  timeManagement(elapsedTime);

  printf("o tid %d ficou %d segundos (?)\n", scheduler->executing->tid, elapsedTime);




  
  
  // se chegou aqui porque uma thread acabou
  if (endingThread) {
    // aqui tem que checar se tem alguma thread esperando a atual acabar
    // por causa do CJOIN
    
    printf("\nparece que alguma thread acabou\n");
  }
  
  endingThread = 1;

  // para o contador de tempo // _______ CHAMANDO ATENÇAO PRA COISAS FALTANDO

  // estamos fingindo que a fila ja esta ordenada

  PFILA2 q = scheduler->able;
  PNODE2 queueNode;
  TCB_t *first;

  firstAtBeggining();

  if (FirstFila2(q) == 0) { // conseguiu por o iterador no primeiro elemento da fila
    
    if (GetAtIteratorFila2(q) != NULL) { // tem alguem na fila pra pegar a cpu
      //NextFila2(q); // ==========> essa linha faz com que funcione (pq aí ele nao escalona pra main)
      printf("- tem alguma thread pra dale, iuhuu\n");
      
      queueNode = (PNODE2)GetAtIteratorFila2(q);
      
      first = queueNode->node;
      scheduler->executing = first; // o em execuçao agora é o primeiro da fila
      DeleteAtIteratorFila2(q); // tira ele da lista de aptos, pq ta executando
      // começa o contador de tempo!!!!!!!!
      printf("- o tid do first eh %d\n", first->tid);
      printf("- vai dar setcontext, se preparem!\n");

      startTimer();
      setcontext(&(scheduler->executing->context)); // poe pra executar o novato
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
  printf("\n\n[ INIT ]\n");
  printf("iniciando o scheduler!\n");

  scheduler = malloc(sizeof(SCHEDULER_t));

  if (scheduler) {
    scheduler->able = malloc(sizeof(FILA2));
    scheduler->blocked = malloc(sizeof(FILA2));
    
    CreateFila2(scheduler->able); // filas vazias
    CreateFila2(scheduler->blocked);
    scheduler->count = 0; // ninguem escalonado ainda
    scheduler->ableCount = 0;

    // seta o dispatcher context
  	getcontext(&scheduler->dispatcherContext);
  	
    char stack_dispatcher[SIGSTKSZ];
  	scheduler->dispatcherContext.uc_stack.ss_sp = stack_dispatcher;
  	scheduler->dispatcherContext.uc_stack.ss_size = sizeof(stack_dispatcher);
  	makecontext(&scheduler->dispatcherContext, (void (*)(void)) dispatcher, 0);
    
    
    // cria a thread da main
    PNODE2 threadNode = malloc(sizeof(PNODE2));
    TCB_t *mainThread = malloc(sizeof(TCB_t));
    ucontext_t mainContext;

    getcontext(&mainContext); // pega o contexto atual (que vai ser o da main)
    
    scheduler->mainContext = mainContext; // define ele como contexto principal no scheduler

    mainThread->state = PROCST_CRIACAO; // preenche as parada da thread
    mainThread->prio = PRIORITY;
    mainThread->tid = 0;
    mainThread->context = mainContext;

    threadNode->node = mainThread; // coloca a thread no PNODE
    //    AppendFila2(scheduler->able, threadNode); // vai pra fila de aptos
    scheduler->executing = mainThread; // main que ta executando
    startTimer(); // começa a contar o tempo da main
    scheduler->count++;

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
  PNODE2 current;
  
  FirstFila2(fila);
  
  do {
    if(fila->it == 0) {
      break;
    }
    
    current = (PNODE2)GetAtIteratorFila2(fila);
    thread = (TCB_t *) current->node;
    
    if(thread->tid == tid) { // obrigado :)
      return TRUE;
    }
  } while(NextFila2(fila) == 0);

  return FALSE;
}

int removeThreadFromFila(int tid, PFILA2 fila) {
  TCB_t *thread;
  PNODE2 current;
  
  if (FirstFila2(fila) != 0) {
    return ERROR;
  }

  do {
    if(fila->it == 0) {
      break;
    }
    
    current = (PNODE2) GetAtIteratorFila2(fila);
    thread = (TCB_t*) current->node;

    if(thread->tid == tid) {
      DeleteAtIteratorFila2(fila);
      return TRUE;
    }
  } while(NextFila2(fila) == 0);

  return FALSE;
}

void timeManagement(unsigned int lastReading) {

  TCB_t *current = scheduler->executing;

  printf("vendo a prioridade da %d\n", current->tid);
  
  if (current->lastTime == 0) { // primeira execuçao nao faz nada
    current->prio = 0;
  }

  else if (current->lastTime <= lastReading) { // a execucao anterior foi maior ou igual, entao aumenta a prio
    if (current->prio < 3) {// só aumenta a prioridade se n for a maxima
      printf("prioridade foi de %d pra %d \n", current->prio, current->prio++);
      current->prio++;
    }
  }

  else { // se nao diminui
    if (current->prio > 0) {
      printf("prioridade foi de %d pra %d \n", current->prio, current->prio--);
      current->prio--;
    }
  }
      
  current->lastTime = lastReading;

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
  }

  // aqui é a nova thread
  printf("\n[ CCREATE ]\n");
    
  TCB_t *newThread = malloc(sizeof(TCB_t));
  
  newThread->state = PROCST_CRIACAO;
  newThread->prio = PRIORITY;
  newThread->tid = scheduler->count;
  
  // mexe direto no contexto que tá dentro da thread
  getcontext(&newThread->context);
  
  newThread->context.uc_link = &(scheduler->dispatcherContext);
  newThread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
  newThread->context.uc_stack.ss_size = SIGSTKSZ;
  
  makecontext(&newThread->context, (void (*)(void))start, ARGC, arg);
  

  PNODE2 newThreadNode = malloc(sizeof(PNODE2));
  newThreadNode->node = newThread;
  AppendFila2(scheduler->able, newThreadNode); // nova thread ta apta
  scheduler->count++; // tem mais uma thread no scheduler

  return newThread->tid;
}


int cyield(void) {
  printf("\n[ CYIELD ]\n");
  
  if (scheduler == NULL) {  // se ainda nao inicializou o scheduler, manda bala
    printf("scheduler nulo: bora inicializa essa merda\n");
    csched_init();
  }

  // PARA O CONTADOR DE TEMPO
  //setcontext(&(scheduler->dispatcherContext));
  PFILA2 q = scheduler->able;

  PNODE2 executing = malloc(sizeof(PNODE2));
  executing->node = scheduler->executing;
  AppendFila2(q, executing); // coloca na fila de aptos quem ta pronto
  //scheduler->executing = NULL; // nao tem ninguem executando

  // printf("o tid %d ta liberando e vai chamar o dispatcher\n", scheduler->executing->tid);
  endingThread = 0;
  
  // vai pro contexto do dispatcher e salva o estado da thread que estava executando
  swapcontext(&(scheduler->executing->context), &(scheduler->dispatcherContext));
}


/*
int cjoin(int tid);
int cwait(csem_t *sem);
int csignal(csem_t *sem);
*/

/* Editado por Octavio Arruda */

/* Função que retorna um ponteiro pra TCB da tid dada.
 ** OBRIGATÓRIO QUE A TID ESTEJA NA FILA **  */

TCB_t *retorna_tcb(int tid, PFILA2 fila){ /* É necessário ter certeza de que a
  tid está na fila para utilização desta função. */
  TCB_t *temp = malloc(sizeof(*temp));
  PNODE2 current;

  if( FirstFila2(fila) == 0 ){

    current = (PNODE2)GetAtIteratorFila2(fila);
    temp = (TCB_t*)current->node;

    if(temp->tid == tid){
      return temp; // ponteiro para TCB da tid
      }

    }  else while(GetAtIteratorFila2(fila) != NULL){

      current = (PNODE2)GetAtIteratorFila2(fila);
      temp =  (TCB_t*)current->node;
      if( temp->tid == tid ) return temp;

      NextFila2(fila);
    }

    if(temp->tid == tid) return temp;
}

/*
  *** Inicialização do semáforo ***
*/


int csem_init(csem_t *sem, int count){
  if (scheduler == NULL) {  // se ainda nao inicializou o scheduler, manda bala
    printf("scheduler nulo: bora inicializa essa merda\n");
    csched_init();
  }

  sem->fila = malloc(sizeof(FILA2));
  CreateFila2(sem->fila);

  if(sem->fila){ // não é nulo, sucesso
    sem->count = 1; //semáforo começa livre

    return 0;
  } // erro
  else return -1;
}

  // *** Sincronização de Término ***

int cjoin(int tid) {
  
  printf("\n[ CJOIN ]\n");

  PFILA2 filaaptos = scheduler->able;
  PFILA2 filabloqueados = scheduler->blocked;
  PFILA2 filacerta;

  TCB_t *tcb;
  TCB_t *chamou = scheduler->executing;

  PNODE2 currentNode;

  if(chamou->waiting != NULL) return ERROR; /* se quem fez cjoin já tá esperando
  retorna código de erro */

  if(threadIsInFila(tid, filaaptos) == TRUE){/* Se a thread que foi passada
    como argumento pela cjoin já estiver na fila de threads, basta checar se ela
    já possui uma thread associada a ela(waited ou waiting).*/
    filacerta = filaaptos;
    } else if(threadIsInFila(tid, filabloqueados) == TRUE){
      filacerta = filabloqueados; /* filacerta é a fila em que a tid está:
      ou filabloqueados ou filaaptos. */
    }else return ERROR; // se não estiver em nenhuma das duas, retorna erro

    tcb = retorna_tcb(tid, filacerta); // pega o TCB da tid

    if( (tcb->waitedby = NULL) && (chamou->waiting = NULL) )
    { /* Se o tcb passado como argumento não tiver ninguém esperando por ele E
      a thread que chamou não estiver esperando por ninguém
      */
     tcb->waitedby = chamou; // Pode fazer um encadeamento entre as duas threads
     chamou->waiting = tcb; // em uma relação de esperado-esperando

    AppendFila2(scheduler->blocked, chamou); /* Quem chamou a cjoin passa a ser
    bloqueada.  */

    endingThread = 0;
    
    swapcontext(&(scheduler->executing->context), &(scheduler->dispatcherContext));

    return SUCCESS;
  } else return ERROR;
}


/* Fim da edição */

// csignal
// Parâmetro: *sem, um ponteiro para uma variável do tipo semáforo.
// Retorno: Quando executada corretamente: retorna SUCCESS (0) Caso contrário, retorna ERROR (-1).

int csignal (csem_t *sem) {
  printf("\n\n[ CSIGNAL ]\n");
  
  printf("-> count: %i\n", sem->count);
  sem->count++;
  printf("-> count: %i\n", sem->count);

  // se sem está livre, não tem ninguém bloqueado. Portanto, retorna dizendo que foi tudo ok.
  if (sem->count > 0) {
    printf("-> não tinha ninguém bloqueado aqui\n");
    return SUCCESS;
  }

  // se sem está ocupado:
  else {
    printf("-> ihh, tinha gente esperando\n");
    
    TCB_t* thread;
    
    // pega o primeiro da fila
    FirstFila2(sem->fila);
    
    PNODE2 blocked_node;
    blocked_node = GetAtIteratorFila2(sem->fila);
    thread = (TCB_t *) blocked_node->node;
    
    // tenta remover dos bloqueados. Se não encontrar a thread lá, retorna erro
    if (!removeThreadFromFila(thread->tid, scheduler->blocked)) {
      printf("-> erro ao tentar desbloquear thread %i\n", thread->tid);
      return ERROR;
    }
    
    printf("-> desbloqueei a thread %i\n", thread->tid);
    
    // adiciona na fila dos aptos
    PNODE2 node = malloc(sizeof(PNODE2));
    node->node = thread;
    AppendFila2(scheduler->able, node);
    
    printf("-> botei a thread %i nos aptos\n", thread->tid);

    return SUCCESS;
  }
}

// Liberação de recurso: a chamada csignal serve para indicar que a thread está liberando o recurso.
// Para cada chamada da primitiva csignal, a variável count deverá ser incrementada de uma unidade.
// Se houver mais de uma thread bloqueada a espera desse recurso a primeira delas, segundo uma política de FIFO,
// deverá passar para o estado apto e as demais devem continuar no estado bloqueado.

// -------------------------------------------------------------------------------------------------------------

// cwait
// Parâmetro: *sem, um ponteiro para uma variável do tipo semáforo.
// Retorno: Quando executada corretamente: retorna SUCCESS (0) Caso contrário, retorna ERROR (-1).

int cwait (csem_t *sem) {
  printf("\n\n[ CWAIT ]\n");
  
  printf("-> count: %i\n", sem->count);
  sem->count--;
  printf("-> count: %i\n", sem->count);
  
  // se sem está livre:
  if (sem->count >= 0) {
    // tem que fazer algo mais aqui?
    printf("-> livre!\n");
    return SUCCESS;
  }
  
  // se sem está ocupado:
  else {
    printf("-> tem gente!\n");
    
    if (sem->fila == NULL) {
      return ERROR;
    }
    
    // ponteiro pra thread executando
    TCB_t* thread = scheduler->executing;
      
    // adiciona thread executando na fila de threads bloqueadas do sem
    PNODE2 node = malloc(sizeof(PNODE2));
    node->node = thread;
    AppendFila2(sem->fila, node);
    
    printf("meteu na fila da sinaleira\n");
  
    // adiciona thread executando na fila de blocked
    AppendFila2(scheduler->blocked, node);
    
    // vai pro dispatcher pegar nova thread pra executar e segue o baile
    endingThread = 0;
    swapcontext(&(scheduler->executing->context), &(scheduler->dispatcherContext));
  
    return SUCCESS;
  }
}

void teste2() {
  // setcontext(&(scheduler->dispatcherContext));
}
