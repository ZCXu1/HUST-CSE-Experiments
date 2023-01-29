import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;


public class Client {
    private static long powerMod(long a,long power,long p){
        long res = 1;
        while (power > 0) {
            if (power % 2 == 1) {
                power -= 1;
                res = res * a % p;
                a = a * a % p;
                power = power >> 1;
            } else {
                power = power >> 1;
                a = a * a % p;
            }
        }
        return res;
    }
    public static void main(String[] args) throws IOException {

        int B = 150,p = 65537, g = 3;
        int YB = (int)powerMod(g,B,p);
        DatagramSocket ds = new DatagramSocket();
        byte[] bys = ("Hello 192.168.60.2 using port 9001 ===> g = "+g+" p = "+p).getBytes();
        byte[] bys2 = ("YB = "+YB).getBytes();
        DatagramPacket dp = new DatagramPacket(bys,bys.length, InetAddress.getByName("192.168.60.2"),9001);
        DatagramPacket dp2 = new DatagramPacket(bys2,bys2.length, InetAddress.getByName("192.168.60.2"),9001);
        ds.send(dp);
        ds.send(dp2);
        ds.close();

    }
}