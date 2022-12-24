// This program converts a pulse profile from pulsars to sound. 
// The main motivation for this project is to refresh my knowledge of C++.
// Future improvements include: 
// 1) Adding support for flexible number of channels 
// 2) Implementing semi-realistic dispersion 
// 3) Read archive file ?
// 4) Read EPN pulse profile database 

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
using namespace std;

const int sampleRate = 44100;
const int bitDepth = 16;

// Function that reads a file and returns a vector of numbers
auto read_numbers_from_file(const string& filename) {
  // Open the file
  ifstream file(filename);

  // Check if the file was opened successfully
  if (!file.is_open()) {
    cout << "Error: Could not open file " << filename << endl;
    return vector<double>();
  }

  // Create a stringstream to read the contents of the file
  stringstream buffer;
  buffer << file.rdbuf();

  // Read the contents of the file into a string
  string contents = buffer.str();

  // Print the contents of the file
  // cout << "Contents of file: " << contents << endl;

  // Create an empty vector to store the numbers from the file
  vector<double> numbers;

  // Create a stringstream from the contents of the file
  stringstream stream(contents);
  // Read the numbers from the stringstream one by one
  double x;
  while (stream >> x) {
    // Add the number to the vector
    numbers.push_back(x);
  }

  // Return the vector of numbers
  return numbers;
}


// Make sinwave oscillator 
class SineOscillator { 
    float frequency, amplitude, angle = 0.0f, offset = 0.0f;
public:
    SineOscillator(float freq, float amp) : frequency(freq), amplitude(amp) {
        offset = 2 * M_PI * frequency / sampleRate;
    }
    float process() {
        auto sample = amplitude * sin(angle);
        angle += offset;
        return sample;
        // Asin(2pif/sr)
    }
};


void writeToFile(ofstream &file, int value, int size) {
    file.write(reinterpret_cast<const char*> (&value), size);
}
int main() {
    // Constants for audio file generation
    //const int sampleRate = 64;
    //const int bitDepth = 16;

    // Other variables for audio file generation
    double period = 0.3;
    int duration = 1;
    int npulse = duration/period;
    ofstream audioFile;
    audioFile.open("waveform.wav", ios::binary);
    //generate base frequency
    SineOscillator sineOscillator(200, 0.8);
    SineOscillator sineOscillator2(400, 0.8);
    SineOscillator sineOscillator3(600, 0.8);
    SineOscillator sineOscillator4(800, 0.8);
    SineOscillator oscillators[] = { sineOscillator, sineOscillator2, sineOscillator3, sineOscillator4 }; 
    // Header chunk
    audioFile << "RIFF";
    audioFile << "----";
    audioFile << "WAVE";

    // Format chunk
    audioFile << "fmt ";
    writeToFile(audioFile, 16, 4); // Size
    writeToFile(audioFile, 1, 2); // Compression code
    writeToFile(audioFile, 4, 2); // Number of channels
    writeToFile(audioFile, sampleRate, 4); // Sample rate
    writeToFile(audioFile, sampleRate * bitDepth / 8, 4 ); // Byte rate
    writeToFile(audioFile, bitDepth / 8, 2); // Block align
    writeToFile(audioFile, bitDepth, 2); // Bit depth

    // Data chunk
    audioFile << "data";
    audioFile << "----";

    int preAudioPosition = audioFile.tellp();

    auto maxAmplitude = pow(2, bitDepth - 1) - 1;

    //Load ascii text, integrated with EVN pulse profile ascii format soon 
    auto sample = read_numbers_from_file("generated_pulseprofile");
    // 1/63*44100
    int n = sampleRate/sample.size()*period;
    cout<< sample.size()<<" "<<n<<endl;
    //Repeat pulse by npulse 
    for (int i = 0; i < npulse; i++) {
        sample.insert(sample.end(), sample.begin(), sample.end());
    }
    cout<< sample.size()<<" "<<npulse <<endl;

    //Upscale the number of sample and interpolate to make it smooth. This part of the code was generated from ChatGPT
    auto upsampledSample = vector<double>();
    for (int i = 0; i < sample.size() - 1; i++) {
    upsampledSample.push_back(sample[i]);
    for (int j = 1; j < n; j++) {
      double x = static_cast<double>(j) / n;
      double y = sample[i] * (1.0 - x) + sample[i + 1] * x;
      upsampledSample.push_back(y);
    }
  }
for (size_t i = 0; i < sampleRate*duration; i++) {
  int intSamples[] = { 0, 0, 0, 0 };
  for (size_t j = 0; j < 4; j++) {
    int tau_dm=100*(4-j)*(4-j); // fake dispersion 
    // cout<<tau_dm<<endl;
    if (i > tau_dm) {
      intSamples[j] = static_cast<int>(upsampledSample[i - tau_dm] * oscillators[j].process() * maxAmplitude);
    } else {
      intSamples[j] = 0;
    }
  }
  writeToFile(audioFile, intSamples[0], 2);
  writeToFile(audioFile, intSamples[1], 2);
  writeToFile(audioFile, intSamples[2], 2);
  writeToFile(audioFile, intSamples[3], 2);
}

    int postAudioPosition = audioFile.tellp();

    audioFile.seekp(preAudioPosition - 4);
    writeToFile(audioFile, postAudioPosition - preAudioPosition, 4);

    audioFile.seekp(4, ios::beg);
    writeToFile(audioFile, postAudioPosition - 8, 4);

    audioFile.close();
    return 0;
}