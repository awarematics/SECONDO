package eu.ehnes.secondoandroid;

import sj.lang.ListExpr;

/**
 * Dient dazu den Namen der aktuellen Datenbank abzufragen
 */
public class AccessDatabase {

	static String ActiveDatabase() {
		
		// Der aktuelle Name der Datenbank wird abgefragt
		ListExpr liste=(ListExpr)SecondoActivity.sh.query("query getDatabaseName()");
		ListOut lo=new ListOut();
		String dbname="";
		
		// Das Ergebnis ist eine Liste, diese Liste enth�lt nur ein Element vom Typ String
		if(liste!=null)  {
			lo.ListToStringArray(liste);
			if(lo.getElem(0).equals("string")) {
				dbname=lo.getElem(1);
			}
		}
		return dbname;
	}
}
