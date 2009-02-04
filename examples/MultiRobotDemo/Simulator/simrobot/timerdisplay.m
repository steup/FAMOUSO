function timerdisplay(obj, event, handles)
    global scenario;
    set(handles.Time,'String',sprintf('%6.2f',etime(clock,scenario.startTime)));
end