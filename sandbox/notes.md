# Notes

## Part A: Hardware
We are using a micro:bit, v2.
1) Where is the microphone?
    - [General specifications](https://tech.microbit.org/hardware/)
    - [Slightly more detailed explanation of v2](https://support.microbit.org/support/solutions/articles/19000119052-details-of-micro-bit-v2)
    - [Detailed schematic (linked from general specifications)](https://github.com/microbit-foundation/microbit-v2-hardware/blob/main/V2/MicroBit_V2.0.0_S_schematic.PDF)
    - V2 pinmap (GPIO on nRF52833)
        - Input: P0.05 (MIC_IN)
            - Also known as Pin K2 or AIN3 in [the schematic for the nRF52833 Infocenter](https://infocenter.nordicsemi.com/pdf/nRF52833_OPS_v0.7.pdf)
        - Power supply: P0.20 (RUN_MIC)
            - Also known as Pin AD16
    - Access through `codal-microbit-v2` [here](https://github.com/lancaster-university/codal-microbit-v2/blob/master/model/MicroBit.h)