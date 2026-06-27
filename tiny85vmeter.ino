// ================= PINOUT =================
#define DATA_PIN   0   // PB0
#define CLOCK_PIN  1   // PB1
#define LATCH_PIN  3   // PB3
#define AUDIO_PIN  A1  // PB2
#define BUTTON_PIN 4   //PB4

uint8_t mode = 0;
uint8_t lastButton = HIGH;

// ================= CONFIG =================
#define LEDS 16

#define LEVEL1_MIN_ADC 120
#define ADC_MAX_TARGET 980

#define PEAK_HOLD_TIME 40

#define FALL_FAST 3
#define FALL_SLOW 1
#define FAST_ZONE 5

// ================= VARIABILI =================
uint8_t targetLevel  = 0;
uint8_t displayLevel = 0;

uint8_t peakLevel    = 0;
uint16_t peakTimer   = 0;

uint16_t adcMax = 600;

// ================= SCALA LOG 16 =================
const uint16_t logTable[LEDS] = {
  120, 150, 180, 220,
  270, 330, 400, 480,
  570, 670, 760, 830,
  880, 900, 910, 920
};


// ================= 74HC595 =================
void shiftOut595(uint16_t data) {
  digitalWrite(LATCH_PIN, LOW);
  for (int i = 15; i >= 0; i--) {
    digitalWrite(CLOCK_PIN, LOW);
    digitalWrite(DATA_PIN, (data >> i) & 1);
    digitalWrite(CLOCK_PIN, HIGH);
  }
  digitalWrite(LATCH_PIN, HIGH);
}

// ================= SETUP =================
void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

// ================= LOOP =================
void loop() {

  uint8_t button = digitalRead(BUTTON_PIN);

if (lastButton == HIGH && button == LOW) {
  mode++;
  if (mode > 1) mode = 0;
  delay(200);   // debounce semplice
}

lastButton = button;

  // ----- ADC + smoothing -----
  static uint16_t smooth = 0;
  uint16_t adc = analogRead(AUDIO_PIN);
  smooth = (smooth * 7 + adc) / 8;

  // ----- AGC lento -----
  if (smooth > adcMax) {
    adcMax = smooth;
  } else if (adcMax > 600) {
    adcMax--;
  }

  // ----- normalizzazione -----
  uint16_t scaled = smooth;
  if (adcMax > 0) {
    scaled = (uint32_t)smooth * ADC_MAX_TARGET / adcMax;
  }
  if (scaled > ADC_MAX_TARGET) scaled = ADC_MAX_TARGET;

  // ----- livello log -----
  uint8_t level = 0;

  if (scaled >= LEVEL1_MIN_ADC) {
    for (uint8_t i = 0; i < LEDS; i++) {
      if (scaled >= logTable[i]) level = i + 1;
    }
  }

  targetLevel = level;

  // ----- decadimento viscoso -----
  if (targetLevel > displayLevel) {
    displayLevel = targetLevel;
  }
  else if (targetLevel < displayLevel) {
    uint8_t delta = displayLevel - targetLevel;
    displayLevel -= (delta > FAST_ZONE) ? FALL_FAST : FALL_SLOW;
    if (displayLevel < targetLevel)
      displayLevel = targetLevel;
  }

  // ----- peak hold -----
  if (displayLevel == 0) {
    peakLevel = 0;
    peakTimer = 0;
  }
  else if (displayLevel > peakLevel) {
    peakLevel = displayLevel;
    peakTimer = PEAK_HOLD_TIME;
  }
  else if (peakTimer > 0) {
    peakTimer--;
  }
  else if (peakLevel > 0) {
    peakLevel--;
  }

uint16_t leds = 0;

if (mode == 0) {

  // ----- modalità barra normale -----
  uint16_t barMask  = (displayLevel > 0) ? ((1UL << displayLevel) - 1) : 0;
  uint16_t peakMask = (peakLevel > 0) ? (1UL << (peakLevel - 1)) : 0;

  leds = barMask | peakMask;
}

else if (mode == 1) {

  // ----- modalità espansione dal centro -----
  int centerL = 7;
  int centerR = 8;

  for (int i = 0; i < displayLevel; i++) {

    int left  = centerL - i;
    int right = centerR + i;

    if (left >= 0)  leds |= (1 << left);
    if (right < 16) leds |= (1 << right);
  }
}

  shiftOut595(leds);

  delay(10);
}
