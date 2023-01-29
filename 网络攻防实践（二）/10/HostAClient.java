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

        int A = 100,p = 65537, g = 3;
        int YA = (int)powerMod(g,A,p);
        DatagramSocket ds = new DatagramSocket();
        byte[] bys = ("Hello 10.0.2.7 using port 9000 ===> g = "+g+" p = "+p).getBytes();
        byte[] bys2 = ("YA = "+YA).getBytes();
        DatagramPacket dp = new DatagramPacket(bys,bys.length, InetAddress.getByName("10.0.2.7"),9000);
        DatagramPacket dp2 = new DatagramPacket(bys2,bys2.length, InetAddress.getByName("10.0.2.7"),9000);
        ds.send(dp);
        ds.send(dp2);
        ds.close();

    }
}