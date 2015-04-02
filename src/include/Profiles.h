/*
----
This file is part of SECONDO.

Copyright (C) 2004, University in Hagen, Department of Computer Science,
Database Systems for New Applications.

SECONDO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

SECONDO is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SECONDO; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
----

//paragraph	[10]	title:		[{\Large \bf ] [}]
//paragraph	[11]	title:		[{\large \bf ] [}]
//paragraph	[12]	title:		[{\normalsize \bf ] [}]
//paragraph	[21]	table1column:	[\begin{quote}\begin{tabular}{l}]	[\end{tabular}\end{quote}]
//paragraph	[22]	table2columns:	[\begin{quote}\begin{tabular}{ll}]	[\end{tabular}\end{quote}]
//paragraph	[23]	table3columns:	[\begin{quote}\begin{tabular}{lll}]	[\end{tabular}\end{quote}]
//paragraph	[24]	table4columns:	[\begin{quote}\begin{tabular}{llll}]	[\end{tabular}\end{quote}]
//[--------]	[\hline]
//characters	[1]	verbatim:	[$]	[$]
//characters	[2]	formula:	[$]	[$]
//characters	[4]	teletype:	[\texttt{]	[}]
//[ae] [\"a]
//[oe] [\"o]
//[ue] [\"u]
//[ss] [{\ss}]
//[<=] [\leq]
//[#]  [\neq]
//[tilde] [\verb|~|]

1 Header File: Profiles

January 2002 Ulrich Telle

January 2005, M. Spiekermann. Key values set in the configuration file can
be overruled by environment variables named SECONDO\_PARAM\_~keyname~.

1.1 Overview

Applications often need a mechanism for providing configuration parameters.
One way to supply those parameters is a profile. A profile is a text file
consisting of one ore more named sections which contain one or more named
parameters and their values, respectively. Each line in the profile is
either a section heading, a key/value pair or a comment. A section heading
is enclosed in square brackets; key name and key value are separated by an
equal sign (leading and trailing whitespace is removed from both key name
and value, but intervening blanks are considered to be part of the name or
the value); a comment line starts with semicolon. All Text in a profile has
to be left aligned. Example:

----  [section 1]
      ;   comment
      key 1=value 1
      key 2=value 2

      [section 2]
      ;   comment
      key 1=value 1
      ....     (and so on)
----

1.1 Interface methods

This module offers routines to get or set a parameter:

[21]	Routines\\
	[--------]
	GetParameter \\
	SetParameter \\

1.3 Class "SmiProfile"[1]

The class ~SmiProfile~ implements routines to manipulate profile strings
stored in a text file:

*/

#ifndef SMI_PROFILES_H
#define SMI_PROFILES_H

#include <string>
#include <sstream>

#include "SecondoConfig.h"
#include "WinUnix.h"

using namespace std;

class SMI_EXPORT SmiProfile
{
 public:
  static string GetParameter( const string& sectionName,
                              const string& keyName,
                              const string& defaultValue,
			      const string& fileName );
/*
Searches the profile ~fileName~ for the key ~keyName~ under the section heading
~sectionName~. If found, the associated string is returned, else the
default value is returned.

If ~sectionName~ is the empty string, it will be ignored. i.e. ALL sections are
searched.

*/


  static string GetUniqueSocketName(const string& fileName)
  {
    stringstream sname;
    sname << GetParameter("Environment", "RegistrarSocketNamePrefix", 
			  "SECREGIS", fileName); 
    sname << "_port";
    sname << GetParameter("Environment", "SecondoPort",
		          "SECREGIS", fileName);
    return sname.str();
  }	  
/*
If many Secondo installations are placed on the same server, the name of the socket used to
communicate with the SecondoRegistrar process needs to be unique. Since the port 
number for client server communication needs also to be unique we use it as
identifier in the socket name. During runtime, a file like tmp/SECREGIS\_port1234
may represent such a socket.

*/



  static long   GetParameter( const string& sectionName,
                              const string& keyName,
                              long          defaultValue,
                              const string& fileName );
/*
Searches the profile ~fileName~ for the key ~keyName~ under the section heading
~sectionName~. The function returns

  * ~zero~ -- if the key value is not an integer,

  * ~actual value~ -- if it is possible to interpret the key value as an
integer (Note that only the beginning of the key value is interpreted, i.e.
"key=345abc"[4] returns "345"[4])

  * ~default value~ -- if key or section not found

If ~sectionName~ is the empty string, it will be ignored. i.e. ALL sections are
searched.

*/
  static bool   SetParameter( const string& sectionName,
                              const string& keyName,
                              const string& keyValue,
                              const string& fileName );
/*
Searches the profile ~fileName~ for the section heading ~sectionName~ and
the key ~keyName~. If found the value is changed to ~keyValue~, otherwise
it will be added.

The first special case is when ~keyValue~ is an empty string. Then the
corresponding line is deleted.

The second special case is when both ~keyValue~ and ~keyName~ are empty
strings. In that case the whole section ~sectionName~ is deleted.
Lines beginning with ';' are however kept since they are considered comments.

*/
};

#endif // SMI_PROFILES_H

