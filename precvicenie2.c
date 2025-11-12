#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

/* ---------------- DYNAMIC_STRING ---------------- */

#define DYNAMIC_STRING_INIT_CAPACITY 16

typedef struct dynamic_string {
    char  *data;
    size_t length;
    size_t capacity;
} DYNAMIC_STRING;

void dynamic_string_init(DYNAMIC_STRING* this) {
    if (this == NULL) {
        return;
    }
    this->data = (char*)malloc(DYNAMIC_STRING_INIT_CAPACITY);
    if (this->data == NULL) {
        this->length = 0;
        this->capacity = 0;
        return;
    }
    this->length = 0;
    this->capacity = DYNAMIC_STRING_INIT_CAPACITY;
    this->data[0] = '\0';
}

void dynamic_string_destroy(DYNAMIC_STRING* this) {
    if (this == NULL) {
        return;
    }
    if (this->data != NULL) {
        free(this->data);
        this->data = NULL;
    }
    this->length = 0;
    this->capacity = 0;
}

const char* dynamic_string_get_data(const DYNAMIC_STRING* this) {
    if (this == NULL || this->data == NULL) {
        return "";
    }
    return this->data;
}

/* nesmie používať indexy ani strlen/strcpy/strcat */
void dynamic_string_append(DYNAMIC_STRING* this, const char* str) {
    if (this == NULL || str == NULL) {
        return;
    }

    /* zisti dĺžku str manuálne */
    size_t add = 0;
    const char *p = str;
    while (*p != '\0') {
        ++p;
        ++add;
    }

    /* potrebná kapacita vrátane '\0' */
    size_t need = this->length + add + 1;

    if (need > this->capacity) {
        size_t new_capacity = this->capacity;
        if (new_capacity == 0) {
            new_capacity = DYNAMIC_STRING_INIT_CAPACITY;
        }
        while (new_capacity < need) {
            size_t next = new_capacity * 2;
            if (next <= new_capacity) {
                break; /* ochrana proti overflowu */
            }
            new_capacity = next;
        }
        char *new_data = (char*)realloc(this->data, new_capacity);
        if (new_data == NULL) {
            return; /* necháme pôvodný stav */
        }
        this->data = new_data;
        this->capacity = new_capacity;
    }

    /* append pomocou ukazovateľov */
    char       *dst = this->data + this->length;
    const char *src = str;
    while (*src != '\0') {
        *dst++ = *src++;
    }
    *dst = '\0';
    this->length += add;
}

/* ---------------- LINKED_LIST ---------------- */

typedef struct linked_list_item {
    void *data;                    /* budeme ukladať ukazovatele na DYNAMIC_STRING */
    struct linked_list_item *next;
} LINKED_LIST_ITEM;

typedef struct linked_list {
    LINKED_LIST_ITEM *first;
    LINKED_LIST_ITEM *last;
    size_t size;
    size_t data_size;             /* podľa zadania, v tomto riešení ho nepotrebujeme */
} LINKED_LIST;

void linked_list_init(LINKED_LIST* this, size_t data_size) {
    if (this == NULL) {
        return;
    }
    this->first = NULL;
    this->last  = NULL;
    this->size  = 0;
    this->data_size = data_size;
}

void linked_list_destroy(LINKED_LIST* this) {
    if (this == NULL) {
        return;
    }
    LINKED_LIST_ITEM *cur = this->first;
    while (cur != NULL) {
        LINKED_LIST_ITEM *next = cur->next;
        /* data NEuvolňujeme tu, lebo v tomto riešení sú to ukazovatele
           na DYNAMIC_STRING, ktoré zrušíme zvlášť v process_files */
        free(cur);
        cur = next;
    }
    this->first = NULL;
    this->last  = NULL;
    this->size  = 0;
}

/* data je ukazovateľ na DYNAMIC_STRING* (alebo všeobecne na nejaké dáta) */
void linked_list_add(LINKED_LIST* this, void* data) {
    if (this == NULL) {
        return;
    }
    LINKED_LIST_ITEM *item = (LINKED_LIST_ITEM*)malloc(sizeof(LINKED_LIST_ITEM));
    if (item == NULL) {
        return;
    }
    item->data = data;
    item->next = NULL;

    if (this->first == NULL) {
        this->first = item;
    } else {
        this->last->next = item;
    }
    this->last = item;
    this->size++;
}

void linked_list_print(const LINKED_LIST *this,
                       FILE *out,
                       const char* (*format_data)(const void* data)) {
    if (this == NULL || out == NULL || format_data == NULL) {
        return;
    }
    LINKED_LIST_ITEM *cur = this->first;
    while (cur != NULL) {
        const char *text = format_data(cur->data);
        if (text != NULL) {
            fputs(text, out);
        }
        cur = cur->next;
    }
}

/* náhodné preusporiadanie – swapujeme len položku data v uzloch */
void linked_list_shuffle(LINKED_LIST *this, size_t shuffle_count) {
    if (this == NULL || this->size < 2) {
        return;
    }

    size_t n = this->size;
    LINKED_LIST_ITEM **items = (LINKED_LIST_ITEM**)malloc(n * sizeof(LINKED_LIST_ITEM*));
    if (items == NULL) {
        return;
    }

    LINKED_LIST_ITEM *cur = this->first;
    size_t i = 0;
    while (cur != NULL && i < n) {
        items[i++] = cur;
        cur = cur->next;
    }

    for (size_t s = 0; s < shuffle_count; ++s) {
        size_t a = (size_t)(rand() % (int)n);
        size_t b = (size_t)(rand() % (int)n);
        if (a != b) {
            void *tmp = items[a]->data;
            items[a]->data = items[b]->data;
            items[b]->data = tmp;
        }
    }

    free(items);
}

/* ---------------- Pomocná funkcia pre LINKED_LIST::print ---------------- */

static const char* format_dynamic_string(const void* data) {
    const DYNAMIC_STRING *s = (const DYNAMIC_STRING*)data;
    return dynamic_string_get_data(s);
}

/* ---------------- process_files ---------------- */

void process_files(FILE* in, FILE* out) {
    LINKED_LIST list;
    linked_list_init(&list, sizeof(DYNAMIC_STRING*));

    DYNAMIC_STRING *current = NULL;
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), in) != NULL) {
        if (current == NULL) {
            current = (DYNAMIC_STRING*)malloc(sizeof(DYNAMIC_STRING));
            if (current == NULL) {
                /* pri chybe pamäte jednoducho skončíme */
                break;
            }
            dynamic_string_init(current);
        }

        dynamic_string_append(current, buffer);

        /* zistíme, či tento chunk obsahoval '\n' */
        bool eol = false;
        const char *p = buffer;
        while (*p != '\0') {
            if (*p == '\n') {
                eol = true;
                break;
            }
            ++p;
        }

        if (eol) {
            /* riadok je kompletný -> vložíme do zoznamu */
            linked_list_add(&list, current);
            current = NULL; /* vlastníctvo prechádza na zoznam */
        }
    }

    /* posledný riadok bez '\n' na konci súboru */
    if (current != NULL) {
        if (current->length > 0) {
            linked_list_add(&list, current);
        } else {
            dynamic_string_destroy(current);
            free(current);
        }
        current = NULL;
    }

    /* náhodne preusporiadaj zoznam – aspoň 5000 výmen */
    linked_list_shuffle(&list, 5000);

    /* zapíš preusporiadané riadky do výstupného súboru */
    linked_list_print(&list, out, format_dynamic_string);

    /* korektné uvoľnenie pamäte – všetky DYNAMIC_STRING + uzly zoznamu */
    LINKED_LIST_ITEM *cur = list.first;
    while (cur != NULL) {
        LINKED_LIST_ITEM *next = cur->next;
        DYNAMIC_STRING *str = (DYNAMIC_STRING*)cur->data;
        if (str != NULL) {
            dynamic_string_destroy(str);
            free(str);
        }
        cur = next;
    }
    linked_list_destroy(&list);
}

/* ---------------- main ---------------- */

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Použitie: %s <vstupny_subor> <vystupny_subor>\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (in == NULL) {
        fprintf(stderr, "Nepodarilo sa otvoriť vstupný súbor '%s'\n", argv[1]);
        return 1;
    }

    FILE *out = fopen(argv[2], "w");
    if (out == NULL) {
        fprintf(stderr, "Nepodarilo sa otvoriť výstupný súbor '%s'\n", argv[2]);
        fclose(in);
        return 1;
    }

    srand((unsigned)time(NULL));

    process_files(in, out);

    fclose(in);
    fclose(out);

    return 0;
}
