[str,maxsize] = computer;
oldpath=cd;

%% Add the FAMOUSO interface pathes
switch str
	case 'PCWIN'
		cd ..\..\..\..\Bindings\Matlab_Simulink\interface
 	case 'PCWIN64'
		cd ..\..\..\..\Bindings\Matlab_Simulink\interface
	case 'GLNX86'
		cd ../../../../Bindings/Matlab_Simulink/interface
end
addpath(genpath(pwd));
eval(sprintf('cd ''%s''',oldpath));	
