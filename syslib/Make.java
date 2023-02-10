package syslib;
import java.io.OutputStream;
import java.io.DataOutputStream;
import java.io.FileOutputStream;

public class Make
{
    public static void main(String[] args) throws Exception {
        DataOutputStream dataOutputStream = new DataOutputStream(new FileOutputStream(args[0]));
        dataOutputStream.writeInt(0xCAFEBABE); // magic
        dataOutputStream.writeShort(0); // minor version 
        dataOutputStream.writeShort(61); // major version (Java 17)
        dataOutputStream.writeShort(6); // Constant Pool Length
        String s = "<init>"; // Element 1 - String <init>
        dataOutputStream.writeByte(1);
        dataOutputStream.writeShort(s.length());
        char[] charArray = s.toCharArray();
        for (int length = charArray.length, i = 0; i < length; ++i) {
            dataOutputStream.writeByte((byte)charArray[i]);
        }
        String s2 = "()V"; // Element 2 - String ()V
        dataOutputStream.writeByte(1);
        dataOutputStream.writeShort(s2.length());
        char[] charArray2 = s2.toCharArray();
        for (int length2 = charArray2.length, j = 0; j < length2; ++j) {
            dataOutputStream.writeByte((byte)charArray2[j]);
        }
        final String s3 = "syslib/VMObj"; // Element 3 - String syslib/VMObj
        dataOutputStream.writeByte(1);
        dataOutputStream.writeShort(s3.length());
        char[] charArray3 = s3.toCharArray();
        for (int length3 = charArray3.length, k = 0; k < length3; ++k) {
            dataOutputStream.writeByte((byte)charArray3[k]);
        }
        String s4 = "Code"; // Element 4 - String Code
        dataOutputStream.writeByte(1);
        dataOutputStream.writeShort(s4.length());
        char[] charArray4 = s4.toCharArray();
        for (int length4 = charArray4.length, l = 0; l < length4; ++l) {
            dataOutputStream.writeByte((byte)charArray4[l]);
        }
        dataOutputStream.writeByte(7); // Element 5 - Class 
        dataOutputStream.writeShort(3); 
        dataOutputStream.writeShort(0x0001); // Class access flags 
        dataOutputStream.writeShort(5); // This class 
        dataOutputStream.writeShort(0); // Super class
        dataOutputStream.writeShort(0); // No interfaces
        dataOutputStream.writeShort(0); // No fields
        dataOutputStream.writeShort(1); // One method 
        dataOutputStream.writeShort(0x0001); // Method access flags
        dataOutputStream.writeShort(1); // Method name
        dataOutputStream.writeShort(2); // Method descriptor
        dataOutputStream.writeShort(1); // No: attributes 
        dataOutputStream.writeShort(4); // Code attribute
        dataOutputStream.writeInt(13); // Attribute size
        dataOutputStream.writeShort(0); // Code stack
        dataOutputStream.writeShort(1); // Code locals
        dataOutputStream.writeInt(1);  // Code length
        dataOutputStream.writeByte(177); // Code exec
        dataOutputStream.writeShort(0); // Code exception table length
        dataOutputStream.writeShort(0); // Code attributes
        dataOutputStream.writeShort(0); // File attributes
    }
}
