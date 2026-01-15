// main.c (C99/C11)
// Skuska UdSP 15. 1. 2026
// Poznamka: Funkcia string_remove_char nepouziva ziadne standardne kniznicne funkcie.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ============================================================
 *  ULOHA 1: Aritmetika ukazovatelov
 * ============================================================ */

char* string_remove_char(char* str, char c) {
    // Nepouzivame ziadnu C kniznicnu funkciu.
    // 2 ukazovatele: read (zdroj) a write (ciel), kopirujeme iba znaky != c.
    if (!str) return str;

    char* read = str;
    char* write = str;

    while (*read != '\0') {
        if (*read != c) {
            *write = *read;
            ++write;
        }
        ++read;
    }
    *write = '\0';
    return str;
}

/* ============================================================
 *  ULOHA 2: Textovy subor - pocet cislic
 * ============================================================ */

size_t file_digits_count(const char* file_name) {
    if (!file_name) return 0;

    FILE* f = fopen(file_name, "rb"); // UTF-8: citame bajty, cislice su ASCII '0'..'9'
    if (!f) return 0;

    size_t count = 0;
    int ch;
    while ((ch = fgetc(f)) != EOF) {
        if (ch >= '0' && ch <= '9') {
            ++count;
        }
    }

    fclose(f);
    return count;
}

void test_file_digits_count(void) {
    size_t c = file_digits_count("wikipedia.txt");
    printf("Pocet cislic v wikipedia.txt: %zu\n", c);
}

/* ============================================================
 *  ULOHA 3: Genericky implicitny zoznam (ARRAY_LIST)
 * ============================================================ */

typedef struct {
    unsigned char* data;   // kontinuálny blok: cap * data_size
    size_t data_size;      // velkost 1 polozky
    size_t size;           // pocet poloziek
    size_t cap;            // kapacita
} ARRAY_LIST;

static void* array_list_item_ptr(ARRAY_LIST* self, size_t index) {
    return (void*)(self->data + index * self->data_size);
}

static const void* array_list_item_ptr_const(const ARRAY_LIST* self, size_t index) {
    return (const void*)(self->data + index * self->data_size);
}

void array_list_init(ARRAY_LIST* self, size_t data_size) {
    if (!self) return;
    self->data = NULL;
    self->data_size = data_size;
    self->size = 0;
    self->cap = 0;
}

void array_list_destroy(ARRAY_LIST* self) {
    if (!self) return;
    free(self->data);
    self->data = NULL;
    self->data_size = 0;
    self->size = 0;
    self->cap = 0;
}

static int array_list_ensure_capacity(ARRAY_LIST* self, size_t needed) {
    if (self->cap >= needed) return 1;

    size_t new_cap = (self->cap == 0) ? 8 : self->cap;
    while (new_cap < needed) {
        new_cap *= 2;
    }

    unsigned char* new_data = (unsigned char*)realloc(self->data, new_cap * self->data_size);
    if (!new_data) return 0;

    self->data = new_data;
    self->cap = new_cap;
    return 1;
}

void array_list_add(ARRAY_LIST* self, void* data) {
    if (!self || !data) return;

    if (!array_list_ensure_capacity(self, self->size + 1)) {
        // Pri nedostatku pamate nevkladame nic (minimalne nespadneme)
        return;
    }

    void* dest = array_list_item_ptr(self, self->size);
    memcpy(dest, data, self->data_size);
    self->size += 1;
}

void array_list_process(ARRAY_LIST* self, void (*process_data)(void* data)) {
    if (!self || !process_data) return;

    for (size_t i = 0; i < self->size; ++i) {
        void* item = array_list_item_ptr(self, i);
        process_data(item);
    }
}

void array_list_sort(ARRAY_LIST* self, int (*compare_data)(const void* data_a, const void* data_b)) {
    if (!self || !compare_data) return;
    if (self->size <= 1) return;

    // Pouzijeme qsort na kontinuálny blok poloziek
    qsort(self->data, self->size, self->data_size, compare_data);
}

/* ---- pomocne funkcie pre test_array_list ---- */

static size_t my_strnlen_200(const char* s) {
    // dlzka reťazca najviac 200 bajtov (polozka ma max 200)
    size_t n = 0;
    if (!s) return 0;
    while (n < 200 && s[n] != '\0') ++n;
    return n;
}

static void process_remove_spaces(void* data) {
    // Volame funkciu z ulohy 1 a odstranujeme medzery ' '
    string_remove_char((char*)data, ' ');
}

static void process_print_line(void* data) {
    printf("%s\n", (char*)data);
}

static int compare_strings_by_length(const void* a, const void* b) {
    const char* sa = (const char*)a;
    const char* sb = (const char*)b;

    size_t la = my_strnlen_200(sa);
    size_t lb = my_strnlen_200(sb);

    if (la < lb) return -1;
    if (la > lb) return 1;
    return 0;
}

void test_array_list(void) {
    // 1) prazdny implicitny zoznam, polozky = retazce max 200 bajtov
    ARRAY_LIST list;
    array_list_init(&list, 200);

    // 2) nacitanie riadkov zo suboru "obce s diakritikou.txt"
    FILE* f = fopen("obce s diakritikou.txt", "rb");
    if (!f) {
        printf("Nepodarilo sa otvorit subor: obce s diakritikou.txt\n");
        array_list_destroy(&list);
        return;
    }

    char buffer[201]; // +1 na '\0'
    while (fgets(buffer, (int)sizeof(buffer), f) != NULL) {
        // odstranit '\n' (unix)
        char* p = buffer;
        while (*p != '\0') {
            if (*p == '\n') { *p = '\0'; break; }
            ++p;
        }

        // pripravime polozku presne 200 bajtov (zero-fill + kopia)
        char item[200];
        memset(item, 0, sizeof(item));

        // skopirujeme maximalne 199 znakov, aby ostal '\0'
        size_t i = 0;
        while (i < 199 && buffer[i] != '\0') {
            item[i] = buffer[i];
            ++i;
        }
        item[i] = '\0';

        array_list_add(&list, item);
    }

    fclose(f);

    // 3) odstranit vsetky medzery vo vsetkych riadkoch (cez process)
    array_list_process(&list, process_remove_spaces);

    // 4) utriedit podla dlzky retazcov
    array_list_sort(&list, compare_strings_by_length);

    // 5) vypisat obsah zoznamu
    array_list_process(&list, process_print_line);

    // 6) korektne zrusit
    array_list_destroy(&list);
}

/* ============================================================
 *  ULOHA 4: MATRIX - transpozicia
 * ============================================================ */

typedef struct {
    size_t m;      // riadky
    size_t n;      // stlpce
    float* data;   // m*n
} MATRIX;

static float* matrix_at(MATRIX* self, size_t i, size_t j) {
    return &self->data[i * self->n + j];
}

static const float* matrix_at_const(const MATRIX* self, size_t i, size_t j) {
    return &self->data[i * self->n + j];
}

void matrix_init(MATRIX* self, size_t m, size_t n) {
    if (!self) return;

    self->m = m;
    self->n = n;
    self->data = NULL;

    if (m == 0 || n == 0) return;

    self->data = (float*)calloc(m * n, sizeof(float)); // inicializacia na 0
    if (!self->data) {
        self->m = 0;
        self->n = 0;
    }
}

void matrix_destroy(MATRIX* self) {
    if (!self) return;
    free(self->data);
    self->data = NULL;
    self->m = 0;
    self->n = 0;
}

static float rand_float_range(float min, float max) {
    // rand() v [0, RAND_MAX]
    float t = (float)rand() / (float)RAND_MAX; // [0,1]
    return min + t * (max - min);
}

void matrix_randomize(MATRIX* self, float min, float max) {
    if (!self || !self->data) return;

    for (size_t i = 0; i < self->m; ++i) {
        for (size_t j = 0; j < self->n; ++j) {
            *matrix_at(self, i, j) = rand_float_range(min, max);
        }
    }
}

void matrix_print(const MATRIX* self) {
    if (!self || !self->data) {
        printf("(prazdna matica)\n");
        return;
    }

    for (size_t i = 0; i < self->m; ++i) {
        for (size_t j = 0; j < self->n; ++j) {
            printf("%8.2f", *matrix_at_const(self, i, j)); // vpravo zarovnane, 2 desatinne
        }
        printf("\n");
    }
}

void matrix_transpose(MATRIX* self) {
    if (!self || !self->data) return;

    // Najjednoduchsie: alokovat novy blok, prekopirovat transponovane, uvolnit povodny.
    size_t new_m = self->n;
    size_t new_n = self->m;

    float* new_data = (float*)malloc(new_m * new_n * sizeof(float));
    if (!new_data) return; // ak zlyha, nechame povodnu

    for (size_t i = 0; i < self->m; ++i) {
        for (size_t j = 0; j < self->n; ++j) {
            // nove[i' = j][j' = i] = stare[i][j]
            new_data[j * new_n + i] = *matrix_at_const(self, i, j);
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

    printf("Matica %zux%zu:\n", m, n);
    matrix_print(&A);

    matrix_transpose(&A);

    printf("Transponovana matica %zux%zu:\n", A.m, A.n);
    matrix_print(&A);

    matrix_destroy(&A);
}

/* ============================================================
 *  ULOHA 5: main - presne volania v poradi
 * ============================================================ */

int main(void) {
    // aby randomize nebol vzdy rovnaky
    srand((unsigned int)time(NULL));

    test_file_digits_count();
    test_array_list();
    test_matrix(4, 7);
    test_matrix(7, 4);

    return 0;
}
