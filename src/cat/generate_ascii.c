#define _DEFAULT_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CYCLE_MAX 30
#define ASCII_MAX 128
#define CHARS_MAX 1024
#define WHITESPACE_MAX 128

int open_file(const char *directory, const char *filename);
void freeDirEnt(struct dirent **nameList, int n);
void generate_random(FILE *file);

int main() {
    int exit_status = EXIT_SUCCESS;
    srand(time(NULL));
    const char directory[] = "../../tests/cat_tests/";
    struct dirent **nameList;
    int n = scandir(directory, &nameList, NULL, alphasort);
    const char test_name[] = "auto_test_";
    if (n > 0) {
        for (int i = 0; i < n && !exit_status; i++) {
            if (strncmp(nameList[i]->d_name, test_name, strlen(test_name)) ==
                0) {
                open_file(directory, nameList[i]->d_name);
            }
        }
    }
    freeDirEnt(nameList, n);
    return exit_status;
}

int open_file(const char *directory, const char *filename) {
    int exit_status = EXIT_SUCCESS;
    char *path_with_filename = calloc(
        strlen(directory) + strlen(filename) + sizeof(char), sizeof(char));
    if (path_with_filename == NULL) {
        exit_status = EXIT_FAILURE;
    } else {
        strcpy(path_with_filename, directory);
        strcat(path_with_filename, filename);
        FILE *file = fopen(path_with_filename, "w");
        if (file == NULL) {
            exit_status = EXIT_FAILURE;
        } else {
            generate_random(file);
            fclose(file);
        }
        free(path_with_filename);
    }
    return exit_status;
}

void generate_random(FILE *file) {
    int max_cycles = rand() % CYCLE_MAX;
    const char whitespace[] = {' ', '\n', '\t'};
    int arr_len = sizeof(whitespace) / sizeof(whitespace[0]);
    for (int cycle = 0; cycle < max_cycles; cycle++) {
        int max_characters = rand() % CHARS_MAX;
        for (int i = 0; i < max_characters; i++) {
            int symbol = rand() % ASCII_MAX;
            fputc(symbol, file);
        }
        int max_space = rand() % WHITESPACE_MAX;
        for (int j = 0; j < max_space; j++) {
            int space_ind = rand() % arr_len;
            fputc(whitespace[space_ind], file);
        }
    }
}

void freeDirEnt(struct dirent **nameList, int n) {
    while (n--) {
        free(nameList[n]);
    }
    free(nameList);
}