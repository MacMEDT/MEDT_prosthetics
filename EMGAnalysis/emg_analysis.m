clear;
clc;

%% Load data from file
fileID = fopen('EMGData/bicep_matthew.txt', 'r');
data = fscanf(fileID, '%g%g%g', [3 Inf]);
fclose(fileID);

Fs = 66; % Hz 
% sampling period = 15ms

emgVals = (data(3,:));

%% Time Domain Response
% Create array for time points
t = 0:1/Fs:(length(emgVals)-1)/Fs;

% Plot time domain response
plot_time(t, emgVals, "EMG Time Domain Signal")

%% Fourier Analysis of Original Signal
[M,ph,f] = fourier_dt(emgVals,Fs,'full');
plotFrequency(f, M, ph)

%% Take out the DC offset
% Filtering the signal
% Txt file = DC offset
% bicep_hannah = 2300
% bicep_matthew = 464
% forearm_arjun = 464
offset = 464;
filtered = emgVals-offset;

% Plot filtered signal against original signal
plot_time2(t, emgVals, filtered, "Original", "DC removed")

% computing the frequency response of filtered signal
[M_f,ph_f,f_f] = fourier_dt(filtered,Fs,'full');

% Frequency spectra of new signal
plotFrequency(f_f, M_f, ph_f)

%% Rectification - absolute value
rectify = abs(filtered);
plot_time(t, rectify, "Rectified Time Domain Signal")

%% Trailing average of last 50 points
avg_n = 50;
avg = movmean(rectify, [avg_n-1 0]);
plot_time2(t, rectify, avg, "Rectified", "Averaged")
plot_time2(t, emgVals, avg, "Original", "Processed")
plot_time2(t, filtered, avg, "DC removed", "Processed")

%% Plot frequency spectra
function plotFrequency(f, M, ph)
    % Plot
    figure
    % Magnitude
    subplot(2,1,1)
    plot(f,M, "color", "k")
    ylabel("Magnitude (µV)")
    % Phase
    subplot(2,1,2)
    plot(f,ph, "color", "k")
    ylabel("Phase (rad)")

    xlabel("Frequency (Hz)")
    sgtitle("EMG Frequency Spectra")
end

%% Plot one series
function plot_time(t, series, titl)
    figure
    plot(t, series, "color", "b")
    title(titl)
    xlabel("Time (s)")
    ylabel("Voltage (µV?)")
    xlim([0 t(end)])
end

%% Plot two series
function plot_time2(t, series1, series2, label1, label2)
    figure
    plot (t, series1, "color", "m")
    hold on
    plot (t, series2, "color", "k")
    xlim([0 t(end)])
    ylabel("Voltage (µV)")
    xlabel("time (t)")
    legend(label1, label2)
    title("EMG Time Domain Signal")
    hold off
end