package example;

import java.io.File;
import org.webbitserver.asyncio.AioRequest;
import org.webbitserver.asyncio.AioCallback;

import static org.webbitserver.asyncio.AsyncIO.*;
import static org.webbitserver.asyncio.NativeFunctions.*;

public class Demo {

  static int last_fd;

  public static void main(String... args) {

    System.load(new File("build/libwebbit-asyncio.jnilib").getAbsolutePath());

    /* avoid relative paths yourself(!) */
    mkdir("eio-test-dir", 0777, 0, callback("mkdir"));
    nop(0, callback("nop"));
    flush();

    stat("eio-test-dir", 0, statCallback("stat"));
    lstat("eio-test-dir", 0, statCallback("stat"));
    open("eio-test-dir/eio-test-file", READ_WRITE | CREATE, 0777, 0, openCallback("open"));
    symlink("test", "eio-test-dir/eio-symlink", 0, callback("symlink"));
    mknod("eio-test-dir/eio-fifo", S_IFIFO, 0, 0, callback("mknod"));
    flush();

//    utime("eio-test-dir", 12345.678, 23456.789, 0, callback("utime"));
//    futime(last_fd, 92345.678, 93456.789, 0, callback("futime"));
    chown("eio-test-dir", getuid(), getgid(), 0, callback("chown"));
    fchown(last_fd, getuid(), getgid(), 0, callback("fchown"));
    fchmod(last_fd, 0723, 0, callback("fchmod"));
    readdir("eio-test-dir", 0, 0, readDirCallback("readdir"));
    readdir("/nonexistant", 0, 0, readDirCallback("readdir"));
    fstat(last_fd, 0, statCallback("stat"));
    long buffer = nativeString("test\nfail\n");
    write(last_fd, buffer, strlen(buffer), 4, 0, callback("write"));
    free(buffer);
    flush();

    //read(last_fd, 0, 8, 0, PRI_DEFAULT, readCallback("read"));
   // readlink("eio-test-dir/eio-symlink", 0, callback("readlink"));
    flush();

    dup2(1, 2, PRI_DEFAULT, callback("dup")); // dup stdout to stderr
    chmod("eio-test-dir", 0765, 0, callback("chmod"));
    ftruncate(last_fd, 9, 0, callback("ftruncate"));
    fdatasync(last_fd, 0, callback("fdatasync"));
    fsync(last_fd, 0, callback("fsync"));
    sync(0, callback("sync"));
//    busy(0.5, 0, callback("busy"));
    flush();

    sendfile(1, last_fd, 4, 5, 0, callback("sendfile")); // write "test\n" to stdout
    fstat(last_fd, 0, statCallback("stat"));
    flush();

    truncate("eio-test-dir/eio-test-file", 6, 0, callback("truncate"));
    readahead(last_fd, 0, 64, 0, callback("readahead"));
    flush();

    close(last_fd, 0, callback("close"));
    //link("eio-test-dir/eio-test-file", "eio-test-dir/eio-test-file-2", 0, callback("link"));
    flush();

    rename("eio-test-dir/eio-test-file", "eio-test-dir/eio-test-file-renamed", 0, callback("rename"));
    flush();

    //unlink("eio-test-dir/eio-fifo", 0, callback("unlink"));
    //unlink("eio-test-dir/eio-symlink", 0, callback("unlink"));
    //unlink("eio-test-dir/eio-test-file-2", 0, callback("unlink"));
    //unlink("eio-test-dir/eio-test-file-renamed", 0, callback("unlink"));
    flush();

    //rmdir("eio-test-dir", 0, callback("rmdir"));
    flush();

  }

  public static AioCallback callback(final String msg) {
    return new AioCallback() {
      @Override
      public void complete(AioRequest req) {
        System.out.println("Complete: " + msg);
        if (!req.success()) {
          throw new RuntimeException("req.success() != true");
        }
      }
    };
  }

  public static AioCallback statCallback(final String msg) {
    return new AioCallback() {
      @Override
      public void complete(AioRequest req) {
        System.out.println("Complete: " + msg);
        if (!req.success()) {
          throw new RuntimeException("req.success() != true");
        }
      }
    };
  }

  public static AioCallback readCallback(final String msg) {
    return new AioCallback() {
      @Override
      public void complete(AioRequest req) {
        System.out.println("Complete: " + msg);
        if (!req.success()) {
          throw new RuntimeException("req.success() != true");
        }
      }
    };
  }

  public static AioCallback readDirCallback(final String msg) {
    return new AioCallback() {
      @Override
      public void complete(AioRequest req) {
        System.out.println("Complete: " + msg);
        if (req.getResult() < 0) {
          System.out.println(" - dir does not exist");
        } else {
          System.out.println(" - files: " + req.getResult());
        }
      }
    };
  }

  public static AioCallback openCallback(final String msg) {
    return new AioCallback() {
      @Override
      public void complete(AioRequest req) {
        System.out.println("Complete: " + msg + " (fd=" + req.getResult() + ")");
        if (!req.success()) {
          throw new RuntimeException("req.success() != true");
        }
        last_fd = req.getResult();
      }
    };
  }

  // TODO
  public static int getuid() { return 801124428; }
  public static int getgid() { return 801112577; }

}
