package progresswatcher;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Observable;
import java.util.Observer;

import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.table.AbstractTableModel;

import org.apache.log4j.Logger;
import org.jfree.data.general.SeriesChangeEvent;
import org.jfree.data.general.SeriesChangeListener;
import org.jfree.data.time.TimeSeriesDataItem;

/**
 * This class provides a framework for the TableWindow classes in this
 * application 
 *
 */
public abstract class AbstractTableWindow extends AbstractWindow {
	
	protected final static Logger logger 
		= Logger.getLogger(AbstractTableWindow.class);

	protected final AbstractTableModel tableModell;
	protected final List<DataEntry> data = new ArrayList<DataEntry>();
	protected final SeriesChangeListener seriesChangeListener;
	protected final AbstractProgressWindow window;
	
	protected final ObservableDouble observableMaxDiff = new ObservableDouble();
	protected final ObservableDouble observableIntDiff = new ObservableDouble();

	
	// A simple data container class for our dataList
	class DataEntry {
		protected long timestamp;
		protected double value;
		protected double optValue;
		
		public DataEntry(long timestamp, double value, double optValue) {
			this.timestamp = timestamp;
			this.value = value;
			this.optValue = optValue;
		}
	}
	
	/**
	 * Register a change listener to the chart
	 * So we got notified if the chart change
	 */
	public AbstractTableWindow() {
		window = (AbstractProgressWindow) AppCtx.getInstance()
			.getWindow(getWindowType());
		
		seriesChangeListener = new SeriesChangeListener() {
			
			public void seriesChanged(SeriesChangeEvent arg0) {
				updateTableModel();
			}
			
		};
		
		tableModell = getTableModel();
	}
	
	/**
	 * Init our window
	 */
	public void init() {
		mainframe.setTitle(getTitle());
		
		final JTable table = new JTable(tableModell);
		final JPanel middlePanel = new JPanel();
		final JPanel southPanel = new JPanel();
		
		// CSV Exporter
		final JButton exportCSVButton = new JButton("Export as .csv");
		exportCSVButton.addActionListener(new ActionListener() {
			
			public void actionPerformed(ActionEvent arg0) {
				final CSVExport exporter = new CSVExport(tableModell);
				exporter.run();
			}
		});
		southPanel.add(exportCSVButton);
		exportCSVButton.setPreferredSize(new Dimension(120, 20));
		
		// Latex exporter
		final JButton exportLatexButton = new JButton("Export as .tex");
		exportLatexButton.addActionListener(new ActionListener() {
			
			public void actionPerformed(ActionEvent arg0) {
				final LatexExporter exporter = new LatexExporter(tableModell);
				exporter.run();
			}
		});
		southPanel.add(exportLatexButton);
		exportLatexButton.setPreferredSize(new Dimension(120, 20));

		final JButton closeButton = new JButton("Close");
		closeButton.addActionListener(new ActionListener() {
			
			public void actionPerformed(ActionEvent arg0) {
				close();
			}
		});
		southPanel.add(closeButton);
		
		// Center panel
		middlePanel.add(new JLabel("Error Max:"));
		final JTextField maxDiffText = new JTextField(10);
		middlePanel.add(maxDiffText);
	
		middlePanel.add(new JLabel("Error Int:"));
		final JTextField intDiffText = new JTextField(10);
		middlePanel.add(intDiffText);
		
		addObserverToTextField(maxDiffText, observableMaxDiff);
		addObserverToTextField(intDiffText, observableIntDiff);

	    mainframe.add(new JScrollPane(table), BorderLayout.NORTH);
	    mainframe.add(middlePanel, BorderLayout.CENTER);
	    mainframe.add(southPanel, BorderLayout.SOUTH);	    
	}

	/**
	 * Register an observer on max diff change events
	 */
	protected void addObserverToTextField(final JTextField maxDiffText, final ObservableDouble observable) {
		// Register on max diff value changes
		observable.addObserver(new Observer() {
			
			public void update(final Observable arg0, final Object arg1) {
				if(arg0 instanceof ObservableDouble) {
					final ObservableDouble obDouble 
						= (ObservableDouble) arg0;
					
					if(obDouble.getData() > 0) {
						maxDiffText.setText(Long.toString(Math.round(obDouble.getData())));
					} else {
						maxDiffText.setText("-------");
					}
				}
			}
		});
	}
	
	
	/**
	 * Get Tabledata for export
	 * @return
	 */
	public String getCVSData() {
		final CSVExport exporter = new CSVExport(tableModell);
		return exporter.getExportData();
	}
	
	/**
	 * Show Table, and add listener to receive data updates
	 * 
	 */
	@Override
	public void show() {
		updateTableModel();
		installListener();	
		super.show();
	}

	/**
	 * Install change listener
	 */
	public void installListener() {
		window.getGraph().getSeriesOut().addChangeListener(seriesChangeListener);
	}
	
	/**
	 * Close table and remove listener
	 * 
	 */
	public void close() {
		window.getGraph().getSeriesOut().removeChangeListener(seriesChangeListener);	
		mainframe.setVisible(false);
		mainframe.dispose();			
	}
	
	/**
	 * Update the table modell
	 * Read the graph and update the values in the modell
	 * 
	 */
	protected void updateTableModel() {
		data.clear();
		
		int items = window.getGraph().getSeriesOut().getItemCount();
		
		if(items > 0) {
			final TimeSeriesDataItem baseItem = window.getGraph()
			.getSeriesOut().getDataItem(0);
			
			final TimeSeriesDataItem endItem = window.getGraph()
				.getSeriesOut().getDataItem(items - 1);
			
			// Make timestamps relative
			long baseItemTime = baseItem.getPeriod().getMiddleMillisecond();
			long endItemTime = endItem.getPeriod().getMiddleMillisecond();
			
			long totalTime = endItemTime - baseItemTime;
			
			double maxDiff = 0;
			double intError = 0;
			long lastTimestamp = 0;
			
			for(int i = 0; i < items; i++) {
				
				final TimeSeriesDataItem item = 
					window.getGraph().getSeriesOut()
					.getDataItem(i);
				
				final long timestamp = 
					item.getPeriod().getMiddleMillisecond() 
					- baseItemTime;
				
				final float optimalValue = calculateOptValue(
						totalTime, timestamp, item, 
						baseItem, endItem);
				
				data.add(new DataEntry(timestamp, 
						item.getValue().doubleValue(), 
						optimalValue));
				
				final double diff = Math.abs(
						optimalValue - item.getValue().doubleValue());
				
				if(maxDiff < diff) {
					maxDiff = diff;
					
					// Update max diff
					observableMaxDiff.setData(maxDiff);
				}
				
				// calculate int error
				if(lastTimestamp > 0) {
					double diffInt = diff * (timestamp - lastTimestamp);
					intError = intError + diffInt;
					observableIntDiff.setData(intError);
				}
				
				lastTimestamp = timestamp;
			}
		}
		
		tableModell.fireTableDataChanged();
	}


	/**
	 * Define the model for the jtable
	 * 
	 */
	protected AbstractTableModel getTableModel() {
		return new AbstractTableModel() {
		
			protected static final long serialVersionUID 
				= 4771548972718384611L;
			
			protected final NumberFormat formatter 
				= new DecimalFormat("#0.00");
			
			public Object getValueAt(int arg0, int arg1) {
				
				if(arg0 >= data.size()) {
					return "---";
				}
				
				DataEntry entry = data.get(arg0);
				
				if(arg1 == 0) {
					return entry.timestamp;
				}
				
				if(arg1 == 1) {
					return formatter.format(entry.value);
				} 
				
				if(arg1 == 2) {
					return formatter.format(entry.optValue);
				}
				
				if(arg1 == 3) {
					final double diff = 
						Math.abs(entry.optValue - entry.value);		
					return formatter.format(diff);
				}
				
				return "---";
			}
			
			public int getRowCount() {
				return data.size();
			}
			
			public int getColumnCount() {
				return 4;
			}
			
			public boolean isCellEditable(int rowIndex, int columnIndex) {
				return false;
			};
			
			public String getColumnName(int column) {
				
				if(column <= 3) {
					return getTableHead()[column];
				}
	
				return "--";
			};
		};
	}
	
	/**
	 * This class provides some basic methods for our
	 * exporter
	 *
	 */
	abstract class AbstractExporter implements Runnable {
		protected AbstractTableModel model;
		
		public AbstractExporter(final AbstractTableModel model) {
			this.model = model;
		}

		/**
		 * Startup export
		 */
		public void run() {
			
			final String filename = File.separator + "tmp";
			
			final JFileChooser fc = new SaveJFileChooser(new File(filename));
		
			fc.showSaveDialog(mainframe);
			
			File selFile = fc.getSelectedFile();	
			if(selFile != null) {
				if(! selFile.getName().endsWith(getFileextension())) {
					JOptionPane.showMessageDialog(mainframe, 
							"Filename must end with " + getFileextension(), 
							"Error", JOptionPane.ERROR_MESSAGE);
				} else {
					final String sb = getExportData();
					try {
						final BufferedWriter bw = new BufferedWriter
							(new FileWriter(new File(selFile.getPath())));
						
						bw.write(sb);
						bw.close();
					} catch (IOException e) {
						logger.warn("Got exception", e);
					}
				}
			}	
		}
		
		//=======================================
		// Abstract methods
		//=======================================
		protected abstract String getExportData();
		protected abstract String getFileextension();
	}
	
	/**
	 * Latex export for our table data
	 *
	 */
	class LatexExporter extends AbstractExporter {

		public LatexExporter(final AbstractTableModel model) {
			super(model);
		}

		/**
		 * Export the table in latex format
		 * 
		 */
		protected String getExportData() {
			final StringBuffer sb = new StringBuffer();
			
			int columns = model.getColumnCount();
			final StringBuffer tmp = new StringBuffer("|");
			for(int i = 0; i < columns; i++) {
				tmp.append(" l |");
			}
			
			sb.append("\\begin{tabular}{ "+ tmp + "  }\n");
			sb.append("\\hline \n");

			// Table header
			sb.append("\\bf{" + model.getColumnName(0) + "}");
			for(int i = 1; i < columns; i++) {
				sb.append(" & " + "\\bf{" + model.getColumnName(i) + "}");
			}
			sb.append("\\\\\n");
			sb.append("\\hline \n");
			sb.append("\\hline \n");

			int rows = model.getRowCount();
			for(int row = 0; row < rows; row++) {
				sb.append(" " + model.getValueAt(row, 0));
				for(int i = 1; i < columns; i++) {
					sb.append(" & " + model.getValueAt(row, i));
				}
				sb.append("\\\\\n");
			}
			
			sb.append("\\hline \n");
			sb.append("\\end{tabular}\n");
			
			final String result = sb.toString();
			return result.replace("(", "$($").replace(")", "$)$");
		}

		protected String getFileextension() {
			return ".tex";
		}
	}
	
	class CSVExport extends AbstractExporter {

		public CSVExport(final AbstractTableModel model) {
			super(model);
		}

		@Override
		protected String getExportData() {
			final StringBuffer sb = new StringBuffer();			
			int columns = model.getColumnCount();

			// Table header
			sb.append("#" + model.getColumnName(0));
			for(int i = 1; i < columns; i++) {
				sb.append("," + model.getColumnName(i));
			}
			sb.append("\n");

			// Rows
			int rows = model.getRowCount();
			for(int row = 0; row < rows; row++) {
				sb.append(model.getValueAt(row, 0));
				for(int i = 1; i < columns; i++) {
					sb.append("," + model.getValueAt(row, i));
				}
				sb.append("\n");
			}
			
			return sb.toString();
		}

		@Override
		protected String getFileextension() {
			return ".csv";
		}
		
	}
	
	//=====================================================
	// Abstract
	//=====================================================
	protected abstract WindowType getWindowType();
	protected abstract String[] getTableHead();

	protected abstract float calculateOptValue(long totalTime, 
			final long timestamp, final TimeSeriesDataItem item, 
			final TimeSeriesDataItem baseItem, 
			final TimeSeriesDataItem endItem);
}
