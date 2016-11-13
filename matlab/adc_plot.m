%% Real time ADC data ploting
% This script can be modified to be used on any platform by changing the
% serialPort variable.
% 
% Example:-
% On Linux:     serialPort = '/dev/ttyS0';
% On MacOS:     serialPort = '/dev/tty.KeySerial1';
% On Windows:   serialPort = 'COM1';
%% Create the serial object
clear;
clc;
delete(instrfindall);
serialPort = 'COM5';
serialObject = serial(serialPort);
serialObject.BaudRate =1000000;
serialObject.InputBufferSize = 10240;
fopen(serialObject);
serialObject.ReadAsyncMode = 'continuous';
readasync(serialObject);

%% Set up the figure window
time = 0;
w1 = 0;
%target1 = 0;
sample = 0;
figureHandle = figure('NumberTitle','off',...
    'Name','ADC Visulization',...
    'Visible','off');

axesHandle = axes('Parent',figureHandle,'YGrid','on','XGrid','on');

hold on;
%plotHandle = plot(axesHandle,time,w1,time,target1,'LineWidth',2);
plotHandle = plot(axesHandle,time,w1,'LineWidth',1);

% Create xlabel
xlabel('t_{seconds}','FontWeight','bold','FontSize',14,'Color',[0 0 1]);

% Create ylabel
ylabel('ADC Value (0~255) 5V','FontWeight','bold','FontSize',14,'Color',[0 0 1]);

% Create title
title('ADC Visulization','FontSize',15,'Color',[0 0 1]);

% microcontroller - time between each value sent on RS232
%sampleTime = 0.0066;
ADC_SR = 152000;
plot_step = 1024;
plot_interval = 1/ADC_SR * plot_step;
% interval pause for PC data collection
pauseInterval = plot_interval/2;

%% Collect data
count = 1;
set(figureHandle,'Visible','on');
set(plotHandle,'Marker','.');
%scrsz = get(groot,'ScreenSize');
%hf=figure('position',[0 0 eps eps],'menubar','none');
%hf=figure('position',[1 scrsz(4)/2 scrsz(3)/2 scrsz(4)/2],'menubar','none');

while count < 1500%(1/plot_interval)   
    %sample = fscanf(serialObject, '%d-%d\n');
    %sample = fscanf(serialObject, '%d');
    sample = fread(serialObject,plot_step);
    time(count) = count * plot_interval;    
    %w1(count) = sample(1);
    w1(count) = sample(1)/255 * 5; % ref V = 5
    %target1(count) = sample(2);
    %set(plotHandle, {'YData'}, {w1;target1}, {'XData'}, {time;time});
    set(plotHandle, 'YData', w1, 'XData', time);
    
    %if count >100
        %axis([time(count) time(count)+1 min(w1)*1.1 max(w1)*1.1]);
        %xlim(axesHandle,[time(count) time(count)+1]);
        %redraw;
    %end
    %pause(pauseInterval); 
    count = count + 1;
    drawnow limitrate;
    
end
drawnow;
%% Clean up the serial object
%close(hf);
fclose(serialObject);
delete(serialObject);
clear serialObject;
clear
clc