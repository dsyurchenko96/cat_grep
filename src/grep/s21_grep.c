#define _GNU_SOURCE
#include "s21_grep.h"

int main(int argc, char *argv[]) {
  int exit_status = EXIT_SUCCESS;
  Flags flags = {false};
  Patterns patterns;
  initialize_patterns(&patterns);
  int opted = 0;
  opterr = 0;
  int cflags = REG_NEWLINE;  // change to REG_EXTENDED for linux(?)
  while ((opted = getopt(argc, argv, "e:ivclnhsf:o")) != -1 &&
         exit_status == EXIT_SUCCESS) {
    parse_flags(&flags, &exit_status, opted, &cflags);
    if (exit_status == EXIT_SUCCESS) {
      if (optopt == 'e') {
        exit_status = add_pattern(&patterns, optarg, strlen(optarg));
      } else if (optopt == 'f') {
        exit_status = read_patterns(&patterns, optarg);
      }
    }
  }
  if (optind >= argc) {
    exit_status = EXIT_FAILURE;
  }
  if (flags.o && (flags.v || flags.c || flags.l)) {
    flags.o = false;
  }
  int first_file_ind;
  if (exit_status == EXIT_SUCCESS && optarg == NULL) {
    exit_status = add_pattern(&patterns, argv[optind], strlen(argv[optind]));
    first_file_ind = optind + 1;
  } else {
    first_file_ind = optind;
  }
  if (argc - first_file_ind > 1 && !flags.h) {
    flags.print_filename = true;
  }
  if (exit_status == EXIT_SUCCESS) {
    exit_status = compile_patterns(&patterns, cflags);
  }
  bool is_match = false;
  for (int i = first_file_ind; i < argc && exit_status != EXIT_FAILURE; i++) {
    FILE *file = fopen(argv[i], "r");
    if (file == NULL) {
      exit_status = NO_FILE_FOUND;
      fprintf(stderr, "grep: %s: No such file or directory\n", argv[i]);
    } else {
      int matches = process_file(file, argv[i], flags, &patterns);
      if (matches > 0) {
        if (flags.l) {
          puts(argv[i]);
        }
        is_match = true;
      }
      fclose(file);
    }
  }
  free_all_patterns(&patterns);
  if (is_match == false) {
    exit_status = EXIT_FAILURE;
  }
  return exit_status;
}

void initialize_patterns(Patterns *patterns) {
  patterns->capacity = INITIAL_CAPACITY;
  patterns->num_patterns = 0;
  patterns->are_compiled = false;
  patterns->num_compiled = 0;
  patterns->string_array = NULL;
  patterns->regex_array = NULL;
}

void parse_flags(Flags *flags, int *exit_status, int opted, int *cflags) {
  if (opted == 'e') {
    flags->e = true;
  } else if (opted == 'i') {
    *cflags |= REG_ICASE;
  } else if (opted == 'v') {
    flags->v = true;
  } else if (opted == 'c') {
    flags->c = true;
  } else if (opted == 'l') {
    flags->l = true;
  } else if (opted == 'n') {
    flags->n = true;
  } else if (opted == 'h') {
    flags->h = true;
  } else if (opted == 's') {
    if (freopen("/dev/null", "w", stderr) == NULL) {
      *exit_status = EXIT_FAILURE;
    }
  } else if (opted == 'f') {
    flags->f = true;
  } else if (opted == 'o') {
    flags->o = true;
  } else {
    if (optopt == 'e' || optopt == 'f') {
      fprintf(stderr, "grep: option requires an argument -- %c\n", optopt);
    } else {
      fprintf(stderr, "grep: invalid option -- %c\n", optopt);
    }
    *exit_status = EXIT_FAILURE;
  }
}

int read_patterns(Patterns *patterns, const char *filename) {
  int exit_status = EXIT_SUCCESS;
  FILE *pattern_file = fopen(filename, "r");
  if (pattern_file == NULL) {
    exit_status = EXIT_FAILURE;
    fprintf(stderr, "grep: %s: No such file or directory\n", filename);
  } else {
    char *line = NULL;
    size_t alloc_size = 0;
    ssize_t line_len = 0;
    while ((line_len = getline(&line, &alloc_size, pattern_file)) != -1 &&
           exit_status == EXIT_SUCCESS) {
      if (line[line_len - 1] == '\n') {
        line_len--;
      }
      exit_status = add_pattern(patterns, line, line_len);
    }
    free(line);
    fclose(pattern_file);
  }
  return exit_status;
}

int process_file(FILE *file, const char *filename, Flags flags,
                 Patterns *patterns) {
  Line line;
  line.string = NULL;
  line.alloc_size = 0;
  line.len = 0;
  line.num = 1;
  int num_matches = 0;
  while ((line.len = getline(&line.string, &line.alloc_size, file)) != -1) {
    line.skip = false;
    for (int pat_ind = 0; pat_ind < patterns->num_patterns; pat_ind++) {
      if (flags.o) {
        num_matches += process_o(&line, filename, patterns, pat_ind, flags);
      } else if (regexec(&patterns->regex_array[pat_ind], line.string, 0, NULL,
                         0) == flags.v &&
                 !line.skip) {
        print_line(line.string, line.len, line.num, filename, flags);
        line.skip = true;
        num_matches++;
      }
    }
    line.num++;
  }
  if (flags.c) {
    if (flags.print_filename) {
      printf("%s:", filename);
    }
    if (flags.l) {
      num_matches = num_matches > 0 ? 1 : 0;
    }
    printf("%d\n", num_matches);
  }
  free(line.string);
  return num_matches;
}

void print_line(const char *line, size_t line_len, int line_num,
                const char *filename, Flags flags) {
  if (!flags.c && !flags.l) {
    if (flags.print_filename) {
      printf("%s:", filename);
    }
    if (flags.n) {
      printf("%d:", line_num);
    }
    print_limited_length(line, line_len);
  }
}

void print_limited_length(const char *line, size_t line_len) {
  for (size_t i = 0; i < line_len; i++) {
    printf("%c", line[i]);
  }
  if (line[line_len - 1] != '\n') {
    printf("\n");
  }
}

int process_o(Line *line, const char *filename, Patterns *patterns, int pat_ind,
              Flags flags) {
  regmatch_t pmatch;
  int eflags = 0;
  bool match = false;
  ssize_t offset = 0;

  while (regexec(&patterns->regex_array[pat_ind], line->string + offset, 1,
                 &pmatch, eflags) == 0 ||
         offset > line->len) {
    eflags = REG_NOTBOL;  // do not let ^ match again.
    if (!match && !line->skip) {
      print_line(line->string + offset + pmatch.rm_so,
                 pmatch.rm_eo - pmatch.rm_so, line->num, filename, flags);
    } else {
      print_limited_length(line->string + offset + pmatch.rm_so,
                           pmatch.rm_eo - pmatch.rm_so);
    }

    match = true;
    line->skip = true;

    offset += pmatch.rm_eo;
    if (pmatch.rm_so == pmatch.rm_eo) {
      offset += 1;
    }
  }
  return match;
}

int add_pattern(Patterns *patterns, const char *pattern_string, size_t len) {
  int exit_status = EXIT_SUCCESS;
  if (patterns->num_patterns == 0) {
    patterns->string_array = calloc(patterns->capacity, sizeof(char *));
  } else if (patterns->num_patterns >= patterns->capacity) {
    patterns->capacity *= 2;
    patterns->string_array =
        realloc(patterns->string_array, sizeof(char *) * patterns->capacity);
  }
  if (patterns->string_array == NULL) {
    exit_status = EXIT_FAILURE;
  } else {
    patterns->num_patterns++;
    patterns->string_array[patterns->num_patterns - 1] =
        calloc(len + 1, sizeof(char));
    if (patterns->string_array[patterns->num_patterns - 1] == NULL) {
      exit_status = EXIT_FAILURE;
    }
    strncpy(patterns->string_array[patterns->num_patterns - 1], pattern_string,
            len);
    patterns->string_array[patterns->num_patterns - 1][len] = '\0';
  }
  return exit_status;
}

int compile_patterns(Patterns *patterns, int cflags) {
  int exit_status = EXIT_SUCCESS;
  patterns->regex_array = calloc(patterns->num_patterns, sizeof(regex_t));
  patterns->are_compiled = true;
  if (patterns->regex_array == NULL) {
    exit_status = EXIT_FAILURE;
  } else {
    for (int i = 0; i < patterns->num_patterns && !exit_status; i++) {
      patterns->num_compiled++;
      if (regcomp(&patterns->regex_array[i], patterns->string_array[i],
                  cflags) != 0) {
        exit_status = EXIT_FAILURE;
      }
    }
  }
  return exit_status;
}

void free_all_patterns(Patterns *patterns) {
  for (int i = 0; i < patterns->num_patterns; i++) {
    free(patterns->string_array[i]);
    if (patterns->are_compiled && i < patterns->num_compiled) {
      regfree(&patterns->regex_array[i]);
    }
  }
  free(patterns->string_array);
  if (patterns->are_compiled) {
    free(patterns->regex_array);
  }
}