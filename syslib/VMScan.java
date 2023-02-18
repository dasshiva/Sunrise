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
  
  public String nextLine() {
    try {
      buffer = new String();
      while (true) {
        char c = (char) src.read();
        if (c == '\n' || c == '\r')
          break;
        buffer += c;
      }
      index = 0;
      String ret = new String(buffer);
      buffer += ' ';
      return ret;
    } catch (IOException e) {
      throw new RuntimeException("IOException happened while reading");
    }
  }
  
  public int nextInt() {
    String token = new String();
    for (;;) {
      if (index < buffer.length()) {
        char c = buffer.charAt(index);
        if (buffer.charAt(index++) == ' ' && token.length() != 0) 
          break;
        index++;
        token += c;
      }
      else 
        nextLine();
    }
    return 0;
  }
}
