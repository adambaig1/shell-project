#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main(int argc, char *argv[]) {
	
  int ut = uptime();
	
  printf("uptime = %d\n", ut);
	
  exit(0);
};
