#include <lcthw/darray.h>
#include <lcthw/dbg.h>
#include "fc_funcs.h"


// A function to create FCount struct instances:
struct FCount *FC_create (char *filename, int fieldcount, int recordcount) {
    FCount *fc = malloc(sizeof(FCount));

    // Did we successfully allocate space for a FCount?
    assert(fc != NULL);

    // Create a duplicate of the string filename:
    fc->filename = strdup(filename);

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

    // We free the name pointer separately because fc only has room for the
    // pointer to the filename string.
    free(fc->filename);

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
void FC_print(FCount *fc)
{
    // Did we get a valid pointer passed in?
    assert(fc != NULL);

    // Print it:
    printf("%d\t%d\t%s\n", fc->fieldcount, fc->recordcount, fc->filename);
}

// Print an array of FCount elements:
void FC_array_print(DArray *darray)
{
    int i = 0;

    // Did we get a valid pointer passed in?
    assert(darray != NULL);

    // Loop through the array:
    for (i = 0; i < darray->end; i++) {
        FC_print( (FCount *)(darray->contents[i]) );
    }

}

int FC_array_push(DArray *darray, char *filename, int fieldcount)
{
    assert(darray != NULL);
    int i = 0;
    int found = 0;

    for (i = 0; i < darray->end; i++) {
        char *array_filename = ( (FCount *)(darray->contents[i]) )->filename;
        int array_fieldcount = ( (FCount *)(darray->contents[i]) )->fieldcount;

        if ( strcmp(filename, array_filename) == 0 && fieldcount == array_fieldcount ) {
            ((FCount *)(darray->contents[i]))->recordcount++;
            found++;
        }
    }

    if (found == 0) {
        FCount *fc = FC_create(filename, fieldcount, 1);
        check(DArray_push(darray, fc) == 0, "Error pushing element into darray.");
    }

    return 0;
error:
    return -1;
}

