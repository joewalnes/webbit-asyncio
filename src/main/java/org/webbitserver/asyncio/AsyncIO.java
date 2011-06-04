package org.webbitserver.asyncio;

public class AsyncIO {

	static {
		init();
	}

	private static native void init();

	/**
	 * Must be called regularly to handle pending requests.
	 *
	 * <p>This will always process any callbacks on the queue
	 * immediately (on the calling thread) and return. If there's
	 * nothing to do, it returns immediately.
	 *
	 * <p>A typical event loop may look like this:
	 * <pre>
	 * // Note: If this actually is your event loop, just call flush() instead.
	 * while (numRequests() &gt; 0) {
	 *   block(); // Don't bother looping unless there's something to do
	 *   poll(); // Process events (if they exist)
	 * }
	 * </pre>
	 *
	 * @return 0 if all requests were handled, -1 if not, 
	 *           or the value of EIO_FINISH if != 0
	 * @see #block()
	 */
	public static native int poll();

	/**
	 * Blocks the calling thread until it's time to actually do something.
	 *
	 * This prevents callers from having to continually call poll() when
	 * there may be nothing to do.
	 */
	public static native int block();

	/**
	 * Wait until all requests have been processed, then
	 * return.
	 */
	public static void flush() {
		while (numRequests() > 0) {
			block();
			poll();
		}
	}

	/**
	 * This is a standard POSIX file descriptor that can be used
	 * in select()/poll()/epoll()/kqueue loops. When there's something
	 * to process on the queue, the FD will be readable.
	 */
	public static native int wakeUpFileDescriptor();

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

	public static native void mkdir(String path, int mode, int priority, AioCallback callback);
	public static native void rmdir(String path, int priority, AioCallback callback);

}
