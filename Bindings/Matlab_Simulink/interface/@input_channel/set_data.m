function a=set_data(a,time,length,values)
    a.messages(a.index).flag=1;
    a.messages(a.index).timestamp=time;
    a.messages(a.index).length=length;
    a.messages(a.index).values=values;
end