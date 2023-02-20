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
      buffer += ' ';
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
    if (buffer == null || index == buffer.length())
      getLine();
    for (int i = 0; i < buffer.length(); i++) {
      char c = buffer.charAt(i);
      if (c == ' ')
        break;
      token += c;
      System.out.println(token);
    }
    return token;
  }
  
  public int nextInt() {
    return Integer.parseInt(next());
  }
}
