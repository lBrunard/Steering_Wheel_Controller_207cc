#include <Arduino.h>

// --- Sorties : simulent les boutons (côté LED du PC817C) ---
#define PIN_SIM_VOL_PLUS  18
#define PIN_SIM_VOL_MINUS 19

// --- Entrées : lisent le transistor du PC817C ---
#define PIN_READ_VOL_PLUS  34
#define PIN_READ_VOL_MINUS 35

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(PIN_SIM_VOL_PLUS,  OUTPUT);
  pinMode(PIN_SIM_VOL_MINUS, OUTPUT);
  pinMode(PIN_READ_VOL_PLUS,  INPUT);
  pinMode(PIN_READ_VOL_MINUS, INPUT);

  // LED éteintes par défaut (boutons relâchés)
  digitalWrite(PIN_SIM_VOL_PLUS,  LOW);
  digitalWrite(PIN_SIM_VOL_MINUS, LOW);

  Serial.println("=== Test Optocouplers PC817C ===");
  Serial.println("Commandes disponibles :");
  Serial.println("  + → Simule appui VOL+");
  Serial.println("  - → Simule appui VOL-");
  Serial.println("  r → Lire l'état des sorties en continu");
}

void simulerAppui(int pinOut, int pinIn, const char* nom) {
  Serial.print("[SIM] Appui sur ");
  Serial.println(nom);

  // Allume la LED → transistor conduit
  digitalWrite(pinOut, HIGH);
  delay(50); // temps de réponse optocoupler

  int lecture = digitalRead(pinIn);
  Serial.print("  → LED allumée, sortie lue : ");
  Serial.println(lecture == LOW ? "LOW ✓ (transistor conduit)" : "HIGH ✗ (problème câblage)");

  delay(300);

  // Éteint la LED → transistor bloque
  digitalWrite(pinOut, LOW);
  delay(50);

  lecture = digitalRead(pinIn);
  Serial.print("  → LED éteinte, sortie lue : ");
  Serial.println(lecture == HIGH ? "HIGH ✓ (transistor bloqué)" : "LOW ✗ (problème câblage)");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == '+') {
      simulerAppui(PIN_SIM_VOL_PLUS, PIN_READ_VOL_PLUS, "VOL+");
    }
    else if (cmd == '-') {
      simulerAppui(PIN_SIM_VOL_MINUS, PIN_READ_VOL_MINUS, "VOL-");
    }
    else if (cmd == 'r') {
      Serial.println("[ETAT ACTUEL]");
      Serial.print("  VOL+  entrée GPIO34 : ");
      Serial.println(digitalRead(PIN_READ_VOL_PLUS)  == LOW ? "LOW" : "HIGH");
      Serial.print("  VOL-  entrée GPIO35 : ");
      Serial.println(digitalRead(PIN_READ_VOL_MINUS) == LOW ? "LOW" : "HIGH");
    }
  }
}
