using System;
using System.Threading;

   public class Subscriber
   {
      public static void Main(string[] args)
      {
         Console.WriteLine("FAMOUSO C# Subscriber");
	 famouso_event_t e = new famouso_event_t();
	 e.subject=0xf100000000000000;
         Console.WriteLine("Subject 0x{0:X}",e.subject);
	if (famouso.famouso_subscribe(e.subject)>0)
		while(true){
			famouso.famouso_poll(e);
			if (e.len>0) {
				Console.WriteLine("Notifyed event");
				Console.WriteLine("Subject 0x{0:X} Length={1} Data={2}",e.subject, e.len, e.data);
				e.len=0;
			} else {
				Thread.Sleep(1000);
			}
		}
      }
   }
