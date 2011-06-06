package org.webbitserver.asyncio;

public class RequestWrapper implements AioRequest,
                                       AioRequest.Open,
                                       AioRequest.Mkdir {

  private final long ptr;

  public RequestWrapper(long ptr) {
    this.ptr = ptr;
  }

  public long ptr() {
    return ptr;
  }

  @Override // AioRequest
  public boolean success() {
    return result(ptr) > -1;
  }

  @Override // AioRequest.{Open,Mkdir}
  public int getFileDescriptor() {
    return int1(ptr);
  }

  /** result of syscall, e.g. result = read (... */
  public static native int result(long ptr);

  /** read, write, truncate, readahead, 
   *  sync_file_range: file offset, mknod: dev_t */
  public static native int offs(long ptr);

  /** read, write, readahead, sendfile, msync, 
   *  mlock, sync_file_range: length */
  public static native int size(long ptr);

  /** all applicable requests: pathname, 
   *  old name; readdir: optional eio_dirents */
  public static native long ptr1(long ptr);

  /** all applicable requests: new name or memory buffer;
   *  readdir: name strings */
  public static native long ptr2(long ptr);

  /** utime, futime: atime; busy: sleep time */
  public static native long nv1(long ptr);

  /** utime, futime: mtime */
  public static native long nv2(long ptr);

  /** EIO_xxx constant ETP */
  public static native int type(long ptr);

  /** all applicable requests: file descriptor;
   *  sendfile: output fd; open, msync, mlockall, readdir: flags */
  public static native int int1(long ptr);

  /** chown, fchown: uid; sendfile: input fd; 
   *  open, chmod, mkdir, mknod: file mode, sync_file_range: flags */
  public static native long int2(long ptr);

  /* chown, fchown: gid */
  public static native long int3(long ptr);

  /* errno value on syscall return */
  public static native int errorno(long ptr);

}

