Stackful
========

A small and simple virtual machine written in C++.

It is designed to run a very simple Lisp-like language, [Lithp](http://github.com/andrakis/node-lithp).

It supports only two main data types:

* Numbers (currently 32bit)

* Lists (which may contain Numbers or Lists)

Everything else the VM requires will be implemented with these two basic constructs.

Some additional data types implemented with these constructs (PROPOSAL):

* `(0, 123)` - The literal number `123`

* `(1, (65, 66, 67))` - a string of `ABC`

* `(2, 42)` - A reference to the atom table, item 42

* `(3, (2061584302, 1076407828))` - a double precision float of `12.34`