using System;
using System.Threading;

   public class Publisher
   {
      public static void Main(string[] args)
      {
         Console.WriteLine("FAMOUSO C# Publisher");
	 famouso_event_t e = new famouso_event_t();
	 e.subject=0xf100000000000000;
	 e.data="P/S-C#";
	 e.len=8;
         Console.WriteLine("Subject 0x{0:X}",e.subject);
	if (famouso.famouso_announce(e.subject)>0)
		while(true){
			Console.WriteLine("Publish event 0x{0:X} Length={1} Data={2}",e.subject, e.len, e.data);
			famouso.famouso_publish(e);
			Thread.Sleep(1000);
		}
      }
   }
