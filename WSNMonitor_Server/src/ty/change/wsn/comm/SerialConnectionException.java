package ty.change.wsn.comm;



public class SerialConnectionException extends Exception {

    /**
     * Constructs a <code>SerialConnectionException</code>
     * with the specified detail message.
     *
     * @param   s   the detail message.
     */
    public SerialConnectionException(String str) {
        super(str);
    }

    /**
     * Constructs a <code>SerialConnectionException</code>
     * with no detail message.
     */
    public SerialConnectionException() {
        super();
    }
}
