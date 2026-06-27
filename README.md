ATtiny85 Mono VU Meter is audio level meter based on the ATtiny85 microcontroller. The input audio signal is conditioned by an LM358 amplifier and sampled by the ATtiny85. Two cascaded 74HC595 shift registers drive a 16-LED bar graph, providing an accurate and visually pleasing level indication.

The firmware includes several features to improve the display quality, including automatic gain control (AGC), logarithmic scaling to better match human hearing, digital smoothing to reduce flicker, a viscous (slow decay) effect for natural LED movement, and a peak hold function that briefly retains the highest detected level.

## Features

- ATtiny85 microcontroller
- Mono audio input
- LM358 analog front-end
- 16-LED bar graph display
- Two cascaded 74HC595 shift registers
- Automatic Gain Control (AGC)
- Logarithmic level scaling
- Digital smoothing
- Viscous (slow decay) LED effect
- Peak hold indication
- Adjustable input sensitivity
- Low component count

<p align="center">
  <img src="images/atiny85v-meter.png" width="600">
</p>
