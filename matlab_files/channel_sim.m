function [outSig,burstEr,burstsHist,t] = channel_sim(data,bitRate,sampRate,chnlCyc,SNR)

numBits = length(data); % number of randomly generated bits
sPerBit = sampRate/bitRate; % samples per bit, currently accepts integers

% Probabilities associated with certain state transitions in the Markov
% model, based on the paper: https://www.google.com/url?q=https://ieeexplore.ieee.org/document/5683891&sa=D&source=editors&ust=1638552760986000&usg=AOvVaw11i0-RtBy6dmB54rFF6jm5
P_a1 = 0.27;
P_a2 = 0.06;
P_b1 = 0.24;
P_b2 = 0.05;

% The tranistion probability matrix for a 4-state markov model is created 
% below with the following assumptions for the indecies:
%       1: LoS High transition (burst free and unstable) GOOD_UNS
%       2: NLoS High transition (bursty and unstable) BAD_UNS
%       3: NLoS Low transition (bursty and stable) BAD_S
%       4: LoS Low transition (burst free and stable) GOOD_S
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

% simulation of analog signal by oversampling:
cleanSig = repelem(data, sPerBit);
% addition of Gaussian noise:
outSig = awgn(cleanSig, SNR); 
% array for the state of the channel during each bit:
chnlState = zeros(fix(numBits/(bitRate*chnlCyc)),1); 
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
burstEr = zeros(fix(numBits/(bitRate*chnlCyc)),1);
burstsHist = zeros(fix(numBits/(bitRate*chnlCyc)),1);
burstLen = 0; % used for keeping track of the length of the burst

% cumulative sum of the transition matrix which will be used along with a
% uniform distrbution number generator (between 0 and 1) to determine the
% next state of the channel:
compM = cumsum(probM,2);
% loop though the number of bits to determine the state of the model and
% existence of burst erasure during each bit
for i = 1 : length(chnlState)
    randnum = rand();
    if i > 1 
        chnlState(i,1) = find(compM(chnlState(i-1),:) > randnum, 1);
    end
    % if in a "bad" (NLoS) state, 100% chance of burst
    if ((chnlState(i,1) == 2) || (chnlState(i,1) == 3))
        burstEr(i,1) = 1;
        burstLen = burstLen + 1;
    else
        burstsHist(i,1) = burstLen;
        burstLen = 0;
    end
end

% oversample the bursts by the same amount as bits for analog emulation:
burstEr = repelem(burstEr,chnlCyc*bitRate); 
bursttail = burstEr(end,1).*ones(mod(numBits,(bitRate*chnlCyc)),1);
burstEr = [burstEr; bursttail];
% wherever there is a burst, the output power level is set to 5 which is
% significantly above the noisy "1" bits. This is for proper distinction of
% erasure vs noise
burstErreg = repelem(burstEr,sPerBit); 
outSig(burstErreg > 0) = 5; 

% for checking the probability ditrubution of burst lengths... needs to 
% follow the results from this paper: (which it does)
% https://ieeexplore.ieee.org/document/5683891
burstsHist = bitRate * chnlCyc * burstsHist(burstsHist ~= 0); 

t = 1 : numBits*sPerBit;
t = t / (bitRate*sPerBit);
end

