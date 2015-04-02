#This file is part of SECONDO.

#Copyright (C) 2004, University in Hagen, Department of Computer Science,
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

operator tupleid alias TUPLEID pattern op( _ )
operator addtupleid alias ADDTUPLEID pattern _ op
operator = alias EQ pattern _ infixop _
operator # alias NE pattern _ infixop _
operator > alias GT pattern _ infixop _
operator < alias LT pattern _ infixop _
operator >= alias GE pattern _ infixop _
operator <= alias LE pattern _ infixop _
operator tid2int alias TID2INT pattern op( _ )
operator int2tid alias INT2TID pattern op( _ )
