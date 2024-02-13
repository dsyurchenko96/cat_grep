#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NO_FILE_FOUND 2
#define INITIAL_CAPACITY 2

// #define RED "\x1B[31m"
// #define NO_COLOUR "\033[0m"

typedef struct Flags {
    bool e, v, c, l, n, h, f, o, print_filename;
} Flags;

typedef struct Patterns {
    int capacity;
    int num_patterns;
    bool are_compiled;
    int num_compiled;
    char **string_array;
    regex_t *regex_array;
} Patterns;

typedef struct Line {
    char *string;
    size_t alloc_size;
    ssize_t len;
    int num;
    bool skip;
} Line;

void initialize_patterns(Patterns *patterns);
void parse_flags(Flags *flags, int *exit_status, int opted, int *cflags);
int read_patterns(Patterns *patterns, const char *filename);
int add_pattern(Patterns *patterns, const char *pattern_string, size_t len);
int compile_patterns(Patterns *patterns, int cflags);
int process_file(FILE *file, const char *filename, Flags flags,
                 Patterns *patterns);
void free_all_patterns(Patterns *patterns);
void print_line(const char *line, size_t line_len, int line_num,
                const char *filename, Flags flags);
void print_limited_length(const char *line, size_t line_len);
int process_o(Line *line, const char *filename, Patterns *patterns, int pat_ind,
              Flags flags);