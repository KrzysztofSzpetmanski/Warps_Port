#include "DaisyDuino.h"
#include "warps/dsp/modulator.h"

DaisyHardware hw;
warps::Modulator modulator;

// Bufor na parametry (potencjometry)
warps::Parameters p;

void AudioCallback(float **in, float **out, size_t size) {
    // 1. Odczytaj parametry z ADC Daisy i zmapuj na format Warps (0.0 - 1.0)
    p.carrier_shape = hw.adc.GetFloat(0); 
    p.modulation_algorithm = hw.adc.GetFloat(1);
    p.modulation_parameters = hw.adc.GetFloat(2);
    p.frequency_shift = 0.5f; // Przykład stałej wartości

    // 2. Przygotuj bufory wejściowe/wyjściowe
    // Warps procesuje ramki (L/R wejście -> L/R wyjście)
    for (size_t i = 0; i < size; i++) {
        warps::ShortFrame input_frame;
        warps::ShortFrame output_frame;

        // Konwersja float (-1.0 do 1.0) na format wewnętrzny Warps (często int16 lub float)
        // Uwaga: Oryginalny Warps często operuje na próbkach 16-bitowych wewnątrz dsp.h
        input_frame.l = in[0][i] * 32767.0f;
        input_frame.r = in[1][i] * 32767.0f;

        modulator.Process(p, &input_frame, &output_frame, 1);

        out[0][i] = output_frame.l / 32767.0f;
        out[1][i] = output_frame.r / 32767.0f;
    }
}

void setup() {
    hw = DaisyHardware::Init();
    float sample_rate = hw.AudioSampleRate();

    // Inicjalizacja silnika Warps
    modulator.Init(sample_rate);

    hw.StartAudio(AudioCallback);
}

void loop() {
    // Tu możesz dodać obsługę wyświetlacza lub MIDI
}
