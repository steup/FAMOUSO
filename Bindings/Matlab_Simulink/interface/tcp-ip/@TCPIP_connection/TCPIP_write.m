function a=TCPIP_write(a, output)
   if strcmp(a.interaction_type,'callback')
       if strcmp(a.tcpobj.status,'open')
           fwrite(a.tcpobj,output,'uchar');
       end
   else
       if (pnet(a.tcpobj,'status')==11)
           pnet(a.tcpobj,'write',output);
       end
   end