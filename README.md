Stackful
========

A small and simple virtual machine written in C++.

It is designed to run a very simple Lisp-like language, [Lithp](http://github.com/andrakis/node-lithp).

It supports only two main data types:

* Numbers (currently 32bit)

* Lists (which may contain Numbers or Lists)

Everything else the VM requires will be implemented with these two basic constructs.
This includes data types such as atoms and strings, the value of which points to
an entry in the atom or string tables.