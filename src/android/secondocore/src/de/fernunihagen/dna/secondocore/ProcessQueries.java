package de.fernunihagen.dna.secondocore;

import java.util.ArrayList;
import java.util.List;

import sj.lang.ListExpr;

public class ProcessQueries {
	private List<String> liste1;
	private StringBuffer listenstring=new StringBuffer();


	public List<String> CreateItemList(ListExpr liste) {

		ListExpr types=liste.first();
		List<String> itemlist=null; 

		if (types != null) {
			ListExpr data=liste.second();
			ListOut typeslist=new ListOut();
			typeslist.ListToStringArray(types);


			if(typeslist.size()>2 && (typeslist.getElem(0).equals("rel")||typeslist.getElem(0).equals("trel")) && typeslist.getElem(1).equals("tuple"))
			{
				itemlist=new ArrayList<String>(); // Muss innerhalb gemacht werden, da sonst itemlist != null
				liste1=new ArrayList<String>();

				createFormattedTypesList(typeslist);

				int position=0;
				int maxposition=liste1.size();
				String itemString;
				if (data != null) {
					while(!data.isEmpty()) {
						listenstring.delete(0, listenstring.length());
						ListExpr data2=data.first();
						while(!data2.isEmpty()) {
							itemString="";
							itemString += liste1.get(position % maxposition);
							if(liste1.get(position % maxposition+1).equals("point")) {
								itemString+=liste1.get(position % maxposition+1)+" :: ";
							}
							if(liste1.get(position % maxposition+1).equals("line")) {
								itemString+=liste1.get(position % maxposition+1)+" :: ";
							}

							if(displaytuples(data2.first())) {
								itemString+=listenstring;
								listenstring.delete(0, listenstring.length());

							}
							itemlist.add(itemString);
							position += 2; 
							if(position%maxposition==0) {
								itemlist.add("");
							}
							data2=data2.rest();
						}
						data=data.rest();
					}
				}
			}
			if(typeslist.size()>=1 && (typeslist.getElem(0).equals("int") || typeslist.getElem(0).equals("double") || 
					typeslist.getElem(0).equals("real") || typeslist.getElem(0).equals("text"))) {

				itemlist=new ArrayList<String>(); // Muss innerhalb gemacht werden, da sonst itemlist != null
				displaytuples(liste.first());
				listenstring.append(" : ");
				displaytuples(liste.second());
				itemlist.add(listenstring.toString());
			}
		}

		return itemlist;
	}

	// Ermittelt die L�nge des l�ngesten Elements in einer Liste
	private int maxElementLength(ListOut data) {
		int maxlength=0;
		for(int i=2;i<data.size();i=i+2) {
			if(data.getElem(i).length()>maxlength) {
				maxlength=data.getElem(i).length();
			}
		}
		return maxlength;
	}

	// F�llt in der Typenliste alle Eintr�ge mit Leerzeichen bis zur L�nge
	// des l�ngesten Eintrages auf
	private void createFormattedTypesList(ListOut data) {

		int maxlength = maxElementLength(data);
		for(int i=2;i<data.size();i+=2) {
			String element=data.getElem(i);
			int elementsize=element.length();
			String padding=" ";
			if(maxlength>elementsize) {
				for(int j=0;j<(maxlength-elementsize);j++) {
					padding += " ";
				}
			}
			padding += ": ";
			liste1.add(data.getElem(i)+padding);
			liste1.add(data.getElem(i+1));
		}

	}

	private boolean displaytuples(ListExpr data) {
		ListExpr localData = data;

		if(localData.isEmpty()) {
			return false;
		} else if(localData.isAtom()) {
			switch(localData.atomType()) {
			case ListExpr.BIN_TEXT: case ListExpr.BIN_STRING: case ListExpr.BIN_SHORTSTRING: case ListExpr.BIN_SHORTTEXT: case ListExpr.BIN_LONGTEXT: case ListExpr.BIN_LONGSTRING:
				listenstring.append(localData.stringValue()+" ");
				break;
			case ListExpr.BIN_DOUBLE: case ListExpr.BIN_REAL:
				listenstring.append(String.valueOf(localData.realValue())+" ");
				break;
			case ListExpr.BIN_SYMBOL: case ListExpr.BIN_SHORTSYMBOL: case ListExpr.BIN_LONGSYMBOL:
				listenstring.append(localData.stringValue()+" ");
				break;
			case ListExpr.BIN_INTEGER: case ListExpr.BIN_SHORTINT:
				listenstring.append(String.valueOf(localData.intValue())+" ");
				break;
			case ListExpr.BIN_BYTE:
				break;
			case ListExpr.BIN_BOOLEAN:
				listenstring.append(String.valueOf(localData.boolValue())+" ");
				break;
			default:
				break;


			}

		} else {
			boolean ok = displaytuples(localData.first());

			if ( !ok ) {
				return (ok);
			}

			while ( !localData.rest().isEmpty())
			{
				localData = localData.rest();
				ok = displaytuples(localData.first() );

				if ( !ok ) {
					return (ok);
				}
			}
		}

		return true;	
	}
}

