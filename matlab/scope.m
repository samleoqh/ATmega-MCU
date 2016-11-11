
% Generates a random signal to display
a = randn(1,1e5);
a = cumsum(a);  %I want some correlation in the signal...

fs = 2e3;  %sampling frequency
timeBase = 2;   %sec

hF = figure;
hAx = gca;

N = length(a);
maxA = max(a);  minA = min(a);
nSamples = round(fs*timeBase);
ind = 1;
hLine = plot(hAx,(1:nSamples)/fs,a(:,ind:ind+nSamples-1));
xlabel('[sec]');
ylim([minA maxA]);

tic;    %start time measuring

% I added the "ishandle" so the program will end in case u closed the figure
while (ind < N-nSamples) & ishandle(hLine)
  
   %instead of using plot I directly change the data in the line
   % this is faster the plot if only because you don't need to reedefine the limits and labels...
   set(hLine,'ydata',a(:,ind:ind+nSamples-1));
   
   drawnow  %updates the display
   
   t = toc; %measuring current time
   ind = round(t*fs); 
   ind = max(ind,1);
   
end