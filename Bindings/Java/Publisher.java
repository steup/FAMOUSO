import java.math.*;

public class Publisher {
	public Publisher(){
		System.out.println("Init Publisher");
	}

	public static void main(String argv[]) {
		System.loadLibrary("famouso");

		System.out.println("Erzeuge Event");
		event_t event=new event_t();
		event.setSubject(new BigInteger("f100000000000000",16));
		event.setLen(16);
		event.setData("P/S-Java Michael");
		System.out.println("Announce subject");
		PublisherEC p = new PublisherEC(event.getSubject());
		if (p.announce()>0)
			while(true){
				System.out.println("Publish event "+event.getSubject());
				p.publish(event);
				try{
					Thread.sleep(1000);
				}
				catch(InterruptedException e){
					System.out.println("Sleep Interrupted");
				}
			}
   }
 }

