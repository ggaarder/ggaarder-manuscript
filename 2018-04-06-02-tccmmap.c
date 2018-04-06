/*
The built-in memory and bounds checker cannot handle mmap'd memory and judges errno wrong

Hi,

I just tried -b to see whether I forgot to free something, and I found that the memory and bounds checker will alert when I use the memory I mmap'd. While I was writing a demo for report, I found that the checker also reported out of region when I accessed errno. I was to print errno to see if I really opened the file to be mmap'd.

I think the warning for the mmap'd memory is not a problem, just needed to be pointed out in the document. But the errno one is quite strange.

The demo is sent together as an attachment.

Output:
  $ tcc 2018-04-06-02-tccmmap.c -b
  $ ./a.out 
  errno demo: 11539
  bcheck.c __bound_ptr_indir4: 0x7f3a50abc698 is outside of the region
  11539: Signal SIGSEGV (11) received
  mmap demo: 11540
  mmap demo no read: 11541
  bcheck.c __bound_ptr_indir1: 0x7f3a50acf003 is outside of the region
  11540: Signal SIGSEGV (11) received

My TCC Version:
  $ tcc -v
  tcc version 0.9.27 (x86_64 Linux)

By the way, this project is extraordinary great! I use it all the time. It is so wonderful for me to find people like you who remember what good software looks like.

Regards,
George Gaarder

Sent to  tinycc-devel@nongnu.org
*/
  
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void handleseg(int signum) {
  fprintf(stderr, "%d: Signal SIGSEGV (%d) received\n", getpid(), signum);
  _exit(EXIT_SUCCESS);
}

int main() {
  int fd;
  pid_t pid = fork();
  signal(SIGSEGV, handleseg);

  if (pid == 0) {
    printf("errno: %d\n", errno);
    _exit(EXIT_SUCCESS);
  } else
    printf("errno demo: %d\n", pid);

  pid = fork();

  if (pid == 0) {
    fd = open("/usr/include/stdio.h", O_RDONLY);
    char *buf = mmap(NULL, BUFSIZ, PROT_READ, MAP_PRIVATE, fd, 0);
    char here = buf[3];
    munmap(buf, BUFSIZ);
    close(fd);
    _exit(EXIT_SUCCESS);
  } else
    printf("mmap demo: %d\n", pid);

  pid = fork();

  if (pid == 0) {
    fd = open("/usr/include/stdio.h", O_RDONLY);
    char *buf = mmap(NULL, BUFSIZ, PROT_READ, MAP_PRIVATE, fd, 0);
    munmap(buf, BUFSIZ);
    close(fd);
    _exit(EXIT_SUCCESS);
  } else
    printf("mmap demo no read: %d\n", pid);
    
  return EXIT_SUCCESS;
}
