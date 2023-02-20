package syslib;

public class VMStrBuild extends VMObj {
  private char[] buffer;
  private int index;
  public VMStrBuild() {
    index = 0;
    buffer = new char[20];
  }
  
  public VMStrBuild append(char c) {
    if (index + 1 == buffer.length) 
      realloc(20);
    buffer[index++] = c;
    return this;
  }
  
  public VMStrBuild append(String str) {
    if (index + str.length() >= buffer.length)
      realloc(str.length() + 20);
    char[] s = str.toCharArray();
    for (int i = 0; i < s.length; index++, i++) {
      buffer[index] = s[i];
    }
    index++;
    return this;
  }
  
  private void realloc(int sz) {
    char[] buf = new char[sz + buffer.length];
    for (int i = 0, j = 0; j < buffer.length; i++, j++) {
      buf[i] = buffer[j];
    }
    buffer = buf;
  }
  
  private String toString() {
    return new String(buffer);
  }
}