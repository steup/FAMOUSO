[str,maxsize] = computer;
oldpath=cd;

%% Add the simulation pathes
% switch str
% 	case 'PCWIN'
% 		cd ..\
%     case 'PCWIN64'
%         cd ..\
% 	case 'GLNX86'
% 		cd ../
% end
addpath(genpath(pwd));
% eval(sprintf('cd ''%s''',oldpath));
%% Add the FAMOUSO interface pathes
switch str
	case 'PCWIN'
		cd ..\..\..\Bindings\Matlab_Simulink\interface
 	case 'PCWIN64'
		cd ..\..\..\Bindings\Matlab_Simulink\interface
	case 'GLNX86'
		cd ../../../Bindings/Matlab_Simulink/interface
end
addpath(genpath(pwd));
eval(sprintf('cd ''%s''',oldpath));	
