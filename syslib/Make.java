package syslib;
import java.io.OutputStream;
import java.io.DataOutputStream;
import java.io.FileOutputStream;

public class Make
{
    public static void main(String[] args) throws Exception {
        DataOutputStream dataOutputStream = new DataOutputStream(new FileOutputStream(args[0]));
        dataOutputStream.writeInt(0xCAFEBABE);
        dataOutputStream.writeShort(0);
        dataOutputStream.writeShort(61);
        dataOutputStream.writeShort(6);
        String s = "<init>";
        dataOutputStream.writeByte(1);
        dataOutputStream.writeShort(s.length());
        char[] charArray = s.toCharArray();
        for (int length = charArray.length, i = 0; i < length; ++i) {
            dataOutputStream.writeByte((byte)charArray[i]);
        }
        String s2 = "()V";
        dataOutputStream.writeByte(1);
        dataOutputStream.writeShort(s2.length());
        char[] charArray2 = s2.toCharArray();
        for (int length2 = charArray2.length, j = 0; j < length2; ++j) {
            dataOutputStream.writeByte((byte)charArray2[j]);
        }
        final String s3 = "Object";
        dataOutputStream.writeByte(1);
        dataOutputStream.writeShort(s3.length());
        char[] charArray3 = s3.toCharArray();
        for (int length3 = charArray3.length, k = 0; k < length3; ++k) {
            dataOutputStream.writeByte((byte)charArray3[k]);
        }
        dataOutputStream.writeByte(1);
        String s4 = "Code";
        dataOutputStream.writeByte(1);
        dataOutputStream.writeShort(s4.length());
        char[] charArray4 = s4.toCharArray();
        for (int length4 = charArray4.length, l = 0; l < length4; ++l) {
            dataOutputStream.writeByte((byte)charArray4[l]);
        }
        dataOutputStream.writeByte(7);
        dataOutputStream.writeShort(3);
        dataOutputStream.writeShort(1);
        dataOutputStream.writeShort(5);
        dataOutputStream.writeShort(1);
        dataOutputStream.writeShort(0);
        dataOutputStream.writeShort(0);
        dataOutputStream.writeShort(1);
        dataOutputStream.writeShort(1);
        dataOutputStream.writeShort(1);
        dataOutputStream.writeShort(2);
        dataOutputStream.writeShort(1);
        dataOutputStream.writeShort(4);
        dataOutputStream.writeInt(13);
        dataOutputStream.writeShort(0);
        dataOutputStream.writeShort(0);
        dataOutputStream.writeInt(1);
        dataOutputStream.writeByte(177);
        dataOutputStream.writeShort(0);
        dataOutputStream.writeShort(0);
        dataOutputStream.writeShort(0);
    }
}
