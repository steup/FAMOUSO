%% Channel definition
% Definition of the FAMOUSO channels
function [input, output, maxLenght]=FAMOUSOconfig()

%% Definition of "interesting" FAMOUSO channels
%       subject                 type       callback_function number  comment          '
input =['F100000000000000      polling    -                 5         velocity        '];

% input =[['F700000000000001      polling    test.m            5       distance        ']
%         ['F300000000000001      polling    -                 5       distanc2        ']
%         ['F400000000000001      polling    -                 5       distanc3        ']
%         ['F200000000000001      polling    test.m            5       velocity        ']];

output=['F200000000000000                                             distance'];

%% maximum length of the data in Byte
maxLenght=8;