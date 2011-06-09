LIBRARY=webbit-asyncio

JNI_INCLUDE=/System/Library/Java/JavaVirtualMachines/1.6.0.jdk/Contents/Headers
CLASSPATH=$(shell echo $(wildcard lib/*.jar) | sed -e 's/ /:/g')

# Function to extract Test class names from a jar. $(call extracttests,foo.jar)
extracttests = $(shell jar tf $(1) | grep 'Test.class$$' | sed -e 's|/|.|g;s|.class$$||')

all: dist/$(LIBRARY).jar test
.PHONY: all

clean:
	rm -rf build dist eio-test-dir test
.PHONY: clean

clobber: clean
	make -C libeio clean
.PHONY: clobber

libeio/eio.o: $(wildcard libeio/*.c)
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

build/lib$(LIBRARY).jnilib: $(wildcard src/main/c/*) libeio/eio.o
	mkdir -p build
	gcc -dynamiclib -o $@ \
		$(wildcard src/main/c/*.c) libeio/eio.o \
		-I$(JNI_INCLUDE) -framework JavaVM \
		-Ilibeio

dist/$(LIBRARY).jar: build/.javac build/lib$(LIBRARY).jnilib
	mkdir -p dist
	jar cf $@ -C build/main/classes .
	jar uf $@ -C build lib$(LIBRARY).jnilib

build/$(LIBRARY)-tests.jar: dist/$(LIBRARY).jar $(shell find src/test/java -type f)
	mkdir -p build/test/classes
	javac -g -d build/test/classes -cp $(CLASSPATH):dist/$(LIBRARY).jar $(shell find src/test/java -type f)
	jar cf $@ -C build/test/classes .

example: build/$(LIBRARY)-tests.jar
	java -cp dist/$(LIBRARY).jar:dist/$(LIBRARY)-tests.jar example.HelloWorld1
.PHONY: example

demo: build/$(LIBRARY)-tests.jar
	-rm -rf eio-test-dir test
	java -cp dist/$(LIBRARY).jar:build/$(LIBRARY)-tests.jar example.Demo
.PHONY: example

# Run tests, and create .tests-pass if they succeed
build/.tests-pass: build/$(LIBRARY)-tests.jar
	@rm -f $@
	java -cp $(CLASSPATH):dist/$(LIBRARY).jar:build/$(LIBRARY)-tests.jar org.junit.runner.JUnitCore $(call extracttests,build/$(LIBRARY)-tests.jar)
	@touch $@
test: build/.tests-pass
.PHONY: test
