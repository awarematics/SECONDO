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

package viewer.queryconstruction2;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.ArrayList;
import java.util.Iterator;
import javax.swing.*;
import sj.lang.ListExpr; ///
import viewer.QueryconstructionViewer2;

/**
 * Dialog class for the operation parameters.
 */
public class OperationsDialog extends JDialog {
    
    private MainPane main;
    private QueryconstructionViewer2 mainViewer;
    private FilterViewer filterViewer =  new FilterViewer(this);
    private Operation operation;
    private String[] params;
    
    private JDialog objectDialog;
    private JPanel buttonPanel;
            
    private JCheckBox[] cbs;
    private ArrayList<ButtonGroup> radiogroup = new ArrayList<ButtonGroup>();
    private ButtonGroup objectButtons = new ButtonGroup();
    private boolean hasButtons = false;
    
    private int hasParameter = 0;
    
    //special characters of the signature
    protected final static char obChar = 'o';
    protected final static char opChar = '#';
    protected final static char pChar = 'p';
    
    /**
     * Construct the OperationsDialog.
     * @param main main panel of the main query
     * @param viewer
     * @param operation operation, that asks for the parameters
     */
    public OperationsDialog(
            MainPane main, QueryconstructionViewer2 viewer, 
            Operation operation) {
        this.main = main;
        this.mainViewer = viewer;
        
        objectDialog = new JDialog();
        objectDialog.setLayout(new BorderLayout());
        
        JPanel objectPane = new JPanel();
        objectPane.setLayout(new GridLayout(0,1));
        JScrollPane objectScrollPane = new JScrollPane(objectPane);
        objectScrollPane.setPreferredSize(new Dimension (200, 200));
        
        objectDialog.add(objectScrollPane, BorderLayout.NORTH);
        objectButtons = new ButtonGroup();
        buttonPanel = objectPane;
        setLayout(new GridLayout(0,1));
        if (main.isShowing())
            this.setLocation(main.getLocationOnScreen());
        
        this.params = operation.getParameter();
        this.operation = operation;
        
        ListExpr opSigs = mainViewer.getOpSigs();    ///
        ListExpr opSpecs = mainViewer.getOpSpecs();  ///
        filterViewer.setOperators2(opSigs, opSpecs); ///
        //filterViewer.setOperators(mainViewer.getOperatorList());
        filterViewer.setObjects(mainViewer.getObjectList());
        filterViewer.addAttributes(mainViewer.getAttributes());
        filterViewer.setViewerControl(viewer.getViewerControl());
    }
    
    /**
     * First usage of the OperationsDialog.
     */
    protected void activate(){
        check();
    }
    
    /**
     * Remove the last object.
     */
    protected void back(){
        main.removeActiveComponent();
    }
    
    /**
     * Show the window to ask for the next parameter.
     * Dialog view depends on the parameter type.
     */
    private void showDialog() {
        this.radiogroup.remove(objectButtons);
        
        String parameter = params[hasParameter].trim();
        
        /* show the dialog with the radiobuttons*/
        if (hasButtons)
            showRadioButtons();
        
        
        if (parameter.startsWith("attrlist")) {
            project();
        }
        else if (parameter.equals("int") || 
                parameter.toLowerCase().equals("string")) {
            text();
        }  
        else if (parameter.equals("const")) {
            constant();
        }
        else if (!parameter.startsWith("attr")) {
            /*
            * boolean parameter and new attributes are generated by an own query
            * in the nested FilterViewer
            */
            nestedQuery(parameter);
            if (parameter.startsWith("new"))
                filterViewer.setLabel("attribute name", 
                        "First letter has to be uppercase.");
            if (parameter.startsWith("fun")) {
                filterViewer.setLabel("tuple name", 
                        "Please insert the name for the new tuple.");
                filterViewer.rename("t");
            }
        }
    }
    
    
    
    /**
     * add object attributes to the FilterViewer
     * @param atts object attributes
     */
    protected void addAttributes(ObjectView[] atts) {
        this.filterViewer.addObjects(atts);
    }
    
    /**
     * adds CheckBoxes for attributes of an object to the dialog window
     * @param objectName
     * @param atts attributes of the object
     */
    protected void addCheckboxes(String objectName, String[] atts, 
            String[] labels) {
        JLabel name = new JLabel(objectName);
        this.add(name);
        int cbsCount = atts.length;
        
        if (labels != null)
            cbsCount = atts.length * labels.length;
        this.cbs = new JCheckBox[cbsCount];
        int i = 0;
        for (String att: atts) {
            if (att != null) {
                if (labels != null) {
                    for (String label: labels) {
                        cbs[i] = new JCheckBox( att + " " + label, false );
                        i++;
                    }
                }
                else {
                    cbs[i] = new JCheckBox( att, false );
                    i++;
                }
            }
        }
    }
    
    /**
     * Add a group of RadioButtons for attributes of an object to 
     * the dialog window.
     * @param objectName
     * @param atts attributes of the object
     */
    protected void addRadiobuttons(String objectName, String[] atts) {
        JLabel name = new JLabel(objectName);
        buttonPanel.add(name);
        ButtonGroup buttons = new ButtonGroup();
        int i = 0;
        for (String att: atts) {
            if (att != null) {
                JRadioButton rb = new JRadioButton( att );
                rb.setActionCommand(att);
                buttonPanel.add(rb);
                buttons.add(rb);
                i++;
            }
        }
        if (i > 0){
            hasButtons = true;
            radiogroup.add(buttons);
        }
    }
    
    /**
     * Add the string value to the result string.
     * @param value 
     */
    protected void addResult(String value, String label){
        hasParameter++;
        
        ObjectView new_object = new ObjectView(value, "param");
        StreamView paramStream = new StreamView("OperationStream", "", 0, 0);
        paramStream.addObject(new_object);
        main.addParamStream(paramStream, label);
        
        check();
    }
    
    /**
     * Adds the tuple as an object to the filterViewer.
     * @param tuple 
     */
    protected void addTuple(ObjectView tuple){
        filterViewer.addObjects(new ObjectView[]{tuple});
    }
    
    /**
     * Check if the input parameters are complete.
     */
    private void check() {
        
        if (hasParameter == params.length) {
            close();
        }
        else {
            showDialog();
        }
    }
    
    /**
     * Close the dialog.
     */
    private void close() {
        this.dispose();
        mainViewer.update();
    }
    
    /**
     * Create a new object of a constant type.
     */
    private void constant() {
        JLabel typeLabel = new JLabel("Please insert the type.");
        final JTextField type = new JTextField(20);
        JLabel valueLabel = new JLabel("Please insert the value.");
        final JTextField value = new JTextField(20);
        ActionListener al = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
                sendConstant(type, value);
            }
        };
        
        add(typeLabel);
        add(type);
        add(valueLabel);
        add(value);
        show(this, al);       
    }
    
    /**
     * Activates the FilterViewer to get a boolean parameter.
     */
    private void nestedQuery(String result){
        filterViewer.setResult(result);
        filterViewer.showViewer();
    }
    
    
    /**
     * Show a list of the Checkboxes with attribute names.
     */
    private void project() {
        for (JCheckBox cb: this.cbs){
            if (cb != null)
                add(cb);
        }
        ActionListener al = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
                sendCheckboxes();
            }
        };
        show(this, al);
    }
    
    /**
    * Adds the name of the chosen object to the operation.
    */
    private void sendButtons() {
        objectDialog.dispose();
        String labels = "";
        if (radiogroup.size() > 1) {
            int i = 0;
            for ( Iterator iter = radiogroup.iterator(); iter.hasNext(); ) {
                ButtonGroup group = (ButtonGroup)iter.next();
                
                if (group.getButtonCount() > 0) {
                    labels += group.getSelection().getActionCommand() + ", ";
                    i++;
                }
            }
            if(i > 0){
                labels = labels.substring(0, labels.length()-2);
            }
        }
        else if (radiogroup.get(0).getButtonCount() > 0)
            labels += radiogroup.get(0).getSelection().getActionCommand();
        
        addResult(labels, null);
    }
    
    /**
     * Add the names of the activated Checkboxes to the main panel.
     */
    private void sendCheckboxes() {
        int i = 0;
        String labels = "";
        for (JCheckBox cb: this.cbs) {
            if (cb != null) {
                if (cb.isSelected()) {
                    labels += cb.getActionCommand() + ", ";
                    i++;
                    if (operation.getOperationName().equals("sortby")){
                        main.addSortedAttribute(cb.getActionCommand()
                                .replace(" asc", "").replace(" desc", ""));
                    }
                    if (operation.getOperationName().equals("sort")){
                        main.setSorted();
                    }
                }
            }
        }
        if(i > 0){
            labels = labels.substring(0, labels.length()-2);
        }
        
        addResult(labels, null);
    }
    
    /**
     * Add the constant with values of the two
     * textfields to the result.
     * @param textfield 
     */
    private void sendConstant(JTextField type, JTextField value){
        addResult("[const " + type.getText() +
                " value " + value.getText() + "]",
                value.getText());
    }
    
    /**
     * adds the text of the textfield to the result
     * @param textfield 
     */
    private void sendText(JTextField textfield){
        String text = textfield.getText();
//        if (params[hasParameter].equals("fun")) {
//            filterViewer.rename(text);
//        }
        addResult(text, null);
    }
    
    /**
     * Set the dialog visible.
     * @param dialog
     * @param al ActionListener for the "ok" button.
     */
    private void show(JDialog dialog, ActionListener al) {
        JButton ok = new JButton("ok");
        ok.addActionListener(al);
        dialog.getRootPane().setDefaultButton(ok);
        dialog.add(ok);
        
        dialog.addWindowListener( new WindowAdapter() {
            public void windowClosing ( WindowEvent e) {
                back();
            }
        } );
        
        //view the dialog at the position of the mouse
        if (main.isShowing())
            dialog.setLocation(main.getLocationOnScreen());
        
        dialog.pack();
        dialog.setVisible(true);
    }
    
    /**
     * Show the dialog with radiobuttons for attributes.
     */
    private void showRadioButtons(){
        if (objectButtons.getButtonCount() > 0)
            radiogroup.add(objectButtons);
        
        ActionListener al = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
                sendButtons();
            }
        };
        show(objectDialog, al);
    }
    
    /**
     * opens a popup window with a textfield
     */
    private void text(){
        final JTextField textfield = new JTextField(20);
        ActionListener al = new ActionListener() {
            public void actionPerformed( ActionEvent e ) {
                sendText(textfield);
            }
        };
        
        add(textfield);
        show(this, al);
    }
    
}