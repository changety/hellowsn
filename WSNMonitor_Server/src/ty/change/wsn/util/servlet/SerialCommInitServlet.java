package ty.change.wsn.util.servlet;


import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;

import ty.change.wsn.comm.SerialConnection;
import ty.change.wsn.comm.SerialDemo;
import ty.change.wsn.util.Constant;

/**
 * @author Ruibron
 *
 */
public class SerialCommInitServlet extends HttpServlet
{
	@Override
	public void init() throws ServletException
	{
			
		SerialDemo serialDemo = new SerialDemo();
		
		SerialConnection serialConnection = serialDemo.getConnection();
		
		
		
		serialDemo.setVisible(true);
		
		serialDemo.repaint();
		
		this.getServletContext().setAttribute(Constant.SERIALAPP,serialDemo);
		
		this.getServletContext().setAttribute("serialConnection",serialConnection);
		
	}
	
}
