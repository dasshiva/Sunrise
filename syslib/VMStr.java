package syslib;

import syslib.VMObj;
public class VMStr extends VMObj {
	private char[] buffer;
	private int len; 
	public VMStr(char[] args) {
		len = args.length;
		buffer = args;
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
	  buffer = new char[1];
	}
	
	public int length() {
	  return len;
	}
	public char charAt(int index) {
	  if (index >= buffer.length) 
	    throw new StringIndexOutOfBoundsException("Index is more than or equal to String length");
	  return buffer[index];
	}
}
