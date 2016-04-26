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
#include "fc_funcs.h"

static const char* program_name = "fcount";


static void try_help () {
    printf("Try '%s --help' for more information.\n", program_name);
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

            case ':':   /* missing option argument */
                fprintf(stderr, "%s: option '-%c' requires an argument\n",
                        argv[0], optopt);
                break;

            // getopt_long already printed an error message.
            case '?':
                try_help();
                break;

            default:
                usage(1);
        }
    }

    if (show_header && !be_quiet) {
        printf("field_count\trecords\tfile\n");
    }

    int j = optind;  // A copy of optind (the number of options at the command-line),
                     // which is not the same as argc, as that counts ALL
                     // arguments.  (optind <= argc).

    // Process any remaining command line arguments (input files).
    do {

        int fieldcount = 0;
        char *filename = NULL;
        char *line = NULL;
        char *token = NULL;
        FILE *fp = NULL;
        size_t len = 0;   // allocated size for line
        ssize_t read = 0; // num of chars read

        // The dynamic array that will hold all field counts:
        DArray *darray = DArray_create(sizeof(FCount), 10);

        // Assume STDIN if no additional arguments, else loop through them:
        if (optind == argc) {
            filename = "-";
        }
        else if (optind < argc) {
            filename = argv[j];
        }
        else if (optind > argc) {
            break;
        }

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

            // Add the count to the dynamic array:
            check(FC_array_push(darray, fieldcount) == 0, "Error pushing element into darray.");

            // If we have more than one field count in this file, set the
            // inconsistent_file flag to 2:
            if (darray->end > 1) {
                inconsistent_file = 2;
            }
        }

        free(line);
        fclose(fp);

        if (!be_quiet) {
            FC_array_print(darray, filename);
        }
        FC_array_destroy(darray);

        j++;

    } while (j < argc);

    if (be_quiet) {
        return inconsistent_file;
    }
    else {
        return 0;
    }

error:
    return -1;
}
