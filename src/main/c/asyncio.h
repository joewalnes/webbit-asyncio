#include <jni.h>

JNIEXPORT jobject JNICALL Java_org_webbitserver_asyncio_AsyncIO_mkdir
  (JNIEnv *env, jobject aio, jstring path, jobject callback);

JNIEXPORT jobject JNICALL Java_org_webbitserver_asyncio_AsyncIO_rmdir
  (JNIEnv *env, jobject aio, jstring path, jobject callback);

