clc
clear
s = serial('COM5','BaudRate',9600);
set(s, 'DataBits', 8);
set(s, 'StopBits', 2);
fopen(s);
s.ReadAsyncMode = 'continuous';
t = 0;
readasync(s);
ss = 0;
while (t<100)
    detlet = fscanf(s, '%c');
    t = t+1;
end

t = 0;
i = 1;

while (ss<1000)
%    while (t < 3)
        bb=fscanf(s,'%c');
%        t=t+1;
%    end
%    t=0;
    ax(i)=[i];
    tt(i) = bb(1,1);
    b(i) = bb(2,1);
%    c(i) = bb(3,1);
%    d(i) = bb(4,1);
    i = i+1;

    ax1 = subplot(3,1,1);
%   ax2 = subplot(3,1,2);
%    ax3 = subplot(3,1,3);

    plot(ax1, ax, ',b','r--')
    title(ax1,'Time')
    ylabel(ax1,'vibx')
    
%    plot(ax2, ax, ',c','r:')
%    title(ax2,'Time')
%    ylabel(ax2,'vibx')
    
%    plot(ax3, ax, ',d','k--')
%    title(ax3,'Time')
%    ylabel(ax3,'vibx')
%    axis([ax1 ax2 ax3],[0,inf -inf inf])
    axis(ax1)
    drawnow
    ss = ss+1;
end
fclose(s)
delete(s)
clear s
