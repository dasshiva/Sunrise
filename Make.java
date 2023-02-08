import java.io.*;

public class Make {
	public static void main(String[] args) throws Exception {
		var s = new DataOutputStream(new FileOutputStream("Object.class"));
		s.writeInt(0xCAFEBABE);
		s.writeShort(0x00);
		s.writeShort(61);
		s.writeShort(6);
		String init = "<init>";
		s.writeByte(1);
		s.writeShort(init.length());
		for (char c: init.toCharArray()) {
			s.writeByte((byte) c);
		}
		String desc = "()V";
		s.writeByte(1);
		s.writeShort(desc.length());
		for (char c: desc.toCharArray()) {                           
			s.writeByte((byte) c); 
		}
		String name = "Object";
		s.writeByte(1);
		s.writeShort(name.length());
		for (char c: name.toCharArray()) {
			s.writeByte((byte) c);
		}
		s.writeByte(1);
		String code = "Code";
		s.writeByte(1);
		s.writeShort(code.length());
		for (char c: code.toCharArray()) {
			s.writeByte((byte) c);
		}
		s.writeByte(7);
		s.writeShort(3);
		s.writeShort(0x0001);
		s.writeShort(5);
		s.writeShort(1);
		s.writeShort(0);
		s.writeShort(0);
		s.writeShort(1);
		s.writeShort(0x0001);
		s.writeShort(1);
		s.writeShort(2);
		s.writeShort(1);
		s.writeShort(4);
		s.writeInt(13);
		s.writeShort(0);
		s.writeShort(0);
		s.writeInt(1);
		s.writeByte(177);
		s.writeShort(0);
		s.writeShort(0);
		s.writeShort(0);
	}
}

