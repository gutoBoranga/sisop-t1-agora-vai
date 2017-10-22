#include <stdio.h>
#include <cthread.h>
#include <cdata.h>

csem_t sinaleira;

void* print_the_sound_of_a_cup_hitting_the_ground(void) {
  printf("\n> creck!\n> Quero entrar na secao critica");
  
  cwait(&sinaleira);
  printf("> SECAO CRITICA NA print_the_sound_of_a_cup_hitting_the_ground\nUHUUL");
  cyield();
  csignal(&sinaleira);
  printf("\n>voltei do csignal e já vou mimbora\n\n");
  
  return NULL;
}

void* print_the_sound_of_a_capybara(void) {
  printf("> capivara tentando entrar na secao critica");
  cwait(&sinaleira);
  printf("\n> SECAO CRITICA!!11!1!!onze!");
  printf("\n\n> AheoUYUOoomTgUU\n\n");
  csignal(&sinaleira);
  return NULL;
}

int main() {
  printf("\n\n> começou a main\n\n");
  
  csem_init(&sinaleira, 0);
  
  int tid1 = ccreate(&print_the_sound_of_a_capybara, NULL, 0);
  int tid2 = ccreate(&print_the_sound_of_a_cup_hitting_the_ground, NULL, 0);
  
  cyield();
  
  printf("\n\n> voltando à main pra encerrar!\n\n");
  
  return 0;
}
