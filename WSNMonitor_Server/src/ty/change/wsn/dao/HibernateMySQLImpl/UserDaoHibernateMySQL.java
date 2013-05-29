package ty.change.wsn.dao.HibernateMySQLImpl;

import java.util.List;

import org.springframework.orm.hibernate3.support.HibernateDaoSupport;

import ty.change.wsn.dao.UserDao;
import ty.change.wsn.entity.User;
import ty.change.wsn.exception.UserNotFoundException;

public class UserDaoHibernateMySQL extends HibernateDaoSupport implements UserDao
{	
	/**
	 * 添加一条user的记录
	 */
	@Override
	public void addUser(User user)
	{
		this.getHibernateTemplate().save(user);
	}
	@Override
    public User getUser(Long id) {
        return (User) getHibernateTemplate().get(User.class, id);
    }
	@Override
    public User saveUser(User user) {
        getHibernateTemplate().saveOrUpdate(user);
        getHibernateTemplate().flush();
        return user;
    }
	@Override
    public void removeUser(Long id) {
        getHibernateTemplate().delete(getUser(id));
    }
	@Override
    public boolean exists(Long id) {
        User user = (User) getHibernateTemplate().get(User.class, id);
        return user != null;
    }
	@Override
    @SuppressWarnings("unchecked")
    public List<User> getUsers() {
        return getHibernateTemplate().find(
                "from User u order by u.createdDate desc");
    }
	@Override
    @SuppressWarnings("unchecked")
    public User getUserByUsername(String username) throws UserNotFoundException {
        List users = getHibernateTemplate().find("from User u where u.username=?", username);
        if (users == null || users.isEmpty()) {
            throw new UserNotFoundException("User '" + username + "' not found");
        } else {
            return (User) users.get(0);
        }
    }
    
    /**
	 * 根据username查询该用户的信息
	 */
	@Override
	public User findUserByUsername(String username)
	{
		List userList = this.getHibernateTemplate().find(
				"from User u where u.username=?", username);
		if (userList != null && userList.size() > 0)
		{
			return (User) userList.get(0);
		}
		else
		{
			return null;
		}
	}

	
//  @SuppressWarnings("unchecked")
//  public User findUserByUsername(String username) {
//      List users = getHibernateTemplate().find("from User where username=?",
//              username);
//      return (users == null || users.isEmpty()) ? null : (User) users.get(0);
//  }

}
