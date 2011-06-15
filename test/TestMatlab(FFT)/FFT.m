
hx=[
1348408,
1348408,
1348408,
1348408,
1348408,
1348408,
1348408,
1348408,
1348408,
1348408,
1348408,
1348408,
1348408,
1348408,
1348408,
1348408,
1348408    
];
L= length(hx);
Fs=8800;

NFFT = 2^nextpow2(L); % Next power of 2 from length of y
Y = fft(hx,NFFT)/L;
f = Fs/2*linspace(0,1,NFFT/2+1);

% Plot single-sided amplitude spectrum.
plot(f,2*abs(Y(1:NFFT/2+1))) 
title('Single-Sided Amplitude Spectrum of y(t)')
xlabel('Frequency (Hz)')
ylabel('|Y(f)|')
