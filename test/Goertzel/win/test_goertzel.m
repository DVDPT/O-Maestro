

function test_goertzel()

clc

N = 20;
A = 3;

n = 0 : 1 : N-1;
fprintf(' Integer sinusoid: \n');
x = fix( A*sin( ((2*pi*440)*n) / 8800 ) )
Px = sum( x.^2 )
Xk = abs(goertzel(x, [2]))


fprintf(' Float sinusoid: \n');
x  =  A*sin( ((2*pi*440)*n) / 8800 ) 
Px = sum( x.^2 ) 
Xk = abs(goertzel(x, [2]))
abs( fft(x)) 
return
