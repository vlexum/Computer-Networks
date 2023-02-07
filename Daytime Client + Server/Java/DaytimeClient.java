import java.io.IOException;
import java.net.Socket;
import java.io.InputStream;
import java.io.InputStreamReader;

public class DaytimeClient {
    public static void main(String[] args) {
        String hostname = "time.nist.gov";
        int port = 13;

        // connect to host
        handleServer(hostname, port);
    }

    // Connects to specified host and port and recieves input from Daytime Server
    public static void handleServer (String host, int port) {
        // try to create socket to host, if accepted read input for time
        try (Socket socket = new Socket(host, port)) {
            InputStream input = socket.getInputStream();
            InputStreamReader reader = new InputStreamReader(input);
            char letter;

            // print the character until reaching the * symbol that marks end of time string
            // faster using Scanner but assingment states to stop when * is read
            while ((letter = (char) reader.read()) != '*') {
                System.out.print(letter);
            }

            System.out.println();
        } catch (IOException err) {
            System.err.println("Error: " + err.getMessage());
        }
    }
}
