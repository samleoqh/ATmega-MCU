%% Representation script
%  See model callbacks / StopFcn

Figure = figure('color','w','numbertitle','off');

% Signals
A(1) = subplot(2,1,1);
hold('on');
plot(T,Sd,'b');
plot(T,Sanf,'color',0.9*[1 1 1]);
plot(T,Sa,'g');
legend({'Digital signal','Analog signal (not filtered)','Analog signal (filtered)'});
xlabel('Time [s]');
ylabel('Magnitude [%]');
title('Digital and analog signals','Fontsize',12,'Fontweight','light');
box('on');

% Digits
A(2) = subplot(2,1,2);
delta = repmat(0:2:2*(9-1),size(D,1),1);
plot(T,fliplr(D)+delta,'b');
box('on');
A(2).YTick = 0:2:2*(9-1);
A(2).YTickLabel = arrayfun(@(x)sprintf('b_%u',x),0:8,'UniformOutput',0);
xlabel('Time [s]');
ylabel('Bit value');
title('Delta sigma digits','Fontsize',12,'Fontweight','light');

linkaxes(A,'x');

drawnow;
warning('off','all');
jFrame = get(Figure,'JavaFrame');
jFrame.setMaximized(true);
warning('on','all');
