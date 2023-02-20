package syslib;

import syslib.VMObj;
public class VMStr extends VMObj {
	private final char[] buffer;
	private final int len; 
	public VMStr(char[] args) {
	  len = args.length;
	  buffer = new char[len];
	  for (int i = 0; i < len; i++) {
	    buffer[i] = args[i];
	  }
	}
	public VMStr(String rhs) {
	  len = rhs.length();
	  buffer = new char[len];
	  for (int i = 0; i < len; i++) {
	    buffer[i] = rhs.charAt(i);
	  }
	}
	public VMStr() {
	  len = 0;
	  buffer = new char[0];
	}
	
	public int length() {
	  return len;
	}
	public char charAt(int index) {
	  if (index >= buffer.length) 
	    throw new StringIndexOutOfBoundsException("Index is more than or equal to String length");
	  return buffer[index];
	}
	public char[] toCharArray() {
	  char[] ret = new char[buffer.length];
	  for (int i = 0; i < ret.length; i++) {
	    ret[i] = buffer[i];
	  }
	  return ret;
	}
}
