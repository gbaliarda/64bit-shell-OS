#include "../syscalls.h"
#include "../libc.h"
#include "test_util.h"

//TO BE INCLUDED
static void endless_loop(){
  printf("I'm process ");
  printInt(sys_getPid());
  printf("\n");
  while(1);
}

static int my_create_process(char * name){
  char argv[6][21];
  argv[1][0] = '&';
  argv[1][1] = 0;
  return createProcess((uint64_t)&endless_loop, 2, argv, NULL, NULL);
}

static uint64_t my_kill(uint64_t pid){
  sys_killProcess(pid);
  return 0;
}

static uint64_t my_block(uint64_t pid){
  sys_changeState(pid);
  return 0;
}
static uint32_t my_unblock(uint32_t pid){
  return my_block(pid);
}

#define MAX_PROCESSES 10 //Should be around 80% of the the processes handled by the kernel

enum State {BLOCKED, READY, KILLED};

typedef struct P_rq{
  uint32_t pid;
  enum State state;
}p_rq;

void test_processes(){
  p_rq p_rqs[MAX_PROCESSES];
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;

  while (1) {
    // Create MAX_PROCESSES processes
    for(rq = 0; rq < MAX_PROCESSES; rq++){
      p_rqs[rq].pid = my_create_process("endless_loop");  // TODO: Port this call as required

      if (p_rqs[rq].pid == -1){                           // TODO: Port this as required
        printf("Error creating process\n");               // TODO: Port this as required
        return;
      }else{
        p_rqs[rq].state = READY;
        alive++;
      }
    }

    int i;
    for (i = 0; i < 150000000; i++); // Busy wait so processes can execute

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0){

      for(rq = 0; rq < MAX_PROCESSES; rq++){
        for (i = 0; i < 30000000; i++);
        action = GetUniform(2) % 2; 

        switch(action){
          case 0:
            if (p_rqs[rq].state == READY || p_rqs[rq].state == BLOCKED){
              if (my_kill(p_rqs[rq].pid) == -1){          // TODO: Port this as required
                printf("Error killing process\n");        // TODO: Port this as required
                return;
              }
              printInt(p_rqs[rq].pid);
              printf(" was killed\n");
              p_rqs[rq].state = KILLED; 
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == READY){
              if(my_block(p_rqs[rq].pid) == -1){          // TODO: Port this as required
                printf("Error blocking process\n");       // TODO: Port this as required
                return;
              }
              printInt(p_rqs[rq].pid);
              printf(" was blocked\n");
              p_rqs[rq].state = BLOCKED; 
            }
            break;
        }
      }

      // Randomly unblocks processes
      for(rq = 0; rq < MAX_PROCESSES; rq++) {
        for (i = 0; i < 30000000; i++);
        if (p_rqs[rq].state == BLOCKED && GetUniform(2) % 2){
          if(my_unblock(p_rqs[rq].pid) == -1){            // TODO: Port this as required
            printf("Error unblocking process\n");         // TODO: Port this as required
            return;
          }
          printInt(p_rqs[rq].pid);
          printf(" was unblocked\n");
          p_rqs[rq].state = READY; 
        }
      }

    }
    printf("Succeed - restarting in 4s\n");
    sys_sleep(4);
  }
}
