#include <stdio.h>
#include <unistd.h>
#include <time.h>

static int a = 1;

int main()
{
  printf("hello, world!\n");
  fflush(stdout);

  time_t last = time(NULL);
  while (1) {
    time_t now = time(NULL);
    if (now - last >= 1) { // 超过1秒
      printf("apple");
      fflush(stdout);
      last = now;
    }
    // 可以适当sleep减少CPU占用
    //usleep(10000); // 10ms
  }
  return 0;
}
