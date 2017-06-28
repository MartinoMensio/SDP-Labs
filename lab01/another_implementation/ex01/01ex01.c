#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#define TXT_F1_NAME "fv1.txt"
#define TXT_F2_NAME "fv2.txt"
#define BIN_F1_NAME "fv1.b"
#define BIN_F2_NAME "fv2.b"

int comparator(const void *a, const void *b) {
    return *(int *)a - *(int *)b;
}

int main(int argc, char *argv[]) {
    int n1, n2;
    int *v1, *v2;
    int i;
    FILE *txt_f1, *txt_f2;
    int bin_f1, bin_f2;

    if(argc != 3) {
        fprintf(stderr, "Usage: %s <n1> <n2>\n", argv[0]);
        return 1;
    }
    n1 = atoi(argv[1]);
    if(n1 <= 0) {
        fprintf(stderr, "n1 must be a valid positive integer number\n");
        return 2;
    }
    n2 = atoi(argv[2]);
    if(n2 <= 0) {
        fprintf(stderr, "n2 must be a valid positive integer number\n");
        return 3;
    }

    v1 = calloc(n1, sizeof(int));
    if(v1 == NULL) {
        fprintf(stderr, "Error allocating v1\n");
        return 4;
    }
    v2 = calloc(n2, sizeof(int));
    if(v2 == NULL) {
        fprintf(stderr, "Error allocating v2\n");
        return 5;
    }
    
    srand(time(NULL));

    for(i = 0; i < n1; i++) {
        v1[i] = rand() % 91 + 10;
    }
    for(i = 0; i < n2; i++) {
        v2[i] = rand() % 81 + 20;
    }
    qsort(v1, n1, sizeof(int), comparator);
    qsort(v2, n2, sizeof(int), comparator);

    txt_f1 = fopen(TXT_F1_NAME, "w");
    if(txt_f1 == NULL) {
        fprintf(stderr, "Error creating output file %s\n", TXT_F1_NAME);
        return 6;
    }
    for(i = 0; i < n1; i++) {
        fprintf(txt_f1, "%d ", v1[i]);
    }
    fclose(txt_f1);

    txt_f2 = fopen(TXT_F2_NAME, "w");
    if(txt_f2 == NULL) {
        fprintf(stderr, "Error creating output file %s\n", TXT_F2_NAME);
        return 6;
    }
    for(i = 0; i < n2; i++) {
        fprintf(txt_f2, "%d ", v2[i]);
    }
    fclose(txt_f2);

    bin_f1 = creat(BIN_F1_NAME, 640);
    if(bin_f1 == -1) {
        perror("Impossible to create output file");
        return 7;
    }
    write(bin_f1, v1, n1 * sizeof(int));
    close(bin_f1);

    bin_f2 = creat(BIN_F2_NAME, 640);
    if(bin_f2 == -1) {
        perror("Impossible to create output file");
        return 7;
    }
    write(bin_f2, v2, n2 * sizeof(int));
    close(bin_f2);

    free(v1);
    free(v2);

    return 0;
}
