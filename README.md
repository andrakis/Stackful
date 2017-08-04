Stackful
========

A small and simple virtual machine written in pure C++ 11. No additional
libraries (eg Boost) are used in this project. The aim is to be portable to
any other C++ 11 compliant platform, particularly the [OpenRISC 1000 platform](https://openrisc.io/).

It is designed to run a very simple Lisp-like language, [Lithp](http://github.com/andrakis/node-lithp).

Under the hood, it operates on only two data types:

* Numbers (currently 32bit)

* Lists (which may contain Numbers or Lists)

Everything else is implemented by extended the basic list type with additional class information.

Some additional data types implemented with these constructs (MAY BE OUT OF DATE):

* `(0, 42)` - A reference to the atom table, item 42

* `(1, 123)` - The literal number `123`

* `(2, (2061584302, 1076407828))` - a double precision float of `12.34`

* `(3, (65, 66, 67))` - a string of `ABC`, also allowing for unicode


Status
------

* *Version 0.20*:

   The interpreter has been implemented, as well as the following builtins:

   * `print/*` - Print given arguments `(print Any::any)::nil`

   * `var/2` - Set a variable in the current scope `(var Name::atom Value::any)::Value`

   * `get/1` - Get a variable from the current scope `(get Name::atom)::Value::any)`

   There is a basic test which constructs an OpChain and runs it, resulting an output message.

* *Priority List*:

   1. Basic control flow and arithmatic builtins

   2. AST compiler

   3. Native parser

   4. Extended builtins

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

   Useful options supported by `Makefile`:

   * `CONF=Debug` - (This is the default) build debug configuration (no optimization, debugging)

   * `CONF=Release` - build release configuration (optimization, no debugging)

   * `CXX=or1k-linux-musl-g++` - cross compilation for `OpenRISC 1000` compiler suite

   * `CXXFLAGS=-s` - strip all possible symbols from output file

   ***Note***: The output file will be `dist/($CONF)/stackful`.

There should be no warnings present. All warnings, including extended warnings and PVS-Studio optimization
warnings have been dealt with. This project aims to build cleanly on all platforms.

Memory is managed by use of C++11 `shared_ptr` template. However, there are probably memory leaks at this
early stage.


Why Stackful?
-------------

The overarching goal is to provide a runtime environment as powerful as [Lithp](https://github.com/andrakis/node-lithp)
whilst also being very small, so as to run it within an [OpenRISC 1000 emulator](http://s-macke.github.io/jor1k/demos/main.html).

To that end, this project is targeted towards GCC 4.9.1, and builds and runs
correctly on Ubuntu 16.04 cross compilation as well as the compilation environment available
within the [jorconsole](https://github.com/andrakis/jorconsole) project. This allows it to be
built on a fast host system or within the emulator, provided enough memory is available. The
resulting binary can then be run in any OpenRISC 1000 emulator, particularly [jor1k](https://github.com/s-macke/jor1k).

***Note***: This project cannot built under [the jor1k demo](http://s-macke.github.io/jor1k/demos/main.html) due to memory constraints. The demo page creates
a virtual machine with only 32Mb RAM. The [jorconsole](https://github.com/andrakis/jorconsole) project allows
running [jor1k](https://github.com/s-macke/jor1k) with additional memory (128M works for compilation.)

Stackful is my attempt at a lightweight VM. It forms the basis of what will
become Stackless, a Lithp implementation similar to Python''s Stackless
implementation.

Having a very simple VM to model will help with my implementation of
Stackless, but I need to write that VM first.
