# SubKing - mono bass synth (VST3) FL Studiolle

Monofoninen bassosyntetisaattori: 2 oskillaattoria + sub + kohina, 24 dB Moog-tyylinen ladder-suodatin,
LFO (tempo-synkattu, wobble), pitch-envelope (808), vääristymä, glide/legato, 10 presettiä.

## Asennus FL Studioon
1. Kopioi `SubKing.vst3` kansioon `C:\Program Files\Common Files\VST3\`
2. FL Studio: Options > Manage plugins > Find installed plugins (Start scan)
3. Add > SubKing (Installed > Generators). FL 21+ / 64-bit.

## Rakennus
A) GitHub (ei vaadi mitään paikallisesti): luo repo, pushaa tämä kansio, avaa Actions > "Build SubKing" > lataa artifact `SubKing-Windows-VST3`.
B) Paikallisesti: asenna Visual Studio 2022 (Desktop development with C++), CMake ja Git, aja `build_windows.bat`.

## Parametrit lyhyesti
- OSC 1/2: aaltomuoto (Saw/Square/Tri/Sine), oktaavi, detune (Reese = Saw+Saw ja detune 15-25 ct)
- SUB: sini/neliö, -1 / -2 oktaavia; NOISE
- VOICE: Glide (ms), Legato (ei retriggeröi overlappaavilla nuoteilla)
- FILTER: Cutoff, Reso, Drive, Env Amt (-1..1), Key Trk; omat FEnv ja AEnv ADSR
- LFO: muoto, nopeus tai tempo-sync (1/1...1/16), kohde Cutoff ja Pitch, Retrig
- PITCH ENV: laskeva pitch-sweep (808-kick/boom)
- DISTORTION: Drive, Mix, Tone (low-pass vääristyksen jälkeen)
- Pitch bend: ±2 puolisävelaskelta
