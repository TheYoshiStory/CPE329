#include "msp.h"
#include "delay.h"
#include "note.h"

// initialize digital speaker
void init_note()
{
    // configure NOTE_CTRL for digital output
    NOTE_CTRL->SEL1 &= BIT1;
    NOTE_CTRL->SEL0 &= BIT1;
    NOTE_CTRL->DIR |= BIT1;
    NOTE_CTRL->OUT &= ~BIT1;
}

// play specified note and delay
void play_note(int note, int delay)
{
    int i;

    for(i = 0; i < NOTE_LENGTH; i += note * 2)
    {
        NOTE_CTRL->OUT |= BIT1;
        delay_us(note);

        NOTE_CTRL->OUT &= ~BIT1;
        delay_us(note);
    }

    delay_ms(delay);
}
