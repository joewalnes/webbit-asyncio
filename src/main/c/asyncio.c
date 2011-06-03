#include "asyncio.h"

#include <stdio.h>
#include <eio.h>

JNIEXPORT jint JNICALL Java_org_webbitserver_asyncio_AsyncIO_poll
  (JNIEnv *env, jobject aio) {
  printf("poll\n");
	return eio_poll();
}

JNIEXPORT jobject JNICALL Java_org_webbitserver_asyncio_AsyncIO_mkdir
  (JNIEnv *env, jobject aio, jstring path, jobject callback) {
  printf("mkdir\n");
	eio_mkdir ("eio-test-dir", 0777, 0, NULL, NULL);
	return NULL;
}

JNIEXPORT jobject JNICALL Java_org_webbitserver_asyncio_AsyncIO_rmdir
  (JNIEnv *env, jobject aio, jstring path, jobject callback) {
  printf("rmdir\n");
	return NULL;
}

