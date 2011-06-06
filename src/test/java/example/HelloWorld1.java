package example;

import java.io.File;
import org.webbitserver.asyncio.AioRequest;
import org.webbitserver.asyncio.AioCallback;

import static org.webbitserver.asyncio.AsyncIO.*;

public class HelloWorld1 {

  public static void main(String... args) {

    System.load(new File("build/libwebbit-asyncio.jnilib").getAbsolutePath());

    mkdir("/tmp/foo", 0750, 0, new AioCallback<AioRequest.Mkdir>() {
      public void complete(AioRequest.Mkdir r) {

				open("/tmp/foo/file", WRITE | CREATE, 0700, 0, new AioCallback<AioRequest.Open>() {
					public void complete(AioRequest.Open r) {
            System.out.println("open() fd = " + r.getFileDescriptor());
            unlink("/tmp/foo/file", 0, null);

						rmdir("/tmp/foo", 0, new AioCallback<AioRequest>() {
							public void complete(AioRequest r) {
								System.out.println("done");
							}
						});

					}
				});

      }
    });

		flush();
  }

}
