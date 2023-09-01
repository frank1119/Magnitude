# Magnitude
## Purpose
Provides a simple way to get the magnitude from an audio input and make it reporting back to the host using a normal parameter

## Parameters:
* 0: RefreshRate: The number of reports per second
* 1: Smoothing: The % to smooth the reports: 0% means no smoothing, best peak performance. 99% means less peak, but also less jumpy values
* 2 - 33: 'Bus X Magn. Out Y': The magnitudes of Bux X, Channel 1 thru 16 

## Channels
There are 2 buses. The number of channels for each can be 1, 2, ... 16
