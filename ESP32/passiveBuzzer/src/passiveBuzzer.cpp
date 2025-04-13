#include <Arduino.h>

const int BUZZER_PIN = 4;

const double NOTE_C0  = 16.35;
const double NOTE_Cp0 = 17.32;
const double NOTE_D0  = 18.35;
const double NOTE_Dp0 = 19.45;
const double NOTE_E0  = 20.60;
const double NOTE_F0  = 21.83;
const double NOTE_Fp0 = 23.12;
const double NOTE_G0  = 24.50;
const double NOTE_Gp0 = 25.96;
const double NOTE_A0  = 27.50;
const double NOTE_Ap0 = 29.14;
const double NOTE_B0  = 30.87;

const double NOTE_C1  = 32.70;
const double NOTE_Cp1 = 34.65;
const double NOTE_D1  = 36.71;
const double NOTE_Dp1 = 38.89;
const double NOTE_E1  = 41.20;
const double NOTE_F1  = 43.65;
const double NOTE_Fp1 = 46.25;
const double NOTE_G1  = 49.00;
const double NOTE_Gp1 = 51.91;
const double NOTE_A1  = 55.00;
const double NOTE_Ap1 = 58.27;
const double NOTE_B1  = 61.74;

const double NOTE_C2  = 65.41;
const double NOTE_Cp2 = 69.30;
const double NOTE_D2  = 73.42;
const double NOTE_Dp2 = 77.78;
const double NOTE_E2  = 82.41;
const double NOTE_F2  = 87.31;
const double NOTE_Fp2 = 92.50;
const double NOTE_G2  = 98.00;
const double NOTE_Gp2 = 103.83;
const double NOTE_A2  = 110.00;
const double NOTE_Ap2 = 116.54;
const double NOTE_B2  = 123.47;

const double NOTE_C3  = 130.81;
const double NOTE_Cp3 = 138.59;
const double NOTE_D3  = 146.83;
const double NOTE_Dp3 = 155.56;
const double NOTE_E3  = 164.81;
const double NOTE_F3  = 174.61;
const double NOTE_Fp3 = 185.00;
const double NOTE_G3  = 196.00;
const double NOTE_Gp3 = 207.65;
const double NOTE_A3  = 220.00;
const double NOTE_Ap3 = 233.08;
const double NOTE_B3  = 246.94;

const double NOTE_C4  = 261.63;
const double NOTE_Cp4 = 277.18;
const double NOTE_D4  = 293.66;
const double NOTE_Dp4 = 311.13;
const double NOTE_E4  = 329.63;
const double NOTE_F4  = 349.23;
const double NOTE_Fp4 = 369.99;
const double NOTE_G4  = 392.00;
const double NOTE_Gp4 = 415.30;
const double NOTE_A4  = 440.00;
const double NOTE_Ap4 = 466.16;
const double NOTE_B4  = 493.88;

const double NOTE_C5  = 523.25;
const double NOTE_Cp5 = 554.37;
const double NOTE_D5  = 587.33;
const double NOTE_Dp5 = 622.25;
const double NOTE_E5  = 659.25;
const double NOTE_F5  = 698.46;
const double NOTE_Fp5 = 739.99;
const double NOTE_G5  = 783.99;
const double NOTE_Gp5 = 830.61;
const double NOTE_A5  = 880.00;
const double NOTE_Ap5 = 932.33;
const double NOTE_B5  = 987.77;

const double NOTE_C6  = 1046.50;
const double NOTE_Cp6 = 1108.73;
const double NOTE_D6  = 1174.66;
const double NOTE_Dp6 = 1244.51;
const double NOTE_E6  = 1318.51;
const double NOTE_F6  = 1396.91;
const double NOTE_Fp6 = 1479.98;
const double NOTE_G6  = 1567.98;
const double NOTE_Gp6 = 1661.22;
const double NOTE_A6  = 1760.00;
const double NOTE_Ap6 = 1864.66;
const double NOTE_B6  = 1975.53;

const double NOTE_C7  = 2093.00;
const double NOTE_Cp7 = 2217.46;
const double NOTE_D7  = 2349.32;
const double NOTE_Dp7 = 2489.02;
const double NOTE_E7  = 2637.02;
const double NOTE_F7  = 2793.83;
const double NOTE_Fp7 = 2959.96;
const double NOTE_G7  = 3135.96;
const double NOTE_Gp7 = 3322.44;
const double NOTE_A7  = 3520.00;
const double NOTE_Ap7 = 3729.31;
const double NOTE_B7  = 3951.07;

const double NOTE_C8  = 4186.01;
const double NOTE_Cp8 = 4434.92;
const double NOTE_D8  = 4698.63;
const double NOTE_Dp8 = 4978.03;
const double NOTE_E8  = 5274.04;
const double NOTE_F8  = 5587.65;
const double NOTE_Fp8 = 5919.91;
const double NOTE_G8  = 6271.93;
const double NOTE_Gp8 = 6644.88;
const double NOTE_A8  = 7040.00;
const double NOTE_Ap8 = 7458.62;
const double NOTE_B8  = 7902.13;

int tempo = 400;  // Điều chỉnh tốc độ bài hát

struct Note {
    float frequency;
    float duration;
};

// Mảng nốt nhạc 
Note melody[] = {
    {NOTE_E5, 0.5}, {NOTE_Gp5, 0.5}, {NOTE_C5, 0.25}, {NOTE_D5, 0.25}, {NOTE_E5, 0.5}, {NOTE_Gp5, 0.5}, {NOTE_C5, 0.25}, {NOTE_D5, 0.25},
    {NOTE_E5, 0.5}, {NOTE_Gp5, 0.5}, {NOTE_C5, 0.25}, {NOTE_D5, 0.25}, {NOTE_E5, 0.5}, {NOTE_Gp5, 0.5}, {NOTE_C5, 0.25}, {NOTE_D5, 0.25},
    {NOTE_E5, 1.0}, {NOTE_Gp5, 0.5}, {NOTE_A5, 0.5}, {NOTE_Gp5, 0.5}, {NOTE_E5, 0.5}, {NOTE_D5, 0.5}, {NOTE_C5, 1.0},
    {NOTE_E5, 0.5}, {NOTE_Gp5, 0.5}, {NOTE_A5, 0.5}, {NOTE_Gp5, 0.5}, {NOTE_E5, 0.5}, {NOTE_D5, 0.5}, {NOTE_C5, 1.0},
    {NOTE_Gp4, 0.5}, {NOTE_B4, 0.5}, {NOTE_D5, 0.25}, {NOTE_E5, 0.25}, {NOTE_Gp5, 0.5}, {NOTE_A5, 0.5}, {NOTE_Gp5, 0.5}, {NOTE_E5, 0.5},
    {NOTE_Gp4, 0.5}, {NOTE_B4, 0.5}, {NOTE_D5, 0.25}, {NOTE_E5, 0.25}, {NOTE_Gp5, 0.5}, {NOTE_A5, 0.5}, {NOTE_Gp5, 0.5}, {NOTE_E5, 0.5},
    {NOTE_C5, 0.5}, {NOTE_E5, 0.5}, {NOTE_Gp5, 0.5}, {NOTE_A5, 0.5}, {NOTE_Gp5, 0.5}, {NOTE_E5, 0.5}, {NOTE_C5, 1.0},
    {NOTE_C5, 0.5}, {NOTE_E5, 0.5}, {NOTE_Gp5, 0.5}, {NOTE_A5, 0.5}, {NOTE_Gp5, 0.5}, {NOTE_E5, 0.5}, {NOTE_C5, 1.0},
    {0, 1.5}  // Nghỉ giữa các đoạn
};

void setup() {
    ledcSetup(0, 1000, 8);
    ledcAttachPin(BUZZER_PIN, 0);
}

void loop() {
    int size = sizeof(melody) / sizeof(melody[0]);

    for (int i = 0; i < size; i++) {
        int noteDuration = tempo * melody[i].duration;

        if (melody[i].frequency == 0) {
            ledcWriteTone(0, 0);  // Nghỉ
        } else {
            ledcWriteTone(0, melody[i].frequency);
        }

        delay(noteDuration);
        ledcWriteTone(0, 0);
        delay(50);
    }
    delay(3000);  // Nghỉ 3 giây trước khi lặp lại bài hát
}