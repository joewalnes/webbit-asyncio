package org.webbitserver.asyncio;

public interface AioRequest {
  boolean success();

  interface Open extends AioRequest {
    int getFileDescriptor();
  }

  interface Mkdir extends AioRequest {
    int getFileDescriptor();
  }

}
