# BLE protocol

Device name: `AutWat` (NimBLE, one service, two characteristics). UUIDs are defined in `include/config.h`.

| Item | UUID | Properties |
|---|---|---|
| Service | `23f892a4-78c9-4eaa-bab5-ac4d6f11b74f` | |
| Status characteristic | `d0331ccb-c3e7-4bda-bd21-229236bad932` | read, notify (8 bytes, notified at most every 2 s) |
| Command characteristic | `49dc7bbb-ebd5-4830-9c89-7a3a0f1047e3` | write (1 byte) |

## Status payload (8 bytes)

| Byte | Meaning |
|---|---|
| 0 | moisture percent |
| 1 | air humidity percent, `255` = invalid |
| 2 | air temperature in whole degrees C, signed int8, `0x80` (-128) = invalid |
| 3 | mode: Idle 0, Scheduled 1, Emergency 2, Lockout 3, Fault 4 |
| 4 | pump: 0 off, 1 on |
| 5 | alarm: None 0, Slow 1, Fast 2 |
| 6 | health: Ok 0, Dry 1, Overwatered 2 |
| 7 | plant index: Cactus 0, AloeVera 1, ZamioculcasZamifolia 2 |

Encoder: `lib/status_codec`, unit tested in `test/test_status_codec`.

## Command byte

| Value | Command |
|---|---|
| `01` | water now (ignored while dry-emergency, locked out or in fault) |
| `02` | stop (also clears Fault) |
| other | ignored |

## Test with nRF Connect (phone)

1. Power the board and complete plant selection.
2. Open nRF Connect and scan. Connect to `AutWat`.
3. Open the service `23f892a4-...`. Read the status characteristic (`d0331ccb-...`) and decode the 8 bytes with the table above. Enable notifications to see updates every 2 s.
4. Write `01` to the command characteristic (`49dc7bbb-...`): the pump starts if the plant is not too wet.
5. Write `02`: the pump stops.

No BLE test has been run yet. Results go in `hardware_results.md`.
