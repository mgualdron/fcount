#ifndef _FC_funcs_h
#define _FC_funcs_h

typedef struct FCount {
    int fieldcount;
    int recordcount;
} FCount;

struct FCount *FC_create (int fieldcount, int recordcount);

void FC_destroy(FCount *fc);

void FC_array_destroy(DArray *darray);

void FC_print(FCount *fc, char *filename);

void FC_array_print(DArray *darray, char *filename);

int FC_array_push(DArray *darray, int fieldcount);

#endif
