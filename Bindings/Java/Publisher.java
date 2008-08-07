import java.math.*;

public class Publisher {
	public Publisher(){
		System.out.println("Init Publisher");
	}

	public static void main(String argv[]) {
		System.loadLibrary("famouso");

		System.out.println("Erzeuge Event");
		famouso_event_t event=new famouso_event_t();
		event.setSubject(new BigInteger("f100000000000000",16));
		event.setLen(8);
		event.setData("P/S-Java");
		System.out.println("Announce subject");
		if (famouso.famouso_announce(event.getSubject())>0)
			while(true){
				System.out.println("Publish event "+event.getSubject());
				famouso.famouso_publish(event);
				try{
					Thread.sleep(1000);
				}
				catch(InterruptedException e){
					System.out.println("Sleep Interrupted");
				}
			}
   }
 }

