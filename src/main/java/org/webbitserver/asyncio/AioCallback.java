package org.webbitserver.asyncio;

public interface AioCallback {
	void complete(AioRequest request);
}
