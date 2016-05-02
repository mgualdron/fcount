// -------------------------------------------------------------------------
// Program Name:    fcount.c
//
// Purpose:         To count the number of fields (columns) in a delimted
//                  text file by scanning every row, and summarizing the
//                  number of lines having each field count.
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
#include "../libcsv/csv.h"

static const char* program_name = "fcount";
static unsigned int fieldcount = 0;
// static unsigned long linecount = 0;

// The callbacks for CSV processing:
void cb1 (void *s, size_t len, void *data);
void cb2 (int c, void *data);

static void try_help (int status) {
    printf("Try '%s --help' for more information.\n", program_name);
    exit(status);
}


static void usage (int status) {
    if (status != 0) {
        try_help(status);
    }
    else {
      printf ("\
Usage: %s [OPTION]... [FILE]...\n\
", program_name);

      printf ("\
Print field (column) count and number of lines for each count.\n\
More than one FILE can be specified.\n\
");

      printf ("\
\n\
  -d, --delimiter        the delimiting character for the input FILE(s)\n\
  -H, --header           print a header line with the output counts\n\
  -q, --quiet            do not output counts, but return 2 if\n\
                         multiple field counts are detected\n\
                         i.e. the file is inconsistent.\n\
");
    }

    exit (status);
}


static struct option long_options[] = {
    {"quiet",     no_argument,       0, 'q'},
    {"csv",       no_argument,       0, 'C'},
    {"header",    no_argument,       0, 'H'},
    {"delimiter", required_argument, 0, 'd'},
    {"help",      no_argument,       0, 'h'},
    {0, 0, 0, 0}
};


// Callback 1 for CSV support, called whenever a field is processed:
void cb1 (void *s, size_t len, void *data)
{
    fieldcount++;
}

// Callback 2 for CSV support, called whenever a record is processed:
void cb2 (int c, void *data)
{
    check(FC_array_push((DArray *)data, fieldcount) == 0, "Error pushing element into darray.");
    fieldcount = 0;

    return;

error:
    exit(1);
}

int file_count(char *filename, DArray *darray, char *delim)
{
    char *line = NULL;
    char *end = NULL;
    char *token = NULL;
    FILE *fp = NULL;
    size_t len = 0;   // allocated size for line
    ssize_t bytes_read = 0; // num of chars read

    if (filename[0] == '-') {
        fp = stdin;
    }
    else {
        fp = fopen(filename, "rb");
    }

    check(fp != NULL, "Error opening file: %s.", filename);

    while ((bytes_read = getline(&line, &len, fp)) != -1) {

        // strsep() will modify it's first argument, so we make a copy:
        end = line;

        fieldcount = 0;
        while ((token = strsep(&end, delim))) {
            fieldcount++;
        }

        // Add the count to the dynamic array:
        check(FC_array_push(darray, fieldcount) == 0, "Error pushing element into darray.");
    }

    free(line);
    fclose(fp);

    return 0;

error:
    return -1;
}

int file_count_csv(char *filename, DArray *darray)
{
    struct csv_parser p;
    char buf[1024];
    FILE *fp = NULL;
    size_t bytes_read = 0; // num of chars read

    if (filename[0] == '-') {
        fp = stdin;
    }
    else {
        fp = fopen(filename, "rb");
    }

    check(fp != NULL, "Error opening file: %s.", filename);

    check(csv_init(&p, 0) == 0, "Error initializing CSV parser.");

    while ((bytes_read=fread(buf, 1, 1024, fp)) > 0) {
        check(csv_parse(&p, buf, bytes_read, cb1, cb2, darray) == bytes_read, "Error while parsing file: %s", csv_strerror(csv_error(&p)));
    }

    csv_fini(&p, cb1, cb2, darray);
    csv_free(&p);

    fclose(fp);

    return 0;

error:
    return -1;
}

int main (int argc, char *argv[])
{
    int c;
    char *delim = "\t";
    int be_quiet = 0;
    int csv_mode = 0;
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

            case 'C':
                debug("option -C");
                csv_mode = 1;
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
                usage(1);
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

        char *filename = NULL;

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

        // Count the file:
        if (csv_mode) {
            check(file_count_csv(filename, darray) == 0, "Error counting CSV file: %s", filename);
        }
        else {
            check(file_count(filename, darray, delim) == 0, "Error counting file: %s", filename);
        }

        // If we have more than one field count in this file, set the
        // inconsistent_file flag to 2:
        if (darray->end > 1) {
            inconsistent_file = 2;
        }

        if (!be_quiet) {
            FC_array_sort(darray, FC_cmp);
            FC_array_print(darray, filename);
        }
        FC_array_destroy(darray);
        DArray_clear(darray);

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
