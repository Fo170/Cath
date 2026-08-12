# Cath — Aide multitâche coopératif pour Arduino

[![GitHub](https://img.shields.io/badge/GitHub-Fo170/Cath-blue?logo=github)](https://github.com/Fo170/Cath)

Fork **MIT** de la librairie `Phildem/Cath` **v1.3** (2021) : helper **multitâche coopératif** pour Arduino, sans RTOS ni interruptions.

---

## Principe

### Multitâche coopératif

Cath n'utilise **ni RTOS, ni interruptions, ni préemption** : c'est un **ordonnanceur coopératif**. Le flot d'exécution reste un unique `loop()` Arduino — chaque tâche « rend la main » en retournant de son `Loop()`, c'est elle qui décide de laisser tourner les autres.

- Chaque tâche est une classe dérivant de `Cath`, qui implémente `SetUp()` et `Loop()`.
- Elle s'enregistre dans son constructeur via `Cath::S_Register()` avec sa période en millisecondes.
- `Cath::S_SetUp()` (dans `setup()`) puis `Cath::S_Loop()` (à chaque `loop()`) pilotent l'ensemble.
- Le minuteur est basé sur `millis()` : un tick est détecté à chaque changement de milliseconde.

### Conséquences

- **Une seule chose à la fois** : pendant qu'une tâche exécute son `Loop()`, rien d'autre ne tourne.
- **Timing indicatif, pas temps réel** : la période est la fréquence *minimale* d'appel du `Loop()`, pas un délai garanti.
- **Pas de sections critiques nécessaires** entre tâches : une variable partagée n'est jamais interrompue au milieu d'un accès par une autre tâche.

## Limites à prendre en compte

| Limite | Conséquence |
|---|---|
| `Loop()` **non bloquant** obligatoire | Un `delay()` long, une attente réseau/Serial ou un calcul long dans une tâche **bloque toutes les autres** et le compteur de ticks. |
| `S_Loop()` appelé trop rarement | Un écart ≥ ~256 ms entre deux appels de `S_Loop()` fait **perdre des ticks** (détection par l'octet faible de `millis()`) : les périodes dérivent. Appeler `S_Loop()` à chaque `loop()`. |
| Charge totale | La somme des durées de tous les `Loop()` doit rester très inférieure à la période la plus courte, sinon les tâches s'espacent et dérivent. |
| Période minimale | 1 ms (résolution de `millis()` ; `S_Register` borne silencieusement à ≥ 1). |
| Période maximale | 65535 ms avec `__CathOpt_SmallCounter__` (compteur `uint16_t`) ; pratiquement illimitée sinon (`uint32_t`). |
| Nombre de tâches | Borné par `kMaxCathTask`, à définir **avant** `#include "Cath.h"` (aucune valeur par défaut). |
| Wrap de `millis()` (~49,7 j) | Sans effet : seule la détection de changement de l'octet faible est utilisée, pas de soustraction de temps absolu. |

---

## Installation

### PlatformIO (recommandé)

Ajoutez à `platformio.ini` :

```ini
lib_deps =
    https://github.com/Fo170/Cath.git@^1.0.0
```

### Arduino IDE

Copiez `Cath.h` dans le dossier de votre sketch puis `#include "Cath.h"`.

---

## Utilisation

```cpp
// À définir AVANT l'inclusion (aucune valeur par défaut)
#define kMaxCathTask 3
#include <Cath.h>

class Blink : public Cath {
  public:
    Blink(int pin) : m_pin(pin) {
      pinMode(m_pin, OUTPUT);
      Cath::S_Register(this, 500);          // période 500 ms
    }
    void SetUp() override {}
    void Loop() override {
      digitalWrite(m_pin, !digitalRead(m_pin));
    }
  private:
    int m_pin;
};

Blink led(LED_BUILTIN);

void setup() { Cath::S_SetUp(); }
void loop()  { Cath::S_Loop(); }
```

---

## API

| Méthode statique | Description |
|---|---|
| `Cath::S_Register(tache, periode_ms, offset_ms = 1)` | Enregistre une tâche. Période/offset bornés silencieusement à ≥ 1 (0 interdit). |
| `Cath::S_SetUp()` | Appelle le `SetUp()` de chaque tâche enregistrée. |
| `Cath::S_Loop()` | Appelle le `Loop()` des tâches dont la période est écoulée. |

---

## Configuration (macros à définir AVANT `#include "Cath.h"`)

| Macro | Rôle |
|---|---|
| `kMaxCathTask` | Nombre max de tâches enregistrables. **Obligatoire**, pas de valeur par défaut. |
| `__CathOpt_SmallCounter__` | Optionnel : `CathCnt` = `uint16_t` → période max 65536 ms (défaut `uint32_t`). |

---

## Macros utilitaires

`_1_MINUTES_` (60000), `_10_SECONDES_` (10000), `_1_SECONDES_` (1000), `_100ms_` (100), `_10ms_` (10), `_1ms_` (1).

---

## Remarques

- Dépend de `millis()` d'Arduino.
- `Cath.h` utilise `uint8_t`/`uint16_t`/`uint32_t` fournis par le core Arduino.

---

## Licence

MIT — voir `LICENSE`. Auteur d'origine : Phildem (v1.3, 2021) ; fork : Olivier FOURNET (`Fo170`).

> **Note :** l'upstream `Phildem/Cath` est passé en GPL-3.0. Ce fork reste **MIT** (base v1.3) et ne fusionnera pas les commits upstream post-1.3.
