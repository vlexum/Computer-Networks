import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.io.InputStreamReader;

public class DaytimeClient {
    public static void main(String[] args) {
        String hostname = "time.nist.gov";
        int port = 13;

        handleServer(hostname, port);
    }

    public static void handleServer (String host, int port) {
        try (Socket socket = new Socket(host, port)) {
            InputStream input = socket.getInputStream();
            InputStreamReader reader = new InputStreamReader(input);
            char letter;

            while ((letter = (char) reader.read()) != '*') {
                System.out.print(letter);
            }
            
            System.out.println();
        } catch (IOException err) {
            System.err.println("Error: " + err.getMessage());
        }
    }
}
