package ty.change.wsn.exception;

/** 
 * Thrown if a requested user could not be loaded.
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class UserNotFoundException extends Exception {

    private static final long serialVersionUID = 1L;

    public UserNotFoundException() {
        super();
    }

    public UserNotFoundException(String message) {
        super(message);
    }

    public UserNotFoundException(Throwable cause) {
        super(cause);
    }

    public UserNotFoundException(String message, Throwable cause) {
        super(message, cause);
    }
}