# fcount
![build status](https://github.com/mgualdron/fcount/actions/workflows/c-cpp.yml/badge.svg)

Count the number of fields (columns) in every line of a delimited text file.

Available command-line options:

    fcount --help

    Usage: fcount [OPTION]... [FILE]...

    Print field (column) count and number of lines for each count.
    More than one FILE can be specified.

      -d, --delimiter=DELIM  the delimiting character for the input FILE(s)
                             (the default is a TAB)
      -H, --header           print a header line with the output counts
      -l, --line-count       print the record-count only (like wc -l)
      -q, --quiet            do not output counts, but return 2 if
                             multiple field counts are detected
                             (i.e. the file is inconsistent)
      -C, --csv              parse CSV files
      -Q, --csv-quote        CSV quoting character (ignored unless --csv)
