package syslib;

public class VMEx extends VMThrow {
  private String message;
  public VMEx() {}
  public VMEx(String msg) {
    message = msg;
  }
}