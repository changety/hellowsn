package org.androidpn.server.xmpp.codec;

import org.androidpn.server.xmpp.net.XmppIoHandler;
import org.apache.mina.core.buffer.IoBuffer;
import org.apache.mina.core.session.IoSession;
import org.apache.mina.filter.codec.CumulativeProtocolDecoder;
import org.apache.mina.filter.codec.ProtocolDecoderOutput;
import org.jivesoftware.openfire.nio.XMLLightweightParser;

/** 
 * Decoder class that parses ByteBuffers and generates XML stanzas.
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class XmppDecoder extends CumulativeProtocolDecoder {

    // private final Log log = LogFactory.getLog(XmppDecoder.class);

    @Override
    public boolean doDecode(IoSession session, IoBuffer in,
            ProtocolDecoderOutput out) throws Exception {
        // log.debug("doDecode(...)...");

        XMLLightweightParser parser = (XMLLightweightParser) session
                .getAttribute(XmppIoHandler.XML_PARSER);
        parser.read(in);

        if (parser.areThereMsgs()) {
            for (String stanza : parser.getMsgs()) {
                out.write(stanza);
            }
        }
        return !in.hasRemaining();
    }

}
