package ty.change.wsn.entity;

import java.util.Date;

import org.apache.commons.lang.builder.ToStringBuilder;
import org.apache.commons.lang.builder.ToStringStyle;

public class User
{
	private static final long serialVersionUID = 4733464888738356502L;

	private long id;

	private String username;

	private String password;

	private String authority;

	private String email;

	private boolean Login;

	private boolean Pn;

	private boolean online;

	private Date createdDate = new Date();

	private Date updatedDate;

	public long getId()
	{
		return id;
	}

	public void setId(long id)
	{
		this.id = id;
	}

	public String getEmail()
	{
		return email;
	}

	public void setEmail(String email)
	{
		this.email = email;
	}

	public String getUsername()
	{
		return username;
	}

	public void setUsername(String username)
	{
		this.username = username;
	}

	public String getAuthority()
	{
		return authority;
	}

	public void setAuthority(String authority)
	{
		this.authority = authority;
	}

	public String getPassword()
	{
		return password;
	}

	public void setPassword(String password)
	{
		this.password = password;
	}

	public boolean isOnline()
	{
		return online;
	}

	public void setOnline(boolean online)
	{
		this.online = online;
	}

	public Date getCreatedDate()
	{
		return createdDate;
	}

	public void setCreatedDate(Date createdDate)
	{
		this.createdDate = createdDate;
	}

	public Date getUpdatedDate()
	{
		return updatedDate;
	}

	public void setUpdatedDate(Date updatedDate)
	{
		this.updatedDate = updatedDate;
	}

	public void setLogin(boolean login)
	{
		Login = login;
	}

	public boolean getLogin()
	{
		return Login;
	}

	public void setPn(boolean pn)
	{
		Pn = pn;
	}

	public boolean getPn()
	{
		return Pn;
	}

	@Override
	public int hashCode()
	{
		int result = 0;
		result = 29 * result + (username != null ? username.hashCode() : 0);
		result = 29 * result
				+ (createdDate != null ? createdDate.hashCode() : 0);
		return result;
	}

	@Override
	public String toString()
	{
		return ToStringBuilder.reflectionToString(this,
				ToStringStyle.MULTI_LINE_STYLE);
	}
}
