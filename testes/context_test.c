#include <stdio.h>
#include <cthread.h>
#include <cdata.h>


void* print_the_sound_of_a_capybara(void) {
  printf("\n\n> AheoUYUOoomTgUU\n\n");
  printf("> vai acabar a thread da capivara\n\n");
  return NULL;
}

void* print_the_sound_of_a_cup_hitting_the_ground(void) {
  printf("\n> creck!\n");
  return NULL;
}

int main() {
  int tid1 = ccreate(&print_the_sound_of_a_capybara, NULL, 0);
  int tid2 = ccreate(&print_the_sound_of_a_cup_hitting_the_ground, NULL, 0);
  
  cyield();
  
  printf("\n\n> voltando à main pra encerrar!\n\n");
  
  return 0;
}
