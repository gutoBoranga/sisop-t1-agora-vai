#include <stdio.h>
#include <cthread.h>
#include <cdata.h>

csem_t sinaleira;

void* print_the_sound_of_a_cup_hitting_the_ground(void) {
  printf("== Tentando entrar na seção crítica ==\n");
  cwait(&sinaleira);
  printf("\n> creck!\n\n");
  
  csignal(&sinaleira);
  printf("\n> cup_hitting_the_ground vai acabar!\n");
  
  return NULL;
}

void* print_the_sound_of_a_capybara(void) {
  printf("== Tentando entrar na seção crítica ==\n");
  cwait(&sinaleira);
  printf("\n\n> AheoUYUOoomTgUU\n\n");
  
  csignal(&sinaleira);
  
  return NULL;
}

void* print_what_I_will_do_soon(void) {
  printf("== Tentando entrar na seção crítica ==\n");
  cwait(&sinaleira);
  printf("\n\n> sleep\n\n");
  
  csignal(&sinaleira);
  
  return NULL;
}

void* print_the_name_of_the_integrants_of_klb(void) {
  printf("== Tentando entrar na seção crítica ==\n");
  cwait(&sinaleira);
  printf("\n\n> Kiko\n");
  printf("> Leandro\n");
  printf("> Bruno\n\n");
  
  csignal(&sinaleira);
  
  return NULL;
}

int main() {
  printf("\n\n> começou a main\n\n");
  
  int tid1 = ccreate(&print_the_sound_of_a_cup_hitting_the_ground, NULL, 0);
  int tid2 = ccreate(&print_the_sound_of_a_capybara, NULL, 0);
  int tid3 = ccreate(&print_what_I_will_do_soon, NULL, 0);
  int tid4 = ccreate(&print_the_name_of_the_integrants_of_klb, NULL, 0);
  
  csem_init(&sinaleira, 1);
  
  cwait(&sinaleira);
  
  cyield();
  
  printf("\n\n> main chamando csignal\n\n");
  
  csignal(&sinaleira);
  
  cyield();
  
  printf("\n\n> voltando à main pra encerrar!\n\n");
  
  return 0;
}
