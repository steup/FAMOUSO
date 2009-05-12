function trigger=trigger_pool()
%% Trigger modi
% intern
trigger(1).triggerMode='timer';
trigger(1).period=0.075;
trigger(1).delay=2;
trigger(1).timerHandle=[];
trigger(1).TCPHandle=[];

%extern triggered
trigger(2).triggerMode='callback';
trigger(2).period=[];
trigger(2).delay=[];
trigger(2).timerHandle=[];
trigger(2).TCPHandle=[];