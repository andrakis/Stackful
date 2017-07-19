Stackful
========

A small and simple virtual machine written in pure C++ 11. No additional
libraries (eg Boost) are used in this project. The aim is to be portable to
any other C++ 11 compliant platform, particularly the [OpenRISC 1000 platform](https://openrisc.io/).

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

Continuous testing is performed for the following environments:

* Visual Studio (2015 or higher)

   Open the Stackful solution and compile.

   The project targets Visual Studio 2015 Community Edition, but should work
   with any newer version of Visual Studio.

* NetBeans

   Open the nbproject folder and compile. Requires a compiler with C++11 support.

   Includes workaround for a [MinGW bug](https://github.com/andrakis/Stackful/commit/c3b722ad7dc2e462ee0e56fbc1997ceaf4df0683) on Windows.

* GCC (4.8.1 or higher)

   Requires GCC 4.8.1, as this is the first with a feature-complete
   implementation of the 2011 C++ standard.

   The `Makefile` generated by Netbeans has been tested working on Ubuntu 16.04.


Why Stackful?
-------------

Stackful is my attempt at a lightweight VM. It forms the basis of what will
become Stackless, a Lithp implementation similar to Python''s Stackless
implementation.

Having a very simple VM to model will help with my implementation of
Stackless, but I need to write that VM first.

