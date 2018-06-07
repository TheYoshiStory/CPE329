#ifndef NOTE_H
#define NOTE_H

/*
 *  ------------------------------------
 *  | - | - | - | - | - | - | SPKR | - |
 *  ------------------------------------
 */
#define NOTE_CTRL P4

#define NOTE_LENGTH 250000

#define C3 7634
#define D3 6803
#define E3 6061
#define F3 5714
#define G3 5102
#define A3 4545
#define B3 4049

#define C4 3817
#define D4 3401
#define E4 3030
#define F4 2865
#define G4 2551
#define A4 2273
#define B4 2024

#define C5 1912
#define D5 1700
#define E5 1517
#define F5 1431
#define G5 1276
#define A5 1136
#define B5 1012

void init_note();
void play_note();

#endif
