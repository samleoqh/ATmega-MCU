clear;
clc;
delete(instrfindall);
s = serial('COM5');
s.BaudRate = 9600;
%s.Terminator = 'CR';
s.ReadAsyncMode = 'continuous';
count =1;
y = 0;
x = 0;
fopen(s);
while (count < 150)
    %data = fscanf(s,'%c');
    data = fread(s,10);
    %data2 = str2double(data);
    y(count)=data(1);
    x(count)=count;
    count = count + 1;
    plot(x,y);
    drawnow;
end
fclose(s);
delete(s);
clear s;
clear;