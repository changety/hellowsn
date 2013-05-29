package org.androidpn.server.xmpp.auth;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import org.androidpn.server.xmpp.UnauthenticatedException;
import org.androidpn.server.xmpp.XmppServer;
import org.apache.commons.codec.binary.Hex;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import ty.change.wsn.exception.UserNotFoundException;
import ty.change.wsn.service.UserService;
import ty.change.wsn.util.ApplicationContextHelper;

/** 
 * This class is to provide the methods associated with user authentication. 
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class AuthManager {

    private static final Log log = LogFactory.getLog(AuthManager.class);

    private static final Object DIGEST_LOCK = new Object();

    //SHA加密
    private static MessageDigest digest;

    static {
        try {
            digest = MessageDigest.getInstance("SHA");
        } catch (NoSuchAlgorithmException e) {
            log.error("Internal server error", e);
        }
    }

    /**
     * Returns the user's password. 
     * 
     * @param username the username
     * @return the user's password
     * @throws UserNotFoundException if the your was not found
     */
    public static String getPassword(String username)
            throws UserNotFoundException {
        return ((UserService)ApplicationContextHelper.getBean("userService")).getUserByUsername(username)
                .getPassword();
    }

    /**
     * Authenticates a user with a username and plain text password, and
     * returns an AuthToken.
     * 
     * @param username the username
     * @param password the password
     * @return an AuthToken
     * @throws UnauthenticatedException if the username and password do not match
     */
    public static AuthToken authenticate(String username, String password)
            throws UnauthenticatedException {
        if (username == null || password == null) {
            throw new UnauthenticatedException();
        }
        username = username.trim().toLowerCase();
        if (username.contains("@")) {
            int index = username.indexOf("@");
            String domain = username.substring(index + 1);
            if (domain.equals(XmppServer.getInstance().getServerName())) {
                username = username.substring(0, index);
            } else {
                throw new UnauthenticatedException();
            }
        }
        try {
            if (!password.equals(getPassword(username))) {
                throw new UnauthenticatedException();
            }
        } catch (UserNotFoundException unfe) {
            throw new UnauthenticatedException();
        }
        return new AuthToken(username);
    }

    /**
     * Authenticates a user with a username, token, and digest, and returns
     * an AuthToken.
     * 
     * @param username the username
     * @param token the token
     * @param digest the digest
     * @return an AuthToken
     * @throws UnauthenticatedException if the username and password do not match 
     */
    public static AuthToken authenticate(String username, String token,
            String digest) throws UnauthenticatedException {
        if (username == null || token == null || digest == null) {
            throw new UnauthenticatedException();
        }
        username = username.trim().toLowerCase();
        if (username.contains("@")) {
            int index = username.indexOf("@");
            String domain = username.substring(index + 1);
            if (domain.equals(XmppServer.getInstance().getServerName())) {
                username = username.substring(0, index);
            } else {
                throw new UnauthenticatedException();
            }
        }
        try {
            String password = getPassword(username);
            String anticipatedDigest = createDigest(token, password);
            if (!digest.equalsIgnoreCase(anticipatedDigest)) {
                throw new UnauthenticatedException();
            }
        } catch (UserNotFoundException unfe) {
            throw new UnauthenticatedException();
        }
        return new AuthToken(username);
    }

    /**
     * Returns true if plain text password authentication is supported according to JEP-0078.
     * 
     * @return true if plain text password authentication is supported
     */
    public static boolean isPlainSupported() {
        return true;
    }

    /**
     * Returns true if digest authentication is supported according to JEP-0078.
     * 
     * @return true if digest authentication is supported
     */
    public static boolean isDigestSupported() {
        return true;
    }
    //返回sha加密.
    private static String createDigest(String token, String password) {
        synchronized (DIGEST_LOCK) {
            digest.update(token.getBytes());
            return Hex.encodeHexString(digest.digest(password.getBytes()));
        }
    }

}
