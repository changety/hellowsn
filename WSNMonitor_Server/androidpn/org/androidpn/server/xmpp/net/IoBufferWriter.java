package org.androidpn.server.xmpp.net;

import java.io.IOException;
import java.io.Writer;
import java.nio.charset.CharsetEncoder;

import org.apache.mina.core.buffer.IoBuffer;

/** 
 * Wrapper on a MINA {@link IoBuffer} that extends the Writer class.
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class IoBufferWriter extends Writer {

    private CharsetEncoder encoder;

    private IoBuffer ioBuffer;

    /**
     * Constructor.
     * 
     * @param ioBuffer the IoBuffer
     * @param encoder the charset encoder
     */
    public IoBufferWriter(IoBuffer ioBuffer, CharsetEncoder encoder) {
        this.encoder = encoder;
        this.ioBuffer = ioBuffer;
    }

    /**
     * Writes a portion of an array of characters.
     * 
     * @param cbuf Array of characters
     * @param off Offset from which to start writing characters
     * @param len Number of characters to write
     */
    @Override
    public void write(char[] cbuf, int off, int len) throws IOException {
        ioBuffer.putString(new String(cbuf, off, len), encoder);
    }

    /**
     * Flushes the stream.
     */
    @Override
    public void flush() throws IOException {
        // Ignore
    }

    /**
     * Closes the stream, flushing it first.
     */
    @Override
    public void close() throws IOException {
        // Ignore
    }

}
