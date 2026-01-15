// main.c (C99/C11)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

/* ============================================================
 * ULOHA 1: Aritmetika ukazovatelov
 * char* string_reverse(char* str)
 * - bez strlen, bez indexovania
 * ============================================================ */

char* string_reverse(char* str) {
    if (str == NULL) {
        return NULL;
    }

    // najdi koniec retazca pomocou ukazovatela
    char* end = str;
    while (*end != '\0') {
        ++end;
    }

    // end teraz ukazuje na '\0', posun na posledny znak (ak existuje)
    if (end == str) {
        return str; // prazdny retazec
    }
    --end;

    // swap z oboch stran smerom do stredu
    char* left = str;
    char* right = end;
    while (left < right) {
        char tmp = *left;
        *left = *right;
        *right = tmp;
        ++left;
        --right;
    }

    return str;
}

/* ============================================================
 * ULOHA 2: Textovy subor - pocet slov (UTF-8)
 * - slovo = skupina znakov ohranicena whitespace
 * ============================================================ */

size_t file_words_count(const char* file_name) {
    if (file_name == NULL) {
        return 0;
    }

    FILE* f = fopen(file_name, "rb"); // binarne - citame bajty
    if (f == NULL) {
        return 0;
    }

    size_t count = 0;
    bool in_word = false;

    int ch;
    while ((ch = fgetc(f)) != EOF) {
        unsigned char c = (unsigned char)ch;

        if (isspace(c)) {
            // whitespace ukonci slovo
            in_word = false;
        } else {
            // ne-whitespace: ak sme prave zacali nove slovo, inkrementuj
            if (!in_word) {
                ++count;
                in_word = true;
            }
        }
    }

    fclose(f);
    return count;
}

void test_file_words_count(void) {
    const char* name = "wikipedia.txt";
    size_t wc = file_words_count(name);
    printf("file_words_count('%s') = %zu\n\n", name, wc);
}

/* ============================================================
 * ULOHA 3: Genericky jednostranne zretazeny zoznam
 * - uklada kopie dat velkosti data_size
 * ============================================================ */

typedef struct linked_list_item {
    void* data;
    struct linked_list_item* next;
} LINKED_LIST_ITEM;

typedef struct linked_list {
    LINKED_LIST_ITEM* first;
    size_t size;
    size_t data_size;
} LINKED_LIST;

void linked_list_init(LINKED_LIST* self, size_t data_size) {
    if (self == NULL) {
        return;
    }
    self->first = NULL;
    self->size = 0;
    self->data_size = data_size;
}

void linked_list_destroy(LINKED_LIST* self) {
    if (self == NULL) {
        return;
    }

    LINKED_LIST_ITEM* cur = self->first;
    while (cur != NULL) {
        LINKED_LIST_ITEM* next = cur->next;

        // uvolni ulozene data
        free(cur->data);
        cur->data = NULL;

        // uvolni uzol
        free(cur);
        cur = next;
    }

    self->first = NULL;
    self->size = 0;
    self->data_size = 0;
}

void linked_list_prepend(LINKED_LIST* self, void* data) {
    if (self == NULL || data == NULL || self->data_size == 0) {
        return;
    }

    LINKED_LIST_ITEM* item = (LINKED_LIST_ITEM*)malloc(sizeof(LINKED_LIST_ITEM));
    if (item == NULL) {
        return;
    }

    item->data = malloc(self->data_size);
    if (item->data == NULL) {
        free(item);
        return;
    }

    // skopiruj data do uzla
    memcpy(item->data, data, self->data_size);

    // prepend
    item->next = self->first;
    self->first = item;
    self->size++;
}

void linked_list_process(LINKED_LIST* self, void (*process_data)(void* data)) {
    if (self == NULL || process_data == NULL) {
        return;
    }

    LINKED_LIST_ITEM* cur = self->first;
    while (cur != NULL) {
        process_data(cur->data);
        cur = cur->next;
    }
}

/* --- pomocne funkcie pre test_linked_list --- */

static void print_cstring_200(void* data) {
    if (data == NULL) return;
    // data je char[200] (resp. aspon nul-terminated)
    puts((const char*)data);
}

void test_linked_list(void) {
    LINKED_LIST list;
    linked_list_init(&list, 200); // polozky su retazce max 200 bajtov

    FILE* f = fopen("obce.txt", "r");
    if (f == NULL) {
        printf("Nepodarilo sa otvorit 'obce.txt'\n\n");
        linked_list_destroy(&list);
        return;
    }

    char line[201]; // 200 + '\0'
    while (fgets(line, (int)sizeof(line), f) != NULL) {
        // odstran '\n' ak tam je (unix format)
        char* p = line;
        while (*p != '\0') {
            if (*p == '\n') {
                *p = '\0';
                break;
            }
            ++p;
        }

        // zabezpec, ze buffer ma max 199 znakov + '\0' (fgets to uz dodrzi)
        // vloz do zoznamu na zaciatok -> tym vznikne opacne poradie
        char item_buf[200];
        // napln nulami aby copy do listu mal stabilny obsah
        memset(item_buf, 0, sizeof(item_buf));
        // skopiruj line do item_buf (bezpecne, lebo line ma max 200 vratane '\0')
        // a item_buf ma 200
        strncpy(item_buf, line, sizeof(item_buf) - 1);

        linked_list_prepend(&list, item_buf);
    }

    fclose(f);

    // 3) nad kazdym riadkom zavolaj string_reverse cez linked_list_process
    // Poznamka: typovy cast (castovanie) - v praxi bezne tolerovane v takychto ulohach
    linked_list_process(&list, (void (*)(void*))string_reverse);

    // 4) vypis obsah zoznamu
    linked_list_process(&list, print_cstring_200);

    // 5) zrus zoznam
    linked_list_destroy(&list);

    printf("\n");
}

/* ============================================================
 * ULOHA 4: Stvorcove matice (double) + "OOP" styl (struct+func)
 * ============================================================ */

typedef struct square_matrix {
    size_t dimension;
    double* data; // row-major: data[r*dimension + c]
} SQUARE_MATRIX;

void square_matrix_init(SQUARE_MATRIX* self, size_t dimension) {
    if (self == NULL) return;

    self->dimension = dimension;
    self->data = NULL;

    if (dimension == 0) {
        return;
    }

    // vsetko na 0
    self->data = (double*)calloc(dimension * dimension, sizeof(double));
    if (self->data == NULL) {
        self->dimension = 0;
    }
}

void square_matrix_destroy(SQUARE_MATRIX* self) {
    if (self == NULL) return;

    free(self->data);
    self->data = NULL;
    self->dimension = 0;
}

static double rand_double_range(double min, double max) {
    // [0,1]
    double t = (double)rand() / (double)RAND_MAX;
    return min + t * (max - min);
}

void square_matrix_randomize(SQUARE_MATRIX* self, double min, double max) {
    if (self == NULL || self->data == NULL) return;

    size_t n = self->dimension * self->dimension;
    for (size_t i = 0; i < n; ++i) {
        self->data[i] = rand_double_range(min, max);
    }
}

void square_matrix_print(const SQUARE_MATRIX* self) {
    if (self == NULL || self->data == NULL) {
        printf("(null matrix)\n");
        return;
    }

    size_t d = self->dimension;
    for (size_t r = 0; r < d; ++r) {
        for (size_t c = 0; c < d; ++c) {
            double v = self->data[r * d + c];
            // zarovnanie vpravo + 2 desatinne miesta
            printf("%10.2f", v);
        }
        printf("\n");
    }
}

SQUARE_MATRIX* square_matrix_multiply(
    SQUARE_MATRIX* dest,
    const SQUARE_MATRIX* src_left,
    const SQUARE_MATRIX* src_right
) {
    if (dest == NULL || src_left == NULL || src_right == NULL) {
        return NULL;
    }
    if (src_left->data == NULL || src_right->data == NULL) {
        return NULL;
    }

    // pre stvorcove matice je nasobenie mozne len ak dimension A == dimension B
    if (src_left->dimension != src_right->dimension) {
        return NULL;
    }

    size_t d = src_left->dimension;

    // korektne inicializuj dest
    square_matrix_destroy(dest);      // ak by uz nieco drzala
    square_matrix_init(dest, d);
    if (dest->data == NULL) {
        return NULL;
    }

    // C = A * B
    for (size_t i = 0; i < d; ++i) {
        for (size_t j = 0; j < d; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < d; ++k) {
                sum += src_left->data[i * d + k] * src_right->data[k * d + j];
            }
            dest->data[i * d + j] = sum;
        }
    }

    return dest;
}

void test_square_matrix(size_t dimension_a, size_t dimension_b) {
    printf("=== test_square_matrix(%zu, %zu) ===\n", dimension_a, dimension_b);

    SQUARE_MATRIX A, B, C;
    A.dimension = 0; A.data = NULL;
    B.dimension = 0; B.data = NULL;
    C.dimension = 0; C.data = NULL;

    square_matrix_init(&A, dimension_a);
    square_matrix_init(&B, dimension_b);

    square_matrix_randomize(&A, -100.0, 100.0);
    square_matrix_randomize(&B, -100.0, 100.0);

    printf("Matrix A (%zux%zu):\n", A.dimension, A.dimension);
    square_matrix_print(&A);

    printf("\nMatrix B (%zux%zu):\n", B.dimension, B.dimension);
    square_matrix_print(&B);

    printf("\n");

    SQUARE_MATRIX* res = square_matrix_multiply(&C, &A, &B);
    if (res == NULL) {
        printf("Multiplication not possible for dimensions %zu and %zu.\n", dimension_a, dimension_b);
    } else {
        printf("Matrix C = A*B (%zux%zu):\n", C.dimension, C.dimension);
        square_matrix_print(&C);
    }

    // destroy
    square_matrix_destroy(&A);
    square_matrix_destroy(&B);
    square_matrix_destroy(&C);

    printf("\n");
}

/* ============================================================
 * ULOHA 5: main
 * ============================================================ */

int main(void) {
    srand((unsigned)time(NULL));

    test_file_words_count();
    test_linked_list();
    test_square_matrix(8, 8);
    test_square_matrix(5, 8);

    return 0;
}
