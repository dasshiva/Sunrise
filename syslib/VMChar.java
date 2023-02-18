package syslib;

public class VMChar extends VMObj {
  public static boolean isDigit(char ch) {
    switch (ch) {
      case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8':
      case '9': return true;
      default: return false;
    }
  }
  public static boolean isLetter(char ch) {
    if (ch >= 65 && ch <= 90)
      return true;
    else if (ch >= 97 && ch <= 122)
      return true;
    return false;
  }
  
  public static boolean isLowerCase(char ch) {
    return (ch >= 97 && ch <= 122);
  }
  
  public static boolean isUpperCase(char ch) {
    return (ch >= 65 && ch <= 90);
  }
  
  public static boolean isLetterOrDigit(char ch) {
    return isLetter(ch) || isDigit(ch);
  }
  
  public static char toLowerCase(char ch) {
    if (isLowerCase(ch))
      return ch - 32;
    return ch;
  }
  
  public static char toUpperCase(char ch) {
    if (isUpperCase(ch))
      return ch + 32;
    return ch;
  }
  
  public static String toString(char ch) {
    return new String(new char[] {ch});
  }
  
}