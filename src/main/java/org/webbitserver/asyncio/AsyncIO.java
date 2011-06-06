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

  // eio_req *eio_nop       (int pri, eio_cb cb, void *data); /* does nothing except go through the whole process */
  // eio_req *eio_busy      (eio_tstamp delay, int pri, eio_cb cb, void *data); /* ties a thread for this long, simulating busyness */
  // eio_req *eio_sync      (int pri, eio_cb cb, void *data);
  // eio_req *eio_fsync     (int fd, int pri, eio_cb cb, void *data);
  // eio_req *eio_fdatasync (int fd, int pri, eio_cb cb, void *data);
  // eio_req *eio_msync     (void *addr, size_t length, int flags, int pri, eio_cb cb, void *data);
  // eio_req *eio_mtouch    (void *addr, size_t length, int flags, int pri, eio_cb cb, void *data);
  // eio_req *eio_mlock     (void *addr, size_t length, int pri, eio_cb cb, void *data);
  // eio_req *eio_mlockall  (int flags, int pri, eio_cb cb, void *data);
  // eio_req *eio_sync_file_range (int fd, off_t offset, size_t nbytes, unsigned int flags, int pri, eio_cb cb, void *data);
	public static native void close(int fd, int priority, AioCallback<AioRequest> callback);
  // eio_req *eio_readahead (int fd, off_t offset, size_t length, int pri, eio_cb cb, void *data);
  // eio_req *eio_read      (int fd, void *buf, size_t length, off_t offset, int pri, eio_cb cb, void *data);
  // eio_req *eio_write     (int fd, void *buf, size_t length, off_t offset, int pri, eio_cb cb, void *data);
  // eio_req *eio_fstat     (int fd, int pri, eio_cb cb, void *data); /* stat buffer=ptr2 allocated dynamically */
  // eio_req *eio_fstatvfs  (int fd, int pri, eio_cb cb, void *data); /* stat buffer=ptr2 allocated dynamically */
  // eio_req *eio_futime    (int fd, eio_tstamp atime, eio_tstamp mtime, int pri, eio_cb cb, void *data);
  // eio_req *eio_ftruncate (int fd, off_t offset, int pri, eio_cb cb, void *data);
  // eio_req *eio_fchmod    (int fd, mode_t mode, int pri, eio_cb cb, void *data);
  // eio_req *eio_fchown    (int fd, uid_t uid, gid_t gid, int pri, eio_cb cb, void *data);
  // eio_req *eio_dup2      (int fd, int fd2, int pri, eio_cb cb, void *data);
  // eio_req *eio_sendfile  (int out_fd, int in_fd, off_t in_offset, size_t length, int pri, eio_cb cb, void *data);
	public static native void open(String path, int flags, int mode, int priority, AioCallback<AioRequest.Open> callback);
  // eio_req *eio_utime     (const char *path, eio_tstamp atime, eio_tstamp mtime, int pri, eio_cb cb, void *data);
  // eio_req *eio_truncate  (const char *path, off_t offset, int pri, eio_cb cb, void *data);
  // eio_req *eio_chown     (const char *path, uid_t uid, gid_t gid, int pri, eio_cb cb, void *data);
  // eio_req *eio_chmod     (const char *path, mode_t mode, int pri, eio_cb cb, void *data);
	public static native void mkdir(String path, int mode, int priority, AioCallback<AioRequest.Mkdir> callback);
  // eio_req *eio_readdir   (const char *path, int flags, int pri, eio_cb cb, void *data); /* result=ptr2 allocated dynamically */
	public static native void rmdir(String path, int priority, AioCallback<AioRequest> callback);
	public static native void unlink(String path, int priority, AioCallback<AioRequest> callback);
  // eio_req *eio_readlink  (const char *path, int pri, eio_cb cb, void *data); /* result=ptr2 allocated dynamically */
  // eio_req *eio_stat      (const char *path, int pri, eio_cb cb, void *data); /* stat buffer=ptr2 allocated dynamically */
  // eio_req *eio_lstat     (const char *path, int pri, eio_cb cb, void *data); /* stat buffer=ptr2 allocated dynamically */
  // eio_req *eio_statvfs   (const char *path, int pri, eio_cb cb, void *data); /* stat buffer=ptr2 allocated dynamically */
  // eio_req *eio_mknod     (const char *path, mode_t mode, dev_t dev, int pri, eio_cb cb, void *data);
  // eio_req *eio_link      (const char *path, const char *new_path, int pri, eio_cb cb, void *data);
  // eio_req *eio_symlink   (const char *path, const char *new_path, int pri, eio_cb cb, void *data);
  // eio_req *eio_rename    (const char *path, const char *new_path, int pri, eio_cb cb, void *data);
  // eio_req *eio_custom    (eio_cb execute, int pri, eio_cb cb, void *data);

}
