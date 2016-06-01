# MSP430_EEProm_25LC320A
This is a simple C driver for the 25LC320A EEProm. This driver was designed for the MSP432 and uses the built in USCI hardware.

## Notes
The clock speed of the MSP432 was too high for this EEProm so every time a read or write operation is performed the clock speed must be lowered.
To use this library, you will need to make changes to the files to support the reduced clock speeds.

