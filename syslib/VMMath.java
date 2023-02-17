package syslib;

public class VMMath extends VMObj {
  public static int min(int a, int b) {
    return (a < b) ? a : b;
  }
  
  public static float min(float a, float b) {
    return (a < b) ? a : b;
 }
 
  public static double min(double a, double b) {
    return (a < b) ? a : b;
  }
  
  public static long min(long a, long b) {
    return (a < b) ? a : b;
  }
}