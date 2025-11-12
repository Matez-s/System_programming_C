#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STATIC_STRING_SIZE 256

// --------------- STATIC STRING ---------------

typedef struct static_string {
    char data[STATIC_STRING_SIZE];
} STATIC_STRING;

void static_string_init(STATIC_STRING* self) {
    self->data[0] = '\0';
}

void static_string_destroy(STATIC_STRING* self) {
    self->data[0] = '\0';
}

void static_string_append(STATIC_STRING* self, const char* str) {
    char *dst = self->data;

    /* najdi koniec (najprv hranica, az potom dereferencia) */
    while ( (dst - self->data) < STATIC_STRING_SIZE && *dst != '\0' ) {
        ++dst;
    }

    /* kopiruj s miestom na '\0' */
    while (*str != '\0' && (dst - self->data) + 1 < STATIC_STRING_SIZE) {
        *dst++ = *str++;
    }

    *dst = '\0';
}

const char* static_string_get_data(const STATIC_STRING* self) {
    return self->data;
}

bool static_string_contains(STATIC_STRING* self, const char* needle) {
    if (*needle == '\0') return true;
    for (char *start = self->data; *start != '\0'; ++start) {
        char *p = start;
        const char *q = needle;
        while (*p != '\0' && *q != '\0' && *p == *q) {
            ++p;
            ++q;
        }
        if (*q == '\0') return true;
    }
    return false;
}

// --------------- ARRAY LIST ---------------

typedef struct array_list {
    void*  data;
    size_t size;
    size_t data_size;
    size_t capacity;
} ARRAY_LIST;

void array_list_init(ARRAY_LIST* self, size_t data_size) {
    self->data_size = data_size;
    self->capacity  = 8;     /* mala pociatocna kapacita */
    self->size      = 0;
    self->data      = malloc(self->capacity * self->data_size);
    if (!self->data) {
        fprintf(stderr, "Alloc failed\n");
        exit(1);
    }
}

void array_list_destroy(ARRAY_LIST* self) {
    free(self->data);
    self->data      = NULL;
    self->size      = 0;
    self->capacity  = 0;
    self->data_size = 0;
}

void array_list_add(ARRAY_LIST* self, const void* data) {
    if (self->size >= self->capacity) {
        self->capacity *= 2;
        self->data = realloc(self->data, self->capacity * self->data_size);
    }
    memcpy((char*)self->data + (self->size * self->data_size), data, self->data_size);
    ++self->size;
}

void array_list_process(ARRAY_LIST *self, void (*process_data)(void*)) {
    char* base = (char*) self->data;
    for (size_t i = 0; i < self->size; ++i) {
        void* elem = base + i * self->data_size;
        process_data(elem);
    }
}

ARRAY_LIST* array_list_select(ARRAY_LIST *self, ARRAY_LIST *other, bool (*predicate)(void*)) {
    char* base = (char*) self->data;
    for (size_t i = 0; i < self->size; ++i) {
        void* elem = base + i * self->data_size;
        if (predicate(elem)) {
            array_list_add(other, elem);
        }
    }
    return other;
}

// ------------------ Helpery zo zadania ------------------

bool static_string_contains_void(void* data) {
    return static_string_contains((STATIC_STRING*)data, "rat");
}

void static_string_print_void(void* data) {
    printf("%s\n", static_string_get_data((const STATIC_STRING*)data));
}

// ------------------ process_txt_file ------------------

void process_txt_file(FILE* f) {
    ARRAY_LIST all;
    array_list_init(&all, sizeof(STATIC_STRING));
    ARRAY_LIST matched;
    array_list_init(&matched, sizeof(STATIC_STRING));

    char buffer[128];
    STATIC_STRING line;
    bool have_line = false;

    static_string_init(&line);

    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        /* odrež \n ak prisiel v tomto chunku */
        char *p = buffer;
        while (*p != '\0' && *p != '\n') ++p;
        bool eol = (*p == '\n');
        if (eol) *p = '\0';

        /* dolep do aktualneho riadku */
        static_string_append(&line, buffer);
        have_line = true;

        if (eol) {
            array_list_add(&all, &line);
            static_string_init(&line);
            have_line = false;
        }
    }

    if (have_line) {
        array_list_add(&all, &line);
        static_string_init(&line);
    }

    /* vyber prvkov obsahujucich "rat" do matched */
    array_list_select(&all, &matched, static_string_contains_void);

    /* vypis len vybranych */
    array_list_process(&matched, static_string_print_void);

    array_list_destroy(&matched);
    array_list_destroy(&all);
}

// ------------------ main ------------------

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Pouzitie: %s <subor>\n", argv[0]);
        return 1;
    }

    FILE* f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Nepodarilo sa otvorit subor: %s\n", argv[1]);
        return 1;
    }

    process_txt_file(f);
    fclose(f);

    return 0;
}
