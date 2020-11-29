#include "lab1.h"
#include "lab2.h"
#include "lab6.h"
#include "ipc.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "pa2345.h"

int can_enter_cs(int *forks, int *dirty, int *reqf, int N, int proc_numb)
{
	int i;
	for(i = 1; i <= N; i++) {
		if (i == proc_numb)
			continue;
		if (forks[i] == 0)
			return 0;
		if (dirty[i] == 1 && reqf[i] == 1)
			return 0;
	}

	return 1;
}

void handle_requests(Message* msg, int proc_numb, SourceProc* selfStruct, int N,
					 int *reqf, int *forks, int *dirty, int *is_trying_cs, int one_loop)
{
	int i;

	while(1) {
		for (i = 1; i <= N; i++) {
			if(i == proc_numb) {
				continue;
			}

			if(receive(selfStruct, i, msg) == -1) {
				continue;
			}

			if(msg->s_header.s_type == CS_REPLY){
				forks[i] = 1;
				dirty[i] = 0;

				if (can_enter_cs(forks, dirty, reqf, N, proc_numb))
					return;

				continue;
			}

			if(msg->s_header.s_type == CS_REQUEST) {
				reqf[i] = 1;

				/* give forks if it is dirty and we have request for it*/
				if (dirty[i] == 1) {
					msg = create_message(CS_REPLY, NULL, 0);
					send(selfStruct, i, msg);

					/* clean forks after sending */
					dirty[i] = 0;
					forks[i] = 0;

					/* request back immediately if proc was trying to access CS*/
					if (*is_trying_cs) {
						msg = create_message(CS_REQUEST, NULL, 0);
						send(selfStruct, i, msg);
						reqf[i] = 0;
					}
				}

				continue;
			}
		}
		if (one_loop)
			return;
	}
}

/*
 * Method to request CS.
 * This methos exits when current process can enter cs.
 */
void cs_request(Message* msg, int proc_numb, SourceProc* selfStruct, int N,
				int *reqf, int *forks, int *dirty, int* is_trying_cs)
{
	int i;
	msg = create_message(CS_REQUEST, NULL, 0);

	/* in case it is the last process */
	if (can_enter_cs(forks, dirty, reqf, N, proc_numb))
		return;

	/* send requests to all neighbours */
	for(i = 1; i <= N; i++) {
		if (i == proc_numb || forks[i] == 1)
			continue;
		
		reqf[i] = 0;
		send(selfStruct, i, msg);
	}

	*is_trying_cs = 1;

	handle_requests(msg, proc_numb, selfStruct, N, reqf, forks, dirty, is_trying_cs, 0);

	*is_trying_cs = 0;

	/* make all forks dirty */
	for(i = 1; i <= N; i++) {
		dirty[i] = 1;
	}
}

/*
 * Method to release CS.
 */
void cs_release(Message* msg, int proc_numb, SourceProc* selfStruct, int N,
				int *reqf, int *forks, int *dirty, int *is_trying_cs)
{
	int i;

	/* process pending requests */
	for(i = 1; i <= N; i++) {
		if (reqf[i] == 1) {
			msg = create_message(CS_REPLY, NULL, 0);
			send(selfStruct, i, msg);
			
			/* clean forks after sending */
			dirty[i] = 0;
			forks[i] = 0;
		}
	}

	/* check new requests before new cs_request */
	handle_requests(msg, proc_numb, selfStruct, N, reqf, forks, dirty, is_trying_cs, 1);
}

int do_prints_mutexl(int*** matrix, local_id proc_numb, int N, int log_fd,
					 int *reqf, int *forks, int *dirty, int* is_trying_cs)
{

	Message* msg=(Message*)malloc(sizeof(Message));
	SourceProc* selfStruct = (SourceProc*)malloc(sizeof(SourceProc));
	int i;
	char buf[100];

	selfStruct->N = N;
	selfStruct->matrix = matrix;
	selfStruct->proc_id = proc_numb;
	selfStruct->fd = log_fd;

	for(i = 0; i < proc_numb * 5; i++) {
		cs_request(msg, proc_numb, selfStruct, N, reqf, forks, dirty, is_trying_cs);

		sprintf(buf, log_loop_operation_fmt, proc_numb, i+1, proc_numb * 5);
		print(buf);

		cs_release(msg, proc_numb, selfStruct, N, reqf, forks, dirty, is_trying_cs);
	}

	return 0;
}
