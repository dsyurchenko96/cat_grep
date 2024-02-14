# Project Title
### cat_grep

## Table of Contents

- [About](#about)
- [Features](#features)
- [Build instructions](#build)
- [Usage](#usage)
- [Testing](#testing)

## About <a name = "about"></a>

A course project on reverse engineering functionality of basic Unix commands - cat and grep.

## Features <a name = "features"></a>
### s21_grep
- Support for regular expression pattern matching.
- Various options including -i for case-insensitive matching, -v for inverting matches, -c for counting matches, -l for printing filenames with matching lines, and more.
- Ability to read patterns from a file using the -f option.
### s21_cat
- Options for numbering lines (-n), numbering non-blank lines (-b), and squeezing consecutive blank lines into one (-s).
- Enhanced output with options like -e for displaying non-printable characters and -t for displaying tabs as ^I.
- Options for controlling the display of end-of-line markers (-e).

## Build instructions <a name = "build"></a>

To build the s21_grep and s21_cat executables, go to their respective directories run the following commands:

```
make
```
To clean up the build files, use:

```
make clean
```

## Usage <a name = "usage"></a>

### s21_grep
```
./s21_grep [OPTIONS] PATTERN [FILE...]
```
Options:

- -i: Perform case-insensitive matching.
- -v: Invert the sense of matching to select non-matching lines.
- -c: Suppress normal output; instead, print a count of matching lines.
- -l: Suppress normal output; instead, print the names of files with matching lines.
- -n: Prefix each line of output with its corresponding line number.
- -h: Suppress the display of filenames when searching multiple files.
- -s: Suppress error messages about nonexistent or unreadable files.
- -f FILE: Obtain patterns from FILE, one per line.
- -o: Show only the part of a line matching PATTERN.
- -e PATTERN: Add multiple patterns for search.

### s21_cat
```
./s21_cat [OPTIONS] [FILE...]
```
Options:

- -n: Number all output lines.
- -b: Number non-blank output lines.
- -s: Squeeze multiple adjacent empty lines into one.
- -e: Display non-printable characters in a visible format.
- -t: Display tabs as ^I.
- -v: Display non-printable characters in a visible format and display tabs as ^I.

## Testing <a name = "testing"></a>

Testing
To run automated tests for s21_grep and s21_cat, go to their respective directories and use:

```
make test
```
To run tests with memory leak detection, use:

```
make test_with_leaks
```

The tests use the compiled s21_ files and match their system equivalent output with any inconsistency stored in ```cmp.log```