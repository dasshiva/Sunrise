package syslib;

public class VMStr extends VMObj {
	private char[] buffer;
	private int len; 
	public VMStr(char[] args) {
		len = args.length;
		buffer = args;
	}
	
	public VMStr() {
	  len = 0;
	  buffer = new char[1];
	}
	
	public char charAt(int index) {
	  if (index >= buffer.length) 
	    throw new StringIndexOutOfBoundsException("Index is more than or equal to String length");
	  return buffer[index];
	}
}
