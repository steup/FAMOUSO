using System;
using System.Threading;

   public class Publisher
   {
      public static void Main(string[] args)
      {
         Console.WriteLine("FAMOUSO C# Publisher");
	 famouso.event_t e = new famouso.event_t();
	 e.subject=0xf100000000000000;
	 e.data="P/S-C# Michael";
	 e.len=16;
         Console.WriteLine("Subject 0x{0:X}",e.subject);
	 famouso.PublisherEC pub = new famouso.PublisherEC(e.subject);
	if (pub.announce()>0)
		while(true){
			Console.WriteLine("Publish event 0x{0:X} Length={1} Data={2}",e.subject, e.len, e.data);
	 		e.data="P/S-C# Michael";
			pub.publish(e);
			Thread.Sleep(100);
	 		e.data="P/S-C+ Michael";
			pub.publish(e);
			Thread.Sleep(100);

		}
      }
   }
