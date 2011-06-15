Fs = 8000;                    % Sampling frequency
Fo1 = 150;                     % Sample time
Fo2 = 250;
Fo3 = 55;
L = Fs*5;                     % Length of signal
%j=0:1:L;
%x = 0.7*cos(2*pi*(Fo1/Fs)*j)+0.9*sin(2*pi*(Fo2/Fs)*j)+0.6*sin(2*pi*(Fo3/Fs)*j);

x =wavrecord(L,Fs,'int16');
NFFT = 2^nextpow2(L); % Next power of 2 from length of y
Y = fft(x,NFFT)/L;
f = Fs/2*linspace(0,1,NFFT/2+1);

% Plot single-sided amplitude spectrum.
plot(f,2*abs(Y(1:NFFT/2+1))) 
title('Single-Sided Amplitude Spectrum of y(t)')
xlabel('Frequency (Hz)')
ylabel('|Y(f)|'),