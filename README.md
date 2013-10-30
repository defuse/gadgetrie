gadgetrie
=========

Gadetrie, pronounced "gadgetry", is a simple ROP gadget finder, written in C.
It is small and simple, but complete. It finds all possible gadgets in the
executable segment(s) of a program and shows them to you. It doesn't do anything
fancy like constructing a payload for you, it's just a gadget finder.

It only supports 32-bit ELF binaries.

License
-------

This software uses the MIT license. See the LICENSE file.

The following paragraph justifies the choice of license and does not have any
affect on the copyright of this software.

I normally use the GPL v3, but in this case I chose the MIT license over the GPL
because this code is most likely to be used by researchers. I don't want to
force researchers to GPL their much larger project in order to use my code. They
know what's best for their project, and that might not be a GNU license.
I believe that any responsible scientifically-minded researcher will do their
best to keep the code open even when they don't have to.

How it Works
------------

### Unintended Instructions

On the x86, instructions are variable-length, and don't have to be aligned, so
it's possible to find "unintended" instructions in machine code by starting the
disassembly at a different offset.

Consider this sequence of bytes:

    05 5A 59 5B 58 C3

If we start disassembling at the first byte, we find out that this is the
sequence of instructions:

    05 5A 59 5B 58      | ADD EAX, 0x585B595A
    C3                  | RET

However, if we skip the first byte, 0x05, and start disassembling at the second
byte, 0x5A, we get something different:

    05                  | (skipped)
    5A                  | POP EDX
    59                  | POP ECX 
    5B                  | POP EBX
    58                  | POP EAX
    C3                  | RET

The programmer wrote an instruction to add a value to EAX, but its machine code
can be interpreted in a different ways to reveal different instructions. We have
to take this into account when searching for gadgets, since these unintended
instructions might prove useful.

### Search Algorithm

We're interested in finding sequences of instructions that end in a return
instruction (RET). These sequences are called gadgets. On x86, this instruction
is encoded as a single byte 0xC3.

To find gadgets in a corpus of machine code, we first search for all return
instructions (0xC3 bytes). Then, for each return instruction, we work backwards
to enumerate the instructions before it. Since x86 instructions are
variable-length, we don't know how long the instruction before the return is, so
we try all possible lengths. We start at 1 byte, and go up to the maximum length
of an x86 instruction, which is 15 bytes. 

Even if we find, say, a two-byte instruction before the return, we will keep
going and check for 3+ byte instructions, because those extra bytes might reveal
different unintended instructions before the return. We always check up to the
maximum length of 15 bytes. Each time we find an instruction before the return,
we add it to a list.

Then, for each instruction before the return, we apply the same process
recursively. We now want to find the instructions before the instruction before
the return. As we do this, we build a tree of instructions that come before the
return.

The root of the tree is the return instruction itself, its children are the
instructions that come directly before the return by looking at different
interpretations of the machine code, and their children are the instructions
that come directly before them, also by looking at different interpretations of
the machine code.

This continues until we've found up to 5 instructions before the return.

So far we have only looked at the instructions coming before a single return. In
a body of machine code, there will be many return instructions, so the next step
is to combine them all into one tree. This is done by keeping one big tree
holding all of the sequences of instructions found before a return. Whenever we
find a sequence of instructions before a return, we check if it is already in
the tree. If not, we add it into the tree.

When the process is finished, we have one huge tree of instructions, whose root
is a return, and whose branches represent all of the gadgets available in the
program code. As we build the tree, we keep in the nodes the address at which
the sequence starts, so from the tree, we can find where the gadgets start.

For example, if we searched the following bytes for gadgets, we would get the
following tree.

    Address:    0  1  2  3  4  5  6  7
    Code:       05 5A 59 5B 58 C3 50 C3
                                ^     ^   (returns)
    Tree:
                +-----------+
                | RET (5,7) |
                +----+------+
                     |
             .-------+-----.-------------------.
            /               \                   \
    +------+-------+     +---+---------+      +--+----------------------+
    | PUSH EAX (6) |     | POP EAX (4) |      | ADD EAX, 0x585B595A (0) |
    +------+-------+     +------+------+      +-------------------------+
                                |
                         +------+------+
                         | POP EBX (3) |
                         +------+------+
                                |    
                         +------+------+
                         | POP ECX (2) |
                         +------+------+
                                |    
                         +------+------+
                         | POP EDX (1) |
                         +-------------+

### Credit

The algorithm used here was presented in "Return Oriented Programming: Systems,
Languages, and Applications" by Ryan Roemer, Erik Buchanan, Hovav Shacham, and
Stefan Savage from UCSD.

