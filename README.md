# fcount
![build status](https://github.com/mgualdron/fcount/actions/workflows/c-cpp.yml/badge.svg)

## Description

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


## Building fcount

The following projects have made `fcount` possible:

- [libcsv](https://github.com/rgamble/libcsv) - Version 3.0.3 of `libcsv` is included with `fcount`.
- [gnulib](https://www.gnu.org/software/gnulib/) - The `getline` module is included with `fcount` for portability.

Please consider contributing to those projects if you find `fcount` useful.

Note that this git repository does not include a `configure` script like a 
distribution tarball normally does.  If you don't want to bother with 
installing `autoconf` and `automake`, then download a distribution
[package](https://github.com/mgualdron/fcount/releases/download/v0.0.1/fcount-0.0.1.tar.gz)
and run `configure`:

```
./configure
```

If you want the `fcount
` binary installed in your `$HOME/bin`, you should 
run something like:

```
./configure --prefix=$HOME
```

If you're building from a copy of this git repository, you'll need to have 
`autoconf` and `automake` installed on your system, and run the following 
command to generate a `configure` script:

```
autoreconf -i
```

...and subsequently run `configure` as mentioned before.

After `configure` completes successfully, you can do the usual:

```
make
make check
make install
```

## Author

Miguel Gualdron (dev at gualdron.com).
