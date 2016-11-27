#include "Adafruit_Pixie.h"

Adafruit_Pixie::Adafruit_Pixie(uint16_t n, Stream *s) :
  numLEDs(n), pixels(NULL), endTime(0), stream(s) {
  if((pixels = (uint8_t *)malloc(n * 4))) {
    memset(pixels, 0, n * 4);
  }
}

Adafruit_Pixie::~Adafruit_Pixie() {
  if(pixels) free(pixels);
}

// No begin() function; instead, just call ser.begin(115200) where 'ser'
// is the previously-opened hardware- or software-serial port for output.

void Adafruit_Pixie::show(void) {
  if(pixels) {
    uint16_t n3 = numLEDs * 4;
    while(!canShow());  // Wait for 1ms elapsed since prior call
    // if(!brightness) {   // No brightness adjust, output full blast
    //   stream->write(pixels, n3);
    // } else {            // Scale back brightness for every pixel R,G,B:
      uint16_t i;
      for(i=0; i<n3; i++) {
        stream->write((pixels[i] * pixels[i+3]) >> 8);
        stream->write((pixels[++i] * pixels[i+2]) >> 8);
        stream->write((pixels[++i] * pixels[i+1]) >> 8);
        i++;
      }
    //}
    endTime = micros(); // Save EOD time for latch on next call
  }
}

// Set pixel color from separate R,G,B, Brightness components:
void Adafruit_Pixie::setPixelColor(
 uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
  if(n < numLEDs) {
    uint8_t *p = &pixels[n * 4];
    p[0] = r;
    p[1] = g;
    p[2] = b;
    p[3] = brightness + 1;
  }
}

// Set pixel color from 'packed' 32-bit RGB color:
void Adafruit_Pixie::setPixelColor(uint16_t n, uint32_t c, uint8_t brightness) {
  if(n < numLEDs) {
    uint8_t r = (uint8_t)(c >> 16),
            g = (uint8_t)(c >>  8),
            b = (uint8_t)c,
           *p = &pixels[n * 4];
    p[0] = r;
    p[1] = g;
    p[2] = b;
    p[3] = brightness + 1;
  }
}

// Convert separate R,G,B into packed 32-bit RGB color.
// Packed format is always RGB, regardless of LED strand color order.
uint32_t Adafruit_Pixie::Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

// Query color from previously-set pixel (returns packed 32-bit RGB value)
uint32_t Adafruit_Pixie::getPixelColor(uint16_t n) const {
  if(n < numLEDs) {
    uint8_t *p = &pixels[n * 4];
    return ((uint32_t)p[0] << 16) |
           ((uint32_t)p[1] <<  8) |
            (uint32_t)p[2];
  } else {
    return 0; // Out of bounds, return no color.
  }
}

// void Adafruit_Pixie::setBrightness(uint8_t b) {
//   // Stored brightness value is different than what's passed.  This
//   // optimizes the actual scaling math later, allowing a fast 8x8-bit
//   // multiply and taking the MSB.  'brightness' is a uint8_t, adding 1
//   // here may (intentionally) roll over...so 0 = max brightness (color
//   // values are interpreted literally; no scaling), 1 = min brightness
//   // (off), 255 = just below max brightness.
//   brightness = b + 1;
// }

// // Return the brightness value
// uint8_t Adafruit_Pixie::getBrightness(void) const {
//   return brightness - 1; // Reverse above operation
// }

void Adafruit_Pixie::clear() {
  memset(pixels, 0, numLEDs * 4);
}
