package syslib;

public class VMStr extends VMObj {
	private char[] buffer;
	private int len; 
	public VMStr(char[] args) {
		len = args.length;
		buffer = args;
	}
}
