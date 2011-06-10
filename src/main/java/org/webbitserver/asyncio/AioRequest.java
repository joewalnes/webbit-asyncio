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

  /**
   * Pointer to the data structure populated
   * by the request (if applicable).
   */
  long getPointer();
}
