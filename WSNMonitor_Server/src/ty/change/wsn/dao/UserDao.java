package ty.change.wsn.dao;

import java.util.List;


import ty.change.wsn.entity.User;
import ty.change.wsn.exception.UserNotFoundException;

public interface UserDao
{
	public void addUser( User user);
	
	public User getUser(Long id);

    public User saveUser(User user);

    public void removeUser(Long id);

    public boolean exists(Long id);

    public List<User> getUsers();

    public User getUserByUsername(String username) throws UserNotFoundException;

    public User findUserByUsername(String username);

}
