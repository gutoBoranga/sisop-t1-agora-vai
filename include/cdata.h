/*
 * cdata.h: arquivo de inclus�o de uso apenas na gera��o da libpithread
 *
 * Esse arquivo pode ser modificado. ENTRETANTO, deve ser utilizada a TCB fornecida
 *
 * Vers�o de 11/09/2017
 *
 */
#ifndef __cdata__
#define __cdata__
#include "ucontext.h"

#define	PROCST_CRIACAO	0
#define	PROCST_APTO	1
#define	PROCST_EXEC	2
#define	PROCST_BLOQ	3
#define	PROCST_TERMINO	4

/* Os campos "tid", "state", "prio" e "context" dessa estrutura devem ser mantidos e usados convenientemente
   Pode-se acrescentar outros campos AP�S os campos obrigat�rios dessa estrutura
*/
typedef struct s_TCB {
	int		tid; 		// identificador da thread
	int		state;		// estado em que a thread se encontra
					// 0: Criação; 1: Apto; 2: Execução; 3: Bloqueado e 4: Término
	unsigned 	int		prio;		// prioridade da thread (higest=0; lowest=3)
	ucontext_t 	context;	// contexto de execução da thread (SP, PC, GPRs e recursos)

        // Se necess�rio, pode-se acresecentar campos nessa estrutura A PARTIR DAQUI! 
	// Para cjoin
	struct s_TCB *waiting;
	struct s_TCB *waitedby;

  unsigned int lastTime;

} TCB_t;

typedef struct s_sem{
int count; // indica se recurso está ocupado ou não (livre > 0, ocupado ≤ 0) 
PFILA2 fila; // ponteiro para uma fila de threads bloqueadas no semáforo. 
}csem_t;

#define XOPEN_SOURCE 600 // essa linha � s� pra funcionar no mac


#define NAMES "Augusto Boranga\nLucas Assis\nOct�vio Arruda\n"
#define NAMES_SIZE 45
#define ARGC 1  // funcoes sempre com um argumento (no max)

#define MAIN_THREAD_TID 0 // tid da main � sempre 0
#define PRIORITY 0 // prioridade � sempre 0

#define ABLE_QUEUE 0  // codigo pra acessar a fila que quero listar
#define BLOCKED_QUEUE 1

#define ERROR -1 // pro retorno das funções
#define SUCCESS 0

#define TRUE 1 // pra controle de fluxo em alto nível
#define FALSE 0

#define STACK 32000

typedef struct s_SCHEDULER {

  ucontext_t mainContext;
  ucontext_t dispatcherContext;
  TCB_t* executing;
  PFILA2 able;
  PFILA2 blocked;
  int ableCount;
  int count;
  unsigned int executionTime;
} SCHEDULER_t;

#endif
