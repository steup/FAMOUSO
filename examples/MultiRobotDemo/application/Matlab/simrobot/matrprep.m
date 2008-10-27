function immap=matrprep(file);
% MATRPREP prepares image (matrix) for simulation. 
%
% Usage:	simmatrix=MATRPREP(file); 
% where FILE is a string - path to the image of environment to be simulated. 
% The image has to be a 1-bit bitmap.

immap=imread(file,'bmp');
immap=~immap;
%immap=double(immap);
