package org.webbitserver.asyncio;

public class AsyncIO {

	static {
		init();
	}

	private static native void init();

	/**
	 * Must be called regularly to handle pending requests.
	 *
	 * @return 0 if all requests were handled, -1 if not, 
	 *           or the value of EIO_FINISH if != 0
	 */
	public static native int poll();

	/**
	 * Number of requests in-flight.
	 */
	public static native int numRequests();

	/**
	 * Number of not yet handled requests.
	 */
	public static native int numReady();

	/**
	 * Number of finished but unhandled requests.
	 */
	public static native int numPending();

	/**
	 * Number of worker threads in use currently.
	 */
	public static native int numThreads();

	public static native AioRequest mkdir(String path, int mode, AioCallback callback);
	public static native AioRequest rmdir(String path, AioCallback callback);

}
