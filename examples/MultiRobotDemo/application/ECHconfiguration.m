%% Channel definition
% Definition of the FAMOUSO channels
function [input, output, maxLenght]=FAMOUSOconfig()

%% Definition of "interesting" FAMOUSO channels
%       subject                 type       callback_function number  comment          '
input =['F200000000000000      polling    -                 5         velocity        '];

output=['F100000000000000                                             distance'];

%% maximum length of the data in Byte
maxLenght=8;