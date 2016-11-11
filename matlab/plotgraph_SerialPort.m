clc;
clear all;
comport = serial('COM6', 'BaudRate', 115200); % setup comport
fopen(comport);  % Open comport
x=0;
while(x<100)
x=x+1;
y1(x)=fscanf(comport, '%d'); % receive ADC1
y2(x)=fscanf(comport, '%d'); % receive ADC2
drawnow; 
plot(y1,'r--','linewidth',3)
grid on;
hold on;
plot(y2,'b--','linewidth',3)
title('ADC Testing');
xlabel('Time in seconds');
ylabel('Digital Value');
pause(0.1);
end
fclose(comport); % Close comport
delete(comport); % Clear comport