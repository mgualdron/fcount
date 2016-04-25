#ifndef _FC_funcs_h
#define _FC_funcs_h

typedef struct FCount {
    char *filename;
    int fieldcount;
    int recordcount;
} FCount;

struct FCount *FC_create (char *filename, int fieldcount, int recordcount);

void FC_destroy(FCount *fc);

void FC_array_destroy(DArray *darray);

void FC_print(FCount *fc);

void FC_array_print(DArray *darray);

int FC_array_push(DArray *darray, char *filename, int fieldcount);

#endif
