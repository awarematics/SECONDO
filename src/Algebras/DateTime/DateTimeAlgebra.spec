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

operator weekday_of alias WEEKDAY_OF pattern  op ( _ )
operator toString alias TOSTRING pattern _ op
operator leapyear alias LEAPYEAR pattern _ op
operator year_of alias YEAR pattern  op ( _ )
operator month_of alias MONTH pattern   op ( _ )
operator day_of alias DAY pattern   op ( _ )
operator hour_of alias HOUR pattern  op ( _ )
operator minute_of alias MINUTE pattern  op ( _ )
operator second_of alias SECOND pattern  op ( _ )
operator millisecond_of alias MILLISECOND pattern  op ( _ )
operator + alias PLUS pattern _ infixop _
operator - alias MINUS pattern _ infixop _
operator = alias EQUAL pattern _ infixop _
operator < alias LESS pattern _ infixop _
operator > alias GREATER pattern _ infixop _
operator * alias PRODUCT pattern _ infixop _
operator now alias NOW pattern op()
operator today alias TODAY pattern op()
operator theInstant alias THEINSTANT pattern op(_)
operator / alias DIVIDE pattern _ infixop _
operator create_duration alias CREATE_DURATION pattern op ( _ , _ )
operator create_instant alias CREATE_INSTANT pattern op ( _ , _ )
operator duration2real alias DURATION2REAL pattern op ( _ )
operator instant2real alias INSTANT2REAL pattern op ( _ )
operator mininstant alias MININSTANT pattern op()
operator maxnstant alias MAXINSTANT pattern op()
operator minduration alias MINDURATION pattern op()
operator maxduration alias MAXDURATION pattern op()
operator div alias DIV pattern _ infixop _



