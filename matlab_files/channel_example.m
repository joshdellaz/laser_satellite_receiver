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

m = 8;
n = 32; % max is 2^m - 1 ;
k = 16; % could be from 1 to n-1
h = n-k;
t = h/2;

dataLength = 2048;

int_d = (n/k)*4*m; % consecutive bits will be intrlv_dep-1 bits apart
li = 2;
int_f = int_d*li;
% with these parameters we can construct the interleaving scheme as follows
% Every intrlv_frq bits are interleaved as a separate set
% For example: starting from index of 1:
% The interleaved bits: 1, 1+int_d, 1+(2*int_d), ..., 1+(int_d*(li-1)),
% 2, 2+int_d, 2+(2*int_d), ..., 2+(int_d*(li-1)), .......,
% int_d+(int_d*(li-1)) {which is equal to int_d*li}

%% Data generation
data = randsrc(dataLength,1,[1,0]);
intInpData = zeros(length(data)/m , 1);
for i = 1:(length(data)/m)
    intInpData(i,1) = bin2dec(num2str(data((1+(i-1)*m) : i*m , 1).'));
end

%% RS encoding
% field = gftuple([-1:2^m-2]', m, 2);
% c = [1 0];
% p(1) = c(1);
% for i = 1:h-1
%     p(1) = gfmul(p(1),1,field);
%     p(2) = 0;
%     c = gfconv(c,p,field);
% end
% g = c;
intInpData_r = reshape(intInpData.', k , numel(intInpData)/k).';
RS_CODE = zeros(fix(length(intInpData)/k) , n); %each row is an encoded msg
msgs = gf(intInpData_r, m);
RS_CODE = rsenc(msgs, n , k);
% for i = 1 : fix(length(intInpData)/k)
%     parity = RS_ENC4(intInpData((k*(i-1) + 1) : k*i , 1).',n,k,g,field);
%     RS_CODE(i,:) = [parity intInpData((k*(i-1) + 1) : k*i , 1).'];
% end

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

RECEIVED = RS_CODE.x;
bitArr = dec2bin(reshape(RECEIVED.', [], 1).', m) - '0';
bitStrm_in = reshape(bitArr.', m*numel(RECEIVED), 1).';
interlvd = zeros(1, m*numel(RECEIVED));
for i = 1 : numel(bitStrm_in)/int_f % each iteration corresponds to a separate set of interleaved bits
    for j = 1 : int_d
        interlvd(1, (i-1)*int_f + (j-1)*li + [1:li], 1) = bitStrm_in(1, ((i-1)*int_f)+(j+int_d*[0:li-1]));
    end
end


%% Digital to Analog Conversion

% as long as the units of the channel cycle correspond to the inverse of
% the bit rate (which is bit period) the function works: like if bit rate = 25e6 bps and channel
% cycle is 50e-6 seconds, we can enter bit_rate=25 and chnl_cyc=50
chnl_cyc = 1;
bit_rate = 128; % the smallest burst wipes chnl_cyc*bit_rate bits
sample_rate = 5*bit_rate; 
SNR = 40;

% TODO need to figure out how to deal with non-integer samples/bit

% TODO make chnlState array in channel_sim flexible for non-integer
% numBits/(bitRate*chnlCyc)

[outSig,burstEr,burstsHist,timeP] = channel_sim(interlvd.',bit_rate,sample_rate,chnl_cyc,SNR); % choosing high SNR for now
% The channel_sim function simulates the power seen by the APD, so during
% the bursts the value of the signal is 5


%% Analog to Digital Conversion and demodulation
% The demodulation done here only "looks" at the value of the analog signal
% in the middle of the bit, which probably introduces more erros than 
% Dealing with erased bits:
%   - discarding bit erasure information (not flagging the erased bits) and
%   recording the bits in a burst as 1's or 0's, which leads to about 50%
%   of them being correct
%   - flagging the erased bits and passing the information to the FEC
%   decoding

% For RS scheme, the 2 approaches above lead to the same (n,k) requirements
% so we'll go with the former, recording lost bits as zeros

mid_bit = fix((sample_rate/bit_rate)/2)+1;
bitStrm_out = zeros(1, length(outSig)*bit_rate/sample_rate);
erased_bits = 0;
fliped_bits = 0;
rawBERcnt = 0;
for ii = mid_bit : (sample_rate/bit_rate) : length(outSig)
    if (outSig(ii,1) == 5)
        bitStrm_out(1+fix((ii-mid_bit)*bit_rate/sample_rate)) = 0; % burst erasure
        erased_bits = erased_bits + 1;
    elseif (outSig(ii,1) > 0.5)
        bitStrm_out(1+fix((ii-mid_bit)*bit_rate/sample_rate)) = 1;
        if interlvd(1+fix((ii-mid_bit)*bit_rate/sample_rate)) ~= 1
            fliped_bits = fliped_bits + 1;
        end
    else
        bitStrm_out(1+fix((ii-mid_bit)*bit_rate/sample_rate)) = 0;
        if interlvd(1+fix((ii-mid_bit)*bit_rate/sample_rate)) ~= 0
            fliped_bits = fliped_bits + 1;
        end
    end
    % for BER calc
    if interlvd(1+fix((ii-mid_bit)*bit_rate/sample_rate)) ~= bitStrm_out(1+fix((ii-mid_bit)*bit_rate/sample_rate)) % counting unsuccessful transmission
        rawBERcnt = rawBERcnt + 1;
    end
end

disp('Total raw BER (awgn errors + all erased):')
rawBER = (fliped_bits+erased_bits)/length(interlvd)
disp('The effective raw BER (with erased bit registered as 0):')
rawBEReff = rawBERcnt/length(interlvd)

%% deinterleave

uninterlvd = zeros(numel(bitStrm_out),1);
for i = 1 : numel(bitStrm_out)/int_f % each iteration corresponds to a separate set of interleaved bits
    for j = 1 : int_d
        for q = 1 : li
            uninterlvd((i-1)*int_f + j + int_d*(q-1)) = bitStrm_out((i-1)*int_f + (j-1)*li + q);
        end
    end
end
intOutData = zeros(1, length(bitStrm_out)/m);
for i = 1 : length(bitStrm_out)/m
    intOutData(1,i) = bin2dec(num2str(uninterlvd((1+(i-1)*m) : i*m).')); % TODO verify
end

%% decode

intOutData = reshape(intOutData.', n , numel(intOutData)/n).';
decoded_data = zeros(numel(intOutData),1);
MSGchckr = zeros(numel(intOutData)/n,1);
DECODED = rsdec(gf(intOutData, m), n, k);
DECODED = DECODED.x;
bitArr = dec2bin(reshape(DECODED.', [], 1).', m) - '0';
dataOut = reshape(bitArr.', numel(bitArr), 1);

transmission = dataOut-data;
disp('Sum of difference between the received bits and the transmitted ones:');
sum(abs(transmission))
% for i = 1 : numel(intOutData)/n
%     DECODED(i,:) = rsdec(intOutData(i,:),[],n,k,t,h,g,field);
%     if DECODED(i,:) == RS_CODE(i,:)
%         MSGchckr(i,1) = 1; % the codeword was recovered successfully
%     end
%     decoded_data((k*(i-1) + 1) : k*i , 1) = DECODED(i,(n-k+1):n).';
% end

%% plots
% TODO BER ratio before and after FEC decoding, throughput (k/n), against
% different sets of (n,k), a few different int_d's with a fixed int_f and
% fixed channel_sim parameters
%then repeat for other FEC schemes

figure();
h = histogram(burstsHist,1*[0.5:1:40.5],'Normalization','probability');

cleanSig = repelem(interlvd.',5);
figure();
plot(timeP,[cleanSig outSig])
%axis([(numBits/2)/bitRate, (80 + numBits/2)/bitRate, -1.0, 2.0])
legend('Original Signal','Received Signal')

figure();
plot(1:length(data),[dataOut data])
