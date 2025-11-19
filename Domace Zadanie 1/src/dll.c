//
// Created by matus on 19.11.2025.
//

#include "../include/dll.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static DLL_ITEM *dll_get_node(DoublyLinkedList *list, size_t index) {
    if (list == NULL) {
        return NULL;
    }
    if (index >= list->size) {
        return NULL;
    }

    DLL_ITEM *cur = list->first;
    size_t i = 0;
    while (cur != NULL && i < index) {
        cur = cur->next;
        i++;
    }
    return cur;
}

static const DLL_ITEM *dll_get_node_const(const DoublyLinkedList *list, size_t index) {
    if (list == NULL) {
        return NULL;
    }
    if (index >= list->size) {
        return NULL;
    }

    const DLL_ITEM *cur = list->first;
    size_t i = 0;
    while (cur != NULL && i < index) {
        cur = cur->next;
        i++;
    }
    return cur;
}

void dll_init(DoublyLinkedList *list,
              size_t item_size,
              void (*print_data)(const void *data)) {
    if (list == NULL) {
        return;
    }
    list->first = NULL;
    list->last = NULL;
    list->size = 0;
    list->item_size = item_size;
    list->print_data = print_data;
}

void dll_clear(DoublyLinkedList *list) {
    if (list == NULL) {
        return;
    }

    DLL_ITEM *cur = list->first;
    while (cur != NULL) {
        DLL_ITEM *next = cur->next;
        free(cur->data);
        free(cur);
        cur = next;
    }

    list->first = NULL;
    list->last = NULL;
    list->size = 0;
}

void dll_destroy(DoublyLinkedList *list) {
    dll_clear(list);
}

int dll_insert(DoublyLinkedList *list, size_t index, const void *data) {
    if (list == NULL || data == NULL) {
        return 0;
    }
    if (index > list->size) {
        return 0;
    }

    DLL_ITEM *item = (DLL_ITEM*)malloc(sizeof(DLL_ITEM));
    if (item == NULL) {
        return 0;
    }

    item->data = malloc(list->item_size);
    if (item->data == NULL) {
        free(item);
        return 0;
    }
    memcpy(item->data, data, list->item_size);

    item->prev = NULL;
    item->next = NULL;

    // prázdny zoznam
    if (list->size == 0) {
        list->first = item;
        list->last = item;
    }
    // vloženie na začiatok
    else if (index == 0) {
        item->next = list->first;
        list->first->prev = item;
        list->first = item;
    }
    // vloženie na koniec
    else if (index == list->size) {
        item->prev = list->last;
        list->last->next = item;
        list->last = item;
    }
    // vloženie niekde v strede
    else {
        DLL_ITEM *cur = dll_get_node(list, index);
        if (cur == NULL) {
            free(item->data);
            free(item);
            return 0;
        }
        item->prev = cur->prev;
        item->next = cur;
        if (cur->prev != NULL) {
            cur->prev->next = item;
        }
        cur->prev = item;
    }

    list->size++;
    return 1;
}

int dll_remove(DoublyLinkedList *list, size_t index, void *out_data) {
    if (list == NULL) {
        return 0;
    }
    if (index >= list->size) {
        return 0;
    }

    DLL_ITEM *cur = dll_get_node(list, index);
    if (cur == NULL) {
        return 0;
    }

    if (out_data != NULL && cur->data != NULL) {
        memcpy(out_data, cur->data, list->item_size);
    }

    if (cur->prev != NULL) {
        cur->prev->next = cur->next;
    } else {
        list->first = cur->next;
    }

    if (cur->next != NULL) {
        cur->next->prev = cur->prev;
    } else {
        list->last = cur->prev;
    }

    free(cur->data);
    free(cur);
    list->size--;

    return 1;
}

int dll_get(const DoublyLinkedList *list, size_t index, void *out_data) {
    if (list == NULL || out_data == NULL) {
        return 0;
    }
    if (index >= list->size) {
        return 0;
    }

    const DLL_ITEM *cur = dll_get_node_const(list, index);
    if (cur == NULL || cur->data == NULL) {
        return 0;
    }

    memcpy(out_data, cur->data, list->item_size);
    return 1;
}

void dll_print(const DoublyLinkedList *list) {
    if (list == NULL || list->print_data == NULL) {
        return;
    }

    const DLL_ITEM *cur = list->first;
    while (cur != NULL) {
        list->print_data(cur->data);
        cur = cur->next;
    }
    printf("\n");
}

void dll_sort(DoublyLinkedList *list,
              int (*cmp)(const void *, const void *)) {
    if (list == NULL || cmp == NULL) {
        return;
    }
    if (list->size < 2) {
        return;
    }

    int swapped;
    do {
        swapped = 0;
        DLL_ITEM *cur = list->first;
        while (cur != NULL && cur->next != NULL) {
            if (cmp(cur->data, cur->next->data) > 0) {
                void *tmp = cur->data;
                cur->data = cur->next->data;
                cur->next->data = tmp;
                swapped = 1;
            }
            cur = cur->next;
        }
    } while (swapped);
}
