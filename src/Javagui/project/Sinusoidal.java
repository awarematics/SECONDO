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

public class Sinusoidal implements Projection{

   public boolean project(double lambda, double phi, java.awt.geom.Point2D.Double result){
     double px=0,py=0;
     try{
        double phi_1 = phi*PI/180;
        double L_1 = (lambda-Lambda_0)*PI/180;
        double rr = L_1*Math.cos(phi_1);
        px= rr*180/PI;
        py= phi;
     }catch(Exception e){
        return false;
     }   
     result.x = px;
     result.y = py;
     return true;
   }

   public boolean showSettings(){
     Reporter.writeWarning("Sinusoisal.showSettings not implemented");
     return true;
   }

   public String getName(){
     return "Sinusoidal";
   }

   public boolean isReversible(){
      return true;
   }

   public boolean getOrig(double x, double y, java.awt.geom.Point2D.Double result){
      try{
         x = x*PI/180;
         y = y*PI/180;
         double x_t = Lambda_0 + x / Math.cos(y);
         result.x= 180*x_t/PI;
         result.y = y;
         return true;
      }catch(Exception e){
         return false;
      }

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
