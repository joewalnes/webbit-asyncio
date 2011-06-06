package org.webbitserver.asyncio;

public interface AioRequest {

  /**
   * Did request complete succesfully?
   */
  boolean success();

  /**
   * Result of syscall, e.g. result = read(...).
   */
  int getResult();

  /**
   * Errno value on syscall return.
   */
  int getErrorNo();

  interface Open extends AioRequest {
    int getFileDescriptor();
    int getFileMode();
  }

  interface Mkdir extends AioRequest {
    int getFileDescriptor();
    int getFileMode();
  }

}
