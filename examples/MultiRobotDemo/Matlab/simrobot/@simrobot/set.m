function a = set(varargin)
%SET Summary of this function goes here
%   Detailed explanation goes here
    if ~isempty(varargin)
        if ~strcmp(class(varargin{1}),'simrobot')
            error('Wrong class type as input parameter !')
        else
            a=varargin{1};
        end
        if mod(length(varargin)+1,2)==0
            for i=2:2:length(varargin)
                if ~ischar(varargin{i})
                    error('Something wrong with the function call !');
                else
                    value=varargin{i+1};
                    switch varargin{i}
                        case 'patchXData'
                            if strcmp(class(a.patch),'double')
                                aux_patch=value;	% Define the patch
                            else
                                a.patch.x=value;
                            end
                        case 'patchYData'
                            if strcmp(class(a.patch),'double')
                                set(a.patch,'XData',aux_patch,'YData',value);% Define the patch
                            else
                                a.patch.y=value;
                            end
                         case 'lineXData'
                            if strcmp(class(a.line),'double')
                                aux_line=value;	% Define the patch
                            else
                                a.line.x=value;
                            end
                        case 'lineYData'
                            if strcmp(class(a.line),'double')
                                set(a.line,'XData',aux_line,'YData',value,'Color','r');
                            else
                                a.line.y=value;
                            end
                        case 'power'
                                a.power=value;
%                         case {'name','number','af','color','scale','crashed','position','heading','velocity','accel'}
%                             eval(sprintf('a.%s=value;',varargin{3}));
                        otherwise
                            error('Wrong argument for class simrobot !!!')
                    end
                end
            end
        else
            error('Wrong number of input arguments with the function call !');
        end
    else
        disp('No input arguments !')
    end
end
