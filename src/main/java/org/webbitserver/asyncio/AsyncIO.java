package org.webbitserver.asyncio;

public class AsyncIO {

	static {
		init();
	}

	private static native void init();

	public static native int poll();

	public static native AioRequest mkdir(String path, int mode, AioCallback callback);
	public static native AioRequest rmdir(String path, AioCallback callback);

}
