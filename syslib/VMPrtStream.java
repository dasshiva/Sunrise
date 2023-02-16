package syslib;

public class VMPrtStream extends VMObj {
  public native void print(String s);
  public native void println(String s);
  public void print(char c) {
	  print(new String(new char[] {c}));
  }
}
