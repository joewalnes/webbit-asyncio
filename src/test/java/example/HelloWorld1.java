package example;

import java.io.File;
import org.webbitserver.asyncio.AsyncIO;
import org.webbitserver.asyncio.AioRequest;
import org.webbitserver.asyncio.AioCallback;

public class HelloWorld1 {

  public static void main(String... args) {

    System.out.println(new File("build/libwebbit-asyncio.jnilib").getAbsolutePath());
    System.load(new File("build/libwebbit-asyncio.jnilib").getAbsolutePath());

    final AsyncIO io = new AsyncIO();

    io.mkdir("/tmp/foo", new AioCallback() {
      public void complete(AioRequest r) {
        io.rmdir("/tmp/foo", null);
      }
    });

  }

}
