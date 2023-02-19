package syslib;

import java.io.InputStream;
import java.io.IOException;

public class VMScan extends VMObj {
  private InputStream src;
  private int index;
  private String buffer;
  public VMScan(InputStream src) {
    if (src != null)
      this.src = src;
    else
      throw new NullPointerException("Source to read input is null");
    buffer = new String();
  }
  
  private void getLine() {
    try {
      index = 0;
      buffer = new String();
      while (true) {
        char c = (char) src.read();
        if (c == '\n' || c == '\r')
          break;
        buffer += c;
      }
    } catch (IOException e) {
      throw new RuntimeException("IOException happened while reading");
    }
  }
  
  public String nextLine() {
    getLine();
    index = buffer.length();
    return new String(buffer);
  }
  
  public String next() {
    String token = new String();
    if (index == buffer.length())
      getLine();
    for (; index < buffer.length(); index++) {
      char c = buffer.charAt(index);
      if (c == ' ') 
        break;
      token += c;
    }
    return token;
  }
  
  public int nextInt() {
    return Integer.parseInt(next());
  }
}
