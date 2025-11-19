//
// Created by matus on 19.11.2025.
//

//
// Zadanie 1 – obojstranne zreťazený zoznam
//

#ifndef DOMACE_ZADANIE_1_DLL_H
#define DOMACE_ZADANIE_1_DLL_H

#include <stddef.h>

typedef struct dll_item {
    void *data;
    struct dll_item *prev;
    struct dll_item *next;
} DLL_ITEM;

typedef struct doubly_linked_list {
    DLL_ITEM *first;
    DLL_ITEM *last;
    size_t size;
    size_t item_size;
    void (*print_data)(const void *data);
} DoublyLinkedList;

/**
 * "Konštruktor" – inicializuje prázdny zoznam.
 *
 * @param list       ukazovateľ na štruktúru
 * @param item_size  veľkosť jedného prvku (napr. sizeof(int))
 * @param print_data funkcia na vypísanie jedného prvku (môže byť NULL)
 */
void dll_init(DoublyLinkedList *list,
              size_t item_size,
              void (*print_data)(const void *data));

/**
 * Vymaže celý obsah zoznamu (uvoľní všetky uzly + ich data).
 */
void dll_clear(DoublyLinkedList *list);

/**
 * Deštruktor – len alias na dll_clear.
 */
void dll_destroy(DoublyLinkedList *list);

/**
 * Vloženie prvku na index.
 *
 * @param list   zoznam
 * @param index  pozícia (0..size)
 * @param data   ukazovateľ na dáta, ktoré sa skopírujú (podľa item_size)
 * @return 1 ak OK, 0 ak chyba (NULL, index > size, malloc fail...)
 */
int dll_insert(DoublyLinkedList *list, size_t index, const void *data);

/**
 * Odobratie prvku z indexu.
 *
 * @param list     zoznam
 * @param index    pozícia (0..size-1)
 * @param out_data buffer, kam sa skopíruje odoberaná hodnota (môže byť NULL)
 * @return 1 ak OK, 0 ak chyba
 */
int dll_remove(DoublyLinkedList *list, size_t index, void *out_data);

/**
 * Sprístupnenie prvku na indexe (bez odobratia).
 *
 * @param list     zoznam (const, nemeníme ho)
 * @param index    pozícia (0..size-1)
 * @param out_data buffer, kam sa skopíruje hodnota prvku
 * @return 1 ak OK, 0 ak chyba
 */
int dll_get(const DoublyLinkedList *list, size_t index, void *out_data);

/**
 * Vypísanie celého zoznamu.
 * Použije sa funkcia list->print_data (ak nie je NULL).
 */
void dll_print(const DoublyLinkedList *list);

/**
 * Utriedenie celej štruktúry (bubble sort).
 *
 * @param list zoznam
 * @param cmp  komparačná funkcia ako v qsort (vracia <0, 0, >0)
 */
void dll_sort(DoublyLinkedList *list,
              int (*cmp)(const void *, const void *));

#endif //DOMACE_ZADANIE_1_DLL_H
