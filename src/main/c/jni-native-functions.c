#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <jni.h>

#define JNI_FUNC(returnType, name) \
  JNIEXPORT returnType JNICALL \
  Java_org_webbitserver_asyncio_NativeFunctions_##name

JNI_FUNC(jlong, malloc)(JNIEnv *env, jobject self, jint size) {
	return (jlong)malloc(size);
}

JNI_FUNC(void, free)(JNIEnv *env, jobject self, jlong ptr) {
	free((void*)ptr);
}

JNI_FUNC(void, memcpy)(JNIEnv *env, jobject self, jlong dest, jlong src, jint size) {
  memcpy((void*)dest, (void*)src, size);
}

JNI_FUNC(void, bzero)(JNIEnv *env, jobject self, jlong ptr, jint size) {
	bzero((void*)ptr, size);
}

JNI_FUNC(jint, strlen)(JNIEnv *env, jobject self, jlong ptr) {
	return strlen((char*)ptr);
}

JNI_FUNC(jint, strcmp)(JNIEnv *env, jobject self, jlong ptr1, jlong ptr2) {
	return strcmp((char*)ptr1, (char*)ptr2);
}

JNI_FUNC(jint, strncmp)(JNIEnv *env, jobject self, jlong ptr1, jlong ptr2, jint size) {
	return strncmp((char*)ptr1, (char*)ptr2, size);
}

JNI_FUNC(jstring, javaString)(JNIEnv *env, jobject self, jlong ptr) {
  return (*env)->NewStringUTF(env, (char*)ptr);
}

JNI_FUNC(jstring, javaStringN)(JNIEnv *env, jobject self, jlong ptr, jint size) {
  char *truncated = malloc(size + 1);
  strncpy(truncated, (char*)ptr, size);
  truncated[size] = 0;
  jstring result = (*env)->NewStringUTF(env, truncated);
  free(truncated);
  return result;
}

JNI_FUNC(jlong, nativeString)(JNIEnv *env, jobject self, jstring str) {
  jsize len = (*env)->GetStringLength(env, str);
  char *chars = malloc(len); // Callers are responsible for free()
  (*env)->GetStringUTFRegion(env, str, 0, len, chars);
  return (jlong)chars;
}

JNI_FUNC(jbyte, readByte)(JNIEnv *env, jobject self, jlong ptr) {
  char *data = (char*)ptr;
  return *data;
}

JNI_FUNC(void, writeByte)(JNIEnv *env, jobject self, jlong ptr, jbyte value) {
  char *data = (char*)ptr;
  *data = value;
}

JNI_FUNC(jchar, readChar)(JNIEnv *env, jobject self, jlong ptr) {
  char *data = (char*)ptr;
  return *data;
}

JNI_FUNC(void, writeChar)(JNIEnv *env, jobject self, jlong ptr, jchar value) {
  char *data = (char*)ptr;
  *data = value;
}
