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

void list_threads(PFILA2 q) {
  
  PNODE2 currentNode;
  TCB_t *currentThread;
  
  printf("\n=== Threads na fila ===\n");
  
  if (FirstFila2(q) == 0) {
    while (GetAtIteratorFila2(q) != NULL) { // iterador no primeiro elemento
      currentNode = (PNODE2) GetAtIteratorFila2(q);
      currentThread = (TCB_t*) currentNode->node;
      
      printf("Tid: %i\n", currentThread->tid);
      
      NextFila2(q);
    }
  }

  else {
    printf("fila vazia ou invalida\n");
  }

  FirstFila2(q);  // volta pro inicio da fila
}

TCB_t* choose_thread() {
  PNODE2 current_node;
  TCB_t *current_thread;
  TCB_t *the_chosen_one = NULL;

  // se deu erro, retorna NULL
  if (FirstFila2(scheduler->able) != 0) {
    return NULL;
  }
  
  while (GetAtIteratorFila2(scheduler->able) != NULL) { // iterador no primeiro elemento
    
    current_node = (PNODE2) GetAtIteratorFila2(scheduler->able);
    current_thread = (TCB_t*) current_node->node;
    
    // na primeira vez que executar, pra nao dar erro ao acessar o tid de algo que aponta pra NULL
    if (the_chosen_one == NULL) {
      the_chosen_one = current_thread;
    }
    
    if (current_thread->prio < the_chosen_one->prio) {
      the_chosen_one = current_thread;
    }

    else if (current_thread->prio == the_chosen_one->prio && current_thread->tid < the_chosen_one->tid) { // segundo criterio é o tid em ordem crescente
      the_chosen_one = current_thread;
    }
    
    NextFila2(scheduler->able);
  }
  
  return the_chosen_one;
}

int dispatcher() {
  // printf("\n[ DISPATCHER ]\n");
  
  // se chegou aqui porque uma thread acabou
  if (endingThread) {

    // verifica se tem alguem esperando pela que acabou
    if (scheduler->executing->waitedby != NULL) {

      // tira a thread que ta sendo esperada da fila de bloqueados e poe na de aptos
      TCB_t *waiter = malloc(sizeof(TCB_t));
      PNODE2 waiterNode = malloc(sizeof(PNODE2));
      
      *waiter = *(scheduler->executing->waitedby); // salva a thread que tava nas bloqueadas
      waiter->waiting = NULL; // diz que ela nao ta esperando mais ninguem
      removeThreadFromFila(waiter->tid, scheduler->blocked); // remove da fila de bloqueados

      waiterNode->node = waiter;
      AppendFila2(scheduler->able, waiterNode); // adiciona na fila de aptos
    }

    else {} // nao precisa fazer nada se nao tem ninguem esperando
  }
  
  endingThread = 1;

  // para o contador de tempo // _______ CHAMANDO ATENÇAO PRA COISAS FALTANDO

  // escolhe thread que vai ganhar o processador
  TCB_t *chosen_thread;
  chosen_thread = choose_thread();
  
  // thread executando passa a ser a escolhida
  scheduler->executing = chosen_thread;
  
  // tira a escolhida de aptos
  removeThreadFromFila(chosen_thread->tid, scheduler->able);
  
  // começa a contar o tempo
  startTimer();

  // seta o contexto
  setcontext(&(scheduler->executing->context)); // poe pra executar o novato
  
  return 0;
}

int csched_init() {
  // printf("\n\n[ INIT ]\n");

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
    mainThread->tid = MAIN_THREAD_TID;
    mainThread->context = mainContext;
    mainThread->lastTime = 0;
    mainThread->waitedby = NULL;
    mainThread->waitedby = NULL;

    scheduler->executing = mainThread; // main que ta executando
    scheduler->count++;
    
    // começa a contar o tempo da main
    startTimer();

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

  if (scheduler == NULL) {  // se ainda nao inicializou o scheduler, inicializa
    csched_init();
  }

  // printf("\n[ CCREATE ]\n");
    
  TCB_t *newThread = malloc(sizeof(TCB_t));
  
  newThread->state = PROCST_CRIACAO;
  newThread->prio = PRIORITY;
  newThread->tid = scheduler->count;
  newThread->lastTime = 0;
  newThread->waitedby = NULL;
  newThread->waiting = NULL;
  
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
  
  if (scheduler == NULL) {  // se ainda nao inicializou o scheduler, inicializa
    csched_init();
  }
  
  // printf("\n[ CYIELD ]\n");


  // PARA O CONTADOR DE TEMPO
  unsigned int elapsedTime = stopTimer();
  scheduler->executing->prio += elapsedTime;
  //timeManagement(elapsedTime);


  PNODE2 executing = malloc(sizeof(PNODE2));
  executing->node = scheduler->executing;
  AppendFila2(scheduler->able, executing); // coloca na fila de aptos quem ta pronto

  endingThread = 0;
  
  // vai pro contexto do dispatcher e salva o estado da thread que estava executando
  swapcontext(&(scheduler->executing->context), &(scheduler->dispatcherContext));
}

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
  
  if (scheduler == NULL) {  // se ainda nao inicializou o scheduler, inicializa
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
  
  // printf("\n[ CJOIN ]\n");

  PFILA2 filaaptos = scheduler->able;
  PFILA2 filabloqueados = scheduler->blocked;
  PFILA2 filacerta;

  TCB_t *tcb;
  TCB_t *chamou = scheduler->executing;

  PNODE2 currentNode;

  if(chamou->waiting != NULL) return ERROR; /* se quem fez cjoin já tá esperando
  retorna código de erro */
  
  if(threadIsInFila(tid, filaaptos) == TRUE) {
    /* Se a thread que foi passada
    como argumento pela cjoin já estiver na fila de threads, basta checar se ela
    já possui uma thread associada a ela(waited ou waiting).*/
    filacerta = filaaptos;
    
  } else if(threadIsInFila(tid, filabloqueados) == TRUE) {
      /* filacerta é a fila em que a tid está:
      ou filabloqueados ou filaaptos. */
      filacerta = filabloqueados;
      
      // se não estiver em nenhuma das duas, retorna erro
  } else return ERROR;

  tcb = retorna_tcb(tid, filacerta); // pega o TCB da tid
    
  if( (tcb->waitedby == NULL) && (chamou->waiting == NULL) ) {
    /* Se o tcb passado como argumento não tiver ninguém esperando por ele E
    a thread que chamou não estiver esperando por ninguém
    */
    tcb->waitedby = chamou; // Pode fazer um encadeamento entre as duas threads
    chamou->waiting = tcb; // em uma relação de esperado-esperando
    
    PNODE2 node = malloc(sizeof(PNODE2));
    node->node = chamou;
    AppendFila2(scheduler->blocked, node); /* Quem chamou a cjoin passa a ser
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
  // printf("\n\n[ CSIGNAL ]\n");
  
  sem->count++;

  // se sem está livre, não tem ninguém bloqueado. Portanto, retorna dizendo que foi tudo ok.
  if (sem->count > 0) {
    return SUCCESS;
  }

  // se sem está ocupado:
  else {
    TCB_t* thread;
    
    // pega o primeiro da fila
    FirstFila2(sem->fila);
    
    PNODE2 blocked_node;
    blocked_node = GetAtIteratorFila2(sem->fila);
    thread = (TCB_t *) blocked_node->node;
    
    // remove da fila do semáforo
    DeleteAtIteratorFila2(sem->fila);
    
    // tenta remover dos bloqueados. Se não encontrar a thread lá, retorna erro
    if (!removeThreadFromFila(thread->tid, scheduler->blocked)) {
      return ERROR;
    }
    
    // adiciona na fila dos aptos
    PNODE2 node = malloc(sizeof(PNODE2));
    node->node = thread;
    AppendFila2(scheduler->able, node);
    
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
  // printf("\n\n[ CWAIT ]\n");
  
  sem->count--;
  
  // se sem está livre:
  if (sem->count >= 0) {
    return SUCCESS;
  }
  
  // se sem está ocupado:
  else {
    
    if (sem->fila == NULL) {
      return ERROR;
    }
    
    // ponteiro pra thread executando
    TCB_t* thread = scheduler->executing;
      
    // adiciona thread executando na fila de threads bloqueadas do sem
    PNODE2 node = malloc(sizeof(PNODE2));
    node->node = thread;
    AppendFila2(sem->fila, node);
    
    // adiciona thread executando na fila de blocked
    AppendFila2(scheduler->blocked, node);
    
    // vai pro dispatcher pegar nova thread pra executar e segue o baile
    endingThread = 0;
    swapcontext(&(scheduler->executing->context), &(scheduler->dispatcherContext));
  
    return SUCCESS;
  }
}
