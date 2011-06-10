package org.webbitserver.asyncio;

public class NativeFunctions {

  public static native long malloc(int size);
  public static native void free(long ptr);

  public static native void memcpy(long dest, long src, int size);
  public static native void bzero(long ptr, int size);

  public static native int strlen(long ptr);
  public static native int strcmp(long ptr1, long ptr2);
  public static native int strncmp(long ptr1, long ptr2, int size);

  public static native String javaString(long ptr);
  public static native String javaStringN(long ptr, int size);

  /**
   * Copies Java String to native C string.
   *
   * Callers take ownership of the memory - when you are
   * finished using the C string, you must call free() on
   * the pointer.
   */
  public static native long nativeString(String str);

  public static native byte readByte(long ptr);
  public static native void writeByte(long ptr, byte value);
  public static native char readChar(long ptr);
  public static native void writeChar(long ptr, char value);

}
