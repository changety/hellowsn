package ty.change.wsn.comm;

import java.awt.*;
import java.awt.event.*;

/**
A single response modal alert dialog. This class is configurable for message 
and title. The width of the dialog will be longer than the longest message 
line.  When the OK button is pressed the dialog returns.
*/
public class AlertDialog extends Dialog implements ActionListener {

    /**
    Creates a new <code>AlertDialog</code> with three lines of message and
    a title.

    @param parent Any Frame.
    @param title The title to appear in the border of the dialog.
    @param lineOne The first line of the message in the dialog.
    @param lineTwo The second line of the message in the dialog.
    @param lineThree The third line of the message in the dialog.
    */
    public AlertDialog(Frame parent, 
		       String title, 
		       String lineOne, 
		       String lineTwo,
		       String lineThree) {
	super(parent, title, true);

	Panel labelPanel = new Panel();
	labelPanel.setLayout(new GridLayout(3, 1));
	labelPanel.add(new Label(lineOne, Label.CENTER));
	labelPanel.add(new Label(lineTwo, Label.CENTER));
	labelPanel.add(new Label(lineThree, Label.CENTER));
	add(labelPanel, "Center");

	Panel buttonPanel = new Panel();
	Button okButton = new Button("OK");
	okButton.addActionListener(this);
	buttonPanel.add(okButton);
	add(buttonPanel, "South");

	FontMetrics fm = getFontMetrics(getFont());
	int width = Math.max(fm.stringWidth(lineOne), 
		 Math.max(fm.stringWidth(lineTwo), fm.stringWidth(lineThree)));

	setSize(width + 40, 150);
	setLocation(parent.getLocationOnScreen().x + 30, 
		    parent.getLocationOnScreen().y + 30);
	setVisible(true);
    }

    /**
    Handles events from the OK button. When OK is pressed the dialog becomes
    invisible, disposes of its self, and retruns.
    */
    public void actionPerformed(ActionEvent e) {
	setVisible(false);
	dispose();
    }
}
