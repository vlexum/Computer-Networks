package collatz;
import java.net.Socket;
import java.io.*;
import java.util.Scanner;

public class CollatzClient implements CollatzConstants {
    public static void main(String[] args) {
        // connect to host
        handleServer(SERVER_ADDRESS, PORT);
    }

    // Connects to specified host and port and recieves input from 3A + 1 Server
    public static void handleServer (String ipAddress, int port) {
        // try to create socket to host, if accepted read input for time
        try (Socket socket = new Socket(ipAddress, port)) {
            DataInputStream fromServer = null;
            DataOutputStream toServer = null;
    
            // first get the streams
            try {
                fromServer = new DataInputStream(socket.getInputStream());
                toServer = new DataOutputStream(socket.getOutputStream());
            } catch (IOException e) {
                System.err.println("Error opening network streams");
                return;
            }
            
            // Get int from user
            Scanner in = new Scanner(System.in); 
            System.out.print("Enter an integer:  ");
            int numIn = in.nextInt();
            in.close();

            toServer.writeInt(numIn);

            int cycles = fromServer.readByte();

            System.out.println("Number of cycles: " + cycles);

            System.out.println();
        } catch (IOException err) {
            System.err.println("Error: " + err.getMessage());
        }
    }
}
