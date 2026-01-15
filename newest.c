// main.c (C99/C11) - UdSP skuska 15.01.2026

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ---------------- ULOHA 1 ---------------- */

char* string_remove_char(char* str, char c) {
    // bez strlen, bez indexov, bez funkcii zo standardnej kniznice
    if (!str) return str;

    char* r = str;   // read
    char* w = str;   // write

    while (*r != '\0') {
        if (*r != c) {
            *w = *r;
            ++w;
        }
        ++r;
    }
    *w = '\0';
    return str;
}

/* ---------------- ULOHA 2 ---------------- */

size_t file_digits_count(const char* file_name) {
    if (!file_name) return 0;

    FILE* f = fopen(file_name, "rb");
    if (!f) return 0;

    size_t cnt = 0;
    int ch;
    while ((ch = fgetc(f)) != EOF) {
        if (ch >= '0' && ch <= '9') cnt++;
    }

    fclose(f);
    return cnt;
}

void test_file_digits_count(void) {
    printf("Pocet cislic v wikipedia.txt: %zu\n", file_digits_count("wikipedia.txt"));
}

/* ---------------- ULOHA 3 ---------------- */

typedef struct {
    char*  data;        // jeden velky blok pamate
    size_t data_size;   // velkost jednej polozky (tu 200)
    size_t size;        // kolko poloziek realne mame
    size_t cap;         // kapacita (kolko sa zmesti)
} ARRAY_LIST;

void array_list_init(ARRAY_LIST* self, size_t data_size) {
    self->data = NULL;
    self->data_size = data_size;
    self->size = 0;
    self->cap = 0;
}

void array_list_destroy(ARRAY_LIST* self) {
    free(self->data);
    self->data = NULL;
    self->data_size = 0;
    self->size = 0;
    self->cap = 0;
}

void array_list_add(ARRAY_LIST* self, void* data) {
    if (!self || !data) return;

    if (self->size == self->cap) {
        size_t new_cap = (self->cap == 0) ? 8 : (self->cap * 2);
        char* new_data = (char*)realloc(self->data, new_cap * self->data_size);
        if (!new_data) return; // nespadnut, len nevlozit
        self->data = new_data;
        self->cap = new_cap;
    }

    memcpy(self->data + self->size * self->data_size, data, self->data_size);
    self->size++;
}

void array_list_process(ARRAY_LIST* self, void (*process_data)(void* data)) {
    if (!self || !process_data) return;

    for (size_t i = 0; i < self->size; i++) {
        void* item = self->data + i * self->data_size;
        process_data(item);
    }
}

void array_list_sort(ARRAY_LIST* self, int (*compare_data)(const void* a, const void* b)) {
    if (!self || !compare_data) return;
    qsort(self->data, self->size, self->data_size, compare_data);
}

/* pomocne pre test_array_list */

static size_t my_len_200(const char* s) {
    size_t n = 0;
    while (n < 200 && s[n] != '\0') n++;
    return n;
}

static void remove_spaces(void* data) {
    string_remove_char((char*)data, ' ');
}

static void print_line(void* data) {
    printf("%s\n", (char*)data);
}

static int cmp_by_len(const void* a, const void* b) {
    const char* sa = (const char*)a;
    const char* sb = (const char*)b;

    size_t la = my_len_200(sa);
    size_t lb = my_len_200(sb);

    if (la < lb) return -1;
    if (la > lb) return 1;
    return 0;
}

void test_array_list(void) {
    ARRAY_LIST list;
    array_list_init(&list, 200); // retazce max 200 bajtov :contentReference[oaicite:2]{index=2}

    FILE* f = fopen("obce s diakritikou.txt", "rb");
    if (!f) {
        printf("Nepodarilo sa otvorit subor: obce s diakritikou.txt\n");
        array_list_destroy(&list);
        return;
    }

    char line[201];
    while (fgets(line, sizeof(line), f)) {
        // odstranit '\n'
        char* p = line;
        while (*p) {
            if (*p == '\n') { *p = '\0'; break; }
            p++;
        }

        // ulozime do polozky velkosti 200 (vyplnit nulami a kopirovat)
        char item[200];
        memset(item, 0, sizeof(item));

        size_t i = 0;
        while (i < 199 && line[i] != '\0') {
            item[i] = line[i];
            i++;
        }
        item[i] = '\0';

        array_list_add(&list, item);
    }

    fclose(f);

    // 3) odstranit medzery cez array_list_process
    array_list_process(&list, remove_spaces);

    // 4) utriedit podla dlzky
    array_list_sort(&list, cmp_by_len);

    // 5) vypisat cez array_list_process
    array_list_process(&list, print_line);

    array_list_destroy(&list);
}

/* ---------------- ULOHA 4 ---------------- */

typedef struct {
    size_t m, n;
    float* data;
} MATRIX;

void matrix_init(MATRIX* self, size_t m, size_t n) {
    self->m = m;
    self->n = n;
    self->data = (float*)calloc(m * n, sizeof(float)); // vsetko na 0 :contentReference[oaicite:3]{index=3}
    if (!self->data) { self->m = 0; self->n = 0; }
}

void matrix_destroy(MATRIX* self) {
    free(self->data);
    self->data = NULL;
    self->m = 0;
    self->n = 0;
}

static float frand_range(float min, float max) {
    float t = (float)rand() / (float)RAND_MAX;
    return min + t * (max - min);
}

void matrix_randomize(MATRIX* self, float min, float max) {
    if (!self || !self->data) return;
    for (size_t i = 0; i < self->m * self->n; i++) {
        self->data[i] = frand_range(min, max);
    }
}

void matrix_print(const MATRIX* self) {
    if (!self || !self->data) return;

    for (size_t i = 0; i < self->m; i++) {
        for (size_t j = 0; j < self->n; j++) {
            printf("%8.2f", self->data[i * self->n + j]); // vpravo + 2 desatinne :contentReference[oaicite:4]{index=4}
        }
        printf("\n");
    }
}

void matrix_transpose(MATRIX* self) {
    if (!self || !self->data) return;

    size_t new_m = self->n;
    size_t new_n = self->m;

    float* new_data = (float*)malloc(new_m * new_n * sizeof(float));
    if (!new_data) return;

    for (size_t i = 0; i < self->m; i++) {
        for (size_t j = 0; j < self->n; j++) {
            new_data[j * new_n + i] = self->data[i * self->n + j];
        }
    }

    free(self->data);
    self->data = new_data;
    self->m = new_m;
    self->n = new_n;
}

void test_matrix(size_t m, size_t n) {
    MATRIX A;
    matrix_init(&A, m, n);

    matrix_randomize(&A, -100.0f, 100.0f);
    matrix_print(&A);

    matrix_transpose(&A);
    matrix_print(&A);

    matrix_destroy(&A);
}

/* ---------------- ULOHA 5 ---------------- */

int main(void) {
    srand((unsigned int)time(NULL));

    test_file_digits_count();
    test_array_list();
    test_matrix(4, 7);
    test_matrix(7, 4);

    return 0;
}
