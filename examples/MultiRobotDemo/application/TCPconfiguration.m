%% Settings for TCP/IP configuration TCP/IP definitions
function [TCPIPsettings]=TCPconfiguration()

disp('Loading communication settings ...')

TCPIPsettings.host='127.0.0.1';
%TCPIPsettings.host='10.68.13.74';
TCPIPsettings.host='192.168.0.122';
TCPIPsettings.port=5005;

TCPIPsettings.message_length=21;