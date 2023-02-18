package syslib;

import java.io.InputStream;
import java.io.IOException;

public class VMScan extends VMObj {
  private InputStream src;
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
      while (true) {
        char c = (char) src.read();
        if (c == '\n' || c == '\r')
          break;
        buffer += c;
      }
      return buffer;
    } catch (IOException e) {
      throw new RuntimeException("IOException happened while reading");
    }
  }
}
