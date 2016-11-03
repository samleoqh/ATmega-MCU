%% DAC_Subfunction
%  Digital to analog converter, multi bit delta sigma
%  Date    : 15/05/2011
%  Author  : E.Ogier
%  Version : 1

function [sys,x0,str,ts] = DAC_SubFunction(t,x,u,flag)

switch flag,
    
    % Initialization
    case 0,   	[sys,x0,str,ts] = mdlInitializeSizes();
        
	% Update
    case 2,  	sys = mdlUpdate(t,x,u);
        
	% Output
    case 3,     sys = mdlOutputs(t,x,u);
        
	% Terminate
    case 9,  	sys = [];
        
	% Unexpected flags
    otherwise,  DAStudio.error('Simulink:blocks:unhandledFlag', num2str(flag));
        
end

end

% mdlInitializeSizes
function [sys,x0,str,ts] = mdlInitializeSizes()

sizes = simsizes;
sizes.NumContStates  = 0;
sizes.NumDiscStates  = 9;
sizes.NumOutputs     = 9;
sizes.NumInputs      = 1;
sizes.DirFeedthrough = 1;
sizes.NumSampleTimes = 1;

sys = simsizes(sizes);

x0  = ones(1,9);
str = [];
ts  = [-1 0];

end

% mdlUpdate
function sys = mdlUpdate(~,~,u)

sys = u*ones(1,9);

end

% mdlOutputs
function sys = mdlOutputs(t,~,u)

if t == 0
    DeltaSigma();
end

% Binary coding
sys = iDAC(u);

% PWM bit
b = find(sys > 0 & sys <1);

% PWM
sys(b) = DeltaSigma(sys(b));

end

% iDAC
function S = iDAC(E)

persistent wLSB0
if isempty(wLSB0)
    wLSB0 = 0;
end

% MSB index
NMSB = [5 4 3 2];

% Vector of powers
W = 2.^NMSB';

% Normalization
e = 32*(E/100+1);

% Integer part
n = floor(e);

% MSB
if n < 64
    MSB = bitget(n,NMSB+1);
else
    MSB = [1 1 1 1];
end

% LSBs weight
wLSB = e-MSB*W;

% LSBs coding
if sum(MSB) >= 1 &&  wLSB <= 1 && wLSB0 > 2
    MSB = bitget(n-4,NMSB+1);
    wLSB = e-MSB*W;
end

% Number of LSBs equal to "1"
NLSB = floor(wLSB);

% LSBs
LSB = [zeros(1,4-NLSB) wLSB-NLSB ones(1,NLSB)];

% Concatenation MSB-LSB
S = [MSB LSB];

% Previous LSBs weight
wLSB0 = wLSB;

end

% Delta Sigma
function s = DeltaSigma(e)

persistent Sigma s0

if ~nargin
    Sigma = [];
    return
end

if isempty(Sigma)
    Sigma = 0;
    s0 = 0;
end

Delta =  e - s0;
Sigma = Sigma + Delta;
s = ge(Sigma+2*eps,1);

s0 = s;

end
