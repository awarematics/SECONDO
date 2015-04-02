#This file is part of SECONDO.

#Copyright (C) 2013, University in Hagen, Department of Computer Science,
#Database Systems for New Applications.

#SECONDO is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.

#SECONDO is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with SECONDO; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

# author: Dirk Zacher

operator atinstant alias ATINSTANT pattern _ infixop _
operator atlocation alias ATLOCATION pattern _ op [_,_]
operator atperiods alias ATPERIODS pattern _ infixop _
operator atrange alias ATRANGE pattern _ op [_,_,_]
operator bbox alias BBOX pattern op(_,_)
operator compose alias COMPOSE pattern _ infixop _
operator deftime alias DEFTIME pattern op(_)
operator fromline alias FROMLINE pattern op(_)
operator fromregion alias FROMREGION pattern op(_,_)
operator getgrid alias GETGRID pattern op(_)
operator inst alias INST pattern op(_)
operator load alias LOAD pattern op(_)
operator map alias MAP pattern _ op [fun]
         implicit parameter element type CELL1
operator map2 alias MAP2 pattern _ _ op [fun]
         implicit parameters elem1, elem2 types CELL1, CELL2
operator matchgrid alias MATCHGRID pattern _ op [_,fun,_]
         implicit parameter element type CELLS
operator maximum alias MAXIMUM pattern op(_)
operator minimum alias MINIMUM pattern op(_)
operator t2mt alias TTOMT pattern  op(_,_,_,_)
operator tiles alias TILES pattern op(_)
operator toraster2 alias TORASTER2 pattern op(_)
operator toregion alias TOREGION pattern _ op
operator val alias VAL pattern op(_)
