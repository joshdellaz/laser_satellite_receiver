% run to get BER (Bit error rate) 
clear all;

% Parameters:
bit_rate = 25; % Mbps
chnl_cycle = 50; % microsecond, should be 50
awgn_SNR = 10; % used in channel model awgn and log-likelihood ratio of OOK
% need to find the relation to SNR, look at 
% file:///C:/Users/khora/OneDrive/Desktop/decoding.html and 
% https://en.wikipedia.org/wiki/Signal-to-noise_ratio
% LDPC parameters
codeword_lengths = [1944]; % or n, Should be one of 648, 1296, and 1944
code_rate = 1/4; % or k/n, Should be one of 1/2, 2/3, 3/4, 5/6
max_decode_iterations = 25;
min_sum = 1;

max_frame_size = 10000; % based on Doppler and clock drifts
preamb_size = 80; % iterations of MLS + frame ID

num_of_frames = 500;

RUNS_INFO = ["{SNR_dB, num_of_frames, codeword_length, code_rate, codewords_per_packet, intrlvd_codewords, packet_size, rawBER, finalBER} : {10, 100, "];
RUNS_INFO = repelem(RUNS_INFO, 20);
for intrlvr = 20
    tic
    codeword_length = codeword_lengths(1); 
    codewords_per_packet = fix((max_frame_size - preamb_size)/codeword_length);
    packet_size = codewords_per_packet * codeword_length;
    intrlvd_codewords = codewords_per_packet * intrlvr; % could have multi-packet interleaving if > 1 
    RUNS_INFO(intrlvr) = RUNS_INFO(intrlvr) + num2str(codeword_length) + ", " + num2str(code_rate) + ", " + num2str(codewords_per_packet) + ", " + ...
        num2str(intrlvd_codewords) + ", " + num2str(packet_size)  + ", ";
    ldpc_code = LDPCCode(0, 0);
    ldpc_code.load_wifi_ldpc(codeword_length, code_rate); % parity check matrix based on 802.11 IEEE
    disp(">>>>>>>>>> This is run " + num2str(intrlvr))
    %% Data generation
    info_data = double(rand(num_of_frames * codewords_per_packet,...
        code_rate * codeword_length) > 0.5); % randomly generated data

    %% LDPC encoding
    coded_bits = zeros(num_of_frames * codewords_per_packet, codeword_length);
    for i = 1 : num_of_frames * codewords_per_packet
        coded_bits(i,:) = ldpc_code.encode_bits(info_data(i,:));
    end
    disp("Data encoded with LDPC")

    %% testing
    % cod_out_data = zeros(num_of_frames * codewords_per_packet, codeword_length);
    % p0 = (-1*coded_bits + 1)*0.8 + 0.1; % 90% proability of of 0 for 0 bits
    % p1 = 1 - p0;
    % llr = log(p0./p1);
    % for i = 1 : num_of_frames * codewords_per_packet
    %     llr(i,486:700) = 0;
    %     %llr(i,1458:1944) = 0;
    %     [decoded_codeword, ~] = ldpc_code.decode_llr(llr(i,:), max_decode_iterations, min_sum);
    %     cod_out_data(i,:) = decoded_codeword;
    % end
    % 
    % result = sum(sum(abs(cod_out_data - coded_bits)))

    %% interleave
    % Block interleaving:
    intrlvd = zeros(numel(coded_bits),1);
    for i = 1 : (num_of_frames * codewords_per_packet / intrlvd_codewords)
        intrlvd((i-1) * intrlvd_codewords * codeword_length + 1: ...
            i * intrlvd_codewords * codeword_length, 1) = ...
            reshape(coded_bits((i-1) * intrlvd_codewords + 1: ...
            i * intrlvd_codewords, :), [], 1);
    end
    disp("Encoded data interleaved")

    %% DAC and Channel model

    % as long as the units of the channel cycle correspond to the inverse of
    % the bit rate (which is bit period) the function works: like if bit rate = 25e6 bps and channel
    % cycle is 50e-6 seconds, we can enter bit_rate=25 and chnl_cyc=50
    %chnl_cyc = 1;
    %bit_rate = 25; % the smallest burst wipes chnl_cyc*bit_rate bits
    sample_per_bit = 4;
    sample_rate = sample_per_bit * bit_rate;

    % TODO need to figure out how to deal with non-integer samples/bit

    [outSig,burst_errs,burstsHist,timeP] = channel_sim(intrlvd,bit_rate,sample_rate,chnl_cycle,awgn_SNR); % choosing high SNR for now
    % The channel_sim function simulates the power seen by the APD, so during
    % the bursts the value of the signal is 5
    %plot(1:numel(outSig), [repelem(intrlvd, 4), outSig])
    disp("Passed trhough channel model")

    %% Analog to Digital Conversion and demodulation
    % The demodulation done here only "looks" at the value of the analog signal
    % in the middle of the bit, which probably introduces more erros than 
    % Dealing with erased bits:
    %   - discarding bit erasure information (not flagging the erased bits) and
    %   recording the bits in a burst as 1's or 0's, which leads to about 50%
    %   of them being correct
    %   - flagging the erased bits and passing the information to the FEC
    %   decoding

    received_mod = outSig;
    demodulated = zeros(numel(coded_bits),1);
    erased_bits = 0;
    fliped_bits = 0;
    rawBERcnt = 0;
    for i = 1 : sample_per_bit : length(received_mod)
        if (mean(received_mod(i : i + sample_per_bit - 1, 1)) == 5)
            demodulated(fix(i/sample_per_bit) + 1) = 0; % burst erasure
            erased_bits = erased_bits + 1;
        elseif (mean(received_mod(i : i + sample_per_bit - 1, 1)) > 0.5)
            demodulated(fix(i/sample_per_bit) + 1) = 1;
            if intrlvd(fix(i/sample_per_bit) + 1) ~= 1
                fliped_bits = fliped_bits + 1;
            end
        else
            demodulated(fix(i/sample_per_bit) + 1) = 0;
            if intrlvd(fix(i/sample_per_bit) + 1) ~= 0
                fliped_bits = fliped_bits + 1;
            end
        end
    end

    disp('Demodulated; Total raw BER (awgn errors + all erased):')
    rawBER = (fliped_bits + erased_bits)/numel(intrlvd)

    %% deinterleave

    deintrlvd_data = zeros(num_of_frames * codewords_per_packet, codeword_length);
    burst_loc = zeros(num_of_frames * codewords_per_packet, codeword_length);
    for i = 1 : (num_of_frames * codewords_per_packet / intrlvd_codewords)

        deintrlvd_data((i-1) * intrlvd_codewords + 1: i * intrlvd_codewords, :) = ...
            reshape(demodulated((i-1) * intrlvd_codewords * codeword_length + 1: ...
            i * intrlvd_codewords * codeword_length, 1), intrlvd_codewords, codeword_length);

        burst_loc((i-1) * intrlvd_codewords + 1: i * intrlvd_codewords, :) = ...
            reshape(burst_errs((i-1) * intrlvd_codewords * codeword_length + 1: ...
            i * intrlvd_codewords * codeword_length, 1), intrlvd_codewords, codeword_length);
    end
    disp("Deinterleaved")

    % deintrlvd_ref = zeros(num_of_frames * codewords_per_packet, codeword_length);
    % for i = 1 : (num_of_frames * codewords_per_packet / intrlvd_codewords)
    %     deintrlvd_ref((i-1) * intrlvd_codewords + 1: i * intrlvd_codewords, :) = ...
    %         reshape(intrlvd((i-1) * intrlvd_codewords * codeword_length + 1: ...
    %         i * intrlvd_codewords * codeword_length, 1), intrlvd_codewords, codeword_length);
    % end
    % disp('Checking deinterleaving:')
    % comparison = sum(sum(abs(deintrlvd_ref - coded_bits)))

    %% decode
    decoded_raw = ones(num_of_frames * codewords_per_packet, codeword_length);
    p0 = (-1*coded_bits + 1)*0.8 + 0.1; % 90% proability of correct bit registration
    p1 = 1 - p0;
    % TODO make this part SNR dependent
    llr = log(p0./p1);
    burst_loc = -1 * burst_loc + 1;
    llr = llr .* burst_loc;
    for i = 1 : num_of_frames * codewords_per_packet
        [decoded_codeword, ~] = ldpc_code.decode_llr(llr(i,:), max_decode_iterations, min_sum);
        decoded_raw(i,:) = decoded_codeword;
    end

    disp('Total BER after FEC:')
    totalBER = sum(sum(abs(decoded_raw - coded_bits))) / numel(coded_bits)

    disp('Final BER of info data:')
    finalBER = sum(sum(abs(decoded_raw(:, 1 : code_rate * codeword_length) - info_data))) / numel(info_data)

    RUNS_INFO(intrlvr) = RUNS_INFO(intrlvr) + num2str(rawBER) + ", " + num2str(finalBER) + "} time: " + num2str(toc) + " seconds.";
end
%% plots
% TODO BER ratio before and after FEC decoding, throughput (k/n), against
% different sets of (n,k), a few different int_d's with a fixed int_f and
% fixed channel_sim parameters
%then repeat for other FEC schemes

% figure();
% h = histogram(burstsHist,1*[0.5:1:40.5],'Normalization','probability');
% 
% cleanSig = repelem(interlvd.',5);
% figure();
% plot(timeP,[cleanSig outSig])
% %axis([(numBits/2)/bitRate, (80 + numBits/2)/bitRate, -1.0, 2.0])
% legend('Original Signal','Received Signal')
% 
% figure();
% plot(1:length(data),[dataOut data])
