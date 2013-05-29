package org.androidpn.server.xmpp.router;

import org.xmpp.packet.Message;

/** 
 * This class is to route Message packets to their corresponding handler.
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class MessageRouter {

    /**
     * Constucts a packet router.
     */
    public MessageRouter() {
    }

    /**
     * Routes the Message packet.
     * 
     * @param packet the packet to route
     */
    public void route(Message packet) {
        throw new RuntimeException("Please implement this!");
    }

}
