// -------------------------------------------------------------------------
// Program Name:    fcount.c
//
// Purpose:         To count the number of fields (columns) in a delimted
//                  text file.
//
// Notes:           Use a struct to store the field-count along with the
//                  frequency for each field-count.  Then with a dynamic
//                  array of structs, record all values observed, and
//                  report them at the end.
//
//                  The dynamic array implementation comes from "Learn C The
//                  Hard Way" by Zed Shaw, who also implemented the debug and
//                  unit testing macros.
// -------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <lcthw/darray.h>
#include <lcthw/dbg.h>
#include "fcount.h"

static const char* program_name = "fcount";

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

static void try_help () {
    printf("Try '%s -h' for more information.", program_name);
    exit(1);
}


static void usage (int status) {
    if (status != 0) {
        try_help();
    }
    else {
      printf ("\
Usage: %s [OPTION]... [FILE]...\n\
", program_name);

      fputs ("\
Print field (column) count and number of lines for each count.\n\
More than one FILE can be specified.\n\
", stdout);

      fputs ("\
\n\
  -d, --delimiter        the delimiting character for the input FILE(s)\n\
  -H, --header           print a header line with the output counts\n\
  -q, --quiet            do not output counts, but return 2 if\n\
                         multiple field counts are detected\n\
                         i.e. the file is inconsistent.\n\
", stdout);
    }

    exit (status);
}


static struct option long_options[] = {
    {"quiet",     no_argument,       0, 'q'},
    {"header",    no_argument,       0, 'H'},
    {"delimiter", required_argument, 0, 'd'},
    {"help",      no_argument,       0, 'h'},
    {0, 0, 0, 0}
};


int main (int argc, char *argv[])
{
    int c;
    char *delim = "\t";
    int be_quiet = 0;
    int show_header = 0;
    int inconsistent_file = 0;

    while (1) {

        // getopt_long stores the option index here.
        int option_index = 0;

        c = getopt_long (argc, argv, "hHqd:", long_options, &option_index);

        // Detect the end of the options.
        if (c == -1) break;

        switch (c) {
            case 0:
                // If this option set a flag, do nothing else now.
                break;

            case 'd':
                debug("option -d with value `%s'", optarg);
                delim = optarg;
                break;

            case 'h':
                debug("option -h");
                usage(0);
                break;

            case 'H':
                debug("option -H");
                show_header = 1;
                break;

            case 'q':
                debug("option -q");
                be_quiet = 1;
                break;

            // getopt_long already printed an error message.
            case '?':
                break;

            default:
                usage(1);
        }
    }

    if (show_header && !be_quiet) {
        printf("field_count\trecords\tfile\n");
    }

    int j = optind;  // A copy of optind (the number of options at the command-line)

    // Process any remaining command line arguments (input files).
    if (j <= argc) {

        do {

            char *filename = argv[j];
            char *line = NULL;
            size_t len = 0;   // allocated size for line
            ssize_t read = 0; // num of chars read
            char *token = NULL;
            int fieldcount = 0;
            DArray *darray = DArray_create(sizeof(FCount), 10);


            if (optind == argc) {
                filename = "-";
            }

            FILE *fp = NULL;

            if (filename[0] == '-') {
                fp = stdin;
            }
            else {
                fp = fopen(filename , "r");
            }

            check(fp != NULL, "Error opening file: %s.", filename);

            while ((read = getline(&line, &len, fp)) != -1) {
                fieldcount = 0;
                while ((token = strsep(&line, delim))) {
                    fieldcount++;
                }
                check(FC_array_push(darray, filename, fieldcount) == 0, "Error pushing element into darray.");
                if (darray->end > 1) { // If we have more than one field count in this file
                    inconsistent_file = 2;
                }
            }

            free(line);
            fclose(fp);

            if (!be_quiet) {
                FC_array_print(darray);
            }
            FC_array_destroy(darray);

            j++;

        } while (j < argc);

    }
  
    if (be_quiet) {
        return inconsistent_file;
    }
    else {
        return 0;
    }

error:
    return -1;
}
