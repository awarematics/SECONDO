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

package viewer.viewer3d.graphic3d;

import gui.idmanager.*;
import viewer.viewer3d.graphic2d.*;

/****************************
*
* Autor   : Thomas Behr
* Version : 1.0
* Datum   : 16.5.2000
*
******************************/

import javax.swing.*;
import java.awt.*;
import java.awt.image.*;

public class World3D extends JComponent {

/** creates a new World3D */
public World3D() {

   Fct3D = new FM3DGraphic();
   W2D   = new World2D();
   FV    = new Figure3DVector();

   Point3DSimple Eye    = Fct3D.getEye();
   Point3DSimple VRP    = Fct3D.getVRP();
   Point3DSimple ViewUp = Fct3D.getViewUp();
   Visitor        = new Visitorclass();

   Visitor.XEye = Eye.getX();
   Visitor.YEye = Eye.getY();
   Visitor.ZEye = Eye.getZ();
   Visitor.XVRP = VRP.getX();
   Visitor.YVRP = VRP.getY();
   Visitor.ZVRP = VRP.getZ();
   Visitor.XViewUp =  ViewUp.getX();
   Visitor.YViewUp  =  ViewUp.getY();
   Visitor.ZViewUp  =  ViewUp.getZ();
   imageChanged=true;
}

/** get the width of the window */
public double getWindowX(){ return Fct3D.getWindowX();}
/** get the height of the window */
public double getWindowY(){return Fct3D.getWindowY(); }
/** get the x-coordinate of the view reference point */
public double getVRPX(){ return Visitor.XVRP; }
/** get the y-coordinate of the view reference point */
public double getVRPY(){ return Visitor.YVRP;}
/** get the z-coordinate of the view reference point */
public double getVRPZ(){ return Visitor.ZVRP;}
/** get the x-coordinate of the position of the eye */
public double getEyeX(){ return Visitor.XEye;}
/** get the y-coordinate of the position of the eye */
public double getEyeY(){ return Visitor.YEye;}
/** get the z-coordinate of the position of the eye */
public double getEyeZ(){ return Visitor.ZEye;}
/** get the x-coordinate of ViewUp-Point */
public double getViewUpX(){ return Visitor.XViewUp;}
/** get the y-coordinate of ViewUp-Point */
public double getViewUpY(){ return Visitor.YViewUp;}
/** get the z-coordinate of ViewUp-Point */
public double getViewUpZ(){ return Visitor.ZViewUp;}
     

/** fill triangles ? */
public void setFill(boolean f) {
   if (f!=fill) {
      fill = f;
      imageChanged=true;
   }
}

/** is filling enabled ?*/
public boolean isFill(){
   return fill;
}


/** single color for figure or gradient-paint */
public void setGradient(boolean V) {
   if (V != this.Gradient) {
       this.Gradient = V;
       imageChanged=true;
   }
}

/** is gradient painting enabled ? */
public boolean isGradient(){
   return Gradient;
}


/** viewport proportional to the window ? */
public void setProportion(boolean P){
 if(P!=proportion){
    proportion = P;
    if(P)
      setViewport(10,10,width-20,height-20);
    else{
      int min = Math.min(width,height);
      setViewport(10,10,min-20,min-20);
    }  
    imageChanged=true;
 }
}

/** is proportional painting enabled ? */
public boolean isProportional(){
  return proportion;
}


/** set the size of this world */
public void setSize(int w, int h) {
    width=w;
    height=h;
    if(proportion)
       setViewport(10,10,w-20,h-20);
    else{
      int min = Math.min(w,h);
      setViewport(10,10,min-20,min-20);
    }
    img = null;
    super.setSize(w,h);
  }

/** set bounds of this world */
public void setBounds(int x, int y, int w, int h){
    width=w;
    height=h;
    if(proportion)
       setViewport(10,10,w-20,h-20);
    else{
      int min = Math.min(w,h);
      setViewport(10,10,min-20,min-20);
    }
    img = null;
    super.setBounds(x,y,w,h);
 }


/** set the view */
public boolean setView(double EyeX, double EyeY, double EyeZ,
                    double VRPX, double VRPY, double VRPZ,
                    double ViewUpX, double ViewUpY, double ViewUpZ){

  boolean result = true;
  try{
    Fct3D.setView(EyeX,EyeY,EyeZ,
                  VRPX,VRPY,VRPZ,
                  ViewUpX,ViewUpY,ViewUpZ);
    Visitor.XEye = EyeX;
    Visitor.YEye = EyeY;
    Visitor.ZEye = EyeZ;
    Visitor.XVRP = VRPX;
    Visitor.YVRP = VRPY;
    Visitor.ZVRP = VRPZ;
    Visitor.XViewUp = ViewUpX;
    Visitor.YViewUp = ViewUpY;
    Visitor.ZViewUp = ViewUpZ;
    imageChanged=true;
  }
  catch(Exception e) {
    result = false;
  }
  return result;
}


/** set the window */
public void setWindow(double x , double y) {
  Fct3D.setWindow(x,y);
  imageChanged=true;
}

/** update the world */
public void update(){
  allNew();
}

/** set the viewport of this world */
public void setViewport(double x0,double y0,double width,double height) {
  Fct3D.setViewport(x0,y0,width,height);
  imageChanged=true;
  allNew();
}


/** insert a new Triangle to the world */
public void insertTriangle(Triangle3D F) {
  FV.append(F);
  imageChanged=true;
}


/** insert a new point to the world */
public void insertPoint(Point3D P) {
   FV.append(P);
   imageChanged=true;
}


/** insert a new line to the world */
public void insertLine(Line3D L){
   FV.append(L);
   imageChanged=true;
}

public void add(Triangle3DVector tv){
   for(int i=0;i<tv.getSize();i++){
       insertTriangle(tv.getTriangle3DAt(i));
   }
}

public void add(Line3DVector lv){
   for(int i=0;i<lv.getSize();i++){
       insertLine(lv.getLine3DAt(i));
   }
}


public void add(Point3DVector pv){
   for(int i=0;i<pv.getSize();i++){
       insertPoint(pv.getPoint3DAt(i));
   }
}


/** removes all figures from the world */
public void removeAll() {
   W2D.deleteAll();
   FV.empty();
   imageChanged=true;
}


/** removes all figures with given ID */
public void removeID(ID RID) {
  W2D.deleteFiguresWithID(RID);
  int i=0;
  int max = FV.getSize();
  while (i<max) {
    if ( FV.getFigure3DAt(i).getID().equals(RID)  ) {
       FV.remove(i);
       max--;
    }else
      i++;
  }
  imageChanged=true;
}



/** paint this world */
public void paint(Graphics G) {
 /*
  if(img==null || img.getHeight()!=getSize().height || img.getWidth()!=getSize().width){
     img =(BufferedImage) createImage(getSize().width,getSize().height);
  }
  if (imageChanged) {
       W2D.deleteAll();
       for(int i=0;i<FV.getSize();i++){
         W2D.insert(FV.get(i).project(Fct3D));
       }
       W2D.paintWorld(img.getGraphics(),getSize().width,getSize().height,fill,Gradient);
       imageChanged=false;
  }*/
 
  if(imageChanged){ 
     W2D.deleteAll();
     for(int i=0;i<FV.getSize();i++){
        Figure3D f3 =  FV.get(i);
        Figure2D f2 =   f3.copy().project(Fct3D);
        W2D.insert(f2);
     }
     W2D.sort();
  }
 
  G.clearRect(0,0,getSize().width,getSize().height);
  W2D.paintWorld(G,getSize().width,getSize().height,fill,Gradient);
  //G.drawImage(img,0,0,null);
}


//##########  private methods

/** update the world */
private void allNew() {
 imageChanged=true;
 repaint();
}

	
/** fill the interior of a triangle */
private boolean       fill=true;                // only a gridpaint ?
/** gradient-paint or single color per figure */
private boolean       Gradient = false;

/** describe the visitor */
private class Visitorclass {
      double  XEye,YEye,ZEye,XVRP,YVRP,ZVRP,
                      XViewUp,YViewUp,ZViewUp;
}


public String toString(){
  return "W3D: Eye =    (" + Visitor.XEye +", " + Visitor.YEye + ", " + Visitor.ZEye + ")\n" +
         "     VRP =    (" + Visitor.XVRP +", " + Visitor.YVRP + ", " + Visitor.ZVRP + ")\n" +
         "     ViewUp = (" + Visitor.XViewUp +", " + Visitor.YViewUp + ", " + Visitor.ZViewUp + ")\n" +
         "     dimension   =  ("+width+", "+height+")";
      

}

/** define the view */
private Visitorclass Visitor;

/** convert 3dim-Objects to 2-dim objects */
private FM3DGraphic   Fct3D;

/** here are the 2D-figures */
private World2D   W2D;

/** the set of triangles */
private Figure3DVector   FV;

/** is the image cahnged ? */
private boolean  imageChanged;
/** here is paint the world */
private BufferedImage img;

/** viewport proportinal to window ? */
private boolean  proportion = false;

private int width;
private int height;

}

