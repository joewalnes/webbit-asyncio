package org.webbitserver.asyncio;

public interface AioCallback<T extends AioRequest> {
	void complete(T request);
}
