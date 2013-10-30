/* ROP gadget finder for i386 ELF */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libelf.h>
#include <gelf.h>
#include <unistd.h>
#include <fcntl.h>

#include "gadget.h"

#define TEST_INSTRS "\xFF\xE0\xC3\x31\xC0\xC3\x40\xC3\xFF\xE0\xC3\x31\xC0\xC3\x40\xC3"
#define TEST_INSTRS_LEN 16
/*
JMP EAX
RET
XOR EAX, EAX
RET
INC EAX
RET
JMP EAX
RET
XOR EAX, EAX
RET
INC EAX
RET
*/

void print_usage(void);
void test(void);

int main(int argc, char **argv)
{
    if (argc != 2) {
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "demo") == 0) {
        test();
        return 0;
    }


    /* Code below adapted from 'libelf by Example' */
    Elf *e;
    GElf_Phdr phdr;
    int fd;
    size_t n, i;

    if (elf_version(EV_CURRENT) == EV_NONE) {
        printf("Error with elf library [%s]\n", elf_errmsg(-1));
        exit(1);
    }

    if ((fd = open(argv[1], O_RDONLY, 0)) < 0) {
        printf("Error reading file.\n");
        exit(1);
    }

    if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL) {
        printf("elf_begin() failed [%s]\n", elf_errmsg(-1));
        exit(1);
    }

    if (elf_kind(e) != ELF_K_ELF) {
        printf("File is not an ELF object.\n");
        exit(1);
    }

    if (elf_getphdrnum(e, &n) != 0) {
        printf("elf_getphdrnum() failed [%s]\n", elf_errmsg(-1));
        exit(1);
    }

    gadget_t gadgets;
    gadget_init(&gadgets);

    /* Loop over each program header (segment) */
    for (i = 0; i < n; i++) {
        if (gelf_getphdr(e, i, &phdr) != &phdr) {
            printf("gelf_getphdr() failed [%s]\n", elf_errmsg(-1));
            exit(1);
        }

        /* If the segment is loaded into memory AND is executable */
        if (phdr.p_type == PT_LOAD && phdr.p_flags & PF_X) {
            /* Load the segment from the ELF file */
            unsigned char *buffer = malloc(phdr.p_filesz);
            lseek(fd, phdr.p_offset, SEEK_SET);
            ssize_t count = read(fd, buffer, phdr.p_filesz);
            if (count != phdr.p_filesz) {
                printf("read sucks.\n"); // FIXME: EINTR etc.
                exit(1);
            }
            /* Add all gadgets in the segment to our gadget tree */
            gadget_add(&gadgets, buffer, phdr.p_filesz, phdr.p_vaddr);
            free(buffer);
        }
    }

    elf_end(e);
    close(fd);

    gadget_print(&gadgets);
    gadget_destroy(&gadgets);

    return 0;
}

void print_usage(void)
{
    printf("Usage: ./gadget_finder <elf file|demo>\n");
}

void test(void)
{
    gadget_t gadgets;
    gadget_init(&gadgets);
    gadget_add(&gadgets, (unsigned char*)TEST_INSTRS, TEST_INSTRS_LEN, 0);
    gadget_print(&gadgets);
    gadget_destroy(&gadgets);
}
