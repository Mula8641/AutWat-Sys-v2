# Known issues and source conflicts

The original sketch (`legacy/AutWaat_Sys.ino`) and the team report disagree in several places. These decisions are made; they are listed so a reader can see what was chosen and why.

| # | Conflict | Decision |
|---|---|---|
| 1 | Cactus critical low is 10 % in the code and 5 % in the report table 19 | **10** (the code) |
| 2 | Water per plant: dose table says Cactus 300, Aloe 300, ZZ 250 mL; plant sheets say Cactus 50 to 150, Aloe 100 to 250, ZZ 250 mL | **300 / 300 / 250 mL** (dose table, tied to the measured flow rate). Settle on hardware |
| 3 | Report schedule tables show 1 to 2 s pump runs; the report calculation gives 14 to 17 s | The **calculation** (dose divided by flow rate). Settle on hardware |
| 4 | Flow rate 26.67 mL/s before the T valve, 17.54 mL/s after | **17.54 mL/s**, configurable (`kFlowRateMlPerSec`) |
| 5 | Prototype used a 16x2 I2C LCD; code and simulation use a 20x4 LCD in 4 bit mode | **20x4** (the code). A real build can swap the display module |
| 6 | Resume says a 3 day cycle; the original code uses 2 days | **3 days** |
| 7 | Report section 4.1.7 and test table 26 say a scheduled run is skipped when soil is already wet; the original code does not | Implemented (controller rule 13) |

## Open questions

- Items 2 and 3 need a measured flow rate and a measured delivered volume per plant (see `hardware_results.md`).
- The real moisture probe's dry and wet ADC values are unknown until a probe is wired; `kRawDry = 0` and `kRawWet = 4095` are simulator values only.
- The relay active level (`kRelayActiveLow`) is set from the intended real module (low level trigger) and must be confirmed against the Wokwi relay and the real board.
- The HTTP API has no authentication. Use it on a trusted local network only.
