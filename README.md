Composite Pattern C++ Template Library
======================================

C++14 Composite pattern implementation following C++ STL style and iterators approach (which makes it available for using in STL algorithms).

Read about Composite pattern on https://en.wikipedia.org/wiki/Composite_pattern.

Composite object allows creation and maintenance of natural and complex hierarchies of objects. It's fundamental part of almost any program, and a good realization of this pattern and accompanying elements is a key to solid program architecture.

Such well knows data structures as file systems, scene graphs, GUI components and resources' assets managers are all based on composite object concept. Usually programmers tend to reimplement this pattern every time they write new program, what is time consuming and error-prone, with the necessity to write a huge amount of good tests. Also C++ is a language which implies additional complications to this process. This is a case when generic template programming may help: implement once and just specialize for every particular needs.

Although idea of writing a template library is simple, but, as you can see from the result, many problems are concealed until faced directly in C++ coding.

The key requirements of the composite object for real programs are:

- ability to form inline hierarchies;
- ability to traverse the formed tree with known depth-first and breadth-first algorithms;
- polymorphic iterators which abstracts from the concrete containers and storage models;
- convenient references and tracking of them (like dealing with so called dangling pointers and null references);
- serialization and easy interaction with databases;
- solid cooperation with other patterns and STL;
- memory related optimizations.

Thus, these are features which are pursed in this project.

*Note:* under development, use with caution. Help is appreciated.

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
