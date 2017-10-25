#include <stdio.h>
#include <cthread.h>
#include <cdata.h>

csem_t sinaleira;

void* print_the_sound_of_a_cup_hitting_the_ground(void) {
  printf("\n> creck!");
  
  int value = cjoin(2);
  printf("Value: %i\n", value);
  
  // deveria vir pra cá só depois de terminar de executar a thread print_the_sound_of_a_capybara
  printf("> creck de novo, depois de dar cjoin\n\n");
  
  return NULL;
}

void* print_the_sound_of_a_capybara(void) {
  printf("\n\n> AheoUYUOoomTgUU\n\n");
  return NULL;
}

int main() {
  printf("\n\n> começou a main\n\n");
  
  int tid1 = ccreate(&print_the_sound_of_a_cup_hitting_the_ground, NULL, 0);
  int tid2 = ccreate(&print_the_sound_of_a_capybara, NULL, 0);
  
  cyield();
  
  printf("\n\n> voltando à main pra encerrar!\n\n");
  
  return 0;
}
