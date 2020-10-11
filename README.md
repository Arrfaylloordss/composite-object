Composite Pattern C++ Template Library
======================================

*Note*: this project is experimental and for learning purpose only.

C++14 Composite pattern implementation following C++ STL style and iterators approach (which makes it available for using in STL algorithms).

Read about Composite pattern on https://en.wikipedia.org/wiki/Composite_pattern.

Composite object allows creation and maintenance of natural and complex hierarchies of objects. It's fundamental part of almost any program, and a good realization of this pattern and accompanying elements is a key to solid program architecture.

Such well known data structures as file systems, scene graphs, ASTs, GUI components and resources' assets managers are all based on composite object concept. Usually programmers tend to reimplement this pattern every time they write new programs, what is time consuming and error-prone, with the necessity to write a huge amount of good tests. Also C++ is a language which infers additional complication. This is a case when generic template programming may help: implement once and just specialize for every particular need.

Although idea of writing a template solution looks simple, many problems have to be faced during C++ coding.

The key requirements of the composite object for real programs are:

- ability to form inline hierarchies;
- ability to traverse the formed tree with known depth-first and breadth-first algorithms;
- polymorphic iterators which abstracts from the concrete containers and storage models;
- convenient references and tracking of them (like dealing with so called dangling pointers and null references);
- serialization and easy interaction with databases;
- replication across network;
- solid cooperation with other patterns and STL;
- memory related optimizations.

Thus, these are features which are pursued by this project.

Installation
------------
The entire code is gathered in one header file, so just copy it in your project's directory and include as usually.

Tested with MS Visual Studio 2015 C++ compiler, GCC version 4.9 and Clang (see Travis status page for more details).

You can use CMake to generate IDE files for development or/and tests' compilation. Example of command on Windows OS: `cmake -G "Visual Studio 14 2015 Win64" -H. -Bbuild -DWITH_TESTS=TRUE`

License
-------
Copyright Andrey Lifanov 2016.
Boost Software License Version 1.0 (http://www.boost.org/LICENSE_1_0.txt).

Travis CI status
----------------
Master branch:

[![Build Status](https://travis-ci.org/AndrSar/composite-object.svg?branch=master)](https://travis-ci.org/AndrSar/composite-object)

Develop branch:

[![Build Status](https://travis-ci.org/AndrSar/composite-object.svg?branch=develop)](https://travis-ci.org/AndrSar/composite-object)
