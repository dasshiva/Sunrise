package syslib;

public class VMInt extends VMObj {
  public static int parseInt(String num) {
    System.out.println(num);
    int val = 0;
    for (int i = num.length() - 1, mul = 1; i >= 0; i--, mul *= 10) {
      char c = num.charAt(i);
      if (Character.isWhitespace(c))
        continue;
      if (!Character.isDigit(c))
        throw new RuntimeException("Cannot parse to int");
      val += mul * (c - 48);
    }
    return val;
  }
}