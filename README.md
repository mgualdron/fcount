#fcount

Count the number of fields (columns) in every line of a delimited text file.

Available command-line options:

    fcount --help

    Usage: fcount [OPTION]... [FILE]...

    Print field (column) count and number of lines for each count.
    More than one FILE can be specified.

      -d, --delimiter        the delimiting character for the input FILE(s)
      -H, --header           print a header line with the output counts
      -q, --quiet            do not output counts, but return 2 if
                             multiple field counts are detected
                             i.e. the file is inconsistent.
