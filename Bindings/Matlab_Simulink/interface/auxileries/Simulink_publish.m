function placeholder=Simulink_publish(var)
    subject=var(1);
    data=var(2:length(var));
    channel_name=id2name('output',subject);
    eval(sprintf('global %s;',channel_name));
    eval(sprintf('publishing(%s,data);',channel_name));
    placeholder=1;