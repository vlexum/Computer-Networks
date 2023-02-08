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

        System.out.println("Daytime server started");

        while (true) {
            System.out.println("Waiting for connections on port #" + port);

            // accpet the client
            Socket clientSocket = serverSocket.accept();

            // pass client socket off to handler that makes use of threads
            ClientHandler client = new ClientHandler(clientSocket);
            client.start();
        }
    } 

    static class ClientHandler extends Thread {
        private Socket clientSocket;


        public ClientHandler(Socket clientSocket) {
            this.clientSocket = clientSocket;
        }

        @Override
        public void run() {
            DataOutputStream toClient = null;;
            char onTimeMarker = '*';

            // show that we are connected to client
            System.out.println("Client connected ...");

            // first get the stream
            try {
                toClient = new DataOutputStream(clientSocket.getOutputStream());
            } catch (IOException e) {
                System.err.println("Error opening network stream");
                return;
            }

            // Get time
            Date currDate = new Date();
            String dateStr = (currDate.toString()) + onTimeMarker;
            
            // write to the client
            try {
                // send the date and time
                toClient.writeChars(dateStr);
            } catch (IOException e) {
                System.err.println("Error writing character to client");
                return;
            }
    

            // close the client socket
            try {
                clientSocket.close();
            } catch (IOException e) {
                System.err.println("Error closing socket to client");
            }


        }
    }

    public static void main(String args[]) throws Exception {
        // create instance of echo server
        // note that hardcoding the port is bad, here we do it just for simplicity reasons
        DaytimeServer server = new DaytimeServer(13);

        // fire up server loop
        server.runServerLoop();
    }
}
