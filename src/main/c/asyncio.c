#include "asyncio.h"

#include <stdio.h>

JNIEXPORT jobject JNICALL Java_org_webbitserver_asyncio_AsyncIO_mkdir
  (JNIEnv *env, jobject aio, jstring path, jobject callback) {
  printf("mkdir\n");
	return NULL;
}

JNIEXPORT jobject JNICALL Java_org_webbitserver_asyncio_AsyncIO_rmdir
  (JNIEnv *env, jobject aio, jstring path, jobject callback) {
  printf("rmdir\n");
}

