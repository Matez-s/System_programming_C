#ifndef DOMACE_ZADANIE_1_UTILS_H
#define DOMACE_ZADANIE_1_UTILS_H

#include <stdio.h>
#include <string.h>

#define LINE_SIZE 8192

int hex_encode(const unsigned char *in, size_t inlen, char *out, size_t outlen);
int hex_decode(const char *in, unsigned char *out, size_t outlen);

void trim_newline(char *s);
void split_first(char *line, char **head, char **tail);
void split_two(char *line, char **a, char **b, char **rest);
char *strdup_safe(const char *s);

void print_help(void);
static void process_input(void);

int read_file(const char *path, unsigned char **out, size_t *outlen);
int write_file(const char *path, const unsigned char *data, size_t len);
int mkdir_p(const char *path);

void swap(void *a, void *b, size_t item_size);
void insert_sort(void *array, size_t item_size, size_t n, int (*cmp_item) (const void *, const void*));
int cmp_int(const void *a, const void *b);
int cmp_double(const void *a, const void *b);

void random_bytes(unsigned char *buf, size_t len);

#endif //DOMACE_ZADANIE_1_UTILS_H