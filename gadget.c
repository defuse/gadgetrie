#include "gadget.h"
#include "misc.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <libdis.h>

#define RET (0xC3)

void gadget_init(gadget_t *gadgets)
{
    gadgets->instr[0] = RET; 
    gadgets->instr_len = 1;
    gadgets->virtual_address = 0;
    gadget_list_init(&gadgets->previous);
    gadgets->next = NULL;
}

// Work backwards from an instruction to find the instructions that come before it.
// gadgets - the gadget_t for the instruction that comes next.
// buffer - the buffer of program code.
// offset - the virtual memory address that buffer starts at.
// position - the index into buffer of the previously-located instruction.
// count - the number of instructions to find (recursively).
void
work_backwards(gadget_t *gadgets, unsigned char *buffer, uint32_t offset, int position, int count)
{
    if (count <= 0) {
        return;
    }

    int instr_start;
    int instr_size, instr_actual_size;
    x86_insn_t instr;

    // Work back from the known instruction one byte at a time.
    for (instr_size = 1; instr_size <= MAX_INSTRUCTION_BYTES; instr_size++) {
        // Disassemble the bytes between our position and the first byte of the
        // known instruction.
        instr_start = position - instr_size;
        if (instr_start < 0)
            break;
        instr_actual_size = x86_disasm(buffer + instr_start, instr_size, 0, 0, &instr);

        /* 
         * If *all* of those bytes make up one instruction, and it's not
         * a return (gadgets should not contain a RET other than at the end),
         * add it to the tree.
         */
        if (instr_actual_size == instr_size &&
                !(instr_size == 1 && *(buffer + instr_start) == RET)) {

            // If we've already seen this sequence of instructions...
            gadget_t *g;
            if ((g = gadget_list_find_instr(&gadgets->previous, buffer + instr_start, instr_actual_size)) != NULL) {
                // ...see if the address of this one is better.
                if (has_zero_byte(g->virtual_address)) {
                    g->virtual_address = offset + instr_start;
                }
                continue;
            }

            // Otherwise, add it to the list.
            gadget_t *newgadget = malloc(sizeof(gadget_t));
            gadget_init(newgadget);
            memcpy(newgadget->instr, buffer + instr_start, instr_actual_size);
            newgadget->instr_len = instr_size;
            newgadget->virtual_address = offset + instr_start;
            // The next instruction (in execution order) is the one this one
            // comes before.
            newgadget->next = gadgets;
            // The instruction we just found is previous (in execution order) to
            // the one we already know about.
            gadget_list_add(&gadgets->previous, newgadget);

            work_backwards(newgadget, buffer, offset, instr_start, count - 1);
        }
    }

}

void gadget_add(gadget_t *gadgets, unsigned char *buffer, size_t buffer_len, uint32_t offset)
{
    int i;
    for (i = 0; i < buffer_len; i++) {
        if (buffer[i] == RET) {
            if (gadgets->virtual_address == 0 || has_zero_byte(gadgets->virtual_address)) {
                gadgets->virtual_address = offset + i;
            }
            work_backwards(gadgets, buffer, offset, i, MAX_BACKWARD_INSTRUCTIONS);
        }
    }
}

void gadget_print(gadget_t *gadgets)
{
    /* If we're at a leaf node in the tree... */
    if (gadgets->previous.head == NULL) {
        /* Print instructions from the leaf up to the root as a gadget. */
        x86_insn_t instr;
        char line[1000];
        gadget_t *cursor = gadgets;
        
        while (cursor != NULL) {
            x86_disasm(cursor->instr, cursor->instr_len, 0, 0, &instr);
            x86_format_insn(&instr, line, sizeof(line), intel_syntax);
            rstrip(line);
            tab_to_space(line);

            printf("0x%08x: %-50s\n", cursor->virtual_address, line);

            /* Set cursor to its parent in the tree (closer to the RET). */
            cursor = cursor->next;
        }

        printf("-----------------------\n");
    } else {
        /* We're not at a leaf, so recursively print all of the children. */
        gadget_list_item_t *cursor = gadgets->previous.head;
        while (cursor != NULL) {
            gadget_print(cursor->gadget);
            cursor = cursor->next;
        }
    }
}

void gadget_destroy(gadget_t *gadgets)
{
    /* Destroy all child nodes (if any). */
    if (gadgets->previous.head != NULL) {
        gadget_list_item_t *cursor = gadgets->previous.head;
        while (cursor != NULL) {
            gadget_destroy(cursor->gadget);
            free(cursor->gadget);
            cursor = cursor->next;
        }
    }

    /* Destroy the linked-list of children. */
    gadget_list_destroy(&gadgets->previous);
}

