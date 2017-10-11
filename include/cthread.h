/*
 * cthread.h: arquivo de inclus�o com os prot�tipos das fun��es a serem
 *            implementadas na realiza��o do trabalho.
 *
 * N�O MODIFIQUE ESTE ARQUIVO.
 *
 * VERS�O: 11/09/2017
 *
 */
#ifndef __cthread__
#define __cthread__

#include "support.h"
#include "cdata.h"

int cidentify (char *name, int size);

int ccreate (void* (*start)(void*), void *arg, int prio);
//int csetprio(int tid, int prio);	// Essa fun��o n�o ser� utilizada em 2017/2
int cyield(void);
int cjoin(int tid);
int csem_init(csem_t *sem, int count);
int cwait(csem_t *sem);
int csignal(csem_t *sem);


#endif
