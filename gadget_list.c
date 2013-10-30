#include "gadget_list.h"
#include "gadget.h"

#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <libdis.h>

void gadget_list_init(gadget_list_t *list)
{
    static int initialized = 0;
    if (!initialized) {
        x86_init(opt_none, NULL, NULL);
        initialized = 1;
    }
    list->head = NULL;
}

void gadget_list_add(gadget_list_t *list, gadget_t *gadget)
{
    gadget_list_item_t *new_item = malloc(sizeof(gadget_list_item_t));
    new_item->gadget = gadget;
    new_item->next = list->head;
    list->head = new_item;
}

gadget_t *gadget_list_find_instr(gadget_list_t *list, unsigned char *instr, size_t instr_len)
{
    gadget_list_item_t *cursor = list->head;
    while (cursor != NULL) {
        if (cursor->gadget->instr_len == instr_len && 
            memcmp(cursor->gadget->instr, instr, instr_len) == 0) {
            return cursor->gadget;
        }
        cursor = cursor->next;
    }
    return NULL;
}

void gadget_list_destroy(gadget_list_t *list)
{
    gadget_list_item_t *cursor = list->head;
    while (cursor != NULL) {
        gadget_list_item_t *to_free = cursor;
        cursor = cursor->next;
        free(to_free);
    }
}
