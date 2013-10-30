/*
 * This implements a linked-list of gadgets.
 * We use this to hold the children of a node in the gadget tree.
 */
#ifndef GADGET_LIST_H
#define GADGET_LIST_H

#include <string.h>

/* This is defined in gadget.h. */
typedef struct GadgetTree gadget_t;
typedef struct GadgetListItem gadget_list_item_t;
typedef struct GadgetList gadget_list_t;

struct GadgetList {
    gadget_list_item_t *head;
};

struct GadgetListItem {
    gadget_t *gadget;
    gadget_list_item_t *next;
};

void gadget_list_init(gadget_list_t *list);
void gadget_list_add(gadget_list_t *list, gadget_t *gadget);
gadget_t *gadget_list_find_instr(gadget_list_t *list, unsigned char *instr, size_t instr_len);
void gadget_list_destroy(gadget_list_t *list);

#endif
