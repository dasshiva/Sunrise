package syslib;

public class VMSys extends VMObj {
  public static VMIStream in;
  public static VMPrtStream out;
  public static VMPrtStream err;
  public native static void exit(int status);
}