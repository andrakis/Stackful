Stackful
========

A small and simple virtual machine written in pure C++ 11. No additional
libraries (eg Boost) are used in this project.

It is designed to run a very simple Lisp-like language, [Lithp](http://github.com/andrakis/node-lithp).

It supports only two main data types:

* Numbers (currently 32bit)

* Lists (which may contain Numbers or Lists)

Everything else the VM requires will be implemented with these two basic constructs.

Some additional data types implemented with these constructs (PROPOSAL):

* `(0, 42)` - A reference to the atom table, item 42

* `(1, 123)` - The literal number `123`

* `(2, (2061584302, 1076407828))` - a double precision float of `12.34`

* `(3, (65, 66, 67))` - a string of `ABC`

Compilation
-----------

* Visual Studio

   Open the Stackful solution and compile.

* GCC

   `g++ -std=c++11 *.cpp src/*.cpp -o stackful`

   Most compilers will require the `std=` option for correct compilation.
  
    

Why Stackful?
-------------

Stackful is my attempt at a lightweight VM. It forms the basis of what will
become Stackless, a Lithp implementation similar to Python''s Stackless
implementation.

Having a very simple VM to model will help with my implementation of
Stackless, but I need to write that VM first.

