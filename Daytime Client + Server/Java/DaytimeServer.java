import java.io.DataInputStream;
import java.io.DataOutputStream;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import java.util.logging.Level;
import java.util.logging.Logger;

import java.util.Date;

public class DaytimeServer {

    private static ServerSocket serverSocket;
    private static int port;
    Socket clientSocket = null;

    public DaytimeServer(int port) {
        try {
            DaytimeServer.port = port;
            DaytimeServer.serverSocket = new ServerSocket(port);
        } catch (IOException ex) {
            Logger.getLogger(DaytimeServer.class.getName()).log(Level.SEVERE, null, ex);
            System.err.println("Error starting server on port " + port);
            System.exit(1);
        }

        DaytimeServer.port = port;
    }

    public void runServerLoop() throws IOException {

        System.out.println("Echo server started");

        while (true) {
            System.out.println("Waiting for connections on port #" + port);

            handleClient(serverSocket.accept());
        }
    }

    public void handleClient(Socket clientSocket) {

        DataInputStream fromClient = null;
        DataOutputStream toClient = null;

        int charFromClient = 0;
        int state = 0;
        boolean keepGoing = true;

        // show that we are connected to client
        System.out.println("Client connected ...");

        // first get the streams
        try {
            fromClient = new DataInputStream(clientSocket.getInputStream());
            toClient = new DataOutputStream(clientSocket.getOutputStream());
        } catch (IOException e) {
            System.err.println("Error opening network streams");
            return;
        }

        // now talk to the client
        while (keepGoing) {
            try {
                charFromClient = fromClient.readByte();
                System.out.print((char)charFromClient);
            } catch (IOException e) {
                System.err.println("Error reading character from client");
                return;
            }

            try {
                toClient.writeByte(charFromClient);
            } catch (IOException e) {
                System.err.println("Error writing character to client");
                return;            }

            if (charFromClient == 'q') {
                System.out.println("\nBailing out!");
                keepGoing = false;
            }
        }

        try {
            clientSocket.close();
        } catch (IOException e) {
            System.err.println("Error closing socket to client");
        }

    }

    public static void main(String args[]) throws Exception {
        // create instance of echo server
        // note that hardcoding the port is bad, here we do it just for simplicity reasons
        DaytimeServer echoServer = new DaytimeServer(23657);

        // fire up server loop
        echoServer.runServerLoop();
    }
}
