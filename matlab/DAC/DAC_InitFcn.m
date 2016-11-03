%% Initialization script
%  See model callbacks / InitFcn

% Time constants
F0 = 1e3;                   % Digital signal frequency
Fe = 10e3;                  % Sampling frequency
T0 = 1/F0;                  % Digital signal period
Te = 1/Fe;                  % Sampling period
Tau = 2*pi/Fe;              % Low pass filter characteristic period

% Magnitudes
SD = 100;                   % Digital signal magnitude [%]/full scale
W = [32 16 8 4 1 1 1 1 1]'; % Digit weights (redundancy, hysteresis according to the signal gradient)