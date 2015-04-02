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
//paragraph	[21]	table1column:	[\begin{quote}\begin{tabular}{l}]	[\end{tabular}\end{quote}]
//paragraph	[22]	table2columns:	[\begin{quote}\begin{tabular}{ll}]	[\end{tabular}\end{quote}]
//paragraph	[23]	table3columns:	[\begin{quote}\begin{tabular}{lll}]	[\end{tabular}\end{quote}]
//paragraph	[24]	table4columns:	[\begin{quote}\begin{tabular}{llll}]	[\end{tabular}\end{quote}]
//[--------]	[\hline]
//characters	[1]	verbatim:	[$]	[$]
//characters	[2]	formula:	[$]	[$]
//characters	[3]	capital:	[\textsc{]	[}]
//characters	[4]	teletype:	[\texttt{]	[}]
//[ae] [\"a]
//[oe] [\"o]
//[ue] [\"u]
//[ss] [{\ss}]
//[<=] [\leq]
//[#]  [\neq]
//[tilde] [\verb|~|]
//[Tilde] [\verb|~|]

1 Header File: File System Management

May 2002 Ulrich Telle

October 2003 M. Spiekermann new function GetParentFolder() added.

June 2009 Sven Jungnickel new function MakeTemp() added.

1.1 Overview

The ~File System Management~ provides several services for handling files and
folders (directories) in an operating system independent manner. There are
functions for inspecting and manipulating the folder (directory) tree. No
functions for file access are provided.

1.3 Interface methods

The class ~SmiEnvironment~ provides the following methods:

[23]    Folder Management & Files and Folders & File management \\
        [--------]
        GetCurrentFolder & FileOrFolderExists & CopyFile          \\
        SetCurrentFolder & RenameFileOrFolder & GetFileAttributes \\
        CreateFolder     & DeleteFileOrFolder & SetFileAttributes \\
        EraseFolder      & FileSearch         &      \\
        GetParentFolder  & AppendSlash        &      \\
                         & MakeTemp           &      \\

1.4 Imports, Constants, Types

*/
#ifndef FILESYSTEM_H
#define FILESYSTEM_H 1

#include "SecondoConfig.h"
#include <string>
#include <vector>

using namespace std;

#ifdef SECONDO_WIN32
#include <windef.h>
typedef DWORD FileAttributes;
#else
typedef uint32_t FileAttributes;
#endif
/*
Is the type for the file attributes for a specific file.

*NOTE*: The values of the file attributes are operating system dependent.
One has to keep this in mind when implementing portable applications.

*/
typedef vector<string> FilenameList;
/*
Is the type for a collection of filenames found by a File Search.

*/
typedef bool (*FileSearchCallbackFunc)
          ( const string& absolutePath,
            const string& fileName,
            FileAttributes attribs );
/*
Is the type of user-supplied functions for filename filtering. The
function arguments are:

  * ~absolutePath~ -- Directory where file resides.

  * ~fileName~ -- Filename without directory.

  * ~attribs~ -- File attributes.

*/

/**************************************************************************
1.3 Class "FileSystem"[1]

This class implements all functions for the file system management as static
members. Since the constructor is private the class cannot be instantiated.

*/
class FileSystem
{
 public:
  static string GetCurrentFolder();
/*
Returns the current folder (directory).

*/
  static string GetParentFolder( const string& folder, int level = 1 );
/*
Returns the parent of directory ~folder~. The optional second parameter
indicates the number of levels to go upwards.

*/

  static bool SetCurrentFolder( const string& folder );
/*
Sets the current folder (directory) to ~folder~.
The function returns "true"[4], if the current folder could be set.

*/
  static bool CreateFolder( const string& folder );
/*
Creates the folder (directory) located at ~folder~.
The function returns "true"[4], if the folder could be created.

*/
  static bool IsDirectory( const string& fileName );
/*
Check, whether fileName is a directory.
The function returns "true"[4], iff the folder exists.

*/

  static bool DeleteFileOrFolder( const string& fileName );
/*
Deletes the file or folder (directory) specified in ~fileName~.
The function returns "true"[4], if the file or folder could be deleted.

The function fails if the file is protected by file attributes or
if the folder to be removed contains one or more files.

*/
  static bool EraseFolder( const string& folder,
                           uint16_t maxLevels = 16 );
/*
Removes the folder (directory) specified in ~folder~.
The function returns "true"[4], if the remove operation succeeded.

This function makes every attempt to delete the folder (directory), such as
removing file protection attributes and files contained within folders
(directories).

~maxLevels~ controls how many levels of subfolders the
function will traverse in order to remove a folder (directory).

*NOTE*: When ~EraseFolder~ deletes multiple files, files will be deleted until
an error occurs. Any files that were successfully deleted before the error
occurred will not be restored. This situation typically occurs if
the user does not have permission to remove a file.

*/
  static bool RenameFileOrFolder( const string& currentName,
                                  const string& newName );
/*
Renames (moves) a file or folder (directory) from ~currentName~ to ~newName~.
The function returns "true"[4], if the copy operation succeeded.

*/
  static bool Copy_File( const string& source, const string& dest );
/*
Copies a file from ~source~ to ~dest~.
The function returns "true"[4], if the copy operation succeeded.

*NOTE*: On Unix systems this function may be used to copy folders (directories)
as well. Keep in mind that this property is not portable.

*/
  static bool FileOrFolderExists( const string& fileName );
/*
Checks for the existence of the file indicated by ~fileName~.
The function returns "true"[4], if the file exists.

*/
  static FileAttributes Get_FileAttributes( const string&
                                             fileName );
/*
Returns the file attributes for the file ~fileName~.
In case of an error the function returns 0.

*/
  static bool Set_FileAttributes( const string& fileName,
                                 FileAttributes attribs );
/*
Sets the file attributes for a file to the values specified in ~attribs~ .
The function returns "true"[4], if the attributes could be set.

*/
  static int32_t GetFileSize( const string& fileName );
/*
Returns the file size of the specified file ~fileName~ in Bytes, iff that
file exists. Return -1 iff the file does not exist or any error occurs.

*/

  static bool FileSearch( const string& folder,
                          FilenameList& filenameList,
                          const string* searchName = 0,
                          uint16_t maxLevels = 1,
                          bool includeFolders = true,
                          bool fullPath = true,
                          FileSearchCallbackFunc
                            fileSearchCallback = 0 );
/*
Returns a list of filenames which meet the search criteria in ~filenameList~.
~folder~ indicates the folder (directory) where the search begins.

If ~searchName~ is specified, the list will only contain the
files matching this name. Wildcard searches are currently *not* supported,
but the callback function ~fileSearchCallback~ can be used to filter the
filenames.

~maxLevels~ controls how many levels of subdirectories will be searched.

~includeFolders~ specifies whether subfolder names are to be included in the
list of filenames.

If ~fullPath~ is "true"[4], the complete pathname of each file will be returned.

*/
  static bool SearchPath( const string& fileName, string& foundFile );
/*
Searches the file ~fileName~ on the path and returns "true"[4] if the file was
found, otherweise "false"[4]. If the file was found the complete pathname of
the file is returned in ~foundFile~.

*/

  static string MakeTemp(const string& templ);
/*
Creates a unique temporary file name. The argument ~templ~ is used as a prefix
and normally contains the directory and a filename prefix. ~MakeTemp~ appends
current CPU clock time and a unique number. 

*/

  static void AppendSlash( string& pathName );
  static void AppendItem( string& pathName, const string& item);

/*
Appends the proper slash character to a pathname.
This character will either be a forward or backward
slash, depending on the operating system used. The second form also adds the given
subitem.

*/

 protected:
 private:
#ifdef SECONDO_WIN32
  static void UnprotectFile( const string& fileName );
/*
Removes file protection attributes from a file, so that
it may be modified or deleted.

*NOTE*: This function is available in Windows only.

*/
#endif
/*
The following functions are not implemented and must never be used.

*/
  FileSystem() {};
  ~FileSystem() {};
  FileSystem( const FileSystem& other );
  FileSystem& operator=( const FileSystem& other );
  int operator==( const FileSystem& other ) const;
};

#endif // FILESYSTEM_H

