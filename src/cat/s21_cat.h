#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NON_PRINT_SHIFT 64

typedef struct Flags {
  bool b, e, n, s, t, v;
} Flags;

void parse_flags(Flags *flags, int *exit_status, int opted);
void process_file(FILE *file, Flags flags);
void print_line_vte(const char *line, Flags flags, ssize_t len);