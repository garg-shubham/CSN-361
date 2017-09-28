import java.io.*;
import java.net.*;
public class NetThread extends Thread {
    protected Socket sock;

    public NetThread(Socket clientSocket) {
        this.sock = clientSocket;
    }

    public void run() {
        try {
            OutputStream ostream = sock.getOutputStream(); 
            PrintWriter pwrite = new PrintWriter(ostream, true);

            InputStream istream = sock.getInputStream();
            BufferedReader receiveRead = new BufferedReader(new InputStreamReader(istream));

            String receiveMessage, sendMessage;               
          
            while(true) {
                if((receiveMessage = receiveRead.readLine()) != null) {
                   System.out.println("Received : " + receiveMessage);         
                }         
                sendMessage = receiveMessage.toUpperCase();
                pwrite.println(sendMessage);
                pwrite.flush();
            }
        } catch (Exception e){
            e.printStackTrace();
            return;
        }
    }
}