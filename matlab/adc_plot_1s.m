%% Real time ADC data ploting
% Author: Qinghui Liu, Date: 2016-11
% This script can be modified to be used on any platform by changing the
% serialPort variable. 
% Example:
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

%% Set up the figure window
time = 0;
adc = 0;
%target1 = 0;
sample = 0;

figureHandle = figure('NumberTitle','off',...
    'Name','ADC Visulization',...
    'Visible','off');

NX=1000;
NY=260;
XLIMS=[0 NX];
YLIMS=[0 NY];
axesHandle = axes('Parent',figureHandle,'XLim',XLIMS,'YLim',YLIMS,'YLimMode','manual',...
                  'XLimMode','manual','YGrid','on','XGrid','on');
hold on;
%plotHandle = plot(axesHandle,time,w1,time,target1,'LineWidth',2);
plotHandle = plot(axesHandle,time, adc, 'LineWidth', 1);
set(axesHandle,'YTick',[0:10:NY]);
set(axesHandle,'XTick',[0:50:NX]);
% Create xlabel
xlabel('t_{milliseconds}','FontWeight','bold','FontSize',12,'Color',[0 0 1]);

% Create ylabel
ylabel('ADC Value (0~255) 5V','FontWeight','bold','FontSize',12,'Color',[0 0 1]);

% Create title
title('ADC Visulization','FontSize',15,'Color',[1 0 1]);
hl = line('XData',time,'YData',adc,...
          'Color', [1 0 1],...
          'LineWidth',1,...
          'Parent',axesHandle);

ADC_SR = 69000;
plot_step = 1000;
plot_interval = plot_step/ADC_SR;

% interval pause for PC data collection

%% Collect data
count = 1;
set(figureHandle,'Visible','on');
set(plotHandle,'Marker','.');
while true 
    sample = fread(serialObject,plot_step);
    time(count) = count * plot_interval * 1000;  %milliseconds
    adc(count) = sample(1); 
    set(hl,'XData', time, 'YData',adc);

    drawnow limitrate;
    %pause(1);
    count = count + 1;
    if count> (1/plot_interval)
        count = 1;
        set(hl,'XData',get(hl,'XData')+1/plot_interval);
    end
    
 
    %#break in case you close the figure
    if ~ishandle(axesHandle), break, end
end
drawnow;
%% Clean up the serial object
fclose(serialObject);
delete(serialObject);
clear serialObject;