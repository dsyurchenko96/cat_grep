#define _GNU_SOURCE
#include "s21_cat.h"

int main(int argc, char **argv) {
    int exit_status = EXIT_SUCCESS;
    Flags flags = {false};
    int opted = 0;
    opterr = 0;
    struct option long_options[] = {{"number", no_argument, NULL, 'n'},
                                    {"number-nonblank", no_argument, NULL, 'b'},
                                    {"squeeze-blank", no_argument, NULL, 's'},
                                    {0, 0, 0, 0}};
    while ((opted = getopt_long(argc, argv, "beEnstTv", long_options, 0)) !=
               -1 &&
           !exit_status) {
        parse_flags(&flags, &exit_status, opted);
    }
    for (int i = optind; i < argc && !exit_status; i++) {
        const char *filename = argv[i];
        FILE *file = fopen(filename, "r");
        if (file == NULL) {
            exit_status = EXIT_FAILURE;
            fprintf(stderr, "cat: %s: No such file or directory\n", filename);
        } else {
            process_file(file, flags);
            fclose(file);
        }
    }
    return exit_status;
}

void parse_flags(Flags *flags, int *exit_status, int opted) {
    switch (opted) {
        case 'b':
            flags->b = true;
            break;
        case 'e':
            flags->e = true;
            flags->v = true;
            break;
        case 'E':
            flags->e = true;
            break;
        case 'n':
            flags->n = true;
            break;
        case 's':
            flags->s = true;
            break;
        case 't':
            flags->t = true;
            flags->v = true;
            break;
        case 'T':
            flags->t = true;
            break;
        case 'v':
            flags->v = true;
            break;
        case '?':
            fprintf(stderr, "cat: illegal option -- %c\n", optopt);
            *exit_status = EXIT_FAILURE;
            break;
        default:
            break;
    }
}

void process_file(FILE *file, Flags flags) {
    char *line = NULL;
    size_t alloc_size = 0;
    ssize_t line_len = 0;
    int line_num = 0;
    bool prev_line_empty = false;
    if (flags.b && flags.n) {
        flags.n = false;
    }
    while ((line_len = getline(&line, &alloc_size, file)) != -1) {
        if (!(flags.s && prev_line_empty && line[0] == '\n')) {
            if (!(flags.b && line[0] == '\n')) {
                line_num++;
            }
            if (flags.n || (flags.b && line[0] != '\n')) {
                printf("%6d\t", line_num);
            } else if (flags.e && flags.b &&
                       line[0] == '\n') {  // only for M1 Mac
                printf("%6c\t", ' ');
            }
            if (flags.v || flags.t || flags.e) {
                print_line_vte(line, flags, line_len);
            } else {
                for (ssize_t i = 0; i < line_len; i++) {
                    printf("%c", line[i]);
                }
            }
            prev_line_empty = true ? line[0] == '\n' : false;
        }
    }
    free(line);
}

void print_line_vte(const char *line, Flags flags, ssize_t len) {
    for (ssize_t i = 0; i < len; i++) {
        unsigned char symbol = line[i];
        if (flags.v && symbol < 32 && symbol != 9 && symbol != 10) {
            printf("^%c", symbol + NON_PRINT_SHIFT);
        } else if (flags.t && symbol == 9) {
            printf("^%c", symbol + NON_PRINT_SHIFT);
        } else if (flags.e && symbol == 10) {
            printf("$\n");
        } else if (flags.v && symbol == 127) {
            printf("^%c", symbol - NON_PRINT_SHIFT);
        } else if (flags.v && symbol > 127 && symbol < 160) {
            printf("M-^%c", symbol - NON_PRINT_SHIFT);
        } else {
            printf("%c", symbol);
        }
    }
}
