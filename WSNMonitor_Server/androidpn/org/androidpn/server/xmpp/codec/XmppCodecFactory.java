package org.androidpn.server.xmpp.codec;

import org.apache.mina.core.session.IoSession;
import org.apache.mina.filter.codec.ProtocolCodecFactory;
import org.apache.mina.filter.codec.ProtocolDecoder;
import org.apache.mina.filter.codec.ProtocolEncoder;

/** 
 * Factory class that specifies the encode and decoder to use for parsing XMPP stanzas.
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class XmppCodecFactory implements ProtocolCodecFactory {

    private final XmppEncoder encoder;

    private final XmppDecoder decoder;

    /**
     * Constructor.
     */
    public XmppCodecFactory() {
        encoder = new XmppEncoder();
        decoder = new XmppDecoder();
    }

    /**
     * Returns a new (or reusable) instance of ProtocolEncoder.
     */
    public ProtocolEncoder getEncoder(IoSession session) throws Exception {
        return encoder;
    }

    /**
     * Returns a new (or reusable) instance of ProtocolDecoder.
     */
    public ProtocolDecoder getDecoder(IoSession session) throws Exception {
        return decoder;
    }

}
