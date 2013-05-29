package org.androidpn.server.xmpp.handler;

import org.androidpn.server.xmpp.UnauthorizedException;
import org.androidpn.server.xmpp.router.PacketDeliverer;
import org.androidpn.server.xmpp.session.SessionManager;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.xmpp.packet.IQ;
import org.xmpp.packet.Packet;
import org.xmpp.packet.PacketError;

/** 
 * This is an abstract class to handle routed IQ packets.
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public abstract class IQHandler {

    protected final Log log = LogFactory.getLog(getClass());

    protected SessionManager sessionManager;

    /**
     * Constructor.
     */
    public IQHandler() {
        sessionManager = SessionManager.getInstance();
    }

    /**
     * Processes the received IQ packet.
     * 
     * @param packet the packet
     */
    public void process(Packet packet) {
        IQ iq = (IQ) packet;
        try {
            IQ reply = handleIQ(iq);
            if (reply != null) {
                PacketDeliverer.deliver(reply);
            }
        } catch (UnauthorizedException e) {
            if (iq != null) {
                try {
                    IQ response = IQ.createResultIQ(iq);
                    response.setChildElement(iq.getChildElement().createCopy());
                    response.setError(PacketError.Condition.not_authorized);
                    sessionManager.getSession(iq.getFrom()).process(response);
                } catch (Exception de) {
                    log.error("Internal server error", de);
                    sessionManager.getSession(iq.getFrom()).close();
                }
            }
        } catch (Exception e) {
            log.error("Internal server error", e);
            try {
                IQ response = IQ.createResultIQ(iq);
                response.setChildElement(iq.getChildElement().createCopy());
                response.setError(PacketError.Condition.internal_server_error);
                sessionManager.getSession(iq.getFrom()).process(response);
            } catch (Exception ex) {
                // Ignore
            }
        }
    }

    /**
     * Handles the received IQ packet.
     * 
     * @param packet the packet
     * @return the response to send back
     * @throws UnauthorizedException if the user is not authorized
     */
    public abstract IQ handleIQ(IQ packet) throws UnauthorizedException;

    /**
     * Returns the namespace of the handler.
     * 
     * @return the namespace
     */
    public abstract String getNamespace();

}
