#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <poll.h>
#include <jni.h>
#include <eio.h>

#define JNI_METHOD(returnType, name) JNIEXPORT returnType JNICALL Java_org_webbitserver_asyncio_AsyncIO_##name

jmethodID callback_jmethod;
struct pollfd pfd;
int pipe_out, pipe_in;

void want_poll(void) {
	char dummy;
	write(pipe_in, &dummy, 1);
}

void done_poll(void) {
	char dummy;
	read(pipe_out, &dummy, 1);
}

JNI_METHOD(void, init)(JNIEnv *env, jobject self) {
	jclass cls = (*env)->FindClass(env, "org/webbitserver/asyncio/AioCallback");
	assert(cls != NULL);
	callback_jmethod = (*env)->GetMethodID(env, cls,
			"complete", "(Lorg/webbitserver/asyncio/AioRequest;)V");
	assert(callback_jmethod != NULL);

  int pipe_args[2];
	assert(pipe(pipe_args) == 0);
	pipe_out = pipe_args[0];
	pipe_in = pipe_args[1];
	pfd.fd = pipe_out;
	pfd.events = POLLIN;

	assert(eio_init(want_poll, done_poll) == 0);
}

JNI_METHOD(jint, poll)(JNIEnv *env, jobject self) { return eio_poll(); } 
JNI_METHOD(void, block)(JNIEnv *env, jobject self) { poll(&pfd, 1, -1); }
JNI_METHOD(jint, wakeUpFileDescriptor)(JNIEnv *env, jobject self) { return pipe_out; }

JNI_METHOD(jint, numRequests)(JNIEnv *env, jobject self) { return eio_nreqs(); }
JNI_METHOD(jint, numReady   )(JNIEnv *env, jobject self) { return eio_nready(); }
JNI_METHOD(jint, numPending )(JNIEnv *env, jobject self) { return eio_npending(); }
JNI_METHOD(jint, numThreads )(JNIEnv *env, jobject self) { return eio_nthreads(); }

struct java_callback {
	JNIEnv* env;
	jobject global;
};

struct java_callback* alloc_java_callback(JNIEnv *env, jobject callback) {
	if (callback == NULL) {
		return NULL;
	}
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
	if (cb == NULL) {
		return;
	}

	JNIEnv* env = cb->env;

	(*env)->CallObjectMethod(env, cb->global, callback_jmethod, NULL);

	free_java_callback(cb);
}

JNI_METHOD(jobject, mkdir)(JNIEnv *env, jobject self, jstring path, jint mode, int priority, jobject callback) {
	const char* pathChars = (*env)->GetStringUTFChars(env, path, 0);
	eio_mkdir(pathChars, mode, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, pathChars);
	return NULL;
}

JNI_METHOD(jobject, rmdir)(JNIEnv *env, jobject self, jstring path, int priority, jobject callback) {
	const char* pathChars = (*env)->GetStringUTFChars(env, path, 0);
	eio_rmdir(pathChars, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, pathChars);
	return NULL;
}

