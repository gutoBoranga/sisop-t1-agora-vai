#include <cthread.h>
#include <cdata.h>

int main() {

  SCHEDULER_t *sched;

  csched_init(sched);
  list_able();
  
  return 0;
}
