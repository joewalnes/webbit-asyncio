package org.webbitserver.asyncio;

public class AsyncIO {

	public native AioRequest mkdir(String path, AioCallback callback); // TODO: mode_t
	public native AioRequest rmdir(String path, AioCallback callback);

}
