gadgetrie
=========

Gadetrie, pronounced "gadgetry", is a simple ROP gadget finder, written in C.
It is small and simple, but complete. It finds all possible gadgets in the
executable segment of a program and shows them to you. It doesn't do anything
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
