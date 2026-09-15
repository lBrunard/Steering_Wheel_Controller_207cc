#include <Arduino.h>

// --- Sortie : pilote la LED IR interne du PC817C (via 220 Ω), comme sur le schéma ---
#define PIN_VOL_PLUS   25
#define PIN_VOL_MINUS  26

int pins[] = {PIN_VOL_MINUS, PIN_VOL_PLUS};
const int NB_PINS = sizeof(pins) / sizeof(pins[0]);

// Éteint tous les optos, puis allume `pin` si actif (pin = -1 → tout éteindre).
// Un seul opto à la fois : sinon les résistances se retrouvent en parallèle.
void setOpto(bool actif, int pin) {
  for (int i = 0; i < NB_PINS; i++) {
    digitalWrite(pins[i], LOW);
  }
  if (pin != -1) {
    digitalWrite(pin, actif ? HIGH : LOW);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  for (int i = 0; i < NB_PINS; i++) {
    pinMode(pins[i], OUTPUT);
  }
  setOpto(false, -1);

  Serial.println("=== Test optocoupleurs VOL+ (GPIO25) / VOL- (GPIO26) ===");
  Serial.println("Commandes :");
  Serial.println("  2 → VOL- maintenu (opto allumé)");
  Serial.println("  1 → VOL+ maintenu (opto allumé)");
  Serial.println("  0 → ALL OFF(opto éteint)");
}

void loop() {
  if (!Serial.available()) return;
  char cmd = Serial.read();

  switch (cmd) {
    case '2':
      setOpto(true, PIN_VOL_MINUS);
      Serial.println("[VOL-] MAINTENU, (attendu ≈ 4,85 kΩ)");
      break;

    case '1':
      setOpto(true, PIN_VOL_PLUS);
      Serial.println("[VOL+] MAINTENU, (attendu ≈ 2,3 kΩ)");
      break;

    case '0':
      setOpto(false, -1);
      Serial.println("ALL REALEASE");
      break;

  }
}
