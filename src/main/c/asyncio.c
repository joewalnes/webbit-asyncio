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

// Most of the remaining code is boilerplate, wiring Java native
// calls to their C counterparts. I considered code-generating these,
// but there were enough subtle differences that it didn't seem worthwhile.
// -jw

JNI_API(void, nop)(JNIEnv *env, jobject self, jint priority, jobject callback) {
	eio_nop(priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, busy)(JNIEnv *env, jobject self, jint delay, jint priority, jobject callback) {
	eio_busy(delay, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, sync)(JNIEnv *env, jobject self, jint priority, jobject callback) {
	eio_sync(priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, fsync)(JNIEnv *env, jobject self, jint fd, jint priority, jobject callback) {
	eio_fsync(fd, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, fdatasync)(JNIEnv *env, jobject self, jint fd, jint priority, jobject callback) {
	eio_fdatasync(fd, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, msync)(JNIEnv *env, jobject self, jlong address, jint length, jint flags, jint priority, jobject callback) {
	eio_msync((void*)address, length, flags, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, mtouch)(JNIEnv *env, jobject self, jlong address, jint length, jint flags, jint priority, jobject callback) {
	eio_mtouch((void*)address, length, flags, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, mlock)(JNIEnv *env, jobject self, jlong address, jint length, jint priority, jobject callback) {
	eio_mlock((void*)address, length, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, mlockall)(JNIEnv *env, jobject self, jint flags, jint priority, jobject callback) {
	eio_mlockall(flags, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, sync_file_range)(JNIEnv *env, jobject self, jint offset, jint nbytes, jint flags, jint priority, jobject callback) {
	eio_sync_file_range(flags, offset, nbytes, flags, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, close)(JNIEnv *env, jobject self, jint fd, jint priority, jobject callback) {
	eio_close(fd, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, readahead)(JNIEnv *env, jobject self, jint fd, jint offset, jint length, jint priority, jobject callback) {
	eio_readahead(fd, offset, length, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, read)(JNIEnv *env, jobject self, jint fd, jlong buffer, jint length, jint offset, jint priority, jobject callback) {
	eio_read(fd, (void*)buffer, length, offset, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, write)(JNIEnv *env, jobject self, jint fd, jlong buffer, jint length, jint offset, jint priority, jobject callback) {
	eio_write(fd, (void*)buffer, length, offset, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, fstat)(JNIEnv *env, jobject self, jint fd, jint priority, jobject callback) {
	eio_fstat(fd, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, fstatvfs)(JNIEnv *env, jobject self, jint fd, jint priority, jobject callback) {
	eio_fstatvfs(fd, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, futime)(JNIEnv *env, jobject self, jint fd, jlong atime, jlong mtime, jint priority, jobject callback) {
	eio_futime(fd, atime, mtime, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, ftruncate)(JNIEnv *env, jobject self, jint fd, jint offset, jint priority, jobject callback) {
	eio_ftruncate(fd, offset, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, fchmod)(JNIEnv *env, jobject self, jint fd, jint mode, jint priority, jobject callback) {
	eio_fchmod(fd, mode, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, fchown)(JNIEnv *env, jobject self, jint fd, jint uid, jint gid, jint priority, jobject callback) {
	eio_fchown(fd, uid, gid, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, dup2)(JNIEnv *env, jobject self, jint fd, jint fd2, jint priority, jobject callback) {
	eio_dup2(fd, fd2, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, sendfile)(JNIEnv *env, jobject self, jint out_fd, jint in_fd, jint offset, jint length, jint priority, jobject callback) {
	eio_sendfile(out_fd, in_fd, offset, length, priority, completion_callback, alloc_java_callback(env, callback));
}

JNI_API(void, open)(JNIEnv *env, jobject self, jstring path, jint flags, jint mode, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_open(path_chars, flags, mode, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, utime)(JNIEnv *env, jobject self, jstring path, jlong atime, jlong mtime, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_utime(path_chars, atime, mtime, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, truncate)(JNIEnv *env, jobject self, jstring path, jint offset, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_truncate(path_chars, offset, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, chown)(JNIEnv *env, jobject self, jstring path, jint uid, jint gid, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_chown(path_chars, uid, gid, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, chmod)(JNIEnv *env, jobject self, jstring path, jint mode, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_chmod(path_chars, mode, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, mkdir)(JNIEnv *env, jobject self, jstring path, jint mode, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_mkdir(path_chars, mode, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, readdir)(JNIEnv *env, jobject self, jstring path, jint flags, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_readdir(path_chars, flags, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, rmdir)(JNIEnv *env, jobject self, jstring path, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_rmdir(path_chars, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, unlink)(JNIEnv *env, jobject self, jstring path, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_unlink(path_chars, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, readlink)(JNIEnv *env, jobject self, jstring path, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_readlink(path_chars, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, stat)(JNIEnv *env, jobject self, jstring path, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_stat(path_chars, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, lstat)(JNIEnv *env, jobject self, jstring path, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_lstat(path_chars, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, statvfs)(JNIEnv *env, jobject self, jstring path, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_statvfs(path_chars, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, mknod)(JNIEnv *env, jobject self, jstring path, jint mode, jint dev, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_mknod(path_chars, mode, dev, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, link)(JNIEnv *env, jobject self, jstring path, jstring new_path, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	const char* new_path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_link(path_chars, new_path_chars, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, new_path, new_path_chars);
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, symlink)(JNIEnv *env, jobject self, jstring path, jstring new_path, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	const char* new_path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_symlink(path_chars, new_path_chars, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, new_path, new_path_chars);
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

JNI_API(void, rename)(JNIEnv *env, jobject self, jstring path, jstring new_path, jint priority, jobject callback) {
	const char* path_chars = (*env)->GetStringUTFChars(env, path, 0);
	const char* new_path_chars = (*env)->GetStringUTFChars(env, path, 0);
	eio_rename(path_chars, new_path_chars, priority, completion_callback, alloc_java_callback(env, callback));
	(*env)->ReleaseStringUTFChars(env, new_path, new_path_chars);
	(*env)->ReleaseStringUTFChars(env, path, path_chars);
}

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
