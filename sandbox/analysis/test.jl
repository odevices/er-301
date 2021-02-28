
##
using Plots
using FFTW
using DSP

##
# Number of points
N = 2^14 - 1
# Sample period
Ts = 1 / (1.1 * N)
# Start time
t0 = 0
tmax = t0 + N * Ts
# time coordinate
t = t0:Ts:tmax

# signal
signal = sin.(2π * 60 .* t) # sin (2π f t)

# Fourier Transform of it
F = fft(signal) |> fftshift
w = fftfreq(length(t), 1.0/Ts) |> fftshift

# plots
time_domain = plot(t, signal, title = "Signal")
freq_domain = plot(w, abs.(F), title = "Spectrum", xlim=(-1000, +1000))
plot(time_domain, freq_domain, layout = 2)

##
f(x,y) = x + y

function g(x,y)
    x * y
end
