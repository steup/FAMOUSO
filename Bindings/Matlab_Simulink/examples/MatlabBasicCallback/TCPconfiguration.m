%% Settings for TCP/IP configuration TCP/IP definitions
function [TCPIPsettings]=TCPconfiguration()

disp('Loading communication settings ...')

TCPIPsettings.host='127.0.0.1';
TCPIPsettings.port=5005;

TCPIPsettings.message_length=21;