function display(a)
% DISPLAY(a) Display an input object
    disp('------------------------------------------------------')
    disp(['Input Class  ' a.comment])
    disp('------------------------------------------------------')
    output=sprintf('Subject:          %s\nNumber:           %d\nCallbackFunction: %s\nCurrentIndex      %d',...
    a.subject, a.number, a.function_name, a.index);
    disp(output);
    display(a.connection);
    disp('------------------------------------------------------')   
    
    output=sprintf('index\tflag\ttimestamp\tlength\tvalues\n');
    for i=1:a.number
        output=[output sprintf('%d\t\t%d\t\t%1.4f\t\t%d\t\t%s\n',...
            i, a.messages(i).flag, a.messages(i).timestamp, ...
            a.messages(i).length, a.messages(i).values)];
    end
    disp(output)
    disp(' ');