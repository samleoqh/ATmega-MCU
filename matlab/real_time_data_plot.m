%% Real time data collection example
% This MATLAB script generates a real time plot of voltage values collected
% from an Agilent(TM) 34401A Digital Multimeter over the serial
% port(RS232). The data is collected and plotted until the predefined stop
% time is reached. This example also demonstrates automating a query based
% interaction with an instrument while monitoring the results live.
%
% This script can be modified to be used on any platform by changing the
% serialPort variable.
% 
% Example:-
%
% On Linux:     serialPort = '/dev/ttyS0';
%
% On MacOS:     serialPort = '/dev/tty.KeySerial1';
%
% On Windows:   serialPort = 'COM1';
%
%
% The script may also be updated to use any instrument/device to collect
% real time data. You may need to update the SCPI commands based on
% information in the instrument's programming manual.
%
% To generate a report of this entire script, you may use the PUBLISH
% command at the MATLAB(R) command line as follows: 
% 
% publish(real_time_data_plot);
% 
% Author: Ankit Desai
% Copyright 2009 - The MathWorks, Inc.

%% Create the serial object
clear;
clc;
delete(instrfindall);
serialPort = 'COM5';
serialObject = serial(serialPort);
serialObject.BaudRate =9600;
fopen(serialObject);
serialObject.ReadAsyncMode = 'continuous';

readasync(serialObject);

%% Set the instrument in remote mode
fprintf(serialObject,'1');

%% Set up the figure window
time = now;
voltage = 0;

figureHandle = figure('NumberTitle','off',...
    'Name','Voltage Characteristics',...
    'Color',[0 0 0],'Visible','off');

% Set axes
axesHandle = axes('Parent',figureHandle,...
    'YGrid','on',...
    'YColor',[0.9725 0.9725 0.9725],...
    'XGrid','on',...
    'XColor',[0.9725 0.9725 0.9725],...
    'Color',[0 0 0]);

hold on;

plotHandle = plot(axesHandle,time,voltage,'Marker','.','LineWidth',1,'Color',[0 1 0]);

xlim(axesHandle,[min(time) max(time+0.001)]);

% Create xlabel
xlabel('Time','FontWeight','bold','FontSize',14,'Color',[1 1 0]);

% Create ylabel
ylabel('Voltage in V','FontWeight','bold','FontSize',14,'Color',[1 1 0]);

% Create title
title('Voltage Characteristics','FontSize',15,'Color',[1 1 0]);


%% Collect data
count = 1;

while count < 300
    time(count) = datenum(clock); 
    %fprintf(serialObject,'r'); % To measure current the command is MEASURE:CURRENT:DC?
    data = fread(serialObject,1);
    %data2 = str2double(data);
    voltage(count)=data(1);
    %voltage(count) = fscanf(serialObject,'%d');  %#ok<SAGROW>
    set(plotHandle,'YData',voltage,'XData',time);
    set(figureHandle,'Visible','on');
    datetick('x','mm/DD HH:MM');
 
    count = count +1;
    drawnow;
end

%% Put the instrument in local mode
%fprintf(serialObject,'SYSTEM:LOCAL');

%% Clean up the serial object
fclose(serialObject);
delete(serialObject);
clear serialObject;