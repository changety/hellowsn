package org.androidpn.server.xmpp.router;

import org.xmpp.packet.IQ;
import org.xmpp.packet.Message;
import org.xmpp.packet.Packet;
import org.xmpp.packet.Presence;

/** 
 * This class is to handle incoming packets and route them to their corresponding handler.
 * 处理 来的 packets  然后 分配 他们 相对应的  handler
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class PacketRouter {

    private MessageRouter messageRouter;

    private PresenceRouter presenceRouter;

    private IQRouter iqRouter;

    /**
     * Constructor. 
     */
    public PacketRouter() {
        messageRouter = new MessageRouter();
        presenceRouter = new PresenceRouter();
        iqRouter = new IQRouter();
    }

    /**
     * Routes the packet based on its type.  根据种类  route 包
     * packet 是  IQ  Message  Presence 的父类
     * @param packet the packet to route
     */
    public void route(Packet packet) {
        if (packet instanceof Message) {
            route((Message) packet);
        } else if (packet instanceof Presence) {
            route((Presence) packet);
        } else if (packet instanceof IQ) {
            route((IQ) packet);
        } else {
            throw new IllegalArgumentException();
        }
    }

    /**
     * Routes the IQ packet.
     * 
     * @param packet the packet to route
     */
    public void route(IQ packet) {
        iqRouter.route(packet);
    }

    /**
     * Routes the Message packet.
     * 
     * @param packet the packet to route
     */
    public void route(Message packet) {
        messageRouter.route(packet);
    }

    /**
     * Routes the Presence packet.
     * 
     * @param packet the packet to route
     */
    public void route(Presence packet) {
        presenceRouter.route(packet);
    }

}
