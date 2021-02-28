##
using Plots
using FFTW
using DSP
sr = 48000
baseFreq = 20
overSamp = 2

maxHarms = floor(Int, sr / (3.0+baseFreq) + 0.5)
v = maxHarms
v -= 1
v |= v >> 1
v |= v >> 2
v |= v >> 4
v |= v >> 8
v |= v >> 16
v += 1
tableLen = v * 2 * overSamp

topFreq = baseFreq * 2 / sr


## sawtooth
nHarms = 32
a = zeros(Float64,tableLen)
for i=1:nHarms
    temp = -1.0/i
    a[i+1]=-temp
    a[tableLen-i+1]=temp
end
# plot(a)

A = fft(a)
plot([real.(A),imag.(A)])

##
shift = 1280
x = imag.(A)
xs = circshift(x,shift)
p1 = plot([x,xs])
X = ifft(xs)
p2 = plot([a[1:32],abs.(X[1:32])])
p3 = plot(unwrap(angle.(X[1:32])))
#p2 = plot([a[1:32],real.(X[1:32]),imag.(X[1:32])],ylims=(-1,1))
plot(p1,p2,p3,layout=(3,1))


##
anim = @animate for shift=range(1,tableLen,step=64)
    x = imag.(A)
    xs = circshift(x,shift)
    p1 = plot([x,xs])
    X = ifft(xs)
    p2 = plot([a[1:nHarms],abs.(X[1:nHarms])])
    p3 = plot(unwrap(angle.(X[1:nHarms])),ylim=(-nHarms*pi,nHarms*pi))
    #p2 = plot([a[1:32],real.(X[1:32]),imag.(X[1:32])],ylims=(-1,1))
    plot(p1,p2,p3,layout=(3,1),size=(600,800))
end

gif(anim,"analysis/test.gif",fps=30)
