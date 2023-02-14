package web;

import java.io.*;
import java.net.*;
import java.util.*;

class Worker extends Thread implements HttpConstants {

    /* Socket to client we're handling */
    private Socket socket;
    /* Server the worker works for */
    CollatzServer server;

    Worker(CollatzServer server, String name) {
        super(name);
        this.server = server;
        this.start();
    }

    synchronized void setSocket(Socket socket) {
        this.socket = socket;
        notify();
    }

    public synchronized void run() {

        server.workerHasStarted();

        while (true) {
            /* nothing to do */
            try {
                wait();
            } catch (InterruptedException e) {
                /* should not happen */
                continue;
            }

            try {
                handleClient();
            } catch (Exception e) {
                e.printStackTrace();
            }
            /* go back in wait queue if there's fewer
             * than numHandler connections.
             */
            Vector pool = server.threads;
            synchronized (pool) {
                if (pool.size() >= server.workers) {
                    /* too many threads, exit this one */
                    return;
                } else {
                    pool.addElement(this);
                }
            }
        }
    }

    void handleClient() throws IOException {
        DataInputStream fromClient = null;
        DataOutputStream toClient = null;

        // first get the streams
        try {
            fromClient = new DataInputStream(socket.getInputStream());
            toClient = new DataOutputStream(socket.getOutputStream());
        } catch (IOException e) {
            System.err.println("Error opening network streams");
            return;
        }
        
        // get integer from client
        int number = fromClient.readInt();

        System.out.println("Received: " + number);

        // find the cycles
        int cycles = collatz(number);

        // return cycles back to client
        toClient.writeInt(cycles);

        System.out.println("Sending Back: " + cycles);

        toClient.flush();
    }

    public int collatz(int number) {
        int numCyles  = 0;

        // keep going until 1
        while(number != 1) {
            // if even - halve it 
            if (number % 2 == 0) {
                number /= 2;
            }
            else { // Odd - 3n + 1 
                number = (3 * number) + 1;
            }

            numCyles++;
        }

        return numCyles;
    }
}

