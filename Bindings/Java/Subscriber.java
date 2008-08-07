import java.math.*;

public class Subscriber {
	public static void main(String argv[]) {
		System.loadLibrary("famouso");

		System.out.println("Erzeuge Event");
		famouso_event_t event=new famouso_event_t();
		event.setSubject(new BigInteger("f100000000000000",16));
		System.out.println("Subscribe subject");
		if (famouso.famouso_subscribe(event.getSubject())>0)
			while(true){
				famouso.famouso_poll(event);
				if (event.getLen()>0) {
					System.out.println("Notifyed event");
					System.out.println("Subject "+event.getSubject()+" Laenge: "
										+event.getLen()+" Daten: "+event.getData());
					event.setLen(0);
				} else {
					try{
						Thread.sleep(100);
					}
					catch(InterruptedException e){
						System.out.println("Sleep Interrupted");
					}
				}
			}
   }
 }

