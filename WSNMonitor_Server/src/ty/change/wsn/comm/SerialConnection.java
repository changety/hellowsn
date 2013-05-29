package ty.change.wsn.comm;

import gnu.io.CommPortIdentifier;
import gnu.io.CommPortOwnershipListener;
import gnu.io.NoSuchPortException;
import gnu.io.PortInUseException;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;
import gnu.io.UnsupportedCommOperationException;

import java.awt.TextArea;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.TooManyListenersException;

import ty.change.wsn.dataparser.DataCheckUtil;
import ty.change.wsn.dataparser.DataParseUtil;
import ty.change.wsn.util.Constant;

/**
 * A class that handles the details of a serial connection. Reads from one
 * TextArea and writes to a second TextArea. Holds the state of the connection.
 */
public class SerialConnection implements SerialPortEventListener,
		CommPortOwnershipListener
{
	private SerialDemo parent;
	private TextArea messageAreaOut;
	private TextArea messageAreaIn;
	private SerialParameters parameters;
	public OutputStream commOutputStream;
	private InputStream commInputStream;
	private KeyHandler keyHandler;
	private CommPortIdentifier portId;
	private SerialPort sPort;

	private boolean open;

	/**
	 * Creates a SerialConnection object and initilizes variables passed in as
	 * params.
	 * 
	 * @param parent
	 *            A SerialDemo object.
	 * @param parameters
	 *            A SerialParameters object.
	 * @param messageAreaOut
	 *            The TextArea that messages that are to be sent out of the
	 *            serial port are entered into.
	 * @param messageAreaIn
	 *            The TextArea that messages comming into the serial port are
	 *            displayed on.
	 */
	public SerialConnection(SerialDemo parent, SerialParameters parameters,
			TextArea messageAreaOut, TextArea messageAreaIn)
	{
		this.parent = parent;
		this.parameters = parameters;
		this.messageAreaOut = messageAreaOut;
		this.messageAreaIn = messageAreaIn;
		open = false;
	}

	public SerialConnection()
	{

	}

	/**
	 * Attempts to open a serial connection and streams using the parameters in
	 * the SerialParameters object. If it is unsuccesfull at any step it returns
	 * the port to a closed state, throws a
	 * <code>SerialConnectionException</code>, and returns.
	 * 
	 * Gives a timeout of 30 seconds on the portOpen to allow other applications
	 * to reliquish the port if have it open and no longer need it.
	 */
	public void openConnection() throws SerialConnectionException
	{

		// Obtain a CommPortIdentifier object for the port you want to open.
		try
		{
			portId = CommPortIdentifier.getPortIdentifier(parameters
					.getPortName());
		}
		catch (NoSuchPortException e)
		{
			throw new SerialConnectionException(e.getMessage());
		}

		// Open the port represented by the CommPortIdentifier object. Give
		// the open call a relatively long timeout of 30 seconds to allow
		// a different application to reliquish the port if the user
		// wants to.
		try
		{
			sPort = (SerialPort) portId.open("SerialDemo", 30000);
		}
		catch (PortInUseException e)
		{
			throw new SerialConnectionException(e.getMessage());
		}

		// Set the parameters of the connection. If they won't set, close the
		// port before throwing an exception.
		try
		{
			setConnectionParameters();
		}
		catch (SerialConnectionException e)
		{
			sPort.close();
			throw e;
		}

		// Open the input and output streams for the connection. If they won't
		// open, close the port before throwing an exception.
		try
		{
			commOutputStream = sPort.getOutputStream();
			commInputStream = sPort.getInputStream();
		}
		catch (IOException e)
		{
			sPort.close();
			throw new SerialConnectionException("Error opening i/o streams");
		}

		// Create a new KeyHandler to respond to key strokes in the
		// messageAreaOut. Add the KeyHandler as a keyListener to the
		// messageAreaOut.
		keyHandler = new KeyHandler(commOutputStream);
		messageAreaOut.addKeyListener(keyHandler);

		// Add this object as an event listener for the serial port.
		try
		{
			sPort.addEventListener(this);
		}
		catch (TooManyListenersException e)
		{
			sPort.close();
			throw new SerialConnectionException("too many listeners added");
		}

		// Set notifyOnDataAvailable to true to allow event driven input.
		sPort.notifyOnDataAvailable(true);

		// Set notifyOnBreakInterrup to allow event driven break handling.
		sPort.notifyOnBreakInterrupt(true);

		// Set receive timeout to allow breaking out of polling loop during
		// input handling.
		try
		{
			sPort.enableReceiveTimeout(30);
		}
		catch (UnsupportedCommOperationException e)
		{
		}

		// Add ownership listener to allow ownership event handling.
		portId.addPortOwnershipListener(this);

		open = true;
		// writeToComm("FE030102007B02FD");
	}

	/**
	 * Sets the connection parameters to the setting in the parameters object.
	 * If set fails return the parameters object to origional settings and throw
	 * exception.
	 */
	public void setConnectionParameters() throws SerialConnectionException
	{

		// Save state of parameters before trying a set.
		int oldBaudRate = sPort.getBaudRate();
		int oldDatabits = sPort.getDataBits();
		int oldStopbits = sPort.getStopBits();
		int oldParity = sPort.getParity();
		@SuppressWarnings("unused")
		int oldFlowControl = sPort.getFlowControlMode();

		// Set connection parameters, if set fails return parameters object
		// to original state.
		try
		{
			sPort.setSerialPortParams(parameters.getBaudRate(),
					parameters.getDatabits(), parameters.getStopbits(),
					parameters.getParity());
			// System.out.println(sPort.isRTS());
			sPort.setRTS(true);
			// System.out.println(sPort.isRTS());
		}
		catch (UnsupportedCommOperationException e)
		{
			parameters.setBaudRate(oldBaudRate);
			parameters.setDatabits(oldDatabits);
			parameters.setStopbits(oldStopbits);
			parameters.setParity(oldParity);
			throw new SerialConnectionException("Unsupported parameter");
		}

		// Set flow control.
		try
		{
			sPort.setFlowControlMode(parameters.getFlowControlIn()
					| parameters.getFlowControlOut());
		}
		catch (UnsupportedCommOperationException e)
		{
			throw new SerialConnectionException("Unsupported flow control");
		}
	}

	/**
	 * Close the port and clean up associated elements.
	 */
	public void closeConnection()
	{
		// If port is alread closed just return.
		if (!open)
		{
			return;
		}

		// Remove the key listener.
		messageAreaOut.removeKeyListener(keyHandler);

		// Check to make sure sPort has reference to avoid a NPE.
		if (sPort != null)
		{
			try
			{
				// close the i/o streams.
				commOutputStream.close();
				commInputStream.close();
			}
			catch (IOException e)
			{
				System.err.println(e);
			}

			// Close the port.
			sPort.close();

			// Remove the ownership listener.
			portId.removePortOwnershipListener(this);
		}

		open = false;
	}

	/**
	 * Reports the open status of the port.
	 * 
	 * @return true if port is open, false if port is closed.
	 */
	public boolean isOpen()
	{
		return open;
	}

	public void writeToComm(String commTxData) throws SerialConnectionException
	{
		try
		{
			commOutputStream.write(DataParseUtil.getInstance().parseCommTxData(
					commTxData));
		}
		catch (IOException e)
		{
			e.printStackTrace();
			throw new SerialConnectionException("串口没有打开");
		}
	}

	/**
	 * Handles SerialPortEvents. The two types of SerialPortEvents that this
	 * program is registered to listen for are DATA_AVAILABLE and BI. During
	 * DATA_AVAILABLE the port buffer is read until it is drained, when no more
	 * data is availble and 30ms has passed the method returns. When a BI event
	 * occurs the words BREAK RECEIVED are written to the messageAreaIn.
	 */
	public void serialEvent(SerialPortEvent e)
	{

		// Create a StringBuffer and int to receive input data.
		StringBuffer inputBuffer = new StringBuffer();
		int newData = 0;
		String wsnData = "";
		// Determine type of event.
		switch (e.getEventType())
		{

		// Read data until -1 is returned. If \r is received substitute
		// \n for correct newline handling.
		case SerialPortEvent.DATA_AVAILABLE:
			while (newData != -1)
			{
				try
				{
					newData = commInputStream.read();
					if (newData == -1)
					{
						break;
					}
					if ('\r' == (char) newData)
					{
						inputBuffer.append('\n');
					}
					else
					{
						/*
						 * byteVar & 0x000000FF的作用是，如果byteVar 是负数，则会清除前面24个零，
						 * 正的byte整型不受影响。 (...) | 0xFFFFFF00的作用是，如果byteVar
						 * 是正数，则置前24位为一， 这样toHexString输出一个小于等于15的byte整型的十六进制时，
						 * 倒数第二位为零且不会被丢弃，这样可以通过substring方法进行截取最后两位即可。
						 */
						inputBuffer.append(Integer.toHexString(
								(newData & 0x000000FF) | 0xFFFFFF00).substring(
								6));// 从第六开始截到最后
					}
				}
				catch (IOException ex)
				{
					System.err.println(ex);
					return;
				}
			}
			// Append received data to messageAreaIn.
			messageAreaIn.append((new String(inputBuffer)).toUpperCase());
			messageAreaIn.append("\n");
			wsnData = new String(inputBuffer).toUpperCase();
			System.out.println(wsnData + "------" + wsnData.length());
			DataCheckUtil.getInstance().commDataCheck(wsnData);
			// System.out.println(wsnData + "------" + wsnData.length());

			// System.out.println("协议头" + "----------------------" +
			// wsnData.substring(0,2));
			// 包长度,可变,去掉CRC,去掉起始位标识,去掉包长(也就是本身的两位),去掉反馈标志后的长度
			// System.out.println("包长度" + "----------------------" +
			// wsnData.substring(2,4));
			// System.out.println("回馈标志,可变" +
			// "----------------------"+wsnData.substring(4,8));
			System.out.println("节点短地址" + "----------------------"
					+ wsnData.substring(8, 12));
			// System.out.println("温度包标志" +
			// "----------------------"+wsnData.substring(12,16));
			// System.out.println("真正有用数据包长度" +"----------------------"
			// +wsnData.substring(16,20));
			// System.out.println("温度包数据" +
			// "----------------------"+wsnData.substring(20,22));
			// System.out.println("电压包数据" +"----------------------"
			// +wsnData.substring(22,24));
			if (!Constant.COOR_DATA_HEAD.equals(wsnData.substring(0, 2)))
			{
				System.out.println("父节点" + "----------------------"
						+ wsnData.substring(24, 28));
			}
			// System.out.println("CRC" +
			// "----------------------"+wsnData.substring(28,30));
			// System.out.println(Integer.parseInt(wsnData.substring(20,21)) *
			// 16 + Integer.parseInt(wsnData.substring(21,22)));
			// System.out.println((Integer.parseInt(wsnData.substring(22,23)) *
			// 1.6 +
			// Integer.parseInt(wsnData.substring(23,24)) * 0.1));

			break;

		// If break event append BREAK RECEIVED message.
		case SerialPortEvent.BI:
			messageAreaIn.append("\n--- BREAK RECEIVED ---\n");
		}

	}

	/**
	 * Handles ownership events. If a PORT_OWNERSHIP_REQUESTED event is received
	 * a dialog box is created asking the user if they are willing to give up
	 * the port. No action is taken on other types of ownership events.
	 */
	public void ownershipChange(int type)
	{
		if (type == CommPortOwnershipListener.PORT_OWNERSHIP_REQUESTED)
		{
			@SuppressWarnings("unused")
			PortRequestedDialog prd = new PortRequestedDialog(parent);
		}
	}

	/**
	 * A class to handle <code>KeyEvent</code>s generated by the messageAreaOut.
	 * When a <code>KeyEvent</code> occurs the <code>char</code> that is
	 * generated by the event is read, converted to an <code>int</code> and
	 * writen to the <code>OutputStream</code> for the port.
	 */
	class KeyHandler extends KeyAdapter
	{
		OutputStream os;

		/**
		 * Creates the KeyHandler.
		 * 
		 * @param os
		 *            The OutputStream for the port.
		 */
		public KeyHandler(OutputStream os)
		{
			super();
			this.os = os;
		}

		/**
		 * Handles the KeyEvent. Gets the
		 * <code>char</char> generated by the <code>KeyEvent</code>, converts it
		 * to an <code>int</code>, writes it to the <code>
	OutputStream</code> for the
		 * port.
		 */
		public void keyTyped(KeyEvent evt)
		{
			char newCharacter = evt.getKeyChar();
			try
			{
				os.write((int) newCharacter);
			}
			catch (IOException e)
			{
				System.err.println("OutputStream write error: " + e);
			}
		}
	}

}
