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


package  viewer.hoese.algebras;

import  sj.lang.ListExpr;
import  viewer.*;
import viewer.hoese.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.text.EditorKit;
import javax.swing.text.StyledEditorKit;
import javax.swing.text.Document;
import tools.Base64Decoder;
import org.jpedal.*;
import tools.Reporter;
import java.io.*;
import java.util.SortedMap;
import java.nio.charset.Charset;
import java.util.Map;
import java.util.Iterator;



/**
 * A displayclass for the html formatted code 
 */
public class Dspltext extends DsplGeneric implements ExternDisplay{

private boolean defined;


 /** Creates a new Instance of this.
   */ 
public  Dspltext(){
   if(Display==null){
      Display = new TextViewerFrame();
   }
}



public String toString(){
   return Entry;
}

public void init (String name, int nameWidth, int indent, ListExpr type,ListExpr value, QueryResult qr)
  {
     String T = name;
     String V;

     if (value.listLength()==1){
         value = value.first();
     }
     defined = !isUndefined(value);
     if(!defined){
        V = "undefined";
        theList = ListExpr.textAtom(V);
     }else{
				 if(value.atomType()!=ListExpr.TEXT_ATOM){
						V =  "error in value ";
						theList = ListExpr.textAtom(V);
            defined = false;
				 }
				 else{
						V =  value.textValue();
						theList = value;
				 }
     }
     T=extendString(T,nameWidth, indent);
     String Text = V;
     computeType(Text);

     if(Type==PLAIN_TYPE){
        if(Text.length()<=MAX_DIRECT_DISPLAY_LENGTH){ // short Text
            Entry = T+" : "+ Text;
            qr.addEntry(Entry); // avoid the possibility to pop up a window
            return; 
         } else{  // long plain text
               Entry = T+" : "+ Text.substring(0,MAX_DIRECT_DISPLAY_LENGTH-4).replace('\n',' ')+" ...";
         }
     }else if(Type==HTML_TYPE){
           Entry = T + " : <html> ...";
     } else if(Type==RTF_TYPE){
         Entry =  T + " : RTF ...";
     } else if(Type==PDF_TYPE){
         Entry = T + " : PDF ...";
     }

     qr.addEntry(this);
     return;
 }


public void displayExtern(){
    Display.setSource(this);
    Display.setVisible(true);    
}

public boolean isExternDisplayed(){
   return Display.isVisible() && this.equals(Display.getSource());
}


private boolean checkForHtml(String Text,int offset){
    int restlength = Text.length()-offset-1;
    if(restlength<4)
      return false;
    String tag = Text.substring(offset,offset+5).toLowerCase();
    if("<html".equals(tag))
       return true;
    // search for "<html ignoring case
    int max = Text.length();
    int index = offset+1;
    int state = 1;
    boolean found = false;
    while(!found && index<max-5){
        char c = Text.charAt(index);
        index++;
        switch(state){
            case 1 : if(c=='<') state = 2;break;
            case 2 : if(c=='h' || c=='H') state=3; else state=1; break;
            case 3 : if(c=='t' || c=='T') state=4; else state=1; break;
            case 4 : if(c=='m' || c=='M') state=5; else state=1; break;
            case 5 : if(c=='l' || c=='L') found=true; else state=1; break;
            default: Reporter.writeError("Undefined state");
        }
        if(c=='<') state = 2; 
    }  
    return found;
}

/** sets the type of this Text 
  * depending on some keywords 
  **/
private void computeType(String Text){
     if(Text.startsWith("JVBE") ){ // base 64 encodes "%PD"
         Type = PDF_TYPE;
         return;
     }
     // search for <html or { *\rtf at the begin of the document ignoring cases
     for(int i=0;i<Text.length()-5;i++){ 
         char c = Text.charAt(i);
         if(c=='<'){
              if(checkForHtml(Text,i))
                  Type=HTML_TYPE;
              else
                  Type=PLAIN_TYPE;
              return;
         } else if(c=='{'){ // possible rtf format
           // search for the next non-whitespace
           for(int j=i+1;j<Text.length()-4;j++){

                c = Text.charAt(j);
                if(!isWhiteSpace(c)){
                   String test = Text.substring(j,j+4);
                   if("\\rtf".equals(test))
                       Type = RTF_TYPE;
                    else
                       Type = PLAIN_TYPE;
                    return;
                }
           }
           Type = PLAIN_TYPE; // no \rtf found
           return;
         } 
         else{
             if(!isWhiteSpace(c)){ // not an html document
                Type = PLAIN_TYPE;
                return;
             }
         }
     }  
   // only whitespaces in text
   Type = PLAIN_TYPE; 
}


private static boolean isWhiteSpace(char c){
   return WHITESPACES.indexOf(c)>=0;
}


private static TextViewerFrame Display=null; 
private String Entry;
private ListExpr theList;

private int Type; // contains the type which is the text (probably)




private static final int PLAIN_TYPE=0;
private static final int HTML_TYPE=1;
private static final int RTF_TYPE=2;
private static final int PDF_TYPE=3;

private static final int MAX_DIRECT_DISPLAY_LENGTH = 30;

private static final String WHITESPACES = " \t\n\r";


private static class TextViewerFrame extends JFrame{


JComboBox encodingCB;
private String encoding=stdEncoding;

private static String stdEncoding;

static {
  try{
    stdEncoding = System.getProperty("file.encoding");
  } catch(Exception e){
     stdEncoding=null;
  }
  if(stdEncoding==null){
     stdEncoding = "UTF-8";
  }
}


public TextViewerFrame(){

  getContentPane().setLayout(new BorderLayout());
  Display = new JTextPane();
  if(EKHtml==null){
      EKHtml = JTextPane.createEditorKitForContentType("text/html");
      EKRtf = JTextPane.createEditorKitForContentType("text/rtf");
  }

  TextScrollPane = new JScrollPane(Display);
  TextPanel = new JPanel(new BorderLayout());
  JPanel SearchPanel = new JPanel();
  SearchPanel.add(CaseSensitive);
  SearchPanel.add(SearchField);
  SearchPanel.add(SearchBtn);
  SearchBtn.addActionListener(new ActionListener(){
     public void actionPerformed(ActionEvent evt){
        searchText();
     }
  });
  SearchField.addKeyListener(new KeyListener(){
     public void keyPressed(KeyEvent evt){
         if(evt.getKeyCode()==KeyEvent.VK_ENTER)
            searchText();
     }
     public void keyTyped(KeyEvent evt){}
     public void keyReleased(KeyEvent evt){}
  });
  TextPanel.add(TextScrollPane,BorderLayout.CENTER);
  TextPanel.add(SearchPanel,BorderLayout.SOUTH); 

  getContentPane().add(TextPanel,BorderLayout.CENTER);
  CloseBtn = new JButton("Close");
  CloseBtn.addActionListener(new ActionListener(){
       public void actionPerformed(ActionEvent evt){
            TheText=null;
            TextViewerFrame.this.setVisible(false);
       }
  } );

  fileChooser = new JFileChooser();
  SaveBtn = new JButton("Save");
  SaveBtn.addActionListener(new ActionListener(){
     public void actionPerformed(ActionEvent evt){
        if(fileChooser.showSaveDialog(TextViewerFrame.this)==JFileChooser.APPROVE_OPTION){
             File F = TextViewerFrame.this.fileChooser.getSelectedFile();
             try{
                 BufferedOutputStream out = new BufferedOutputStream(new FileOutputStream(F));
                 if(TextViewerFrame.this.Source.Type!=PDF_TYPE){
                     out.write(TheText.getBytes());
                 } else{
                     out.write(Base64Decoder.decode(TheText));
                 }
                 out.flush();
                 out.close();
               }catch(Exception e){
                  Reporter.debug(e);
                  Reporter.showError("Error in saving file ");
               }
        }
     }
  });
  

  // create the pdf viewer
  if(pdf_viewer==null){
    try{
       pdf_viewer = new PDFPanel();
    }catch(Exception e){
      Reporter.writeError("cannot initialize pdf-viewer");
    }
  }


  encodingCB = new JComboBox();
  Iterator acs = Charset.availableCharsets().entrySet().iterator();
  //for(int i=0; i<encodings.length; i++){
  //  encodingCB.addItem(encodings[i]);
  //}
  while(acs.hasNext()){
    Map.Entry e = (Map.Entry) acs.next();
    Charset cs = (Charset) e.getValue();
    encodingCB.addItem(cs.displayName());
  }
  encodingCB.setSelectedItem(stdEncoding);
 


  encodingCB.addActionListener(new ActionListener(){
    public void actionPerformed(ActionEvent evt){
        try{
          TextViewerFrame.this.TheText = new String(TextViewerFrame.this.Display.getText().getBytes(encoding));
        } catch(Exception e){
           TextViewerFrame.this.TheText = TextViewerFrame.this.Display.getText();
        }
        String enc = (String) encodingCB.getSelectedItem();
        encoding = enc;
        setToPlain(false);
    }
  });
  

  PlainBtn = new JButton("plain");
  resetBtn = new JButton("reset");
  resetBtn.addActionListener(new ActionListener(){
    public void actionPerformed(ActionEvent evt){
       setSource(Source);
    }
  });
  HtmlBtn = new JButton("html");
  RtfBtn = new JButton("rtf"); 
  PdfBtn = new JButton("pdf");

  ActionListener FormatSwitcher = new ActionListener(){
     public void actionPerformed(ActionEvent evt){
         Object src = evt.getSource();
         LastSearchPos=0;
         // get the text if it is editable 
         if(TextViewerFrame.this.Display.isEditable()&&!ISPDF){
             try{
                TextViewerFrame.this.TheText = new String(TextViewerFrame.this.Display.getText().getBytes(encoding));
             } catch(Exception e){
                   TextViewerFrame.this.TheText = TextViewerFrame.this.Display.getText();
             }
         }
         Container CP=TextViewerFrame.this.getContentPane();
         if(TextViewerFrame.this.PlainBtn.equals(src)){
            setToPlain(false);
         } else
         if(TextViewerFrame.this.HtmlBtn.equals(src)){
            try{
              TextViewerFrame.this.Display.setEditorKit(TextViewerFrame.EKHtml);
              TextViewerFrame.this.Display.setEditable(false);
              TextViewerFrame.this.Display.setText(TextViewerFrame.this.TheText);
              TextViewerFrame.this.Display.setCaretPosition(0);
              TextViewerFrame.this.Source.Type = Dspltext.HTML_TYPE;
              TextViewerFrame.this.PlainBtn.setEnabled(true);
              TextViewerFrame.this.encodingCB.setEnabled(false);
              TextViewerFrame.this.HtmlBtn.setEnabled(false);
              TextViewerFrame.this.RtfBtn.setEnabled(true);
              TextViewerFrame.this.PdfBtn.setEnabled(true);
              if(TextViewerFrame.this.ISPDF){
                 CP.remove(pdf_viewer);
                 CP.add(TextPanel,BorderLayout.CENTER);
                 ISPDF=false;
                 TextViewerFrame.this.invalidate();
                 TextViewerFrame.this.validate();
                 TextViewerFrame.this.repaint(); 
              }
            }catch(Exception e){
               setToPlain(true);
            }
         }else
         if(TextViewerFrame.this.RtfBtn.equals(src)){
            try{
              TextViewerFrame.this.Display.setEditorKit(TextViewerFrame.EKRtf);
              TextViewerFrame.this.Display.setEditable(false);
              TextViewerFrame.this.Display.setText(TextViewerFrame.this.TheText);
              TextViewerFrame.this.Display.setCaretPosition(0);
              TextViewerFrame.this.Source.Type = Dspltext.RTF_TYPE;
              TextViewerFrame.this.PlainBtn.setEnabled(true);
              TextViewerFrame.this.encodingCB.setEnabled(false);
              TextViewerFrame.this.HtmlBtn.setEnabled(true);
              TextViewerFrame.this.RtfBtn.setEnabled(false);
              TextViewerFrame.this.PdfBtn.setEnabled(true);
              if(TextViewerFrame.this.ISPDF){
                 CP.remove(pdf_viewer);
                 CP.add(TextPanel,BorderLayout.CENTER);
                 ISPDF=false;
                 TextViewerFrame.this.invalidate();
                 TextViewerFrame.this.validate();
                 TextViewerFrame.this.repaint(); 
              }
            } catch(Exception e){
              setToPlain(true); 
            }
         }else
         if(TextViewerFrame.this.PdfBtn.equals(src)){
             byte[]  content = Base64Decoder.decode(TheText);
             if(pdf_viewer.setPdfData(content)){   
                TextViewerFrame.this.PlainBtn.setEnabled(true);
                TextViewerFrame.this.encodingCB.setEnabled(false);
                TextViewerFrame.this.HtmlBtn.setEnabled(true);
                TextViewerFrame.this.RtfBtn.setEnabled(true);
                TextViewerFrame.this.PdfBtn.setEnabled(false);
                if(!TextViewerFrame.this.ISPDF ){
                    CP.remove(TextPanel);
                    CP.add(pdf_viewer,BorderLayout.CENTER);
                    ISPDF=true;
                    TextViewerFrame.this.invalidate();
                    TextViewerFrame.this.validate();
                    TextViewerFrame.this.repaint(); 
                }
             }else {
                setToPlain(true);
             }

         }

     }
  };

  PlainBtn.addActionListener(FormatSwitcher);
  HtmlBtn.addActionListener(FormatSwitcher);
  RtfBtn.addActionListener(FormatSwitcher);
  PdfBtn.addActionListener(FormatSwitcher);

  JPanel ControlPanel = new JPanel(new BorderLayout());
  
  JPanel FormatPanel = new JPanel();
  FormatPanel.add(new JLabel("show as : "));
  JPanel p1 = new JPanel(new GridLayout(3,1));

  p1.add(PlainBtn);
  p1.add(encodingCB);
  p1.add(resetBtn);
  FormatPanel.add(p1);
  FormatPanel.add(HtmlBtn);
  FormatPanel.add(RtfBtn); 
  FormatPanel.add(PdfBtn);
  
  ControlPanel.add(FormatPanel,BorderLayout.NORTH);
  ControlPanel.add(CloseBtn,BorderLayout.CENTER);
  ControlPanel.add(SaveBtn,BorderLayout.SOUTH);

  getContentPane().add(ControlPanel,BorderLayout.SOUTH);
  setSize(640,480); 
}

/** searchs the text in the textfield in the document and
  * marks its if found
  */
private void searchText(){
  String Text = SearchField.getText();
  if(Text.length()==0){
    Reporter.showError("no text to search");
    return;
  }
  try{
     Document Doc = Display.getDocument();
     String DocText = Doc.getText(0,Doc.getLength());
     if(!CaseSensitive.isSelected()){
        DocText = DocText.toUpperCase();
        Text = Text.toUpperCase();
     }
     int pos = DocText.indexOf(Text,LastSearchPos);
     if(pos<0){
        Reporter.showError("end of text is reached");
        LastSearchPos=0;
        return;
     }
     pos = pos;
     int i1 = pos;
     int i2 = pos+Text.length();
     LastSearchPos = pos+1;
     Display.setCaretPosition(i1);
     Display.moveCaretPosition(i2);
     Display.getCaret().setSelectionVisible(true);
  } catch(Exception e){
    Reporter.debug(e);
    Reporter.showError("error in searching text");

  }

}


private void setToPlain(boolean isError){
    if(isError){
      Reporter.showError("Cannot show the text in specified format\n"+
                       ", switch to plain text");
    }
    
    LastSearchPos=0;

    String contentType = "text/plain; charset="+encoding;

   // Display.setContentType(contentType);

    EditorKit EKPlain = Display.getEditorKitForContentType(contentType);

    /*
     Before switching to plain text, we switch to rtf.
     This is a work around to avoid display failures when switching directly
     from html to plain text. 
    */
    Display.setEditorKit(EKRtf);


    Display.setEditorKit(EKPlain);


    Display.setEditable(true);
    try{
       TextViewerFrame.this.Display.read( new InputStreamReader(
                                            new ByteArrayInputStream(
                                                TextViewerFrame.this.TheText.getBytes()),encoding),
                                                null); 
    } catch(Exception e){
       Reporter.debug(e);
       TextViewerFrame.this.Display.setText(TextViewerFrame.this.TheText); 
    }
    Display.setCaretPosition(0);
    Source.Type = Dspltext.PLAIN_TYPE;
    PlainBtn.setEnabled(false);
    encodingCB.setEnabled(true);
    HtmlBtn.setEnabled(true);
    RtfBtn.setEnabled(true);
    PdfBtn.setEnabled(true);
    Display.updateUI();
    if(ISPDF){
       getContentPane().remove(pdf_viewer); 
       getContentPane().add(TextPanel,BorderLayout.CENTER);
       ISPDF=false;
    } 
    invalidate();
    validate();
    repaint();
}

public void setSource(Dspltext S){
    Source = S;
    LastSearchPos=0;
    PlainBtn.setEnabled(true);
    encodingCB.setEnabled(false);
    HtmlBtn.setEnabled(true);
    RtfBtn.setEnabled(true);
    PdfBtn.setEnabled(true);
    if(S.Type==Dspltext.PLAIN_TYPE){
       Display.setContentType("text/plain; charset="+encoding);
       PlainBtn.setEnabled(false);
       encodingCB.setEnabled(true);
       Display.setEditable(true); 
    }else if(S.Type==Dspltext.HTML_TYPE){
       Display.setEditorKit(EKHtml);
       HtmlBtn.setEnabled(false);
       Display.setEditable(false); 
    } else if(S.Type==Dspltext.RTF_TYPE){
       Display.setEditorKit(EKRtf);
       RtfBtn.setEnabled(false);
       Display.setEditable(false); 
    }
    TheText = S.theList.textValue();
    if(S.Type!=PDF_TYPE){
       try{
         if(PlainBtn.isEnabled()){
           Display.setText(TheText);
         } else { // plain text
          TextViewerFrame.this.Display.read( new InputStreamReader(
                       new ByteArrayInputStream(
                       TheText.getBytes()),encoding),
                       null); 
         }
       } catch(Exception e){
         setToPlain(true); 
       }
        Display.setCaretPosition(0);// go to top 
       if(ISPDF){
          ISPDF=false;
          getContentPane().remove(pdf_viewer);
          getContentPane().add(TextPanel,BorderLayout.CENTER);
          invalidate();validate();repaint();
       }
    } else{
       try{
          byte[] content = Base64Decoder.decode(TheText);
          pdf_viewer.setPdfData(content);
          PdfBtn.setEnabled(false);
          if(!ISPDF){
             getContentPane().remove(TextPanel);
             getContentPane().add(pdf_viewer,BorderLayout.CENTER);
             ISPDF=true;
             invalidate();validate();repaint();
           }
       }catch(Exception e){
          Reporter.debug(e);
          setToPlain(true);
       }
    }
}

public Dspltext getSource(){
     return Source;
}

private class PDFPanel extends JPanel{

public PDFPanel(){
     super();
     setLayout(new BorderLayout());
     JPanel CommandPanel = new JPanel();
     CommandPanel.add(FirstBtn);
     CommandPanel.add(PrevBtn);
     CommandPanel.add(NextBtn);
     CommandPanel.add(LastBtn);
     CommandPanel.add(new JLabel("    "));
     CommandPanel.add(GBtn);
     CommandPanel.add(LBtn);
     PdfScrollPane = new JScrollPane(CurrentPage);
     add(PdfScrollPane,BorderLayout.CENTER);
     add(CommandPanel,BorderLayout.SOUTH);
     ActionListener Control= new ActionListener(){
        public void actionPerformed(ActionEvent evt){
            Object src = evt.getSource();
            if(!dataAvailable)
               return;
            int newpage = page;
            if(src.equals(FirstBtn)){
                newpage = 1;
            } else if(src.equals(PrevBtn)&& page>1){
                newpage = page -1;
            } else if(src.equals(NextBtn) && page < NumberOfPages){
                newpage = page+1;
            } else if(src.equals(LastBtn)){
                newpage=NumberOfPages;
            }
            if(newpage!=page){
               page=newpage;
               try{
                  CurrentPage.setImage(pdf_decoder.getPageAsImage(page));
                  PDFPanel.this.repaint(); 
               } catch(Exception e){
                  Reporter.debug(e);
                  CurrentPage.setImage(null);
                  PDFPanel.this.repaint();
               }
           }
        }
     };
     FirstBtn.addActionListener(Control);
     PrevBtn.addActionListener(Control);
     NextBtn.addActionListener(Control);
     LastBtn.addActionListener(Control);
     pdf_decoder.setExtractionMode(0,300,scale);
     ActionListener Magnifier = new ActionListener(){
         public void actionPerformed(ActionEvent evt){
            Object src = evt.getSource();
            if(src.equals(GBtn))
               scale=scale*SCALEFACTOR;
            if(src.equals(LBtn))
               scale=Math.max(0.01F,scale/SCALEFACTOR);
            pdf_decoder.setExtractionMode(0,300,scale);
            if(!dataAvailable){ 
               return;
            }
            try{
              CurrentPage.setImage(pdf_decoder.getPageAsImage(page));
              TextViewerFrame.this.invalidate();
              TextViewerFrame.this.validate();
              TextViewerFrame.this.repaint(); 
            }catch(Exception e){
               Reporter.debug(e);
               CurrentPage.setImage(null);
               PDFPanel.this.repaint();
            }
         }};
     GBtn.addActionListener(Magnifier);
     LBtn.addActionListener(Magnifier);
}


private class PicturePanel extends JPanel{
         public void paint(Graphics g){
              if(image!=null)
                   g.drawImage(image,0,0,null);
         }
         public Dimension getPreferredSize(){
            return MyDimension;
         }          
         public Dimension getMinimumSize(){
            return MyDimension;
         }
         public void setImage(Image image){
             this.image = image;
             if(image!=null){
                MyDimension.width = image.getWidth(null);
                MyDimension.height = image.getHeight(null);
             }else{
                MyDimension.width=MyDimension.height=10;
             }
         }
         Image image;
         Dimension MyDimension = new Dimension();
     };

private PicturePanel CurrentPage= new PicturePanel();

public boolean setPdfData(byte[] data){
   try{
       pdf_decoder.openPdfArray(data);
       NumberOfPages = pdf_decoder.getPageCount();
       dataAvailable=true;
       CurrentPage.setImage(pdf_decoder.getPageAsImage(1));
       page = 1;
       return true;
   } catch(Exception e){
       NumberOfPages = -1;
       dataAvailable=false;
       return false;
   }
} 
private PdfDecoder pdf_decoder=new PdfDecoder();
private JScrollPane PdfScrollPane;
private JButton FirstBtn = new JButton("|<");
private JButton PrevBtn = new JButton("<");
private JButton NextBtn = new JButton(">");
private JButton LastBtn = new JButton(">|");
private JButton GBtn = new JButton("(+)");
private JButton LBtn = new JButton("(-)"); 
private JLabel  Pages  = new JLabel("# pages ");
private int NumberOfPages=-1;
private boolean dataAvailable = false;
private int page; // the number of the current page
private float scale=1.0F;
static final float SCALEFACTOR=1.2F;

}


private JTextPane Display;
private JButton CloseBtn;
private JButton SaveBtn;
private JFileChooser fileChooser;
private Dspltext Source;
private JButton PlainBtn;
private JButton resetBtn;
private JButton HtmlBtn;
private JButton RtfBtn;
private JButton PdfBtn;
private String TheText;
private static EditorKit EKHtml=null;
private static EditorKit EKRtf=null;
private static JScrollPane TextScrollPane;
private static PDFPanel pdf_viewer;     // Panel for displaying pdf contents
private static JPanel TextPanel; // Panel for displaying text in plain, html and rtf format
private boolean ISPDF = false;
private JCheckBox CaseSensitive = new JCheckBox("case sensitive");
private JTextField SearchField = new JTextField(20);
private JButton SearchBtn = new JButton("search");
private int LastSearchPos=0;

}

}



