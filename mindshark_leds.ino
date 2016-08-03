#include <OctoWS2811.h>

const int ledsPerStrip = 150;
DMAMEM int displayMemory[ledsPerStrip * 6];
int drawingMemory[ledsPerStrip * 6];
OctoWS2811 leds(
  ledsPerStrip,
  displayMemory,
  drawingMemory,
  WS2811_GRB | WS2811_800kHz
);

void setup() {
  // Normally, Serial.read*() calls are going to block for up to
  // a whole second if they don't get the length of data they want.
  // This makes them timeout faster, which helps avoid long blocking
  // calls.
  Serial.setTimeout(50);
  leds.begin();
  for (int i = 0; i < leds.numPixels(); i++) {
    leds.setPixel(i, 0x000000);
  }
  leds.show();
}

// we wait for two bytes.
//  if we get a 0, we exit (return false)
//  if we get a 1, we flush
//  if we get a n that is neither 0 nor 1, then:
//   - LED index = n - 2
//   - we wait for 3 bytes specifying color
void loop() {
  char bytes[5];
  int bytes_read = Serial.readBytes(bytes, 5);
  if (bytes_read != 5) {
    // Still waiting...
    return;
  }
  int first = bytes[0] | (bytes[1] << 8);
  if (first == 0) {
    if (!leds.busy()) {
      leds.show();
    }
    return;
  }
  int LED_index = first - 1;
  int color = bytes[2]
    | (bytes[3] << 8)
    | (bytes[4] << 16);
  leds.setPixel(LED_index, color);
  return;
}

void flashColor(int color) {
  for (int i = 0; i < 8 * ledsPerStrip; i++) {
    leds.setPixel(i, color);
  }
  leds.show();
}

