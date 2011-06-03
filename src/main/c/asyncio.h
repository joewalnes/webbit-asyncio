#include <jni.h>

JNIEXPORT jint JNICALL Java_org_webbitserver_asyncio_AsyncIO_poll
  (JNIEnv *env, jobject aio);

JNIEXPORT jobject JNICALL Java_org_webbitserver_asyncio_AsyncIO_mkdir
  (JNIEnv *env, jobject aio, jstring path, jobject callback);

JNIEXPORT jobject JNICALL Java_org_webbitserver_asyncio_AsyncIO_rmdir
  (JNIEnv *env, jobject aio, jstring path, jobject callback);

