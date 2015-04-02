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

package viewer;

import gui.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import sj.lang.ListExpr;
import java.util.Vector;
import sj.lang.IntByReference;
import tools.Reporter;

public class QueryViewer extends SecondoViewer implements ViewerControl,ViewerChangeListener{

/** creates a new QueryViewer */
public QueryViewer(){
  RelSplitter = new RelSplit();
  dummy = new JPanel();
  SplitPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,RelSplitter,dummy);
  SplitPane.setPreferredSize(new Dimension(400,400));
  SplitPane.setDividerLocation(150);
  setLayout(new BorderLayout());
  add(SplitPane,BorderLayout.CENTER);

  // build the MenuExtension
  MenuExtension = new MenuVector();
  SubViewersMenu = new JMenu("SubViewers");
  ShowMenu = new JMenu("show");
  createShowMenu();
  MenuExtension.addMenu(SubViewersMenu);
  MenuExtension.addMenu(ShowMenu);
  RelSplitter.addMouseListener(new MouseAdapter(){
     public void mouseClicked(MouseEvent evt){
        if(evt.getClickCount()>1){
           SecondoObject SO = QueryViewer.this.RelSplitter.getSelectedObject();
           if(SO==null)
             Reporter.showError("no Object selected");
           else{
             QueryViewer.this.getCurrentViewer().addObject(SO);
           }

	}
     }

  });
}


//#####  SecondoViewer-Methods ######
 public String getName(){
    return "Query-Viewer";
 }


 public boolean addObject(SecondoObject o){
   if (!canDisplay(o))
     return false;
   else{
      RelSplitter.addObject(o);
      return true;
   }
 }

 public boolean displayAt(String viewerName, SecondoObject o){
   int index  = getViewerIndex(viewerName);
   if(index<0){
      return false;
   }
   SecondoViewer sv = (SecondoViewer) SubViewers.get(index);
   if(!sv.canDisplay(o)){
      return false;
   }
   selectViewerIndex(index);
   sv.addObject(o);
   return true;
 }


 int getViewerIndex(String name){
   if(name==null){
     return -1;
   }
   for(int i=0;i<SubViewers.size();i++){
      SecondoViewer sv = (SecondoViewer) SubViewers.get(i);
      if(name.equals(sv.getName())){
        return i;
      }
   }
   return -1;
 }


 public void removeObject(SecondoObject o){
   RelSplitter.removeObject(o);
   SecondoViewer SV;
   SecondoObject[] SOS = RelSplitter.getAllObjects();
   for(int i=0;i<SubViewers.size();i++){
       SV = (SecondoViewer) SubViewers.get(i);
       for(int j=0;j<SOS.length;j++)
           SV.removeObject(SOS[j]);
   }
 }

 public void removeAll(){
   RelSplitter.removeAll();
   SecondoViewer SV;
   for(int i=0;i<SubViewers.size();i++){
      ((SecondoViewer) SubViewers.get(i)).removeAll();
   }
 }

 public  boolean canDisplay(SecondoObject o){
    return RelSplitter.canDisplay(o);
 }

 public boolean isDisplayed(SecondoObject o){
    return RelSplitter.isDisplayed(o);
 }


 public boolean selectObject(SecondoObject O){
    return RelSplitter.selectObject(O);
 }

 public void updateMarks(){
     if(VC!=null){
       VC.updateMarks();
     }
 }


 /** get the MenuExtension for MainWindow
   *
   **/
 public MenuVector getMenuVector(){
   return MenuExtension;
 }

//############   ViewerControl Methods #############

 public boolean canActualDisplay(SecondoObject SO){
   return canDisplay(SO);
 }

 public boolean isActualDisplayed(SecondoObject SO){
   return isDisplayed(SO);
 }

 public boolean showObject(SecondoObject SO){
    return addObject(SO);
 }

 public void hideObject(Object Sender,SecondoObject SO){
   removeObject(SO);
 }


public void selectObject(Object Sender,SecondoObject SO){
   selectObject(SO);
}


/** this method should be invoked from viewer if the menu is changed **/
public void updateMenu(){
    computeMenuExtension();
}


public Frame getMainFrame(){
  if (VC==null)
    return null;
  else
    return VC.getMainFrame();
}

public void updateObject(SecondoObject SO){
   if(VC!=null)
       VC.updateObject(SO);
}


public SecondoViewer[] getViewers(){
   Reporter.writeWarning("QueryViewer.getViewers not implemented");
   return null;
}

public void addViewerChangeListener(ViewerChangeListener VCL){
  Reporter.writeWarning("QueryViewer.addViewerChangeListener not implemented");
}

public void removeViewerChangeListener(ViewerChangeListener VCL){
   Reporter.writeWarning("QueryViewer.removeViewerChangeListener not implemented");
}

public int execCommand(String cmd){
  if(VC!=null)
      return VC.execCommand(cmd);
  return 1;
}
public ListExpr getCommandResult(String cmd){
  if(VC!= null)
     return VC.getCommandResult(cmd);
  else
     return null;

}
public boolean execUserCommand(String cmd){
   if(VC!=null)
      return VC.execUserCommand(cmd);
   else
      return false;
}
public boolean execCommand(String cmd, IntByReference errorCode, ListExpr resultList, StringBuffer errorMessage){
   if(VC!=null)
      return VC.execCommand(cmd,errorCode,resultList,errorMessage);
   else{
         errorMessage.append("Queryviewer has no ViewerControl");
         errorCode.value=-1;
         return false;
   }
}



public void setViewerControl(ViewerControl VC){
      super.setViewerControl(VC);
      if(VC!=null){
         VC.addViewerChangeListener(this);
         updateViewers();
      }
 }


/** get all Viewers from VC and set first viewer*/
private void updateViewers(){
   SubViewers.clear();
   SubViewersMenu.removeAll();
   if(VC!=null){
     SecondoViewer[] Vs= VC.getViewers();
     for(int i=0;i<Vs.length;i++){
          if(!Vs[i].equals(this)){
             SubViewers.add(Vs[i]);
             JMenuItem MI_Viewer = SubViewersMenu.add(Vs[i].getName());
             MI_Viewer.addActionListener(new ActionListener(){
                public void actionPerformed(ActionEvent evt){
                    Object Source = evt.getSource();
                    int No = SubViewersMenu.getItemCount();
                    int pos=-1;
                    boolean found=false;
                    for(int i=0;i<No && !found;i++)
                       if(Source.equals(SubViewersMenu.getItem(i))){
                          pos = i;
                          found = true;
                    }
                    selectViewerIndex(pos);
              }
            });
          }
     }
     VC.updateMenu();
   }
   selectViewerIndex(0);
}


public void viewerChanged(){
  updateViewers();
}




/** select Viewer at index */
private void selectViewerIndex(int index){
  if(index<0 | index>SubViewers.size()-1)
     index=0;
  if(SubViewers.size()==0)  // no viewer present
     SplitPane.setRightComponent(dummy);
  else{
     SecondoViewer Viewer = (SecondoViewer) SubViewers.get(index);
     SplitPane.setRightComponent(Viewer);
     Viewer.setViewerControl(this);
     computeMenuExtension();
  }
}


/** compute the Menuextension for the gui */
private void computeMenuExtension(){
   MenuExtension.clear();
   if(VC!=null)
      VC.updateMenu();
   MenuExtension.addMenu(SubViewersMenu);
   MenuExtension.addMenu(ShowMenu);
   Object CurrentViewer = SplitPane.getRightComponent();
   if(!(CurrentViewer==null || CurrentViewer.equals(dummy))){
      SecondoViewer SV = (SecondoViewer) CurrentViewer;
      MenuVector ME = SV.getMenuVector();
      if(ME!=null)
         for(int i=0;i<ME.getSize();i++)
            MenuExtension.addMenu(ME.get(i));
   }
   if(VC!=null)
      VC.updateMenu();
}


private SecondoViewer getCurrentViewer(){
  Component C = SplitPane.getRightComponent();
  if(C==null || C.equals(dummy))
    return null;
  else
    return (SecondoViewer) C;
}

/** create the ShowMenu */
public void createShowMenu(){
  MI_ShowSelectedObject = ShowMenu.add("show selected object");
  MI_ShowSelectedAttribut = ShowMenu.add("show selected attribut");
  MI_ShowAllObjects = ShowMenu.add("show all objects");
  ShowMenu.addSeparator();
  MI_ShowSelectedTuple = ShowMenu.add("show selected Tuple");
  MI_ShowAllTuples = ShowMenu.add("show all Tuples");
  ShowMenu.addSeparator();
  MI_ShowRelation = ShowMenu.add("show relation");

  ActionListener ShowListener = new ActionListener(){
    public void actionPerformed(ActionEvent evt){
        Object Source = evt.getSource();
        if(Source.equals(MI_ShowSelectedObject)){
           SecondoObject SO = RelSplitter.getSelectedObject();
           if(SO==null)
             Reporter.showError("no Object selected");
           else{
             getCurrentViewer().addObject(SO);
           }
        } else if(Source.equals(MI_ShowSelectedAttribut)){
           SecondoObject[] SOs = RelSplitter.getSelectedAttribute();
           if(SOs==null)
             Reporter.showError("no attribute selected");
           else{
             SecondoViewer CV = getCurrentViewer();
             if(SOs.length>0){
                if (CV.canDisplay(SOs[0]))        // all objects have the same type
                   for(int i=0;i<SOs.length;i++)
                      CV.addObject(SOs[i]);
              }
           }
        } else if(Source.equals(MI_ShowAllObjects)){
             SecondoObject[] SOs = RelSplitter.getAllObjects();
             SecondoViewer CV = getCurrentViewer();
             for(int i=0;i<SOs.length;i++)
                 CV.addObject(SOs[i]);

        } else if(Source.equals(MI_ShowSelectedTuple)){
           SecondoObject SOs = RelSplitter.getSelectedTuple();
           if(SOs==null)
             Reporter.showError("no attribute selected");
           else{
             SecondoViewer CV = getCurrentViewer();
             CV.addObject(SOs);
           }
       } else if(Source.equals(MI_ShowAllTuples)){
          SecondoObject[] TPs = RelSplitter.getAllTuples();
          SecondoViewer CV = getCurrentViewer();
          for(int i=0;i<TPs.length;i++){
             CV.addObject(TPs[i]);
          }
       } else if(Source.equals(MI_ShowRelation)){
           getCurrentViewer().addObject(RelSplitter.getRelation());
       }
    } // actionPerformed
  };

  MI_ShowSelectedObject.addActionListener(ShowListener);
  MI_ShowSelectedAttribut.addActionListener(ShowListener);
  MI_ShowRelation.addActionListener(ShowListener);
  MI_ShowAllObjects.addActionListener(ShowListener);
  MI_ShowSelectedTuple.addActionListener(ShowListener);
  MI_ShowAllTuples.addActionListener(ShowListener);

}


private Vector SubViewers = new Vector();
private MenuVector MenuExtension;
private RelSplit RelSplitter;
private JSplitPane SplitPane;
private JPanel dummy;  // if there is no SubViewer
private JMenu SubViewersMenu;
private JMenu ShowMenu;
private JMenuItem MI_ShowSelectedObject;
private JMenuItem MI_ShowSelectedAttribut;
private JMenuItem MI_ShowSelectedTuple;
private JMenuItem MI_ShowAllTuples;
private JMenuItem MI_ShowAllObjects;
private JMenuItem MI_ShowRelation;
private JMenuItem MI_HideSelectedObject;
private JMenuItem MI_HideSelectedAttribut;
private JMenuItem MI_HideAllObjects;
private JMenuItem MI_HideRelation;

}

