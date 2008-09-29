function a=connect(a)
    a.connection=TCPIP_startClient(a.connection);
end