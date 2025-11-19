#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "include/dll.h"
#include "include/util.h"   // kvôli cmp_int, cmp_double, trim_newline

typedef struct town {
    char name[128];
} Town;

static void print_int_item(const void *data) {
    const int *p = (const int*)data;
    printf("%d ", *p);
}

static void print_double_item(const void *data) {
    const double *p = (const double*)data;
    printf("%.2f ", *p);
}

static void print_town_item(const void *data) {
    const Town *t = (const Town*)data;
    printf("%s\n", t->name);
}

static int cmp_town(const void *a, const void *b) {
    const Town *ta = (const Town*)a;
    const Town *tb = (const Town*)b;
    return strcmp(ta->name, tb->name);
}

static void test_int_list(void) {
    printf("=== Test A: 1000 nahodnych int ===\n");

    DoublyLinkedList list;
    dll_init(&list, sizeof(int), print_int_item);

    for (int i = 0; i < 1000; i++) {
        int value = rand() % 10000;
        dll_insert(&list, list.size, &value);
    }

    int first;
    if (dll_get(&list, 0, &first)) {
        printf("Prvy prvok pred sort: %d\n", first);
    }

    dll_sort(&list, cmp_int);

    if (dll_get(&list, 0, &first)) {
        printf("Prvy prvok po sort (najmensi): %d\n", first);
    }

    int removed;
    if (dll_remove(&list, list.size / 2, &removed)) {
        printf("Odstraneny prvok zo stredu: %d\n", removed);
    }

    printf("Prvych 10 prvkov po sort:\n");
    for (size_t i = 0; i < 10 && i < list.size; i++) {
        int v;
        dll_get(&list, i, &v);
        printf("%d ", v);
    }
    printf("\n");

    dll_destroy(&list);
}

static void test_double_list(void) {
    printf("\n=== Test B: 1000 nahodnych double ===\n");

    DoublyLinkedList list;
    dll_init(&list, sizeof(double), print_double_item);

    for (int i = 0; i < 1000; i++) {
        double value = (double)rand() / (double)RAND_MAX * 1000.0;
        dll_insert(&list, list.size, &value);
    }

    double value_before;
    if (dll_get(&list, 10, &value_before)) {
        printf("Prvok na indexe 10 pred sort: %.2f\n", value_before);
    }

    dll_sort(&list, cmp_double);

    double value_after;
    if (dll_get(&list, 10, &value_after)) {
        printf("Prvok na indexe 10 po sort: %.2f\n", value_after);
    }

    printf("Prvych 10 prvkov po sort:\n");
    for (size_t i = 0; i < 10 && i < list.size; i++) {
        double v;
        dll_get(&list, i, &v);
        printf("%.2f ", v);
    }
    printf("\n");

    dll_destroy(&list);
}

static void test_town_list(void) {
    printf("\n=== Test C: Obce zo suboru ===\n");

    DoublyLinkedList list;
    dll_init(&list, sizeof(Town), print_town_item);

    FILE *f = fopen("Obce s diakritikou.txt", "r");
    if (f == NULL) {
        // záloha – ak máš subor pomenovaný Obce s diakritikou.txt
        f = fopen("Obce s diakritikou.txt", "r");
    }

    if (f == NULL) {
        fprintf(stderr, "Nepodarilo sa otvorit subor s obcami.\n");
        return;
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        // odstránime \r\n z konca
        trim_newline(buffer);
        if (buffer[0] == '\0') {
            continue;
        }

        Town t;
        t.name[0] = '\0';
        strncpy(t.name, buffer, sizeof(t.name) - 1);
        t.name[sizeof(t.name) - 1] = '\0';

        dll_insert(&list, list.size, &t);
    }

    fclose(f);

    printf("Pocet nacitanych obci: %zu\n", list.size);

    dll_sort(&list, cmp_town);

    printf("Prvych 20 obci v abecednom poradi:\n");
    size_t limit = list.size < 20 ? list.size : 20;
    for (size_t i = 0; i < limit; i++) {
        Town t;
        dll_get(&list, i, &t);
        printf("%s\n", t.name);
    }

    dll_destroy(&list);
}

int main(void) {
    srand((unsigned int)time(NULL));

    test_int_list();
    test_double_list();
    test_town_list();

    return 0;
}
