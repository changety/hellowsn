package ty.change.wsn.service.impl;

import java.util.List;
import javax.persistence.EntityExistsException;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.springframework.dao.DataIntegrityViolationException;
import ty.change.wsn.dao.UserDao;
import ty.change.wsn.entity.User;
import ty.change.wsn.exception.UserExistsException;
import ty.change.wsn.exception.UserNotFoundException;
import ty.change.wsn.service.UserService;

public class UserServiceImpl implements UserService
{
	private UserDao userdao;

	public UserDao getUserdao()
	{
		return userdao;
	}

	public void setUserdao(UserDao userdao)
	{
		this.userdao = userdao;
	}

	protected final Log log = LogFactory.getLog(getClass());

	private UserDao userDao;

	public void setUserDao(UserDao userDao)
	{
		this.userDao = userDao;
	}

	@Override
	public User getUser(String userId)
	{
		return userDao.getUser(new Long(userId));
	}

	@Override
	public List<User> getUsers()
	{
		return userDao.getUsers();
	}

	@Override
	public User saveUser(User user) throws UserExistsException
	{
		try
		{
			return userDao.saveUser(user);
		}
		catch (DataIntegrityViolationException e)
		{
			e.printStackTrace();
			log.warn(e.getMessage());
			throw new UserExistsException("User '" + user.getUsername()
					+ "' already exists!");
		}
		catch (EntityExistsException e)
		{ // needed for JPA
			e.printStackTrace();
			log.warn(e.getMessage());
			throw new UserExistsException("User '" + user.getUsername()
					+ "' already exists!");
		}
	}

	@Override
	public User getUserByUsername(String username) throws UserNotFoundException
	{
		return (User) userDao.getUserByUsername(username);
	}

	@Override
	public User findUserByUsername(String username)
	{
		return this.userDao.findUserByUsername(username);
	}

	@Override
	public void removeUser(Long userId)
	{
		log.debug("removing user: " + userId);
		userDao.removeUser(userId);
	}

	@Override
	public void addUser(User user)
	{
		this.userdao.addUser(user);
	}

}
