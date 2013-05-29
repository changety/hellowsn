package org.androidpn.server.xmpp.presence;

import org.androidpn.server.xmpp.session.ClientSession;
import org.androidpn.server.xmpp.session.SessionManager;
import org.xmpp.packet.Presence;

import ty.change.wsn.entity.User;

/** 
 * This class is to manage the presences of users. 
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class PresenceManager {

    private SessionManager sessionManager;

    /**
     * Constructor.
     */
    public PresenceManager() {
        sessionManager = SessionManager.getInstance();
    }

    /**
     * Returns the availability of the user.是否存在这个session
     * 
     * @param user the user
     * @return true if the user is available
     */
    public boolean isAvailable(User user) {
        return sessionManager.getSession(user.getUsername()) != null;
    }

    /**
     * Returns the current presence of the user.
     * 
     * @param user the user
     * @return the current presence of the user.
     */
    public Presence getPresence(User user) {
        if (user == null) {
            return null;
        }
        Presence presence = null;
        //与该用户名关联的session会话.
        ClientSession session = sessionManager.getSession(user.getUsername());
        if (session != null) {
            presence = session.getPresence();
        }
        return presence;
    }

}
