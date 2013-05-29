package ty.change.wsn.comm;

import gnu.io.*;
import java.awt.*;
import java.awt.event.*;
import java.util.Enumeration;

/**
 * Main file for SerialDemo program. This program illustrates many of the
 * abilities of the javax.comm api. This file contains the GUI framework that
 * the program runs in.
 */

public class SerialDemo extends Frame implements ActionListener
{
	final int HEIGHT = 450;
	final int WIDTH = 410;
	private Button openButton;
	private Button closeButton;
	private Button clearButton;
	private Button sendButton;
	private Panel buttonPanel;
	private Panel messagePanel;
	private TextArea messageAreaOut;
	private TextArea messageAreaIn;
	private ConfigurationPanel configurationPanel;
	private SerialParameters parameters;
	private SerialConnection connection;

	
	//因为 serialDemo里的connection是私有变量，为了让外部能拿到connection，所以配置一个get方法。
	public SerialConnection getConnection()
	{
		return connection;
	}
	public synchronized static SerialDemo getInstance() 
	{
		if(null == instance)
		{
			return new SerialDemo();
		}
		return instance;
	}
	public static SerialDemo instance = null;
	public SerialDemo() 
	{
		//super("WSNComm");
		super("WSN串口通信模块");
		parameters = new SerialParameters();
		// Set up the GUI for the program
		addWindowListener(new CloseHandler(this));
		
		messagePanel = new Panel();
		messagePanel.setLayout(new GridLayout(2, 1));

		messageAreaOut = new TextArea();
		messagePanel.add(messageAreaOut);

		messageAreaIn = new TextArea();
		messageAreaIn.setEditable(false);
		messagePanel.add(messageAreaIn);

		add(messagePanel, "Center");

		configurationPanel = new ConfigurationPanel(this);

		buttonPanel = new Panel();

		openButton = new Button("打开端口");
		openButton.addActionListener(this);
		buttonPanel.add(openButton);

		closeButton = new Button("关闭端口");
		closeButton.addActionListener(this);
		closeButton.setEnabled(false);
		buttonPanel.add(closeButton);

		clearButton = new Button("清除");
		clearButton.setActionCommand("Clear");
		clearButton.addActionListener(this);
		clearButton.setEnabled(true);
		buttonPanel.add(clearButton);
		
		sendButton = new Button("发送");
		sendButton.setActionCommand("Send");
		sendButton.addActionListener(this);
		sendButton.setEnabled(true);
		buttonPanel.add(sendButton);

		Panel southPanel = new Panel();

		GridBagLayout gridBag = new GridBagLayout();
		GridBagConstraints cons = new GridBagConstraints();

		southPanel.setLayout(gridBag);

		cons.gridwidth = GridBagConstraints.REMAINDER;
		gridBag.setConstraints(configurationPanel, cons);
		cons.weightx = 1.0;
		southPanel.add(configurationPanel);
		gridBag.setConstraints(buttonPanel, cons);
		southPanel.add(buttonPanel);

		add(southPanel, "South");
		
		//把参数传给connection
		connection = new SerialConnection(this, parameters, messageAreaOut,
				messageAreaIn);
		
		//把参数设进去  显示在初始化出来的界面当中
		setConfigurationPanel();
		
		
		
		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();

		setLocation(screenSize.width / 2 - WIDTH / 2, screenSize.height / 2
				- HEIGHT / 2);

		setSize(WIDTH, HEIGHT);
		
		//一运行串口工具的时候立刻打开串口
//		configurationPanel.setParameters();
//		try
//		{
//			connection.openConnection();
//		}
//		catch (SerialConnectionException e)
//		{
//			e.printStackTrace();
//		}
	}

	/**
	 * Sets the GUI elements on the configurationPanel.
	 */
	public void setConfigurationPanel()
	{
		configurationPanel.setConfigurationPanel();
	}

	//自己抽取了打开串口开关的函数，便于远程调用。（改变开关状态）
	public void openComm()
	{
		openButton.setEnabled(false);
		Cursor previousCursor = getCursor();
		setNewCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
		configurationPanel.setParameters();
		try
		{
			connection.openConnection();
		}
		catch (SerialConnectionException e2)
		{
			@SuppressWarnings("unused")
			AlertDialog ad = new AlertDialog(this, "Error Opening Port!",
					"Error opening port,", e2.getMessage() + ".",
					"Select new settings, try again.");
			openButton.setEnabled(true);
			setNewCursor(previousCursor);
			return;
		}
		portOpened();
		setNewCursor(previousCursor);
	}
	/**
	 * Responds to the menu items and buttons.
	 */
	public void actionPerformed(ActionEvent e)
	{
		String cmd = e.getActionCommand();

		// Opens a port.
		if (cmd.equals("打开端口"))
		{	
			openComm();
		}
		// Closes a port.
		if (cmd.equals("关闭端口"))
		{
			portClosed();
		}
		// Sends a break signal to the port.
		if (cmd.equals("Clear"))
		{
			messageAreaIn.setText("");
		}
		if (cmd.equals("Send"))
		{
			try
			{
				connection.writeToComm(messageAreaOut.getText());
			}
			catch (SerialConnectionException e1)
			{
				e1.printStackTrace();
			}
		}
	}

	/**
	 * Toggles the buttons to an open port state.
	 */
	public void portOpened()
	{
		openButton.setEnabled(false);
		closeButton.setEnabled(true);
		clearButton.setEnabled(true);
	}

	/**
	 * Calls closeConnection on the SerialConnection and toggles the buttons to
	 * a closed port state.
	 */
	public void portClosed()
	{
		connection.closeConnection();
		openButton.setEnabled(true);
		closeButton.setEnabled(false);
		clearButton.setEnabled(true);
	}

	/**
	 * Sets the <code>Cursor</code> for the application.
	 * 
	 * @param c
	 *            New <code>Cursor</code>
	 */
	private void setNewCursor(Cursor c)
	{
		setCursor(c);
		messageAreaIn.setCursor(c);
		messageAreaOut.setCursor(c);
	}



	/**
	 * Cleanly shuts down the applicaion. first closes any open ports and cleans
	 * up, then exits.
	 */
	public void shutdown()
	{
		connection.closeConnection();
		System.exit(1);
	}
	
	//单例模式下的serialDemo 直接调用connection 里面的 write方法就可以了
//	public void writeToComm(String commTxData)
//	{
//		try
//		{
//			connection.writeToComm(commTxData);
//		}
//		catch (SerialConnectionException e)
//		{
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		};
//	}


	/**
	 * GUI element that holds the user changable elements for connection
	 * configuration.
	 */
	class ConfigurationPanel extends Panel implements ItemListener
	{

		private Frame parent;

		private Label portNameLabel;
		private Choice portChoice;

		private Label baudLabel;
		private Choice baudChoice;

		private Label flowControlInLabel;
		private Choice flowChoiceIn;

		private Label flowControlOutLabel;
		private Choice flowChoiceOut;

		private Label databitsLabel;
		private Choice databitsChoice;

		private Label stopbitsLabel;
		private Choice stopbitsChoice;

		private Label parityLabel;
		private Choice parityChoice;

		/**
		 * Creates and initilizes the configuration panel. The initial settings
		 * are from the parameters object.
		 */
		public ConfigurationPanel(Frame parent)
		{
			this.parent = parent;

			setLayout(new GridLayout(4, 4));

			portNameLabel = new Label("端口名:", Label.LEFT);
			add(portNameLabel);

			portChoice = new Choice();
			portChoice.addItemListener(this);
			add(portChoice);
			listPortChoices();
			portChoice.select(parameters.getPortName());

			baudLabel = new Label("波特率:", Label.LEFT);
			add(baudLabel);

			baudChoice = new Choice();
			baudChoice.addItem("300");
			baudChoice.addItem("2400");
			baudChoice.addItem("9600");
			baudChoice.addItem("14400");
			baudChoice.addItem("28800");
			baudChoice.addItem("38400");
			baudChoice.addItem("57600");
			baudChoice.addItem("152000");
			baudChoice.select(Integer.toString(parameters.getBaudRate()));
			baudChoice.addItemListener(this);
			add(baudChoice);

			flowControlInLabel = new Label("Flow Control In:", Label.LEFT);
			add(flowControlInLabel);

			flowChoiceIn = new Choice();
			flowChoiceIn.addItem("None");
			flowChoiceIn.addItem("Xon/Xoff In");
			flowChoiceIn.addItem("RTS/CTS In");
			flowChoiceIn.select(parameters.getFlowControlInString());
			flowChoiceIn.addItemListener(this);
			add(flowChoiceIn);

			flowControlOutLabel = new Label("Flow Control Out:", Label.LEFT);
			add(flowControlOutLabel);

			flowChoiceOut = new Choice();
			flowChoiceOut.addItem("None");
			flowChoiceOut.addItem("Xon/Xoff Out");
			flowChoiceOut.addItem("RTS/CTS Out");
			flowChoiceOut.select(parameters.getFlowControlOutString());
			flowChoiceOut.addItemListener(this);
			add(flowChoiceOut);

			databitsLabel = new Label("数据位:", Label.LEFT);
			add(databitsLabel);

			databitsChoice = new Choice();
			databitsChoice.addItem("5");
			databitsChoice.addItem("6");
			databitsChoice.addItem("7");
			databitsChoice.addItem("8");
			databitsChoice.select(parameters.getDatabitsString());
			databitsChoice.addItemListener(this);
			add(databitsChoice);

			stopbitsLabel = new Label("终止位:", Label.LEFT);
			add(stopbitsLabel);

			stopbitsChoice = new Choice();
			stopbitsChoice.addItem("1");
			stopbitsChoice.addItem("1.5");
			stopbitsChoice.addItem("2");
			stopbitsChoice.select(parameters.getStopbitsString());
			stopbitsChoice.addItemListener(this);
			add(stopbitsChoice);

			parityLabel = new Label("奇偶校验:", Label.LEFT);
			add(parityLabel);

			parityChoice = new Choice();
			parityChoice.addItem("None");
			parityChoice.addItem("Even");
			parityChoice.addItem("Odd");
			parityChoice.select("None");
			parityChoice.select(parameters.getParityString());
			parityChoice.addItemListener(this);
			add(parityChoice);
		}

		/**
		 * Sets the configuration panel to the settings in the parameters
		 * object.
		 */
		public void setConfigurationPanel()
		{
			portChoice.select(parameters.getPortName());
			baudChoice.select(parameters.getBaudRateString());
			flowChoiceIn.select(parameters.getFlowControlInString());
			flowChoiceOut.select(parameters.getFlowControlOutString());
			databitsChoice.select(parameters.getDatabitsString());
			stopbitsChoice.select(parameters.getStopbitsString());
			parityChoice.select(parameters.getParityString());
		}

		/**
		 * Sets the parameters object to the settings in the configuration
		 * panel.
		 */
		public void setParameters()
		{
			parameters.setPortName(portChoice.getSelectedItem());
			parameters.setBaudRate(baudChoice.getSelectedItem());
			parameters.setFlowControlIn(flowChoiceIn.getSelectedItem());
			parameters.setFlowControlOut(flowChoiceOut.getSelectedItem());
			parameters.setDatabits(databitsChoice.getSelectedItem());
			parameters.setStopbits(stopbitsChoice.getSelectedItem());
			parameters.setParity(parityChoice.getSelectedItem());
		}

		/**
		 * Sets the elements for the portChoice from the ports available on the
		 * system. Uses an emuneration of comm ports returned by
		 * CommPortIdentifier.getPortIdentifiers(), then sets the current choice
		 * to a mathing element in the parameters object.
		 */
		void listPortChoices()
		{
			CommPortIdentifier portId;

			@SuppressWarnings("rawtypes")
			Enumeration en = CommPortIdentifier.getPortIdentifiers();

			// iterate through the ports.
			while (en.hasMoreElements())
			{
				portId = (CommPortIdentifier) en.nextElement();
				//判断得到的是端口类型是不是串口类型
				if (portId.getPortType() == CommPortIdentifier.PORT_SERIAL)
				{
					portChoice.addItem(portId.getName());
				}
			}
			portChoice.select(parameters.getPortName());
		}

		/**
		 * Event handler for changes in the current selection of the Choices. If
		 * a port is open the port can not be changed. If the choice is
		 * unsupported on the platform then the user will be notified and the
		 * settings will revert to their pre-selection state.
		 */
		public void itemStateChanged(ItemEvent e)
		{
			// Check if port is open.
			if (connection.isOpen())
			{
				// If port is open do not allow port to change.
				if (e.getItemSelectable() == portChoice)
				{
					// Alert user.
					@SuppressWarnings("unused")
					AlertDialog ad = new AlertDialog(parent, "Port Open!",
							"Port can not", "be changed",
							"while a port is open.");
					// Return configurationPanel to pre-choice settings.
					setConfigurationPanel();
					return;
				}
				// Set the parameters from the choice panel.
				setParameters();
				try
				{
					// Attempt to change the settings on an open port.
					connection.setConnectionParameters();
				}
				catch (SerialConnectionException ex)
				{
					// If setting can not be changed, alert user, return to
					// pre-choice settings.
					@SuppressWarnings("unused")
					AlertDialog ad = new AlertDialog(parent,
							"Unsupported Configuration!",
							"Configuration Parameter unsupported,",
							"select new value.",
							"Returning to previous configuration.");
					setConfigurationPanel();
				}
			}
			else
			{
				// Since port is not open just set the parameter object.
				setParameters();
			}
		}
	}

	/**
	 * Handles closing down system. Allows application to be closed with window
	 * close box.
	 */
	class CloseHandler extends WindowAdapter
	{

		SerialDemo sd;

		public CloseHandler(SerialDemo sd)
		{
			this.sd = sd;
		}

		public void windowClosing(WindowEvent e)
		{
			sd.shutdown();
		}
	}
}
