close all
amp = 1;
freq = 12.5e6;
phase = 0;
samplerate = 105e6;
num_banks = 4;
rolloff = 0;
N = samplerate/(2*freq);
Neff = 4;%estimated place holder to make functions happy
samples_offset = -num_banks*Neff*phase/(2*pi());
span = 4;%num of symbols of filter
sps = 3*Neff;%num of samples in symbol period
large_offset = num_banks*round(N) + 1;


% sine1 = dsp.SineWave('Frequency', freq, 'PhaseOffset', phase, 'SampleRate', samplerate);
% sine1.SamplesPerFrame = 100;
% input = sine1();
% 
% input = upsample(input, num_banks);
% input = input';

%default = sqrt cosine. Change?
fircoefs = rcosdesign(rolloff, span, sps);

% figure();
% stem(input(1:50));
% title('Oversampled input signal');
% ylabel('r(n)');
% xlabel('n');

%TODO add sample phase offset and test
t = linspace(0, 419, 420);
t = pi()*t./(N);
sq_input = square(t);
%sq_input = upsample(sq_input, num_banks); %not as good
sq_input = resample(sq_input, 4, 1, 2, 9);

figure
stem(sq_input((1+large_offset):(49+large_offset)));

input = sq_input;


% figure();
% x = -24:24;
% stem(x, fircoefs);
% title('Unshifted Matched Filter');
% ylabel('Amplitude');
% xlabel('n');

for i = 0:(2*N)
    for j = 1:num_banks
        
        shiftedinput = circshift(input,-(i*num_banks + j + large_offset));
        bankout = abs(shiftedinput(1:49).*fircoefs);
        banksum(i*num_banks + j) = sum(bankout);

    end
end

filterphaseoffset = (0:15)./15;
filterphaseoffset(16:31) = 1+(0:15)./15;


figure
stem(filterphaseoffset, banksum(1:31));
title('Sum of shifted filter output (x(n)*h(n-k))');
ylabel('x(n)*h(n-k)');
xlabel('k/N (fraction of symbol)');

