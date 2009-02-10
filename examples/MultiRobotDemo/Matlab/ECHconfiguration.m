%% Channel definition
% Definition of the FAMOUSO channels
function [input, output, maxLenght]=FAMOUSOconfig()

%% Definition of "interesting" FAMOUSO channels
%       subject                 type       callback_function number  comment          '
input =['44697374616E6365       polling     -                 5         distance        '];

output=['56656C6F63697479                                             velocity'];

%% maximum length of the data in Byte
maxLenght=8;