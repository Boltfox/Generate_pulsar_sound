#Pulse Profile to Sound Converter
This program converts a pulse profile from pulsars to sound. The pulse profile data is read from a file called generated_pulseprofile. The program generates a .wav file called waveform.wav which contains the converted sound.

##Dependencies
This program requires the following libraries:

-iostream
-cmath
-fstream
-string
-vector
-sstream
-algorithm

## Compiling and Running

###To compile the program, use the following command:
g++ main.cpp -o pulse_to_sound

##To run the program, use the following command:

'./pulse_to_sound'


##Configuration
The following constants can be modified to change the behavior of the program:

-sampleRate: the sample rate of the generated .wav file (in Hz)
-bitDepth: the bit depth of the generated .wav file
-period: the period of the pulse profile (in seconds)
-duration: the duration of the generated .wav file (in seconds)

##Future Improvements
-Add support for flexible number of channels
-Implement semi-realistic dispersion
-ETC 

##Author
Jompoj Wongphechauxsorn
