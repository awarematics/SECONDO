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

operator oleftrange alias OLEFTRANGE pattern _ op [list]
operator orightrange alias ORIGHTRANGE pattern _ op [list]
operator orange alias ORANGE pattern _ op [list; list]
operator oshortestpathd alias OSHORTESTPATHD pattern _ op [ _, _,_; fun] 
         implicit parameter tuple type TUPLE
operator oshortestpatha alias OSHORTESTPATHA pattern _ op [ _, _,_; fun, fun]
         implicit parameter tuple type TUPLE
