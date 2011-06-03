#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <jni.h>
#include <eio.h>

#define JNI_METHOD(returnType, name) JNIEXPORT returnType JNICALL Java_org_webbitserver_asyncio_AsyncIO_##name

jmethodID callback_jmethod;

JNI_METHOD(void, init)(JNIEnv *env, jobject self) {
	jclass cls = (*env)->FindClass(env, "org/webbitserver/asyncio/AioCallback");
	assert(cls != NULL);
	callback_jmethod = (*env)->GetMethodID(env, cls,
			"complete", "(Lorg/webbitserver/asyncio/AioRequest;)V");
	assert(callback_jmethod != NULL);
}

JNI_METHOD(jint, poll       )(JNIEnv *env, jobject self) { return eio_poll(); }
JNI_METHOD(jint, numRequests)(JNIEnv *env, jobject self) { return eio_nreqs(); }
JNI_METHOD(jint, numReady   )(JNIEnv *env, jobject self) { return eio_nready(); }
JNI_METHOD(jint, numPending )(JNIEnv *env, jobject self) { return eio_npending(); }
JNI_METHOD(jint, numThreads )(JNIEnv *env, jobject self) { return eio_nthreads(); }

struct java_callback {
	JNIEnv* env;
	jobject global;
};

struct java_callback* alloc_java_callback(JNIEnv *env, jobject callback) {
	struct java_callback* cb;
	cb = malloc(sizeof(cb));
	cb->env = env;
	cb->global = (*env)->NewGlobalRef(env, callback);
	return cb;
}

void free_java_callback(struct java_callback* cb) {
	JNIEnv* env = cb->env;
	(*env)->DeleteGlobalRef(env, cb->global);
	free(cb);
}

int completion_callback(eio_req *req) {
	struct java_callback* cb = (struct java_callback*)(req->data);
	JNIEnv* env = cb->env;

	(*env)->CallObjectMethod(env, cb->global, callback_jmethod, NULL);

	free_java_callback(cb);
}

JNI_METHOD(jobject, mkdir)(JNIEnv *env, jobject self, jstring path, jint mode, jobject callback) {
	const char* pathChars = (*env)->GetStringUTFChars(env, path, 0);
	eio_mkdir(pathChars, mode, 0, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, pathChars);
	return NULL;
}

JNI_METHOD(jobject, rmdir)(JNIEnv *env, jobject self, jstring path, jobject callback) {
	const char* pathChars = (*env)->GetStringUTFChars(env, path, 0);
	eio_rmdir(pathChars, 0, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, pathChars);
	return NULL;
}

