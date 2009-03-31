function a=connect(a)
    a.connection=TCPIP_endClient(a.connection);
end