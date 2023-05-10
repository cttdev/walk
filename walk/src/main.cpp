#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <TeensyThreads.h>
#include <util.h>

#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include <FastLED.h>

#include <vector>

// Display
// Use pins: 2, 14, 7, 8
const int numPins = 4;
byte pinList[numPins] = {2, 14, 7, 8};

const int chipSelect = BUILTIN_SDCARD; 

// Display Dimensions
const int displayWidth = 32;
const int displayHeight = 32;

const int brightness = 10;

const int ledsPerStrip = 256;

// These buffers need to be large enough for all the pixels.
// The total number of pixels is "ledsPerStrip * numPins".
// Each pixel needs 3 bytes, so multiply by 3.  An "int" is
// 4 bytes, so divide by 4.  The array is created using "int"
// so the compiler will align it to 32 bit memory.
const int bytesPerLED = 3;  // change to 4 if using RGBW
DMAMEM int displayMemory[ledsPerStrip * numPins * bytesPerLED / 4];
int drawingMemory[ledsPerStrip * numPins * bytesPerLED / 4];

const int config = WS2811_GBR | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config, numPins, pinList);

// Frame Buffer
std::vector<std::vector<std::vector<int>>> frameBuffer;
std::mutex frameBufferLock;

// FPS
int fps = 30.0;
elapsedMicros frameTimer;

// File to read the frames from
std::vector<File> files;

void display_thread_function() {
  while (true) {
    {
      std::lock_guard<std::mutex> lock(frameBufferLock);
      if (frameBuffer.size() < 2) {
        // Serial.println("Buffering Frames...");
      } else {
        break;
      }
    }
    threads.delay(5);
  }

  while (true) {
    frameTimer = 0;

    // Print the first frame in the buffer
    {
      std::lock_guard<std::mutex> lock(frameBufferLock);
      if (frameBuffer.size() != 0) {
        // Unwrap the frame
        std::vector<std::vector<std::vector<int>>> frame = unwrap(frameBuffer[0], displayWidth, displayHeight);

        // Display the frame
        for (unsigned int i = 0; i < frame.size(); i++) {
          for (unsigned int j = 0; j < frame[i].size(); j++) {
            const unsigned int color = Color(frame[i][j][0] * brightness / 255, frame[i][j][1] * brightness / 255, frame[i][j][2] * brightness / 255);
            leds.setPixel(get_led_index(i, j, displayWidth, 8), color);
          }
        }

        leds.show();

        // Remove the frame from the buffer
        frameBuffer.erase(frameBuffer.begin());
      }
    }

    // Wait for the next frame
    while (frameTimer < 1000000.0 / fps);
  }
}

void file_reader_thread_function() {
  // Choose the first file in the list, loop back to the beginning if we reach the end
  int fileIndex = 0;

  while (true) {
    // Open the file
    File file = files[fileIndex];
    file.seek(0);
    fileIndex = (fileIndex + 1) % files.size();

    // Read the file into the frame buffer
    while (file.available()) {
      std::vector<std::vector<int>> frame;

      // Read the next line
      std::string line = file.readStringUntil('\n').c_str();

      // Remove the newline character
      line = line.substr(0, line.length() - 1);

      // Extract the pixels from the line, the pixels are stored row-major
      std::vector<std::string> pixels = split(line, ' ');

      // For each pixel extract the RGB values
      for (int i = 0; i < pixels.size(); i++) {
        // Remove the brackets from the pixel
        std::vector<int> pixel;
        std::vector<std::string> rgb = split(pixels[i].substr(1, pixels[i].length() - 2), ',');

        // Add the RGB values to the row
        for (int j = 0; j < rgb.size(); j++) {
          pixel.push_back(std::stoi(rgb[j]));
        }

        // Add the pixel to the frame
        frame.push_back(pixel);
      }

      // Wait for the frame buffer to have space
      while (true) {
        {
          std::lock_guard<std::mutex> lock(frameBufferLock);
          if (frameBuffer.size() < 2) {
            break;
          }
        }
        threads.delay(10);
      }

      {
        // Add the frame to the frame buffer
        std::lock_guard<std::mutex> lock(frameBufferLock);
        frameBuffer.push_back(frame);
      }
    }
  }
}

void setup() {
  // Serial.begin(9600);

  // // Wait for the serial port to be opened
  // while (!Serial) {
  //   threads.delay(10);
  // }

  // Initialize the display
  leds.begin();
  leds.show();

  // if (CrashReport)
  // {
  //     Serial.print(CrashReport); // Once called any crash data is cleared
  // }

  // Initialize SD card
  // Serial.println("Initializing SD card...");

  if (!SD.begin(BUILTIN_SDCARD)) {
    // Serial.println("Initialization failed!");
    while (1);
  }
  // Serial.println("Initialization done.");

  // Get all the video files on the SD card
  File root = SD.open("/videos");
  while (true) {
    File entry = root.openNextFile();

    if (!entry) {
      // No more files
      break;
    }

    if (entry.isDirectory()) {
      // Skip directories
      continue;
    }

    // Serial.println(entry.name());
    files.push_back(entry);
  }

  // Start the threads
  // threads.addThread(display_thread);
  std::thread display_thread(display_thread_function);
  std::thread file_reader_thread(file_reader_thread_function);
  file_reader_thread.join();
}

FLASHMEM void loop() {}
