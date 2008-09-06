using System;
using System.Threading;

   public class Subscriber
   {
      public static void Main(string[] args)
      {
         Console.WriteLine("FAMOUSO C# Subscriber");
	 famouso.event_t e = new famouso.event_t();
	 e.subject=0xf100000000000000;
         Console.WriteLine("Subject 0x{0:X}",e.subject);
	famouso.SubscriberEC S = new famouso.SubscriberEC(e.subject);
	if (S.subscribe()>0)
		while(true){
			if (S.poll(e)>0) {
				Console.WriteLine("Notifyed event");
				Console.WriteLine("Subject 0x{0:X} Length={1} Data={2}",e.subject, e.len, e.data);
				e.len=0;
			} else {
				Thread.Sleep(1000);
			}
		}
      }
   }
