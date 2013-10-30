/*
 * Search for ROP gadgets in a buffer of x86 code. 
 * The algorithm used is described in:
 * "Return Oriented Programming: Systems, Languages, and Applications"
 * Basically, you keep a 'trie' of instructions, where the root node is RET, and
 * the children of a node are the instructions that can be found directly before
 * the sequence of instructions from that node to (and including) the root.
 */
#ifndef GADGET_H
#define GADGET_H

#include <stdint.h>
#include <string.h>
#include "gadget_list.h"

#define MAX_INSTRUCTION_BYTES 8
#define MAX_BACKWARD_INSTRUCTIONS 5

typedef struct GadgetTree {
    unsigned char instr[MAX_INSTRUCTION_BYTES];
    size_t instr_len;
    uint32_t virtual_address;
    gadget_list_t previous;
    gadget_t *next;
} gadget_t;

void gadget_init(gadget_t *gadgets);
void gadget_add(gadget_t *gadgets, unsigned char *buffer, size_t buffer_len, uint32_t offset);
void gadget_print(gadget_t *gadgets);
void gadget_destroy(gadget_t *gadgets);

#endif
