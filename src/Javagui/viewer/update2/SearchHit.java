//This file is part of SECONDO.

//Copyright (C) 2004, University in Hagen, Department of Computer Science, 
//Database Systems for New Applications.

//SECONDO is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.

//SECONDO is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with SECONDO; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

package viewer.update2;

public class SearchHit implements Comparable<SearchHit>
{

	private int rowIndex=-1;
	private int start=-1;
	private int end=-1;
	
	public SearchHit(int pRowIndex, int pStart, int pEnd)
	{
		this.rowIndex = pRowIndex;
		this.start = pStart;
		this.end = pEnd;
	}
	
	public int getRowIndex()
	{
		return this.rowIndex;
	}
	
	public int getStart()
	{
		return this.start;
	}
	
	public int getEnd()
	{
		return this.end;
	}
	
	public int compareTo(SearchHit o)
	{
		int result = this.rowIndex - ((SearchHit)o).getRowIndex();
		if (result == 0) 
		{
			result = this.start - ((SearchHit)o).getStart();
		}		
		return result;
	}
	
	@Override
	public boolean equals(Object pOther)
	{
		SearchHit o = (SearchHit)pOther;
		return (this.rowIndex == o.getRowIndex()
				&& this.start == o.getStart()
				&& this.end == o.getEnd());
	}
	
	@Override
	public String toString()
	{
		StringBuffer sb = new StringBuffer("[viewer.update2.SearchHit]");
		sb.append(" rowIndex=").append(this.rowIndex);
		sb.append(", start=").append(this.start);
		sb.append(", end=").append(this.end);
		
		return sb.toString();
	}
}

