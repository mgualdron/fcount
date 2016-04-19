// -------------------------------------------------------------------------
// Program Name:    fcount.c
//
// Purpose:         To count the number of fields (columns) in a delimted
//                  text file.
//
// Notes:           Use a struct to store the field-count and file name,
//                  along with the frequency for each field-count.  Then with
//                  an array of structs, record all values observed, and
//                  report them at the end.
// -------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <lcthw/darray.h>
#include <lcthw/dbg.h>
#include "fcount.h"


static int verbose_flag;


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
void FC_destroy(FCount *fc) {

    // Did we get a valid pointer passed in?
    assert(fc != NULL);

    // We free the name pointer separately because fc only has room for the
    // pointer to the filename string.
    free(fc->filename);

    // Free the space used by the FCount struct instance:
    free(fc);
}

// Reclaim memory from the heap after we discard a FCount DArray:
void FC_array_destroy(DArray *darray) {
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


int main (int argc, char *argv[])
{
    int c;
    char *delim = "\t";

    while (1) {
        static struct option long_options[] = {
            // These options set a flag.
            {"verbose", no_argument,       &verbose_flag, 1},
            {"brief",   no_argument,       &verbose_flag, 0},
            // These options don't set a flag.
            {"delimiter", required_argument, 0, 'd'},
            {"lines",     no_argument,       0, 'l'},
            {0, 0, 0, 0}
        };

        // getopt_long stores the option index here.
        int option_index = 0;

        c = getopt_long (argc, argv, "ld:", long_options, &option_index);

        // Detect the end of the options.
        if (c == -1) break;

        switch (c) {
            case 0:
                // If this option set a flag, do nothing else now.
                if (long_options[option_index].flag != 0) {
                  break;
                }

                debug("option %s", long_options[option_index].name);
                if (optarg) {
                  debug(" ...with arg %s", optarg);
                }
                //printf("\n");

                break;

            case 'l':
                debug("option -l");
                break;

            case 'd':
                debug("option -d with value `%s'\n", optarg);
                delim = optarg;
                break;

            // getopt_long already printed an error message.
            case '?':
                break;

            default:
                abort ();
        }
    }

    // Report the final status resulting from "--verbose" or "--brief"
    if (verbose_flag) {
        debug("verbose flag is set");
    }
  
    // Process any remaining command line arguments (input files).
    if (optind < argc) {
        while (optind < argc) {

            char *filename = argv[optind];
            char *line = NULL;
            size_t len = 0;   // allocated size for line
            ssize_t read = 0; // num of chars read
            char *token;
            int fieldcount = 0;
            DArray *darray = DArray_create(sizeof(FCount), 10);

            debug("filename = %s", filename);

            FILE *fp = fopen(filename , "r");
            check(fp != NULL, "Error opening file: %s.", filename);

            while ((read = getline(&line, &len, fp)) != -1) {
                fieldcount = 0;
                while ((token = strsep(&line, delim))) {
                    fieldcount++;
                }
                check(FC_array_push(darray, filename, fieldcount) == 0, "Error pushing element into darray.");
            }

            free(line);
            fclose(fp);

            FC_array_print(darray);
            FC_array_destroy(darray);

            optind++;
        }
    }
  
    return 0;

error:
    return -1;
}
