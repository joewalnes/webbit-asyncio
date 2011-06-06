#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <poll.h>
#include <jni.h>
#include <eio.h>

#define JNI_API(returnType, name) \
  JNIEXPORT returnType JNICALL \
  Java_org_webbitserver_asyncio_AsyncIO_##name

#define JNI_REQ(returnType, name) \
  JNIEXPORT returnType JNICALL \
  Java_org_webbitserver_asyncio_RequestWrapper_##name

jmethodID callback_jmethod;
jclass req_class;
jmethodID req_constructor;
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

JNI_API(void, init)(JNIEnv *env, jobject self) {
	jclass callback_cls = (*env)->FindClass(env, "org/webbitserver/asyncio/AioCallback");
	assert(callback_cls != NULL);
	callback_jmethod = (*env)->GetMethodID(env, callback_cls,
			"complete", "(Lorg/webbitserver/asyncio/AioRequest;)V");
	assert(callback_jmethod != NULL);

  jclass req_class_local = (*env)->FindClass(env, "org/webbitserver/asyncio/RequestWrapper");
	req_class = (*env)->NewGlobalRef(env, req_class_local);
	assert(req_class != NULL);
	req_constructor = (*env)->GetMethodID(env, req_class,
			"<init>", "(J)V");
	assert(req_constructor != NULL);

  int pipe_args[2];
	assert(pipe(pipe_args) == 0);
	pipe_out = pipe_args[0];
	pipe_in = pipe_args[1];
	pfd.fd = pipe_out;
	pfd.events = POLLIN;

	assert(eio_init(want_poll, done_poll) == 0);
}

JNI_API(jint, poll)(JNIEnv *env, jobject self) { return eio_poll(); } 
JNI_API(void, block)(JNIEnv *env, jobject self) { poll(&pfd, 1, -1); }
JNI_API(jint, wakeUpFileDescriptor)(JNIEnv *env, jobject self) { return pipe_out; }

JNI_API(jint, numRequests)(JNIEnv *env, jobject self) { return eio_nreqs(); }
JNI_API(jint, numReady   )(JNIEnv *env, jobject self) { return eio_nready(); }
JNI_API(jint, numPending )(JNIEnv *env, jobject self) { return eio_npending(); }
JNI_API(jint, numThreads )(JNIEnv *env, jobject self) { return eio_nthreads(); }

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

  jobject java_request = (*env)->NewObject(
      env, req_class, req_constructor, (jlong)req);
	(*env)->CallObjectMethod(
      env, cb->global, callback_jmethod, java_request);

	free_java_callback(cb);
}

// eio_req *eio_nop       (int pri, eio_cb cb, void *data); /* does nothing except go through the whole process */
// eio_req *eio_busy      (eio_tstamp delay, int pri, eio_cb cb, void *data); /* ties a thread for this long, simulating busyness */
// eio_req *eio_sync      (int pri, eio_cb cb, void *data);
// eio_req *eio_fsync     (int fd, int pri, eio_cb cb, void *data);
// eio_req *eio_fdatasync (int fd, int pri, eio_cb cb, void *data);
// eio_req *eio_msync     (void *addr, size_t length, int flags, int pri, eio_cb cb, void *data);
// eio_req *eio_mtouch    (void *addr, size_t length, int flags, int pri, eio_cb cb, void *data);
// eio_req *eio_mlock     (void *addr, size_t length, int pri, eio_cb cb, void *data);
// eio_req *eio_mlockall  (int flags, int pri, eio_cb cb, void *data);
// eio_req *eio_sync_file_range (int fd, off_t offset, size_t nbytes, unsigned int flags, int pri, eio_cb cb, void *data);

JNI_API(void, close)(JNIEnv *env, jobject self, jint fd, jint priority, jobject callback) {
	eio_close(fd, priority, completion_callback, alloc_java_callback(env, callback));
}

// eio_req *eio_readahead (int fd, off_t offset, size_t length, int pri, eio_cb cb, void *data);
// eio_req *eio_read      (int fd, void *buf, size_t length, off_t offset, int pri, eio_cb cb, void *data);
// eio_req *eio_write     (int fd, void *buf, size_t length, off_t offset, int pri, eio_cb cb, void *data);
// eio_req *eio_fstat     (int fd, int pri, eio_cb cb, void *data); /* stat buffer=ptr2 allocated dynamically */
// eio_req *eio_fstatvfs  (int fd, int pri, eio_cb cb, void *data); /* stat buffer=ptr2 allocated dynamically */
// eio_req *eio_futime    (int fd, eio_tstamp atime, eio_tstamp mtime, int pri, eio_cb cb, void *data);
// eio_req *eio_ftruncate (int fd, off_t offset, int pri, eio_cb cb, void *data);
// eio_req *eio_fchmod    (int fd, mode_t mode, int pri, eio_cb cb, void *data);
// eio_req *eio_fchown    (int fd, uid_t uid, gid_t gid, int pri, eio_cb cb, void *data);
// eio_req *eio_dup2      (int fd, int fd2, int pri, eio_cb cb, void *data);
// eio_req *eio_sendfile  (int out_fd, int in_fd, off_t in_offset, size_t length, int pri, eio_cb cb, void *data);

JNI_API(void, open)(JNIEnv *env, jobject self, jstring path, jint flag, jint mode, jint priority, jobject callback) {
	const char* pathChars = (*env)->GetStringUTFChars(env, path, 0);
	eio_open(pathChars, flag, mode, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, pathChars);
}

// eio_req *eio_utime     (const char *path, eio_tstamp atime, eio_tstamp mtime, int pri, eio_cb cb, void *data);
// eio_req *eio_truncate  (const char *path, off_t offset, int pri, eio_cb cb, void *data);
// eio_req *eio_chown     (const char *path, uid_t uid, gid_t gid, int pri, eio_cb cb, void *data);
// eio_req *eio_chmod     (const char *path, mode_t mode, int pri, eio_cb cb, void *data);

JNI_API(void, mkdir)(JNIEnv *env, jobject self, jstring path, jint mode, jint priority, jobject callback) {
	const char* pathChars = (*env)->GetStringUTFChars(env, path, 0);
	eio_mkdir(pathChars, mode, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, pathChars);
}

// eio_req *eio_readdir   (const char *path, int flags, int pri, eio_cb cb, void *data); /* result=ptr2 allocated dynamically */

JNI_API(void, rmdir)(JNIEnv *env, jobject self, jstring path, jint priority, jobject callback) {
	const char* pathChars = (*env)->GetStringUTFChars(env, path, 0);
	eio_rmdir(pathChars, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, pathChars);
}

JNI_API(void, unlink)(JNIEnv *env, jobject self, jstring path, jint priority, jobject callback) {
	const char* pathChars = (*env)->GetStringUTFChars(env, path, 0);
	eio_unlink(pathChars, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, pathChars);
}

// eio_req *eio_readlink  (const char *path, int pri, eio_cb cb, void *data); /* result=ptr2 allocated dynamically */
// eio_req *eio_stat      (const char *path, int pri, eio_cb cb, void *data); /* stat buffer=ptr2 allocated dynamically */
// eio_req *eio_lstat     (const char *path, int pri, eio_cb cb, void *data); /* stat buffer=ptr2 allocated dynamically */
// eio_req *eio_statvfs   (const char *path, int pri, eio_cb cb, void *data); /* stat buffer=ptr2 allocated dynamically */
// eio_req *eio_mknod     (const char *path, mode_t mode, dev_t dev, int pri, eio_cb cb, void *data);
// eio_req *eio_link      (const char *path, const char *new_path, int pri, eio_cb cb, void *data);
// eio_req *eio_symlink   (const char *path, const char *new_path, int pri, eio_cb cb, void *data);
// eio_req *eio_rename    (const char *path, const char *new_path, int pri, eio_cb cb, void *data);
// eio_req *eio_custom    (eio_cb execute, int pri, eio_cb cb, void *data);

JNI_REQ(jint, result)(JNIEnv *env, jobject self, jlong ptr) {
  eio_req* req = (eio_req*)ptr;
  return req->result;
}

JNI_REQ(jint, offs)(JNIEnv *env, jobject self, jlong ptr) {
  eio_req* req = (eio_req*)ptr;
  return req->offs;
}

JNI_REQ(jint, size)(JNIEnv *env, jobject self, jlong ptr) {
  eio_req* req = (eio_req*)ptr;
  return req->size;
}

JNI_REQ(jlong, ptr1)(JNIEnv *env, jobject self, jlong ptr) {
  eio_req* req = (eio_req*)ptr;
  return (jlong)req->ptr1;
}

JNI_REQ(jlong, ptr2)(JNIEnv *env, jobject self, jlong ptr) {
  eio_req* req = (eio_req*)ptr;
  return (jlong)req->ptr2;
}

JNI_REQ(jlong, nv1)(JNIEnv *env, jobject self, jlong ptr) {
  eio_req* req = (eio_req*)ptr;
  return req->nv1;
}

JNI_REQ(jlong, nv2)(JNIEnv *env, jobject self, jlong ptr) {
  eio_req* req = (eio_req*)ptr;
  return req->nv2;
}

JNI_REQ(jint, type)(JNIEnv *env, jobject self, jlong ptr) {
  eio_req* req = (eio_req*)ptr;
  return req->type;
}

JNI_REQ(jint, int1)(JNIEnv *env, jobject self, jlong ptr) {
  eio_req* req = (eio_req*)ptr;
  return req->int1;
}

JNI_REQ(jlong, int2)(JNIEnv *env, jobject self, jlong ptr) {
  eio_req* req = (eio_req*)ptr;
  return req->int2;
}

JNI_REQ(jlong, int3)(JNIEnv *env, jobject self, jlong ptr) {
  eio_req* req = (eio_req*)ptr;
  return req->int3;
}

JNI_REQ(jint, errorno)(JNIEnv *env, jobject self, jlong ptr) {
  eio_req* req = (eio_req*)ptr;
  return req->errorno;
}
