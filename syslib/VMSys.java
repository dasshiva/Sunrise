package syslib;

public class VMSys extends VMObj {
  public static VMPrtStream out;
  public native static void exit(int status);
}