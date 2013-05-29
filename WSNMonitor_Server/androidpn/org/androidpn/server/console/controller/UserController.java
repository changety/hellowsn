package org.androidpn.server.console.controller;

import java.util.List;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.androidpn.server.xmpp.presence.PresenceManager;
import org.springframework.web.servlet.ModelAndView;
import org.springframework.web.servlet.mvc.multiaction.MultiActionController;

import ty.change.wsn.entity.User;
import ty.change.wsn.service.UserService;
import ty.change.wsn.util.ApplicationContextHelper;

/**
 * A controller class to process the user related requests.
 * 
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class UserController extends MultiActionController
{

	private UserService userService;
	public UserController()
	{
		userService = (UserService) ApplicationContextHelper
				.getBean("userService");
		System.out.println(userService);
	}
	public ModelAndView list(HttpServletRequest request,
			HttpServletResponse response) throws Exception
	{
		PresenceManager presenceManager = new PresenceManager();
		List<User> userList = userService.getUsers();
		for (User user : userList)
		{
			if (presenceManager.isAvailable(user))
			{
				// Presence presence = presenceManager.getPresence(user);
				user.setOnline(true);
			}
			else
			{
				user.setOnline(false);
			}
			System.out.println("user.online=" + user.isOnline());
		}
		ModelAndView mav = new ModelAndView();
		mav.addObject("userList", userList);
		mav.setViewName("user/userlist");
		return mav;
	}

}
