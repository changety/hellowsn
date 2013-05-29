package ty.change.wsn.service;

import java.util.List;

import ty.change.wsn.entity.User;
import ty.change.wsn.exception.UserExistsException;
import ty.change.wsn.exception.UserNotFoundException;

public interface UserService
{
 	public User getUser(String userId);

    public List<User> getUsers();

    public User saveUser(User user) throws UserExistsException;

    public User getUserByUsername(String username) throws UserNotFoundException;

    public void removeUser(Long userId);

	public void addUser(User user);
	
	public User findUserByUsername(String username);
	
}
