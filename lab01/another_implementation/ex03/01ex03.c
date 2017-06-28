#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>



#define INPUT_FILE_1_NAME "fv1.b"
#define INPUT_FILE_2_NAME "fv2.b"

sem_t client_request, server_response; // sync between clients and server
pthread_mutex_t clients_me, finished_barrier_me; // used only by clients
volatile int val, n_finished;

void server();
void *client_thread(void *);

int main(int argc, char *argv[]) {
    pthread_t t1, t2;

    sem_init(&client_request, 0, 0);
    sem_init(&server_response, 0, 0);
    pthread_mutex_init(&clients_me, NULL);
    pthread_mutex_init(&finished_barrier_me, NULL);

    if(pthread_create(&t1, NULL, client_thread, "1") != 0) {
        fprintf(stderr, "Error creating client thread 1\n");
        return 1;
    }
    if(pthread_create(&t2, NULL, client_thread, "2") != 0) {
        fprintf(stderr, "Error creating client thread 2\n");
        return 1;
    }

    server();

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_mutex_destroy(&clients_me);
    pthread_mutex_destroy(&finished_barrier_me);
    sem_destroy(&server_response);
    sem_destroy(&client_request);
    
    return 0;
}

void server() {
    int n_processed = 0;
    while(1) {
        sem_wait(&client_request);
        // TODO check ME
        if(n_finished == 2) {
            printf("Server processed %d requests\n.", n_processed);
            return;
        }
        val *= 2;
        sem_post(&server_response);
        n_processed++;
    }
}

void *client_thread(void *arg) {
    char *filename;
    char id;
    int fd;
    int val_private;
    
    id = ((char *)arg)[0];
    if(id == '1') {
        filename = INPUT_FILE_1_NAME;
    } else if(id == '2') {
        filename = INPUT_FILE_2_NAME;
    } else {
        fprintf(stderr, "Wrong parameter to client thread\n");
        exit(1);
    }
    fd = open(filename, O_RDONLY);
    if(fd == -1) {
        perror("error opening file");
        exit(2);
    }
    while(read(fd, &val_private, sizeof(int)) == sizeof(int)) {
        printf("client %d read value %d\n", id, val_private);
        pthread_mutex_lock(&clients_me);
        val = val_private;
        sem_post(&client_request);
        sem_wait(&server_response);
        val_private = val;
        pthread_mutex_unlock(&clients_me);
        printf("client %d new value %d\n", id, val_private);
    }
    pthread_mutex_lock(&finished_barrier_me);
    val_private = ++n_finished;
    pthread_mutex_unlock(&finished_barrier_me);
    if(val_private == 2) {
        // unlock server that is waiting
        sem_post(&client_request);
    }
    return NULL;
}