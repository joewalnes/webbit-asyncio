package org.webbitserver.asyncio;

public class AsyncIO {

	// Flags
	public static int READ               = 0x0000;
	public static int WRITE              = 0x0001;
	public static int READ_WRITE         = 0x0002;
	public static int APPEND             = 0x0008;
	public static int SHARED_LOCK        = 0x0010;
	public static int EXCLUSIVE_LOCK     = 0x0020;
	public static int NO_FOLLOW_SYMLINKS = 0x0100;
	public static int CREATE             = 0x0200;
	public static int TRUNCATE           = 0x0400;
	public static int EXCLUSIVE          = 0x0800;
	
  // Priorities
  public static int PRI_MIN            = -4;
  public static int PRI_MAX            = 4;
  public static int PRI_DEFAULT        = 0;

  public static int S_IFIFO            = 0010000;

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

  /** Does nothing except go through the entire process. */
	public static native void nop(int priority, AioCallback<AioRequest> callback);

  /** Ties a thread for this long, simulating business. */
	public static native void busy(int delay, int priority, AioCallback<AioRequest> callback);

	public static native void sync(int priority, AioCallback<AioRequest> callback);
	public static native void fsync(int fd, int priority, AioCallback<AioRequest> callback);
	public static native void fdatasync(int fd,int priority, AioCallback<AioRequest> callback);
	public static native void msync(long addr, int length, int flags, int priority, AioCallback<AioRequest> callback);
	public static native void mtouch(long addr, int length, int flags, int priority, AioCallback<AioRequest> callback);
	public static native void mlock(long addr, int length, int priority, AioCallback<AioRequest> callback);
	public static native void mlockall(int flags, int priority, AioCallback<AioRequest> callback);
	public static native void sync_file_range(int fd, int offset, int nbytes, int flags, int priority, AioCallback<AioRequest> callback);
	public static native void close(int fd, int priority, AioCallback<AioRequest> callback);
	public static native void readahead(int fd, int offset, int length, int priority, AioCallback<AioRequest> callback);
	public static native void read(int fd, long buffer, int length, int offset, int priority, AioCallback<AioRequest> callback);
	public static native void write(int fd, long buffer, int length, int offset, int priority, AioCallback<AioRequest> callback);
	public static native void fstat(int fd, int priority, AioCallback<AioRequest> callback);
	public static native void fstatvfs(int fd, int priority, AioCallback<AioRequest> callback);
	public static native void futime(int fd, long atime, long mtime, int priority, AioCallback<AioRequest> callback);
	public static native void ftruncate(int fd, int offset, int priority, AioCallback<AioRequest> callback);
	public static native void fchmod(int fd, int mode, int priority, AioCallback<AioRequest> callback);
	public static native void fchown(int fd, int uid, int gid, int priority, AioCallback<AioRequest> callback);
	public static native void dup2(int fd, int fd2, int priority, AioCallback<AioRequest> callback);
	public static native void sendfile(int outFd, int inFd, int offset, int length, int priority, AioCallback<AioRequest> callback);
	public static native void open(String path, int flags, int mode, int priority, AioCallback<AioRequest.Open> callback);
	public static native void utime(String path, int mode, int priority, AioCallback<AioRequest> callback);
	public static native void truncate(String path, int offset, int priority, AioCallback<AioRequest> callback);
	public static native void chown(String path, int uid, int gid, int priority, AioCallback<AioRequest> callback);
	public static native void chmod(String path, int mode, int priority, AioCallback<AioRequest> callback);
	public static native void mkdir(String path, int mode, int priority, AioCallback<AioRequest.Mkdir> callback);
	public static native void readdir(String path, int flags, int priority, AioCallback<AioRequest.Mkdir> callback);
	public static native void rmdir(String path, int priority, AioCallback<AioRequest> callback);
	public static native void unlink(String path, int priority, AioCallback<AioRequest> callback);
	public static native void readlink(String path, int priority, AioCallback<AioRequest> callback);
	public static native void stat(String path, int priority, AioCallback<AioRequest> callback);
	public static native void lstat(String path, int priority, AioCallback<AioRequest> callback);
	public static native void statvfs(String path, int priority, AioCallback<AioRequest> callback);
	public static native void mknod(String path, int mode, int dev, int priority, AioCallback<AioRequest> callback);
	public static native void link(String path, String newPath, int priority, AioCallback<AioRequest> callback);
	public static native void symlink(String path, String newPath, int priority, AioCallback<AioRequest> callback);
	public static native void rename(String path, String newPath, int priority, AioCallback<AioRequest> callback);

	// TODO: public static native void custom(Runnable cmd, int priority, AioCallback<AioRequest> callback);
  // eio_req *eio_custom    (eio_cb execute, int pri, eio_cb cb, void *data);

  // TODO: C helpers
  // - obtain uid/gid
  // - malloc/free
  // - mmap
  // - access raw memory
  // - helpers to generate/read file mode_t, dev_t
  // - constants for flags
}
