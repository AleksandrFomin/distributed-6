#ifndef _LAB6_H
#define _LAB6_H

#include "ipc.h"

int do_prints_mutexl(int*** matrix, local_id proc_numb, int N, int log_fd,
					 int *reqf, int *forks, int *dirty, int* is_trying_cs);

#endif
