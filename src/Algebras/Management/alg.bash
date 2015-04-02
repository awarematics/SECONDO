#This file is part of SECONDO.
#
#Copyright (C) 2004, University in Hagen, Department of Computer Science, 
#Database Systems for New Applications.
#
#SECONDO is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.
#
#SECONDO is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with SECONDO; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#!/bin/sh
#
# This shell script creates a regular expression
# as a disjunction of all algebra names. For these Algebras
# the configuration parameters are extracted from the file AlgebraList.i.cfg
# and prefixed with an "ALGEBRA_INCLUDE("

# Parameters:
# $1 file with algebra IDs
# $2 - $n algebra names

file="$1"
regexp="(,$1"
shift
while [ $# -ne 0 ];
do
   grep ", *$1" $file
   shift
done


