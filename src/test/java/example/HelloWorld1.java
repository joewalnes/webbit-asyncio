package example;

import java.io.File;
import org.webbitserver.asyncio.AioRequest;
import org.webbitserver.asyncio.AioCallback;

import static org.webbitserver.asyncio.AsyncIO.*;

public class HelloWorld1 {

  public static void main(String... args) {

    System.load(new File("build/libwebbit-asyncio.jnilib").getAbsolutePath());

    mkdir("/tmp/foo", 0750, 0, new AioCallback() {
      public void complete(AioRequest r) {
        rmdir("/tmp/foo", 0, new AioCallback() {
					public void complete(AioRequest r) {
						System.out.println("done");
					}
				});
      }
    });

		while(numRequests() > 0) {
			block();
			poll();
		}

  }

}
