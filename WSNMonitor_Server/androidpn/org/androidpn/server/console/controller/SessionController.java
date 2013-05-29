package org.androidpn.server.console.controller;

import java.util.ArrayList;
import java.util.List;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.androidpn.server.console.vo.SessionVO;
import org.androidpn.server.xmpp.session.ClientSession;
import org.androidpn.server.xmpp.session.Session;
import org.androidpn.server.xmpp.session.SessionManager;
import org.springframework.web.servlet.ModelAndView;
import org.springframework.web.servlet.mvc.multiaction.MultiActionController;
import org.xmpp.packet.Presence;

/** 
 * A controller class to process the session related requests.  
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class SessionController extends MultiActionController {

    //private UserService userService;

    public SessionController() {
        //userService = ServiceLocator.getUserService();
    }

    public ModelAndView list(HttpServletRequest request,
            HttpServletResponse response) throws Exception {
        ClientSession[] sessions = new ClientSession[0];
        sessions = SessionManager.getInstance().getSessions().toArray(sessions);

        List<SessionVO> voList = new ArrayList<SessionVO>();
        for (ClientSession sess : sessions) {
            SessionVO vo = new SessionVO();
            vo.setUsername(sess.getUsername());
            vo.setResource(sess.getAddress().getResource());
            // Status
            if (sess.getStatus() == Session.STATUS_CONNECTED) {
                vo.setStatus("CONNECTED");
            } else if (sess.getStatus() == Session.STATUS_AUTHENTICATED) {
                vo.setStatus("AUTHENTICATED");
            } else if (sess.getStatus() == Session.STATUS_CLOSED) {
                vo.setStatus("CLOSED");
            } else {
                vo.setStatus("UNKNOWN");
            }
            // Presence
            if (!sess.getPresence().isAvailable()) {
                vo.setPresence("Offline");
            } else {
                Presence.Show show = sess.getPresence().getShow();
                if (show == null) {
                    vo.setPresence("Online");
                } else if (show == Presence.Show.away) {
                    vo.setPresence("Away");
                } else if (show == Presence.Show.chat) {
                    vo.setPresence("Chat");
                } else if (show == Presence.Show.dnd) {
                    vo.setPresence("Do Not Disturb");
                } else if (show == Presence.Show.xa) {
                    vo.setPresence("eXtended Away");
                } else {
                    vo.setPresence("Unknown");
                }
            }
            vo.setClientIP(sess.getHostAddress());
            vo.setCreatedDate(sess.getCreationDate());
            voList.add(vo);
        }

        ModelAndView mav = new ModelAndView();
        mav.addObject("sessionList", voList);
        mav.setViewName("session/sessionlist");
        return mav;
    }

}
