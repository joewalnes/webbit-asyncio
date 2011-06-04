Webbit AsyncIO
==============

Event based asynchronous Java IO library - built on [libeio](http://software.schmorp.de/pkg/libeio.html)

__ This is a work in progress - not even remotely functional yet __

Why?

Webbit is inspired by [NodeJS](http://nodejs.org/) - a single event loop
of non-blocking calls. It makes it very easy to build scalable servers
that deal with concurrent users, without having to worry about the woes
of locking and synchronization. The model works.

However, building non-blocking applications in Java is hard as the core
libraries typically block (encouraging multithreaded shared-memory
approaches). In particular IO is painful - you either have to create
your own background worker threads and coordinate events, or use the
obscure java.nio non-blocking features, which are, well, frankly
horrendous. This library takes the former approach but uses the
(excellent) libeio library to do the hardwork.

libeio is a POSIXy library - this will perform well on Linux, OSX, Solaris,
etc but will suck on Windows. Future work may provide a Windows
alternative using IO Completion Ports (or Ryan Dahl's LibUV, when it's
ready). libeio has been battle tested by NodeJS - it's certainly ready.

Under the hood, the C libeio library shall create a pool of worker
threads and offload the IO tasks to those, invoking callbacks when done.

The API will look something like this:

    AIO aio = new AIO(executor); // pass in java.lang.concurrent.Executor
                                 // that all result callbacks will be
                                 // executed on.
    
    aio.open("/some/file", AIO.READ, new FileCallback() { // doesn't block
      public void complete(AioFile file) {
        file.readLines(new LineCallback() { // this doesn't block either
          public void processLine(String line) {
            // do something
          }
        });
      }
    });
    
    executor.runLoop(); // process all IO tasks.
                        // alternatively, if you have your own event loop
                        // you can process events on the queue one at a
                        // time.

Yeah, it's a bit verbose in Java, but it works. Alternative JVM languages (such as Clojure, Scala, Groovy, JRuby, Jython, etc) can condense the code. With Java 8 lambdas, the above code will look like this:


    AIO aio = new AIO(executor); 
    aio.open("/some/file", AIO.READ, #{file -> file.readLines(#{line -> /* do something */})});
    executor.runLoop();
