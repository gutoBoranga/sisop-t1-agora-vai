#include <stdio.h>
#include <ucontext.h>
#include <support.h>
#include <cthread.h>
#include <cdata.h>

int csem_init(csem_t *sem, int count){

  CreateFila2 (PFILA2 pFilaSem) // Cria a fila para o semáforo
  sem count = 1; //semáforo começa livre
  sem PFILA2 pFilaSem;

  if(check_malloc(sizeof(sem)) == NULL){ // retorna null quando houve erro
    return -1
  } // se não houve erro, a região foi alocada corretamente
  else return 0

}
