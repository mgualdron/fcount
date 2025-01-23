#ifndef _FC_funcs_h
#define _FC_funcs_h

typedef struct FCount {
    int fieldcount;
    int recordcount;
} FCount;

typedef int (*FC_compare) (const void *a, const void *b);

FCount *FC_create (int fieldcount, int recordcount);

void FC_destroy(FCount *fc);

void FC_array_destroy(DArray *darray);

void FC_print(FCount *fc, char *filename);

void FC_array_print(DArray *darray, char *filename);

int FC_array_push(DArray *darray, int fieldcount);

int FC_cmp(const void *a, const void *b);

int FC_array_sort(DArray *darray, FC_compare cmp);

#endif
