package org.androidpn.server.console.controller;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.androidpn.server.util.Config;
import org.androidpn.server.xmpp.push.NotificationManager;
import org.springframework.web.bind.ServletRequestUtils;
import org.springframework.web.servlet.ModelAndView;
import org.springframework.web.servlet.mvc.multiaction.MultiActionController;

/** 
 * A controller class to process the notification related requests.  
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class NotificationController extends MultiActionController {

    private NotificationManager notificationManager;

    public NotificationController() {
        notificationManager = new NotificationManager();
    }

    public ModelAndView list(HttpServletRequest request,
            HttpServletResponse response) throws Exception {
        ModelAndView mav = new ModelAndView();
        mav.setViewName("notification/form");
        return mav;
    }

    public ModelAndView send(HttpServletRequest request,
            HttpServletResponse response) throws Exception {
        String broadcast = ServletRequestUtils.getStringParameter(request,
                "broadcast", "Y");
        String username = ServletRequestUtils.getStringParameter(request,
                "username");
        String title = ServletRequestUtils.getStringParameter(request, "title");
        String message = ServletRequestUtils.getStringParameter(request,
                "message");
        String uri = ServletRequestUtils.getStringParameter(request, "uri");

        String apiKey = Config.getString("apiKey", "");
        logger.debug("apiKey=" + apiKey);

        if (broadcast.equalsIgnoreCase("Y")) {
            notificationManager.sendBroadcast(apiKey, title, message, uri);
        } else {
            notificationManager.sendNotifcationToUser(apiKey, username, title,
                    message, uri);
        }

        ModelAndView mav = new ModelAndView();
        mav.setViewName("redirect:notification.do");
        return mav;
    }

}
