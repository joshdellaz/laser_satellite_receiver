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

m = 4;
n = 2^m - 1 ;
k = 3; % could be from 1 to n-1
h = n-k;
t = h/2;

intrlv_dep = 2*m; % consecutive bits will be intrlv_dep-1 bits apart

%% Data generation
data = randsrc(1000,1,[1,0]);
intInpData = zeros(length(data)/m , 1);
for i = 1:(length(data)/m)
    intInpData(i,1) = bin2dec(num2str(data((1+(i-1)*m) : i*m , 1).'));
    if intInpData(i,1) > (2^m - 2)
        intInpData(i,1) = 14; % note this
        data(i*m , 1) = 0;
    end
end

%% RS encoding
field = gftuple([-1:2^m-2]', m, 2);
c = [1 0];
p(1) = c(1);
for i = 1:h-1
    p(1) = gfmul(p(1),1,field);
    p(2) = 0;
    c = gfconv(c,p,field);
end
g = c;

RS_CODE = zeros(fix(length(intInpData)/k) , n); %each row is an encoded msg
for i = 1 : fix(length(intInpData)/k)
    parity = RS_ENC4(intInpData((k*(i-1) + 1) : k*i , 1).',n,k,g,field);
    RS_CODE(i,:) = [parity intInpData((k*(i-1) + 1) : k*i , 1).'];
end

%% testing
% % RECEIVED = RS_CODE;
% % RECEIVED(:,3) = gfadd(RECEIVED(3),randi([-1 n-1],1,1),field);
% % RECEIVED(:,4) = gfadd(RECEIVED(4),randi([-1 n-1],1,1),field);
% % RECEIVED(1,6) = gfadd(RECEIVED(6),randi([-1 n-1],1,1),field);
% % RECEIVED(:,14) = gfadd(RECEIVED(14),randi([-1 n-1],1,1),field);
% % RECEIVED(:,15) = gfadd(RECEIVED(15),randi([-1 n-1],1,1),field);
% % RECEIVED(:,1) = -2;
% % RECEIVED(:,7) = -2;
% % RECEIVED(:,9) = -2;
% % RECEIVED(:,11) = -2;
% % erasures = [1 7 9 11];  %This polynomial contains the positions of the erasures
% % 
% % DECODED = RECEIVED;
% % MSGS = zeros(length(intInpData),1);
% % chckr = zeros(fix(length(intInpData)/k),1);
% % for i = 1 : fix(length(intInpData)/k)
% %     DECODED(i,:) = RS_E_E_DEC(RECEIVED(i,:),erasures,n,k,t,h,g,field);
% %     if DECODED(i,:) == RS_CODE(i,:)
% %         chckr(i,1) = 1;
% %     end
% %     MSGS((k*(i-1) + 1) : k*i , 1) = DECODED(i,(n-k+1):n).';
% % end

%% interleave
% TODO

% RECEIVED = RS_CODE;
% RECEIVED(RECEIVED == -Inf) = 15; % CAUTION
% bitStrm = reshape(dec2bin(reshape(RECEIVED, 1, numel(RECEIVED)), m) - '0', 1, m*numel(RECEIVED));
% interlvd = zeros(m*numel(RECEIVED),1);
% for i = intrlv_dep : intrlv_dep : length(bitStrm)
%     interlvd(i,1) = intrlv_dep
% end


%% Digital to Analog Conversion
% TODO

% [outSig,burstEr,burstsHist,t] = channel_sim(data,250,2750,5,10); % choosing high SNR for now


%% Analog to Digital Conversion and demodulation (with 11 samples per bit, the 6th sample is the mid point
% TODO

% DemodOut = zeros(length(outSig)/11,1);
% checker = 0;
% awgncheck = 0;
% for ii = 6 : 11 : length(outSig)
%     if (outSig(ii,1) > 2)
%         DemodOut(1+fix(ii/11),1) = -1; % burst erasure
%     elseif (outSig(ii,1) > 0.5)
%         DemodOut(1+fix(ii/11),1) = 1;
%         if data(1+fix(ii/11),1) ~= 1
%             awgncheck = awgncheck + 1;
%         end
%     else
%         DemodOut(1+fix(ii/11),1) = 0;
%         if data(1+fix(ii/11),1) ~= 0
%             awgncheck = awgncheck + 1;
%         end
%     end
%     % for BER calc
%     if data(1+fix(ii/11),1) == DemodOut(1+fix(ii/11),1) % counting successful transmission
%         checker = checker + 1;
%     end
% end
% 
% disp('Total BER:')
% passage = checker/length(data)
% disp('The BER due to awgn:')
% disp(awgncheck);

%% de-interleave
% TODO

%% decode
% TODO

% DECODED = RECEIVED;
% MSGS = zeros(length(intInpData),1);
% chckr = zeros(fix(length(intInpData)/k),1);
% for i = 1 : fix(length(intInpData)/k)
%     DECODED(i,:) = RS_E_E_DEC(RECEIVED(i,:),erasures,n,k,t,h,g,field);
%     if DECODED(i,:) == RS_CODE(i,:)
%         chckr(i,1) = 1;
%     end
%     MSGS((k*(i-1) + 1) : k*i , 1) = DECODED(i,(n-k+1):n).';
% end

%% plots

% figure();
% h = histogram(burstsHist,5*[0.5:1:40.5],'Normalization','probability');
% 
% cleanSig = repelem(data,11);
% figure();
% plot(t,[cleanSig outSig])
% %axis([(numBits/2)/bitRate, (80 + numBits/2)/bitRate, -1.0, 2.0])
% legend('Original Signal','Received Signal')
% 
% 
% figure();
% plot(1:length(data),[data DemodOut])
% 
