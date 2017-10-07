#include <cthread.h>
#include <cdata.h>

int main() {

  SCHEDULER_t *sched;

  int i = csched_init(sched);
  
  return 0;
}
