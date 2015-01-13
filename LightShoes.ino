#include <Adafruit_NeoPixel.h>

//define our animation types
const int ANIM_COLOR_WIPE = 1;
const int ANIM_HEAL_TO_TOE = 2;
const int ANIM_TOE_TO_HEAL = 3;
const int ANIM_OUTSIDE_TO_INSIDE = 4;
const int ANIM_INSIDE_TO_OUTSIDE = 5;
const int ANIM_SEIZURE = 6;
const int ANIM_SOLID_COLOR = 7;
const int ANIM_FLAG_ESTONIA = 8;
const int ANIM_FLAG_USA = 9;
const int ANIM_FLAG_ELECTROBATION = 10;

//set if we want to use a random animation or if
//we will cycle through each of the set animations one by one
const boolean RANDOM_ANIMATION = true;
//last animation starts at none. if we use random animation, we don't want to repeat the same animation over and over
int lastAnimation = -1;

//IMPORTANT! This array will be all the animations that are used.
//If RANDOM_ANIMATION is true, then a random ANIM_ will be taken from this array and will be displayed
const int ANIMATIONS[] = {
  ANIM_HEAL_TO_TOE, ANIM_TOE_TO_HEAL, ANIM_OUTSIDE_TO_INSIDE, ANIM_INSIDE_TO_OUTSIDE
};

const int ANIMATIONS_COUNT = (sizeof(ANIMATIONS) / sizeof(int));

const int LED_COUNT = 45;
const int THRESHOLD = 24;//left shoe
//const int THRESHOLD = 35;//right shoe
const int ANALOG_IN_PIN = A9;
const int LED_OUT_PIN = 6;
const int DELAY = 20;
const int HEAL_PIXEL = 4;
const int TOE_PIXEL = 26;
const int OUTSIDE_PIXEL = 37;
const int INSIDE_PIXEL = 15;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_OUT_PIN, NEO_GRB + NEO_KHZ800);

const int COLORS_COUNT = 8;
const int PATTERN_COLOR_COUNT = 3;

const uint32_t OFF = strip.Color(0, 0, 0);

const uint32_t RED = strip.Color(255, 0, 0);//red
const uint32_t PINK = strip.Color(255, 0, 255);//pink
const uint32_t BLUE = strip.Color(0, 0, 255);//blue
const uint32_t TEAL = strip.Color(0, 255, 255);//teal
const uint32_t GREEN = strip.Color(0, 255, 0);//green
const uint32_t YELLOW = strip.Color(255, 255, 0);//yellow
const uint32_t ORANGE = strip.Color(255, 128, 0);//orange
const uint32_t WHITE = strip.Color(255, 255, 255);//white

const uint32_t COLORS[COLORS_COUNT] = {
  RED, PINK, BLUE, TEAL, GREEN, YELLOW, ORANGE, WHITE,
};

const uint32_t PATTERN_ESTONIA[PATTERN_COLOR_COUNT] = {
  BLUE,
  OFF,
  WHITE,
};

const uint32_t PATTERN_USA[PATTERN_COLOR_COUNT] = {
  RED,
  WHITE,
  BLUE,
};

const uint32_t PATTERN_ELECTROBATION[PATTERN_COLOR_COUNT] = {
  WHITE,
  OFF,
  YELLOW,
};

int lastColorIndex = -1;

void setup() {
  Serial.begin(9600);

  pinMode(9, INPUT_PULLUP);

  randomSeed(analogRead(A10));

  strip.begin();
  strip.show();
}

void loop() {
  int sensorValue = analogRead(ANALOG_IN_PIN);

  Serial.print("sensor = " );
  Serial.println(sensorValue);

  if (sensorValue > THRESHOLD) {
    //pick an animation at random but don't use the last used animation
    if (RANDOM_ANIMATION) {
        int animation = -1;
        
        while (true) {
          animation = random(ANIMATIONS_COUNT);
      
          if (animation != lastAnimation) {
            break;
          }
        }
      
        lastAnimation = animation;
        
        doAnimation(ANIMATIONS[animation]);
    }
    else {
      //cycle through each animation in the list
      for (int i = 0; i < ANIMATIONS_COUNT; i++) {
        doAnimation(ANIMATIONS[i]);
      }
    }
  }
  else {
    clearLights();
  }
}

void doAnimation(int animation) {
  switch (animation) {
      case ANIM_FLAG_ESTONIA:
        patternCircle(PATTERN_ESTONIA, DELAY);
        break;
      case ANIM_FLAG_USA:
        patternCircle(PATTERN_USA, DELAY);
        break;
      case ANIM_FLAG_ELECTROBATION:
        patternCircle(PATTERN_ELECTROBATION, DELAY);
        break;
      case ANIM_COLOR_WIPE:
        colorWipe(getColor(), DELAY);
        break;
      case ANIM_HEAL_TO_TOE:
        expand(getColor(), DELAY, HEAL_PIXEL);
        break;
      case ANIM_TOE_TO_HEAL:
        expand(getColor(), DELAY, TOE_PIXEL);
        break;
      case ANIM_OUTSIDE_TO_INSIDE:
        expand(getColor(), DELAY, INSIDE_PIXEL);
        break;
      case ANIM_INSIDE_TO_OUTSIDE:
        expand(getColor(), DELAY, OUTSIDE_PIXEL);
        break;
      case ANIM_SEIZURE:
        seizure(DELAY);
        break;
      case ANIM_SOLID_COLOR:
        solidColor(getColor(), DELAY);
        break;
  }
}

void colorWipe(uint32_t color, int wait) {
  Serial.print("colorWipe - color: ");
  Serial.print(color);
  Serial.print(" wait: ");
  Serial.println(wait);

  int pixelCount = strip.numPixels();
  uint16_t pixel;

  for (uint16_t i = 0; i < pixelCount; i++) {
    pixel = getPixel((i + HEAL_PIXEL), pixelCount);

    strip.setPixelColor(pixel, color);
    strip.show();
    delay(wait);
  }
}

void expand(uint32_t color, int wait, int startPixel) {
  Serial.print("expand - color: ");
  Serial.print(color);
  Serial.print(" wait: ");
  Serial.print(wait);
  Serial.print(" startPixel: ");
  Serial.println(startPixel);

  int pixelCount = strip.numPixels();
  int pixel;
  int oppositePixel;

  for (uint16_t i = 0; i <= (pixelCount / 2); i++) {
    pixel = getPixel((startPixel + i), pixelCount);

    if (pixel != startPixel) {
      oppositePixel = getPixel((startPixel - i), pixelCount);

      strip.setPixelColor(pixel, color);
      strip.setPixelColor(oppositePixel, color);
    }
    else {
      strip.setPixelColor(pixel, color);
    }

    strip.show();
    delay(wait);
  }
}

void seizure(int wait) {
  Serial.print("seizure - wait: ");
  Serial.println(wait);

  uint32_t color;

  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    color = (rand() % (COLORS_COUNT + 1));

    if (color > COLORS_COUNT) {
      color = OFF;
    }
    else {
      color = COLORS[color];
    }
    strip.setPixelColor(i, color);
  }

  strip.show();
  delay(wait);
}

void solidColor(uint32_t color, int wait) {
  Serial.print("solidColor - color: ");
  Serial.print(color);
  Serial.print(" wait: ");
  Serial.println(wait);

  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }

  strip.show();
  delay(wait);
}

void patternCircle(const uint32_t *pattern, int wait) {
  Serial.print("patternCircle - wait: ");
  Serial.println(wait);

  uint8_t patternSize = PATTERN_COLOR_COUNT + 1;
  int patternPosition = (patternSize - 1);

  for (uint8_t y = 0; y < patternSize; y++) {
    for (int i = (strip.numPixels() - 1); i >= 0; i--) {
      if (patternPosition < 0) {
        patternPosition = (patternSize - 1);
      }

      strip.setPixelColor(i, (patternPosition < PATTERN_COLOR_COUNT ? pattern[patternPosition] : OFF));

      patternPosition--;
    }

    strip.show();
    delay(wait);
  }
}

void clearLights() {
  Serial.println("clearLights");

  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, OFF);
  }

  strip.show();
}

uint16_t getPixel(int pixel, int pixelCount) {
  int returnPixel = pixel;

  if (returnPixel >= pixelCount) {
    returnPixel = (pixelCount - returnPixel) * -1;
  }
  else if (returnPixel < 0) {
    returnPixel = pixelCount + returnPixel;
  }

  return returnPixel;
}

uint32_t getColor() {
  int color = -1;

  while (true) {
    color = random(COLORS_COUNT);

    if (color != lastColorIndex) {
      break;
    }
  }

  lastColorIndex = color;

  return COLORS[color];
}
