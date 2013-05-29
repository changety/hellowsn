package ty.change.wsn.exception;

/** 
 * Thrown if an unique constraint violation occured during saving a user. 
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class UserExistsException extends Exception {

    private static final long serialVersionUID = 1L;

    public UserExistsException() {
        super();
    }

    public UserExistsException(String message) {
        super(message);
    }

    public UserExistsException(Throwable cause) {
        super(cause);
    }

    public UserExistsException(String message, Throwable cause) {
        super(message, cause);
    }
}