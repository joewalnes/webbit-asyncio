#include <jni.h>
#include <stdio.h>
#include <eio.h>

#define JNI_METHOD(returnType, name) JNIEXPORT returnType JNICALL Java_org_webbitserver_asyncio_AsyncIO_##name

JNI_METHOD(jint, poll)(JNIEnv *env, jobject self) {
  printf("poll\n");
	return eio_poll();
}

JNI_METHOD(jobject, mkdir)(JNIEnv *env, jobject self, jstring path, jint mode, jobject callback) {
  printf("mkdir\n");
	eio_mkdir ("eio-test-dir", mode, 0, NULL, NULL);
	return NULL;
}

JNI_METHOD(jobject, rmdir)(JNIEnv *env, jobject self, jstring path, jobject callback) {
  printf("rmdir\n");
	return NULL;
}

