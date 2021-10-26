%TODO define/know actual meaning of phase offset - make sure input and calc
%phase are same for cases > pi/2

%parameter definitions
clear
freq = 12.5e6;
phase = pi()/4;%in rads
samplerate = 105e6;
num_banks = 4;
rolloff = 0;
N = samplerate/(2*freq);
span = 4;%num of symbols of filter
sps = 3*round(N);%num of samples in symbol period
initial_n_offset = num_banks*round(N) + 1;
num_of_data_bits = 100;

%generate square wave w/ 20 bit alternation and then random sequence
input_data(1:20) = [0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1];
input_data(21:120) = randi([0, 1], num_of_data_bits, 1);
input_samples = repelem(input_data, 21);%TODO get rid of magic numbers used to get 4.2 sps
input_samples = circshift(input_samples, -(round(42*phase/(2*pi()))));
input_samples = downsample(input_samples, 5);
if phase > pi()/2%if shift > half of sample, first sample is shifted by one
    input_data = circshift(input_data, -1);
elseif phase >= 3*pi()/2
    input_data = circshift(input_data, -2);
end


disp("Input Phase offset (rads) = ");
disp(phase);

figure
tiledlayout(3,1)
nexttile
stem(input_samples(2:51));
title(["Output of ADC w/ Phase offset = " phase]);

%print random sequence
%disp(input_data(21:120));%use num2str to make prettier?

%correct waveform and apply polyphase filterbank
input_signal = (input_samples - 0.5).*2;

input_signal = resample(input_signal, 4, 1, 2, 9);

nexttile
stem(input_signal(1:200));
title('Upsampled Signal');

fircoefs = rcosdesign(rolloff, span, sps);

for i = 0:(2*round(N))
    for j = 1:num_banks
        
        shiftedinput = circshift(input_signal,-(i*num_banks + j + initial_n_offset));
        bankout = abs(shiftedinput(1:49).*fircoefs);
        banksum(i*num_banks + j) = sum(bankout);

    end
end

% filterphaseoffset = (0:15)./15;
% filterphaseoffset(16:31) = 1+(0:15)./15;


% figure
% stem(filterphaseoffset, banksum(1:31));
% title('Sum of shifted filter output (x(n)*h(n-k))');
% ylabel('x(n)*h(n-k)');
% xlabel('k/N (fraction of symbol)');

%find phase offset
[maxval, index] = min(banksum);%use averaging?
best_sample = round(index/num_banks);
best_bank = mod(index, num_banks);
n_offset =  best_sample*round(N) + best_bank;
phase = (n_offset/(round(N)*num_banks))*pi() - pi()/2;%assuming 1 symbol = pi phase

%do the sampling
for i = 0:(20+num_of_data_bits - 1)
    if input_signal(round((i)*N*num_banks) + n_offset) >= 0 %rounding issues due to N?
        output(i+1) = 1;
        selected_points_x(i+1) = round((i)*N*num_banks) + n_offset;%for later infographic
    else
        output(i+1) = 0;
        selected_points_x(i+1) = round((i)*N*num_banks) + n_offset;%for later infographic
    end
end

%plot selected sample point & print phase offset
nexttile
hold on 
stem(input_signal(1:200));
stem(selected_points_x(1:12), (output(1:12)-0.5).*2, 'LineWidth',2);
legend('Input signal (Oversampled)', 'Selected data sample points');
title('Optimal Sampling Points');
hold off
disp("Calculated Phase offset (rads) = ");
disp(abs(phase));

%print decoded sequence
disp("Generated Input Data = ");
disp(num2str(input_data));
disp("Decoded sequence = ");
disp(num2str(output));

%compare decoded sequence
if(isequal(input_data, output))
    disp("All bits successfully sampled");
else
    disp("Unsuccessful sampling");
end