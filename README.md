# Cath — Aide multitâche coopératif pour Arduino

[![GitHub](https://img.shields.io/badge/GitHub-Fo170/Cath-blue?logo=github)](https://github.com/Fo170/Cath)

Fork **MIT** de la librairie `Phildem/Cath` **v1.3** (2021) : helper **multitâche coopératif** pour Arduino, sans RTOS ni interruptions.

---

## Principe

Chaque tâche est une classe dérivant de `Cath`, qui implémente `SetUp()` et `Loop()`.
La tâche s'enregistre dans son constructeur via `Cath::S_Register()` avec sa période en millisecondes.
`Cath::S_SetUp()` (appelé dans `setup()`) puis `Cath::S_Loop()` (appelé à chaque `loop()`) pilotent l'ensemble.

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
