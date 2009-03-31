function display(a)
% DISPLAY(a) Display an input object
    disp('------------------------------------------------------')
    disp(['Output Class  ' a.comment])
    disp('------------------------------------------------------')
    output=sprintf('Subject:          %s',a.subject);
    disp(output);
    display(a.connection);