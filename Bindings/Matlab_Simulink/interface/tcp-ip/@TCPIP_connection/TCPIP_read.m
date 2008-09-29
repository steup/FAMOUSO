function input=TCPIP_read(a)
   if strcmp(a.interaction_type,'callback')
       if strcmp(a.tcpobj.status,'open')
           aux=get(a.tcpobj, 'BytesAvailable');
           % with this construction multiple callback function calls can be
           % managed - I hope so
           if aux>0
               input = fread(a.tcpobj,aux,'uint8');
           end
       end
   else
       if (pnet(a.tcpobj,'status')==11)
           input=pnet(a.tcpobj,'read','noblock','uint8');
       else
           error('Connection not open for reading !')
       end
   end