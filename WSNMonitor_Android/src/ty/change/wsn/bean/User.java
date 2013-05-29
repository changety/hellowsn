package ty.change.wsn.bean;

import java.io.Serializable;

public class User implements Serializable {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1242817429406656274L;

	public User() {

	}

	public User(String username, String password, String email, String authority) {
		this.username = username;
		this.password = password;
		this.email = email;
		this.authority = authority;
	}

	private String authority;

	private String username;

	private String password;

	private String email;

	public String getUsername() {
		return username;
	}

	public void setUsername(String username) {
		this.username = username;
	}

	public String getPassword() {
		return password;
	}

	public void setPassword(String password) {
		this.password = password;
	}

	public String getEmail() {
		return email;
	}

	public void setEmail(String email) {
		this.email = email;
	}

	public String getAuthority() {
		return authority;
	}

	public void setAuthority(String authority) {
		this.authority = authority;
	}

}
