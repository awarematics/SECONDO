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
---
*/

package viewer.update2;
 

/*
 * This class contains constants that each name a state of the 'UpdateViewer2'.
 */
public final class States 
{
	
	// The controller is always in one certain state 
	public final static int INITIAL= 0;
	public final static int LOADED = 1;
	public final static int INSERT = 2;
	public final static int DELETE = 3;
	public final static int UPDATE = 4;
	public final static int FORMAT = 5;

	public final static int LOADED_READ_ONLY = 11;
}
