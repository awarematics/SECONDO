/*
----
This file is part of the PD system
Copyright (C) 1998 Ralf Hartmut Gueting, Fachbereich Informatik, FernUniversitaet Hagen

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
----

02.07.2004 M. Spiekermann 

filterpd reads in a list of filenames and checks if the file 
ends with ".cpp" or ".h".

*/

#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;

int main( int argc, char* argv[] ) {

  if ( argc < 2 ) {
    cout << "no files specified" << endl;
    return 1;
  }

  for (int i=1; i < argc; i++) {
  
    string file(argv[i]);
    int strLen = file.length();
    int p1 = file.rfind(".cpp");
    int p2 = file.rfind(".h");

    if (  p1 == abs(strLen-4)  ||  p2 == abs(strLen-2) ) {
      cout << file << endl;
    }
  }

  return 1;
}


