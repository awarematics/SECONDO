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

package  viewer.update2.gui;

import java.awt.Dimension;
import java.awt.Point;
import java.awt.Rectangle;
import java.io.FileNotFoundException;
import java.io.IOException;
import javax.swing.JPanel;
import javax.swing.Scrollable;
import java.util.ArrayList;
import java.util.List;


import viewer.update2.*;
import viewer.update2.format.*;

/**
 * Panel that displays a formatted document.
 * 
 */
public abstract class DocumentPanel extends JPanel implements Scrollable
{
 	protected RelationPosition currentPosition;
	protected DocumentFormatter formatter;
	
	public DocumentPanel()
	{
		super();
		this.currentPosition = null;
		this.setSize(new Dimension(800,600));
	}
	
	/**
	 * Returns formatter of class '<pFormatType>Formatter' if that class exists in package viewer.update2.format.
	 */
	public static DocumentPanel createDocumentPanel(DocumentFormatter pFormatter) throws Exception
	{
		// try to instatiate specific formatter class
		Class panelClass = Class.forName("viewer.update2.gui." + pFormatter.getType() + "DocumentPanel");
		Object o = panelClass.newInstance();
		if(!(o instanceof DocumentPanel))
		{
			throw new Exception("Found class does not extend the abstract class DocumentPanel. ");
		}
		DocumentPanel panel = (DocumentPanel)o;
		panel.formatter = pFormatter;
		return panel;
	}
	
	/**
	 * Returns current position in formatted document:
	 * relation name, attribute name, tuple id, position within value.
	 * This information is read from the hidden markup in the document.
	 */
	public RelationPosition getCurrentRelationPosition() { return this.currentPosition;	}	
	
	/**
	 * Scrolls the Document to the specified position.
	 */
	public abstract void goTo(RelationPosition pRelationPosition);
	
	/**
	 * Displays pages sequentially in appropriate Components.
	 */
	public abstract void load(List<Object> pOutputPages);
	
	/**
	 * Determines the RelationPosition from the mouse position in the specified component.
	 */
	public abstract RelationPosition trackPosition(Point pMousePosition, Object pComponent);
	
	
	// Methods of Interface Scrollable
	public Dimension getPreferredScrollableViewportSize() {	return getPreferredSize();	}
    public int getScrollableBlockIncrement(Rectangle arg0, int arg1, int arg2) { return 50;	}
	public boolean getScrollableTracksViewportHeight() { return false; }
	public boolean getScrollableTracksViewportWidth() { return true; }
	public int getScrollableUnitIncrement(Rectangle arg0, int arg1, int arg2) {	return 10; }
}
