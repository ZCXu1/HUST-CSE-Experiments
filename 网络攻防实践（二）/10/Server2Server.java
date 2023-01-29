import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;


public class Server {
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
        DatagramSocket ds = new DatagramSocket(9000);
        byte[] bys = new byte[1024];
        byte[] bys2 = new byte[1024];
        DatagramPacket dp = new DatagramPacket(bys, bys.length);
        DatagramPacket dp2 = new DatagramPacket(bys2, bys2.length);
        ds.receive(dp);
        ds.receive(dp2);
        String dataString = new String(dp.getData(),0,dp.getLength());
        String dataString2 = new String(dp2.getData(), 0, dp2.getLength());
        System.out.println(dataString);
        System.out.println(dataString2);
        ds.close();
		int B = 150,p = 65537;
		int YA = Integer.parseInt(dataString2.substring(5,dataString2.length()));
		int YAB = (int)powerMod(YA,B,p);
		System.out.println(YAB);
    }
}
