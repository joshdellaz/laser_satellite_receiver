% In this program I have described a very simple yet effective way of
% applying a costas loop in receiving a DSB-SC signal with unknown phase
% offset in carrier. The basic contruction of a costas receiver includes
% Low pass filters and Voltage controlled oscillator with central frequency
% same as carrier frequency.
% 
% For applying a low pass filter a simple Integrator can be used. In
% digital domain integrator can be replaced by summator and hence I have
% replaced integrator with summator which generate output summing previous
% Tc/Ts samples, where Tc is period of carrier wave
%                      Ts is sampling time
% The reason is we are considering integrator as y(t) = int(x(t)) from 0 to
% Tc. For this approximate lowpass filter best bandwidth achieved is 200Hz 
% so consider this before designing your message.
% The VCO operation can be approximated as 
% phi(i) = phi(i-1) - sign(.25*m(t)*sin(phi(i-1))*cos(phi(i-1)));
% That is, if phase of carrier is greater than VCO phase then we increase
% phase in next iteration and vice versa.
% After doing iteration all over sample we will get our desired message
% signal and will get initial phase offset applied to input carrier.
% ------------------------------------------------------------------------
% For any suggestions please contact :
% gauravgg@iitk.ac.in
% gaurav71531@gmail.com
% ------------------------------------------------------------------------
% --------------------------GENERATOR PART--------------------------------
% Lets generate a sample input signal with known phase offset or you can
% use your own signal 
t = 0:16000;                 % time scale
fs = 4e5;                    % Sampling Frequency
fc = 4000;                   % Carrier Frequency 

m = sin(2*pi*150*t/fs);      % sample message signal
c = cos(2*pi*fc*t/fs + pi/3); % carrier signal with phase offset

st = m.*c;                   % DSB-SC signal
% -----------------------------------------------------------------------

% ---------------------------RECEIVER PART-------------------------------
N = length(st);              
t = 0:1:N-1;                 % Time vector
phi = zeros(1,N);            % Phase vector of VCO initialize
s1 = zeros(1,N); 
s2 = zeros(1,N);
y1 = zeros(1,N);
y2 = zeros(1,N);

for i = 1:N
    
    if i>1
% The step in which phase is changed is pi*5*10*-5, it can be varied.        
       
        phi(i) = phi(i-1) - (5*10^-5)*pi*sign(y1(i-1)*y2(i-1));
    end
    
    s1(i) = st(i) * cos(2*pi*fc*t(i)/fs  + phi(i));
    s2(i) = st(i) * sin(2*pi*fc*t(i)/fs  + phi(i));

% -----------------------INTEGRATOR------------------------------------
    if i<=100
%  If sample index is less than 100 (Tc/Ts) then we sum available previous
%  samples
        for j=1:i
            y1(i) = y1(i) + s1(j);
            y2(i) = y2(i) + s2(j);
        end
      
    else
% Summing previous 100 (Tc/Ts) values        
        for j = i-99:i
            y1(i) = y1(i) + s1(j);
            y2(i) = y2(i) + s2(j);
        end
    end
%----------------------------------------------------------------------    
end

figure;
plot(t,y1);title('Output signal');
Xlabel('Time');Ylabel('Amplitude');
figure;
plot(t,phi);title('phase for Signal vs time');
Xlabel('Time');Ylabel('Phase');

%For getting initial phase of carrier wave we approximate it with final
%value of phase attained by our VCO

phase = phi(end);
disp(phase);