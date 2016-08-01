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
  char first_bytes[2];
  int bytes_read = Serial.readBytes(first_bytes, 2);
  if (bytes_read != 2) {
    // Still waiting...
    return;
  }
  int first = first_bytes[0] | (first_bytes[1] << 8);
  if (first == 0) {
    // This used to mean something
    return;
  }
  if (first == 1) {
    if (!leds.busy()) {
      leds.show();
    }
    return;
  }
  int LED_index = first - 2;
  char color_bytes[3];
  bytes_read = Serial.readBytes(color_bytes, 3);
  if (bytes_read != 3) {
    // Should never happen, would be bad...
    return;
  }
  int color = color_bytes[0]
    | (color_bytes[1] << 8)
    | (color_bytes[2] << 16);
  leds.setPixel(LED_index, color);
  return;
}

void flashColor(int color) {
  for (int i = 0; i < 8 * ledsPerStrip; i++) {
    leds.setPixel(i, color);
  }
  leds.show();
}

