#include <stdio.h>
#include <cthread.h>
#include <cdata.h>




void* print_the_sound_of_a_cup_hitting_the_ground(void) {
  printf("\n> creck!\n");
  return NULL;
}

void* print_the_sound_of_a_capybara(void) {
  printf("\n\n> AheoUYUOoomTgUU\n\n");
  int tid2 = ccreate(&print_the_sound_of_a_cup_hitting_the_ground, NULL, 0);
  printf("TID: %i\n", tid2);
  
  // cjoin(tid2);
  
  // printf("\n\n> capivara fez cjoin\n\n");
  // printf("\n\n> voltando à capivara pra encerrar\n\n");
  return NULL;
}

int main() {
  printf("\n\n> começou a main\n\n");
  
  int tid1 = ccreate(&print_the_sound_of_a_capybara, NULL, 0);
  printf("TID: %i\n", tid1);
  
  
  cyield();
  
  printf("\n\n> voltando à main pra encerrar!\n\n");
  
  return 0;
}
