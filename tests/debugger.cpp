#include "track/instrument/fluidsynth/FSInstrument.hpp"
#include <iostream>

int main()
{
    FSInstrument instrument(
        "FluidR3",
        "/home/space_alyen/Music/Libraries/Soundfonts/FluidR3 GM + GS.sf2",
        512,
        44100.0
    );

    auto presets = instrument.getInstruments();

    for (const auto& p : presets)
    {
        std::cout
            << p.bank << " : "
            << p.program << " : "
            << p.name << '\n';
    }

    return 0;
}