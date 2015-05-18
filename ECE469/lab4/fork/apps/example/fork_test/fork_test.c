#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  int childpid;
  int apple = 1337;
  // Now print a message to show that everything worked
  Printf("fork_test (id=%d): Hello world!\n", getpid());
  Printf("fork_test (id=%d): Before fork -- apples=%d\n", getpid(), apple);  
  Printf("PID=%d: xxxxxxxxxx FORKING NOW xxxxxxx\n", getpid());
  childpid = fork();
  if(childpid != 0) {
    // parent process
    Printf("fork_test (id=%d): I am parent process\n", getpid());
    Printf("fork_test (id=%d): After fork -- apples=%d cpid=%d\n", getpid(), apple, childpid);
    apple = 1010;
    Printf("fork_test (id=%d): After fork -- apples=%d\n", getpid(), apple);
  }
  else {
    Printf("fork_test (id=%d): I am child process\n", getpid());
    Printf("fork_test (id=%d): After fork -- apples=%d cpid=%d\n", getpid(), apple, childpid);
    apple = 7777;
    Printf("fork_test (id=%d): After fork -- apples=%d\n", getpid(), apple);
  }

  Printf("PID=%d: xxxxxxxxxx Done xxxxxxxxxx\n", getpid());
}