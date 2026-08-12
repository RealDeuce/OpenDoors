# OpenDoors TODO

- `ODCfgGetWordHex()` passes a possibly negative plain `char` to `toupper()`. The C ctype contract only accepts `EOF` or an `unsigned char` value, and the MSVC debug CRT may assert for negative input. Add a regression case with a high-bit byte and convert through `unsigned char` before calling `toupper()`.
