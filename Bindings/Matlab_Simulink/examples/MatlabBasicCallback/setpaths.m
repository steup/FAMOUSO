[str,maxsize] = computer;
oldpath=cd;
if ispc 
    cd ..\..\
end
if isunix
    cd ../../
end
addpath(genpath(pwd));
eval(sprintf('cd ''%s''',oldpath));	