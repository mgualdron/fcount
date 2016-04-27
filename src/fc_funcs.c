#include <lcthw/darray.h>
#include <lcthw/dbg.h>
#include "fc_funcs.h"


// A function to create FCount struct instances:
struct FCount *FC_create (int fieldcount, int recordcount)
{
    FCount *fc = malloc(sizeof(FCount));

    // Did we successfully allocate space for a FCount?
    assert(fc != NULL);

    // Assign the counts:
    fc->fieldcount = fieldcount;
    fc->recordcount = recordcount;

    return fc;
}

// Reclaim memory from the heap after we discard a FCount instance:
void FC_destroy(FCount *fc)
{

    // Did we get a valid pointer passed in?
    assert(fc != NULL);

    // Free the space used by the FCount struct instance:
    free(fc);
}

// Reclaim memory from the heap after we discard a FCount DArray:
void FC_array_destroy(DArray *darray)
{
    int i = 0;

    // Did we get a valid pointer passed in?
    assert(darray != NULL);

    // Loop through the elements and free each separately:
    for (i = 0; i < darray->end; i++) {
        FC_destroy( (FCount *)(darray->contents[i]) );
    }

    // Free the array struct itself:
    DArray_destroy(darray);
}

// Print an instance of FCount:
void FC_print(FCount *fc, char *filename)
{
    // Did we get a valid pointer passed in?
    assert(fc != NULL);

    // Print it:
    printf("%d\t%d\t%s\n", fc->fieldcount, fc->recordcount, filename);
}

// Print an array of FCount elements:
void FC_array_print(DArray *darray, char *filename)
{
    int i = 0;

    // Did we get a valid pointer passed in?
    assert(darray != NULL);

    // Loop through the array:
    for (i = 0; i < darray->end; i++) {
        FC_print( (FCount *)(darray->contents[i]), filename );
    }

}

int FC_array_push(DArray *darray, int fieldcount)
{
    assert(darray != NULL);
    int i = 0;
    int found = 0;

    for (i = 0; i < darray->end; i++) {
        int array_fieldcount = ( (FCount *)(darray->contents[i]) )->fieldcount;

        if ( fieldcount == array_fieldcount ) {
            ((FCount *)(darray->contents[i]))->recordcount++;
            found++;
            break;  // Only one count matches, so break if we found it
        }
    }

    if (found == 0) {
        FCount *fc = FC_create(fieldcount, 1);
        check(DArray_push(darray, fc) == 0, "Error pushing element into darray.");
    }

    return 0;
error:
    return -1;
}

int FC_cmp(const void *a, const void *b)
{
    // a is a pointer to an element of an array holding a pointer to a struct:
    int x = (*(FCount **)a)->recordcount;
    int y = (*(FCount **)b)->recordcount;

    debug("x = %d, y = %d, x-y = %d", x, y, x-y);
    return (y - x);
}

int FC_array_sort(DArray *darray, FC_compare FC_cmp)
{
    debug("FCount  = %lu", sizeof(FCount));
    debug("FCount* = %lu", sizeof(FCount *));
    debug("void  * = %lu", sizeof(void *));
    qsort(darray->contents, DArray_count(darray), sizeof(void *), FC_cmp);
    return 0;
}
