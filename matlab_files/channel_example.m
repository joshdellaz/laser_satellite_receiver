% This script creates a certain number of randomly generated bits and then  
% adds 2 types of signal deterioration encountered in Free-Space Optical 
% channels: white Gaussian noise and burst erasures 
% The burst erasures are made using a 4-state markov model based on the 
% paper from https://ieeexplore.ieee.org/document/5683891 

% For faster simulation we can scale down the parameters of our hardware
% together: 25 Mbps, 105 Msps, 50 us bursts => (250 bps, 1050 sps, 5 s)

% when entering parameters, please ensure the time between samples is
% greater than the min burst length by an integer factor
clear all;

data = randsrc(1000000,1,[1,0]);

[outSig,burstEr,burstsHist,t] = channel_sim(data,250,2750,5,10); % choosing high SNR for now

figure();
h = histogram(burstsHist,5*[0.5:1:40.5],'Normalization','probability');

cleanSig = repelem(data,11);
figure();
plot(t,[cleanSig outSig])
%axis([(numBits/2)/bitRate, (80 + numBits/2)/bitRate, -1.0, 2.0])
legend('Original Signal','Received Signal')

% demodulation (with 11 samples per bit, the 6th sample is the mid point
DemodOut = zeros(length(outSig)/11,1);
checker = 0;
awgncheck = 0;
for ii = 6 : 11 : length(outSig)
    if (outSig(ii,1) > 2)
        DemodOut(1+fix(ii/11),1) = -1; % burst erasure
    elseif (outSig(ii,1) > 0.5)
        DemodOut(1+fix(ii/11),1) = 1;
        if data(1+fix(ii/11),1) ~= 1
            awgncheck = awgncheck + 1;
        end
    else
        DemodOut(1+fix(ii/11),1) = 0;
        if data(1+fix(ii/11),1) ~= 0
            awgncheck = awgncheck + 1;
        end
    end
    % for BER calc
    if data(1+fix(ii/11),1) == DemodOut(1+fix(ii/11),1) % counting successful transmission
        checker = checker + 1;
    end
end

disp('Total BER:')
passage = checker/length(data)
disp('The BER due to awgn:')
disp(awgncheck);

figure();
plot(1:length(data),[data DemodOut])

