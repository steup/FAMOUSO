function a=connect(a)
    a.connection=TCPIP_startClient(a.connection,1);
end