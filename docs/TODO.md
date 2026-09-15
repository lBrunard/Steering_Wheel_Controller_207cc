# Steering Wheel Controller 207 CC — Où j'en suis

> Commandes au volant d'une Peugeot 207 CC (lues sur le CAN confort) → autoradio Sony
> (entrée REMOTE IN, échelle de résistances via optocoupleurs PC817C).
>
> Dernière mise à jour : 2026-09-15

---

## ✅ Fait

### Schéma
- [x] Schéma V1.0 sous EasyEDA (07/03 → 15/03/2026) : [CANBUS_207_schema_v1.0.pdf](CANBUS_207_schema_v1.0.pdf.pdf)

### Sortie autoradio (optocoupleurs)
- [x] VOL+ câblé sur **GPIO25** (220 Ω → PC817C, 2,2 kΩ côté émetteur) — mesuré 2,36 kΩ
- [x] VOL- câblé sur **GPIO26** (4,7 kΩ) — validé au multimètre
- [x] Programme de test des optos : `src/main.cpp` (env `esp32dev`, commandes `1` / `2` / `0`)
- [x] Branchement jack validé : **pointe** = CN2-1 (collecteur), **corps** = CN2-2
- [x] Sony en mode **CUSTOM** (apprentissage) → VOL+ et VOL- commandent bien l'autoradio
      → les valeurs de résistances du schéma sont conservées

### Bus CAN
- [x] Sniffer CAN en écoute seule : `src/can_sniffer.cpp` (env `can_sniffer`)
- [x] Bus trouvé sur le **Quadlock** de l'autoradio d'origine, bloc 16 broches :
      **10 = CAN-H**, **13 = CAN-L**, **16 = masse**
- [x] Type de bus identifié : **CAN basse vitesse fault-tolerant** (repos : CAN-H ≈ 0,3 V, CAN-L ≈ 4,5 V)
- [x] Résistance de 120 Ω du module SN65HVD230 retirée (elle court-circuitait le bus)
- [x] Le sniffer lit les trames

---

## 🔧 En cours / prochaines étapes

### 1. Sécuriser l'écoute seule (matériel)
- [ ] Débrancher GPIO4 et relier **TX (= D) du module au 3V3**
- [ ] Vérifier 3,3 V sur TX du module, y compris pendant un reset de l'ESP32
- [ ] Vérifier que le sniffer reçoit toujours les trames
- [ ] *(Optionnel)* Test sur table : 120 Ω provisoire + programme qui tente d'émettre → 0 V entre CANH et CANL
- [ ] Vérifier qu'aucun voyant / code défaut n'est resté après le court-circuit du bus par la 120 Ω

### 2. Raccordement propre au Quadlock
- [ ] Choisir la méthode de piquage (sonde arrière pour les tests, puis solution définitive)
- [ ] Réaliser le raccordement définitif (après achat du nouveau fer à souder)

### 3. Repérer les trames des boutons du volant
- [ ] Noter le débit qui fonctionne (125 kbit/s attendu)
- [ ] Pour chaque bouton : ID, octet, bit, valeur appuyé / relâché (piste : ID `0x21F`)

| Bouton volant | ID | Octet | Bit / valeur | Remarque |
|---|---|---|---|---|
| VOL+ | | | | |
| VOL- | | | | |
| Suivant | | | | |
| Précédent | | | | |
| Source / Mode | | | | |
| Molette | | | | |
| Autres | | | | |

### 4. Finir la sortie autoradio
- [ ] Câbler NEXT — **GPIO27**, 8,2 kΩ (attendu ≈ 8,35 kΩ)
- [ ] Câbler PREV — **GPIO14**, 12 kΩ (attendu ≈ 12,15 kΩ)
- [ ] Câbler MODE — **GPIO12**, 1,2 kΩ (attendu ≈ 1,35 kΩ) — ⚠ broche de strapping : vérifier que l'ESP32 boote toujours
- [ ] Enregistrer les 3 nouveaux boutons dans le mode CUSTOM du Sony

### 5. Firmware final
- [ ] Passer du driver `sandeepmistry/CAN` au driver **TWAI d'ESP-IDF** en `TWAI_MODE_LISTEN_ONLY`
      (pas de passage en mode normal, pas de bug de débit divisé par 2)
- [ ] Décoder les trames du volant → allumer l'opto correspondant **tant que le bouton est appuyé**
- [ ] Un seul opto allumé à la fois
- [ ] Sécurité : tout relâcher si plus de trame reçue depuis quelques centaines de ms
- [ ] Test complet dans la voiture

### 6. Alimentation dans la voiture
- [ ] Choisir la source 12 V : le Quadlock n'a que du **+12 V permanent** (broche 12) → risque de décharger la batterie
      Pistes : +12 V après contact de l'adaptateur de l'autoradio, sortie « REM OUT » du Sony (vérifier le courant max), ou mise en veille de l'ESP32
- [ ] Monter et tester le buck 12 V → 5 V
- [ ] Protections d'entrée : fusible, diode anti-inversion, TVS

### 7. Schéma V1.1 (corrections à reporter)
- [ ] **TPS54202 : ajouter le pont diviseur sur FB** (actuellement relié directement à VOUT → sortie ≈ 0,6 V au lieu de 5 V)
- [ ] Protections d'entrée 12 V (fusible, diode, TVS)
- [ ] SN65HVD230 : **D → 3V3**, supprimer la liaison vers IO4 ; RS → 3V3 (standby) si puce seule
- [ ] Ne jamais mettre de terminaison 120 Ω (bus basse vitesse)
- [ ] Noter sur le schéma : jack pointe = CN2-1, corps = CN2-2
- [ ] Router le PCB

---

## 🧹 Rangement du projet
- [ ] Renommer `docs/CANBUS_207_schema_v1.0.pdf.pdf` (double extension `.pdf`)
- [ ] Ajouter l'export source EasyEDA dans `docs/`
- [ ] Supprimer ou déplacer `src/CAN_207.code-workspace` (pointe vers un dossier vide)
- [ ] Commiter `src/can_sniffer.cpp` et `platformio.ini`

---

## 📌 Aide-mémoire

| Élément | Valeur |
|---|---|
| Opto LED | GPIO → 220 Ω → PC817C broche 1 ; broche 2 → GND |
| Opto sortie | Broche 4 (collecteur) → CN2-1 ; broche 3 (émetteur) → R → CN2-2 |
| CAN module | RX → GPIO5 ; TX → **3V3** ; VCC → 3V3 ; GND → GND voiture |
| Quadlock (bloc 16 br.) | 10 = CAN-H, 13 = CAN-L, 16 = masse, 12 = +12 V permanent |
| Téléverser test optos | `~/.platformio/penv/bin/pio run -t upload` |
| Téléverser sniffer | `~/.platformio/penv/bin/pio run -e can_sniffer -t upload` |
| Moniteur série | `~/.platformio/penv/bin/pio device monitor` |
