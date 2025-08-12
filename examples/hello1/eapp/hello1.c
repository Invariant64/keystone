#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main()
{
  printf("hello, world 1!\n");
  fflush(stdout);

  time_t last = time(NULL);
  while (1) {
    time_t now = time(NULL);
    if (now - last >= 1) { // 超过1秒
      printf("banana");
      fflush(stdout);
      last = now;
    }
    // 可以适当sleep减少CPU占用
    //usleep(10000); // 10ms
  }
  return 0;
}
