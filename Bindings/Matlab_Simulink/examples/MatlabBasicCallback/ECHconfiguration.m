%% Channel definition
% Definition of the FAMOUSO channels
function [input, output, maxLenght]=FAMOUSOconfig()

%% Definition of "interesting" FAMOUSO channels
%       subject                 type       callback_function number  comment          '
input =[['F300000000000000      callback   ChannelF2.m       5       distance        ']];

output=['F100000000000000                                             result'];

%% maximum length of the data in Byte
maxLenght=8;