package org.androidpn.server.xmpp.handler;

import org.androidpn.server.xmpp.UnauthorizedException;
import org.xmpp.packet.IQ;

/** 
 * This class is to handle the TYPE_IQ jabber:iq:roster protocol.
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class IQRosterHandler extends IQHandler {
    
    private static final String NAMESPACE = "jabber:iq:roster";
    
    /**
     * Constructor.
     */
    public IQRosterHandler() {        
    }

    /**
     * Handles the received IQ packet.
     * 
     * @param packet the packet
     * @return the response to send back
     * @throws UnauthorizedException if the user is not authorized
     */
    public IQ handleIQ(IQ packet) throws UnauthorizedException {
        // TODO
    	System.out.println("roster");
        return null;
    }
    
    /**
     * Returns the namespace of the handler.
     * 
     * @return the namespace
     */
    public String getNamespace() {
        return NAMESPACE;
    }

}
