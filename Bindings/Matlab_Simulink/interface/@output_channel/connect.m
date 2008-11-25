function a=connect(a)
    a.connection=TCPIP_startClient(a.connection,0);
end