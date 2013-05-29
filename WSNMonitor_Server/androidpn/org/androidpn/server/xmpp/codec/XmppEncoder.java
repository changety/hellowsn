package org.androidpn.server.xmpp.codec;

import org.apache.mina.core.session.IoSession;
import org.apache.mina.filter.codec.ProtocolEncoder;
import org.apache.mina.filter.codec.ProtocolEncoderOutput;

/** 
 *  Encoder class that does nothing (to the already encoded data). 
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class XmppEncoder implements ProtocolEncoder {

    // private final Log log = LogFactory.getLog(XmppEncoder.class);

    public void encode(IoSession session, Object message,
            ProtocolEncoderOutput out) throws Exception {
        // log.debug("encode()...");
    }

    public void dispose(IoSession session) throws Exception {
        // log.debug("dispose()...");
    }

}
