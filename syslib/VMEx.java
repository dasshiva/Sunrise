package syslib;

public class VMEx extends VMThrow {
  private static native void throwEx(String message);
  public VMEx(String msg) {
    throwEx(msg);
  }
}