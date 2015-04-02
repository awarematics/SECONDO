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

package project;

import java.util.Properties;

import tools.Reporter;

public class Mercator implements Projection{

  public boolean project(double lambda, double phi, java.awt.geom.Point2D.Double result){
     if(phi>90 || phi<-90)
        return false;

     if(phi<=(-90+secure_distance))
	       phi = -90+secure_distance;
     if(phi>=(90-secure_distance))
        phi = 90-secure_distance;

     result.x = lambda-Lambda_0;
     double phi_2 = phi*PI/180;
     double y =   Math.log(Math.tan(PI/4 + phi_2/2))/LOG_E;
     result.y = y*180/PI;
     return true;
  }
   
  public boolean showSettings(){
     Reporter.writeWarning("Mercator.showSettings not implemented");
     return true;
   }

   public String getName(){
     return "Mercator";
   }

   public boolean isReversible(){
      return true;
   }


  public boolean getOrig(double prjx, double prjy, java.awt.geom.Point2D.Double result){
     double lambda = 0;
     double phi = 0;
     try{
       lambda = prjx + Lambda_0; 
         prjy = prjy*PI/180;
         double res =  2*Math.atan(Math.exp(prjy))-0.5*PI;    
         phi = res*180/PI;
     }catch(Exception e){
        return false;
     }
     result.x = lambda;
     result.y = phi;
     return true;
  }

	public Properties getProperties() {
		Properties p = new Properties();
		p.setProperty(KEY_LAMBDA0, "" + Lambda_0);
		p.setProperty(KEY_SECDISTANCE, "" + secure_distance);
		return p;
	}

	public boolean setProperties(Properties p) {
		try {
			String o = p.getProperty(KEY_LAMBDA0);
			Lambda_0 = Double.parseDouble(o);
			o = p.getProperty(KEY_SECDISTANCE);
			secure_distance = Double.parseDouble(o);
		} catch (Exception e) { // set defaults
			Lambda_0 = 0;
			secure_distance = 1;
			return false;
		}
		return true;
	}

	private static String KEY_LAMBDA0 = "LAMBDA0";
	private static String KEY_SECDISTANCE = "SECDISTANCE";

	private double Lambda_0 = 0;
	private double secure_distance = 1;

}
