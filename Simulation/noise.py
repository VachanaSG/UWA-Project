import numpy as np
import matplotlib.pyplot as plt
# Noise model
# fq: max frequency in Hz
# n : number of points
# w : wind speed
# s : shipping factor 0 to 1
def noise(fq, n, w, s): 
    f = np.linspace(1, int(fq), int(n))/1e3 # divide by 1000 to convert Hz to kHz
    nt = 17 - 30*np.log(f) # turbulence noise
    ns = 40 + 20*(s - 0.5) + 26*np.log(f) - 60*np.log(f+0.03) #ships noise
    nw = 50 + 7.5*(w**0.5) + 20*np.log(f) - 40*np.log(f+0.04) # wind noise
    nth = -15 + 20*np.log(f) # thermal noise
    tot_noise = nt + ns + nw + nth # total noise in db per Hz
    temp = np.power(10,tot_noise/20)
    phase = 2*np.pi*np.random.randn(n)
    mag = np.sqrt(temp)
    FFT = mag * np.exp(1j*phase)
    noise = np.real(np.fft.ifft(FFT))
    plt.figure(figsize=(20,10))
    plt.stem(tot_noise) # plotting psd of noise
    plt.figure(figsize=(20,10))
    plt.plot(noise) # plotting noise

samp_freq = 40000
wind_speed = 3.6
ship_fac = 0.5 
noise(samp_freq/2-1, samp_freq, wind_speed, ship_fac)
plt.show()