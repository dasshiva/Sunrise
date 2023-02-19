package syslib;

public class VMChar extends VMObj {
  public static boolean isDigit(char ch) {
    return (ch >= 48 && ch <= 57);
  }
  public static boolean isLetter(char ch) {
    return (ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122);
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
      return (char) (ch - 32);
    return ch;
  }
  
  public static boolean isWhitespace(char ch) {
    return ch == ' ';
  }
  public static char toUpperCase(char ch) {
    if (isUpperCase(ch))
      return (char) (ch + 32);
    return ch;
  }
  
  public static String toString(char ch) {
    return new String(new char[] {ch});
  }
  
}