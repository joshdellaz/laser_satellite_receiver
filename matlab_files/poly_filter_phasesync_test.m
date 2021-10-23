close all
amp = 1;
freq = 12.5e6;
phase = 3*pi()/8;
samplerate = 100e6;
num_banks = 4;
rolloff = 0;
N = samplerate/(2*freq);
samples_offset = -num_banks*N*phase/(2*pi());
span = 4;%num of symbols of filter
sps = 3*N;%num of samples in symbol period


sine1 = dsp.SineWave('Frequency', freq, 'PhaseOffset', phase, 'SampleRate', samplerate);
sine1.SamplesPerFrame = 100;
input = sine1();

input = upsample(input, 4);
input = input';

%default = sqrt cosine. Change?
fircoefs = rcosdesign(rolloff, span, sps);

figure();
stem(input(1:50));
title('Oversampled input signal');
ylabel('r(n)');
xlabel('n');

figure();
x = -24:24;
stem(x, fircoefs);
title('Unshifted Matched Filter');
ylabel('Amplitude');
xlabel('n');

for i = 1:(2*N)
    for j = 1:num_banks
        
        shiftedinput = circshift(input,-(i*num_banks + j + samples_offset));
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


