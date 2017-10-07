/*
 * cdata.h: arquivo de inclusão de uso apenas na geração da libpithread
 *
 * Esse arquivo pode ser modificado. ENTRETANTO, deve ser utilizada a TCB fornecida
 *
 * Versão de 11/09/2017
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
   Pode-se acrescentar outros campos APÓS os campos obrigatórios dessa estrutura
*/
typedef struct s_TCB { 
	int		tid; 		// identificador da thread
	int		state;		// estado em que a thread se encontra
					// 0: Criação; 1: Apto; 2: Execução; 3: Bloqueado e 4: Término
	unsigned 	int		prio;		// prioridade da thread (higest=0; lowest=3)
	ucontext_t 	context;	// contexto de execução da thread (SP, PC, GPRs e recursos) 
	
	/* Se necessário, pode-se acresecentar campos nessa estrutura A PARTIR DAQUI! */
	
	
} TCB_t;

#define _XOPEN_SOURCE 600 // essa linha Ž s— pra funcionar no mac


#define NAMES "Augusto Boranga\nLucas Assis\nOct‡vio Arruda\n"
#define NAMES_SIZE 45
#define ARGC 1  // funcoes sempre com um argumento (no max)

#define MAIN_THREAD_TID 0 // tid da main Ž sempre 0
#define PRIORITY 0 // prioridade Ž sempre 0

#define ABLE_QUEUE 0  // codigo pra acessar a fila que quero listar
#define BLOCKED_QUEUE 1


#define STACK 64000

typedef struct s_SCHEDULER {
  ucontext_t *mainContext;
  int executing;
  PFILA2 able;
  PFILA2 blocked;
  int count;
} SCHEDULER_t;


#endif
