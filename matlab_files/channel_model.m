% This script creates a certain number of randomly generated bits and then  
% adds 2 types of signal deterioration encountered in Free-Space Optical 
% channels: white Gaussian noise and burst erasures 
% The burst erasures are made using a 4-state markov model based on the 
% paper from https://ieeexplore.ieee.org/document/5683891 

% when entering parameters, please ensure the time between samples is
% greater than the min burst length by an integer factor
clear all;
bitRate = 1000; % bits per second
chnlCyc = 0.05; % min burst length in seconds
sampRate = 10000; % sample per second, oversample to emulate analog signal
numBits = 1000000; % number of randomly generated bits
SNR = 10; % signal to noise ratio in dB, used for Gaussian noise
sPerBit = sampRate/bitRate;

% Probabilities associated with certain state transitions in the Markov
% model, based on the paper linked above 
P_a1 = 0.27;
P_a2 = 0.06;
P_b1 = 0.24;
P_b2 = 0.05;

% The tranistion probability matrix for a 4-state markov model is created 
% below with the following assumptions for the indecies:
%       1: LoS High transition (burst free and unstable)
%       2: NLoS High transition (bursty and unstable)
%       3: NLoS Low transition (bursty and stable)
%       4: LoS Low transition (burst free and stable)
% probM(i,j) represents the probability of transition from state i to j
probM = zeros(4); 
probM(1,1) = 0;
probM(1,2) = P_a1;
probM(1,3) = 0;
probM(1,4) = 1 - P_a1;
probM(2,1) = P_b1;
probM(2,2) = 0;
probM(2,3) = 1 - P_b1;
probM(2,4) = 0;
probM(3,1) = P_b2;
probM(3,2) = 0;
probM(3,3) = 1 - P_b2;
probM(3,4) = 0;
probM(4,1) = 0;
probM(4,2) = P_a2;
probM(4,3) = 0;
probM(4,4) = 1 - P_a2;

% random bits (associated with OOK signal power):
data = randsrc(numBits,1,[1,0]);
% simulation of analog signal by oversampling:
cleanSig = repelem(data, sPerBit);
% addition of Gaussian noise:
outSig = awgn(cleanSig, SNR); 
% array for the state of the channel during each bit:
chnlState = zeros(numBits/(bitRate*chnlCyc),1); 
initProb = rand(); % for the initial state of the channel
if (initProb <= 0.25)
    chnlState(1,1) = 1;
elseif (initProb <= 0.5)
    chnlState(1,1) = 2;
elseif (initProb <= 0.75)
    chnlState(1,1) = 3;
else
    chnlState(1,1) = 4;
end

% burstEr for the erased bits (bursts) and burstsHist for testing the
% markov model (burst length probability density)
burstEr = zeros(numBits/(bitRate*chnlCyc),1);
burstsHist = zeros(numBits/(bitRate*chnlCyc),1);
burstLen = 0; % used for keeping track of the length of the burst

% cumulative sum of the transition matrix which will be used along with a
% uniform distrbution number generator (between 0 and 1) to determine the
% next state of the channel:
compM = cumsum(probM,2);
% loop though the number of bits to determine the state of the model and
% existence of burst erasure during each bit
for i = 2 : length(chnlState)
    randnum = rand();
    chnlState(i,1) = min(find(compM(chnlState(i-1),:) > randnum));
    % if in a "bad" (NLoS) state, 100% chance of burst (Steven is 
    % considering 50% chance of burst in his code, which could be done here
    % by replacing the 0 below with 0.5)
    if (rand() > 0) && ((chnlState(i,1) == 2) || (chnlState(i,1) == 3))
        burstEr(i,1) = 1;
        burstLen = burstLen + 1;
    else
        burstsHist(i,1) = burstLen;
        burstLen = 0;
    end
end % the first index is missing from the burst length calculation, which 
% is ok due to the relatively large # of iterations (bits)

% oversample the bursts by the same amount as bits for analog emulation:
burstEr = repelem(burstEr,sPerBit*chnlCyc*bitRate); 
% wherever there is a burst, the output power level is set to 5 which is
% significantly above the noisy "1" bits. This is for proper distinction of
% erasure vs noise
outSig(burstEr > 0) = 5; 

% for checking the probability ditrubution of burst lengths... needs to 
% follow the results from this paper: (which it does)
% https://ieeexplore.ieee.org/document/5683891
burstsHist = chnlCyc * burstsHist(burstsHist ~= 0); 
figure();
h = histogram(burstsHist, chnlCyc*[0.5:1:40.5], 'Normalization','probability');

% Power signal plots of the 20 middle bits, with and without the expected
% detriorations (Gaussian noise + Burst erasures)
figure();
t = 1 : numBits*sPerBit;
t = t / (bitRate*sPerBit);
plot(t,[cleanSig outSig])
axis([(numBits/2)/bitRate, (80 + numBits/2)/bitRate, -1.0, 2.0])
legend('Original Signal','Received Signal')
