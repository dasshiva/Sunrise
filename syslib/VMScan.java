package syslib;

import java.io.InputStream;
import java.io.IOException;

public class VMScan extends VMObj {
  private InputStream src;
  public VMScan(InputStream src) {
    if (src != null)
      this.src = src;
    else
      throw new NullPointerException("Source to read input is null");
  }
}