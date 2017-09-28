import java.io.*;
import java.net.*;
public class Server
{
	ServerSocket void main (String [] args) throws Exception
	{
		ServerSocket sersock = new ServerSocket(3000);
		System.out.println("Server Ready");
		while(true)
		{
			Socket sock = sersock.accept();
			new NetThread(sock).start();
		}
	}
}