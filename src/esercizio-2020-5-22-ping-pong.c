/******* esercizio-2020-5-22-ping-pong ********/

/*Scrivere un programma che realizza un "ping-pong" tra due processi utilizzando una coppia di pipe,
 * una per ogni direzione.
 Il contatore (di tipo int) viene incrementato ad ogni ping ed ad ogni pong
 e viene trasmesso attraverso la pipe.
 Quanto il contatore raggiunge il valore MAX_VALUE il programma termina.
 proc_padre manda a proc_figlio il valore 0 attraverso pipeA.
 proc_figlio riceve il valore 0, lo incrementa (=1) e lo manda a proc_padre attraverso pipeB.
 proc_padre riceve il valore 1, lo incremente (=2) e lo manda a proc_figlio attraverso pipeA.
 proc_figlio riceve il valore 2 .....
 fino a MAX_VALUE, quando termina il programma.
 #define MAX_VALUE 1000000
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <errno.h>
#include <semaphore.h>

#define MAX_VALUE 1000000

#define CHECK_MALLOC(a,msg) {if ((a) == NULL) { perror((msg)); exit(EXIT_FAILURE); } }
#define CHECK_ERR(a,msg) {if ((a) == -1) { perror((msg)); exit(EXIT_FAILURE); } }

int *count;

int main(int argc, char *argv[]) {

	count = mmap(NULL,
			sizeof(int),
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS,
			-1,
			0);

	if (count == MAP_FAILED) {
		perror("mmap()");
		exit(EXIT_FAILURE);
	}

	*count = 0;

	int res;

	char * chil_buf = malloc(sizeof(int));
	char * parent_buf = malloc(sizeof(int));

	CHECK_MALLOC(chil_buf, "malloc() child")
	CHECK_MALLOC(parent_buf, "malloc() parent")

	int child_counter;
	int father_counter;

	int pipe_f[2];   // pipe father 2 child
	int pipe_c[2];   // pipe child 2 father
	// pipe[0] : estremità di lettura della pipe
	// pipe[1] : estremità di scrittura della pipe

	if (pipe(pipe_f) == -1) {
		perror("pipe()");
		exit(EXIT_FAILURE);
	}
	if (pipe(pipe_c) == -1) {
		perror("pipe()");
		exit(EXIT_FAILURE);
	}

	switch (fork()) {
	case -1:
		perror("problema con fork");
		exit(EXIT_FAILURE);
	case 0: // processo FIGLIO
		close(pipe_f[1]);  //Leggo dalla prima pipe e chiudo la scrittura
		close(pipe_c[0]);  //Scrivo nella seconda pipe e chiudo la lettura
		// fd1[0] LETTURA
		// fd2[1] SCRITTURA
		//child_process(&fd2[1], &fd1[0]);

		//pread(int fd, void *buf, size_t count, off_t offset);
		while ((res = read(pipe_f[0], chil_buf, sizeof(int))) > 0) {
			if (*count >= MAX_VALUE){
				close(pipe_c[1]);
				exit(EXIT_SUCCESS);
			}
			sscanf(chil_buf, "%d", &child_counter);
			printf("proc_figlio riceve il valore %d, ", child_counter);
			child_counter++;

			//char tempbuf[sizeof(int)];
			sprintf(chil_buf, "%d", child_counter);

			usleep(1000);

			res = write(pipe_c[1], chil_buf, strlen(chil_buf));

			CHECK_ERR(res, "write()")

			printf("lo incrementa (=%d) e lo manda a proc_padre attraverso pipe2\n",
					child_counter);

			*count += 1;

		}
		break;

	default:
		// processo PADRE

		close(pipe_f[0]);  // Scrivo nella prima pipe e chiudo la lettura
		close(pipe_c[1]);  // Leggo dalla seconda e chiudo la scrittura
		// fd1[1] SCRITTURA
		// fd2[0] LETTURA

		sprintf(parent_buf, "%d", 0);
		printf("[parent] tempbuff %s \n", parent_buf);

		int res = write(pipe_f[1], parent_buf, strlen(parent_buf));

		CHECK_ERR(res, "write()")

		printf("proc_padre manda a proc_figlio il valore %s attraverso pipe1\n",
				parent_buf);

		while ((res = read(pipe_c[0], parent_buf, sizeof(int))) > 0) {
			if (*count >= MAX_VALUE){
				close(pipe_f[1]);
				goto end;
			}

			sscanf(parent_buf, "%d", &father_counter);

			printf("proc_padre riceve il valore %d, ", father_counter);
			father_counter++;

			//char tempbuf[sizeof(int)];
			sprintf(parent_buf, "%d", father_counter);

			usleep(1000);

			res = write(pipe_f[1], parent_buf, strlen(parent_buf));
			CHECK_ERR(res, "write()")

			printf("lo incrementa (=%d) e lo manda a proc_figlio attraverso pipe1\n",
					father_counter);

			*count += 1;
		}

		end:
		wait(NULL);
		printf("Counter: %d\nBye!\n", *count);
		exit(EXIT_SUCCESS);
	}

}
