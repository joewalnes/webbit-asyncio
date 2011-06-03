LIBRARY=webbit-asyncio

JNI_INCLUDE=/System/Library/Java/JavaVirtualMachines/1.6.0.jdk/Contents/Headers

all: dist/$(LIBRARY).jar dist/$(LIBRARY)-tests.jar
.PHONY: all

clean:
	rm -rf build dist
.PHONY: clean

clobber: clean
	make -C libeio clean
.PHONY: clobber

libeio/libeio.la: $(wildcard libeio/*.c)
	@echo ==== Building libeio ====
	cd libeio && ./autogen.sh
	cd libeio && ./configure
	cd libeio && make
	@echo ==== Completed libeio build ====

build/.javac: $(wildcard src/main/java/org/webbitserver/asyncio/*.java)
	@rm -f build/.javac
	mkdir -p build/main/classes
	javac -g -d build/main/classes $^
	@touch build/.javac

build/lib$(LIBRARY).jnilib: src/main/c/asyncio.h src/main/c/asyncio.c
	mkdir -p build
	gcc -dynamiclib -o $@ src/main/c/asyncio.c -I$(JNI_INCLUDE) -framework JavaVM

dist/$(LIBRARY).jar: build/.javac build/lib$(LIBRARY).jnilib
	mkdir -p dist
	jar cf $@ -C build/main/classes .
	jar uf $@ -C build lib$(LIBRARY).jnilib

dist/$(LIBRARY)-tests.jar: dist/$(LIBRARY).jar $(shell find src/test/java -type f)
	mkdir -p build/test/classes
	javac -g -d build/test/classes -cp dist/$(LIBRARY).jar $(shell find src/test/java -type f)
	jar cf $@ -C build/test/classes .

