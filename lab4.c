#include "lab1.h"
#include "lab2.h"
#include "lab4.h"
#include "ipc.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "priority_queue.h"
#include "pa2345.h"

int do_prints(local_id proc_numb, int N){
	int i;
	char buf[100];

	for(i = 0;i<proc_numb*5;i++){
		sprintf(buf, log_loop_operation_fmt, proc_numb, i+1, proc_numb*5);
		print(buf);
	}
	return 0;
}
