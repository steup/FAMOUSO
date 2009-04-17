import java.math.*;
import famouso.*;

public class Subscriber {
    public static void main(String argv[]) {
        System.loadLibrary("famouso");

        System.out.println("Erzeuge Event");
        event_t event = new event_t();
        event.setSubject(new BigInteger("f100000000000000", 16));
        System.out.println("Subscribe subject");
        SubscriberEC s = new SubscriberEC(event.getSubject());
        if (s.subscribe() > 0)
            while (true) {
                if (s.poll(event) > 0) {
                    System.out.println("Notifyed event");
                    System.out.println("Subject " + event.getSubject() + " Laenge: "
                                       + event.getLen() + " Daten: " + new String(event.getData()));
                } else {
                    try {
                        Thread.sleep(100);
                    } catch (InterruptedException e) {
                        System.out.println("Sleep Interrupted");
                    }
                }
            }
    }
}

