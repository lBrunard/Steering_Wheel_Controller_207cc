#include <Arduino.h>
#include <CAN.h>
#include <map>

// =====================================================================
//  Sniffer CAN en ÉCOUTE SEULE : aucune trame émise, aucun ACK envoyé.
//
//  Câblage (schéma) : SN65HVD230  D ← GPIO4 (TX)   R → GPIO5 (RX)
//                                 RS → GND        VCC → 3V3
//  ⚠ Relier le GND de l'ESP32 à la masse de la voiture.
//  ⚠ Pas de résistance 120 Ω : le bus de la voiture est déjà terminé.
// =====================================================================

#define PIN_CAN_RX  5
#define PIN_CAN_TX  4

// ID probable des commandes au volant sur PSA (à confirmer par le sniff)
#define ID_VOLANT   0x21F

// Délai pour la commande 'i' : les IDs qui ont changé dans cette fenêtre sont masqués
#define FENETRE_IGNORE_MS  3000
#define PERIODE_BILAN_MS   10000

struct Trame {
  uint8_t  data[8];
  uint8_t  len;
  uint32_t compteur;
  unsigned long dernierChangement;
  bool     ignoree;
};

std::map<uint32_t, Trame> trames;  // dernière trame reçue pour chaque ID

long debit            = 125E3;
bool modeChangements  = true;   // true : n'affiche que les trames nouvelles ou modifiées
bool filtreVolant     = false;  // true : n'affiche que ID_VOLANT
bool canDemarre       = false;
uint32_t tramesPeriode = 0;
unsigned long dernierBilan = 0;

// Échantillonnage de la broche RX : mesure l'activité électrique indépendamment du débit
uint32_t echantillonsRx = 0;
uint32_t echantillonsRxBas = 0;

// Débits demandés à la librairie pour le balayage (commande 'a')
const long DEBITS_BALAYAGE[] = {1000000, 500000, 250000, 200000, 125000, 100000, 80000, 50000};
#define DUREE_BALAYAGE_MS  3000

bool demarrerCAN(long bps) {
  if (canDemarre) CAN.end();

  CAN.setPins(PIN_CAN_RX, PIN_CAN_TX);

  // Bug de la librairie : sur ESP32 révision >= 2, begin() active le bit BRP_DIV
  // et le débit réel est divisé par 2. On demande donc le double.
  long demande = (ESP.getChipRevision() >= 2) ? bps * 2 : bps;

  canDemarre = CAN.begin(demande);
  if (!canDemarre) {
    Serial.printf("[ERREUR] Débit non supporté : %ld bit/s\n", bps);
    return false;
  }

  CAN.observe();  // écoute seule — ne pas appeler CAN.filter() ensuite, ça repasse en mode normal
  debit = bps;
  trames.clear();
  Serial.printf("[CAN] Écoute seule à %ld kbit/s\n", bps / 1000);
  return true;
}

void afficherAide() {
  Serial.println();
  Serial.println("=== Sniffer CAN 207 (écoute seule) ===");
  Serial.println("  c → afficher : changements seulement / toutes les trames");
  Serial.printf ("  f → filtre sur l'ID 0x%03X on/off\n", ID_VOLANT);
  Serial.printf ("  i → masquer les IDs qui ont changé dans les %d dernières s\n", FENETRE_IGNORE_MS / 1000);
  Serial.println("  r → tout réinitialiser (IDs connus et masqués)");
  Serial.println("  l → lister les IDs vus");
  Serial.println("  1 / 2 / 5 → débit 125 / 250 / 500 kbit/s");
  Serial.println("  a → balayage automatique de tous les débits (diagnostic)");
  Serial.println("  h → cette aide");
  Serial.println();
}

void afficherOctetsBinaires(uint8_t octet) {
  for (int b = 7; b >= 0; b--) {
    Serial.print((octet >> b) & 1);
  }
}

void afficherTrame(uint32_t id, bool etendu, const uint8_t* data, uint8_t len, const Trame* avant) {
  Serial.printf("%8lu ms  ", millis());
  if (etendu) Serial.printf("0x%08X", id);
  else        Serial.printf("0x%03X     ", id);
  Serial.printf(" [%d] ", len);

  for (int i = 0; i < len; i++) {
    bool change = avant && (i >= avant->len || data[i] != avant->data[i]);
    Serial.printf("%02X%c", data[i], change ? '*' : ' ');
  }
  if (!avant && modeChangements) Serial.print(" (nouveau)");
  Serial.println();

  // Détail bit à bit des octets modifiés : pratique pour trouver le bit d'un bouton
  if (avant) {
    for (int i = 0; i < len; i++) {
      uint8_t ancien = (i < avant->len) ? avant->data[i] : 0;
      if (i < avant->len && data[i] == ancien) continue;
      Serial.printf("            octet %d : ", i);
      afficherOctetsBinaires(ancien);
      Serial.print(" → ");
      afficherOctetsBinaires(data[i]);
      Serial.println();
    }
  }
}

void listerIDs() {
  Serial.printf("[LISTE] %d IDs vus\n", trames.size());
  for (auto& entree : trames) {
    const Trame& t = entree.second;
    Serial.printf("  0x%03X  x%-6u %s ", entree.first, t.compteur, t.ignoree ? "(masqué)" : "        ");
    for (int i = 0; i < t.len; i++) Serial.printf("%02X ", t.data[i]);
    Serial.println();
  }
}

void masquerIDsActifs() {
  unsigned long maintenant = millis();
  int nb = 0;
  for (auto& entree : trames) {
    Trame& t = entree.second;
    if (!t.ignoree && maintenant - t.dernierChangement < FENETRE_IGNORE_MS) {
      t.ignoree = true;
      nb++;
    }
  }
  Serial.printf("[MASQUE] %d IDs masqués\n", nb);
}

void echantillonnerRx() {
  echantillonsRx++;
  if (digitalRead(PIN_CAN_RX) == LOW) echantillonsRxBas++;
}

// Essaie chaque débit brut de la librairie, sans la correction de révision,
// et compte les trames reçues : couvre aussi le cas où la correction serait fausse.
void balayerDebits() {
  bool diviseParDeux = ESP.getChipRevision() >= 2;
  Serial.println("[BALAYAGE] Débit demandé → débit réel probable → trames reçues");

  for (long brut : DEBITS_BALAYAGE) {
    if (canDemarre) CAN.end();
    CAN.setPins(PIN_CAN_RX, PIN_CAN_TX);
    canDemarre = CAN.begin(brut);
    if (!canDemarre) continue;
    CAN.observe();

    uint32_t nb = 0;
    unsigned long debut = millis();
    while (millis() - debut < DUREE_BALAYAGE_MS) {
      while (CAN.parsePacket() > 0) nb++;
      echantillonnerRx();
    }

    long reel = diviseParDeux ? brut / 2 : brut;
    Serial.printf("  %4ld kbit/s → %4ld kbit/s (ou %4ld) → %u trames%s\n",
                  brut / 1000, reel / 1000, (diviseParDeux ? brut : brut / 2) / 1000, nb,
                  nb > 0 ? "  ◄◄◄" : "");
  }

  demarrerCAN(debit);
}

void traiterCommande(char cmd) {
  switch (cmd) {
    case 'c':
      modeChangements = !modeChangements;
      Serial.println(modeChangements ? "[MODE] Changements seulement" : "[MODE] Toutes les trames (des trames peuvent être perdues)");
      break;
    case 'f':
      filtreVolant = !filtreVolant;
      Serial.printf("[FILTRE] 0x%03X %s\n", ID_VOLANT, filtreVolant ? "activé" : "désactivé");
      break;
    case 'i': masquerIDsActifs(); break;
    case 'r':
      trames.clear();
      Serial.println("[RESET] IDs connus et masqués oubliés");
      break;
    case 'l': listerIDs(); break;
    case '1': demarrerCAN(125E3); break;
    case '2': demarrerCAN(250E3); break;
    case '5': demarrerCAN(500E3); break;
    case 'a': balayerDebits(); break;
    case 'h': afficherAide(); break;
  }
}

void lireTrames() {
  // Note : parsePacket() renvoie la DLC, donc les trames de longueur 0 sont ignorées
  int dlc;
  while ((dlc = CAN.parsePacket()) > 0) {
    tramesPeriode++;

    uint32_t id  = CAN.packetId();
    bool etendu  = CAN.packetExtended();
    uint8_t data[8] = {0};
    uint8_t len  = 0;

    if (!CAN.packetRtr()) {
      while (CAN.available() && len < 8) {
        data[len++] = CAN.read();
      }
    }

    auto it = trames.find(id);
    bool nouvelle = (it == trames.end());
    bool modifiee = !nouvelle && (len != it->second.len || memcmp(data, it->second.data, len) != 0);

    bool afficher = !filtreVolant || id == ID_VOLANT;
    if (!nouvelle && it->second.ignoree) afficher = false;
    if (modeChangements && !nouvelle && !modifiee) afficher = false;

    if (afficher) {
      afficherTrame(id, etendu, data, len, nouvelle ? nullptr : &it->second);
    }

    Trame& t = trames[id];  // crée l'entrée si nouvelle
    if (nouvelle || modifiee) t.dernierChangement = millis();
    memcpy(t.data, data, sizeof(data));
    t.len = len;
    t.compteur++;
  }
}

void afficherBilan() {
  if (millis() - dernierBilan < PERIODE_BILAN_MS) return;
  dernierBilan = millis();

  if (tramesPeriode == 0) {
    Serial.printf("[BILAN] Aucune trame depuis %d s à %ld kbit/s — vérifier : contact mis, GND commun, "
                  "CAN-H/CAN-L pas inversés, débit (1 / 2 / 5)\n", PERIODE_BILAN_MS / 1000, debit / 1000);
  } else {
    Serial.printf("[BILAN] %u trames en %d s, %d IDs différents\n",
                  tramesPeriode, PERIODE_BILAN_MS / 1000, trames.size());
  }

  // Activité électrique sur R : 0 % = rien n'arrive, ~100 % = ligne bloquée à l'état dominant
  float pctBas = echantillonsRx ? 100.0 * echantillonsRxBas / echantillonsRx : 0;
  Serial.printf("        Broche RX (GPIO%d) à l'état bas : %.2f %% du temps\n", PIN_CAN_RX, pctBas);

  tramesPeriode = 0;
  echantillonsRx = 0;
  echantillonsRxBas = 0;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.printf("ESP32 révision %d\n", ESP.getChipRevision());
  afficherAide();
  demarrerCAN(debit);
}

void loop() {
  if (Serial.available()) {
    traiterCommande(Serial.read());
  }
  if (canDemarre) {
    lireTrames();
  }
  echantillonnerRx();
  afficherBilan();
}
