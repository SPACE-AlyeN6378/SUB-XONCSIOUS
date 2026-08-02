# BASIC SETUP
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(sub-xonscious VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
pkg_check_modules(SFIZZ REQUIRED sfizz)
pkg_check_modules(FLUIDSYNTH REQUIRED fluidsynth)

include_directories(${PROJECT_SOURCE_DIR}/include)

# -------------------------------------------------
# Oscillator library
# -------------------------------------------------
add_library(oscillator include/audio_source/Oscillator.cpp)
target_include_directories(oscillator PUBLIC ${PROJECT_SOURCE_DIR}/include)

# -------------------------------------------------
# Audio Clip Player Library
# -------------------------------------------------
add_library(clip_player include/audio_source/audio_clip/AudioClipPlayer.cpp)
target_include_directories(clip_player PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(clip_player PUBLIC sndfile)

# -------------------------------------------------
# Sforzando Library
# -------------------------------------------------
add_library(sfz_instrument
    include/track/instrument/sfz/SFZInstrument.cpp
)

target_include_directories(sfz_instrument
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${SFIZZ_INCLUDE_DIRS}
)

target_link_libraries(sfz_instrument
    PUBLIC
    ${SFIZZ_LIBRARIES}
)

# -------------------------------------------------
# Fluisynth Instrument Library
# -------------------------------------------------
add_library(fs_instrument
    include/track/instrument/fluidsynth/FSInstrument.cpp
)

target_include_directories(fs_instrument 
    PUBLIC ${PROJECT_SOURCE_DIR}/include 
    ${FLUIDSYNTH_INCLUDE_DIRS})

target_link_libraries(fs_instrument 
    PUBLIC 
    ${FLUIDSYNTH_LIBRARIES})

# -------------------------------------------------
# Voice Library
# -------------------------------------------------
add_library(voice include/track/voice/Voice.cpp)
target_include_directories(voice PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(voice PUBLIC oscillator)


# -------------------------------------------------
# Track Library
# -------------------------------------------------
add_library(midi_track include/track/MIDITrack.cpp)
target_include_directories(voice PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(midi_track PUBLIC)


# AUDIO EFFECTS ==========================================================================
# -------------------------------------------------
# BiQuad Filter library
# -------------------------------------------------
add_library(biquad include/audio_fx/eq_filter/BiQuadFilter.cpp)
target_include_directories(biquad PUBLIC ${PROJECT_SOURCE_DIR}/include)

# -------------------------------------------------
# State Variable Filter library
# -------------------------------------------------
add_library(svf include/audio_fx/eq_filter/StateVariableFilter.cpp)
target_include_directories(svf PUBLIC ${PROJECT_SOURCE_DIR}/include)

# -------------------------------------------------
# Compressor library
# -------------------------------------------------
add_library(compressor include/audio_fx/compressor/Compressor.cpp)
target_include_directories(compressor PUBLIC ${PROJECT_SOURCE_DIR}/include)

# -------------------------------------------------
# Delay & Reverb library
# -------------------------------------------------
add_library(feedback_delay include/audio_fx/delay/FeedbackDelay.cpp)
target_include_directories(feedback_delay PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(reverb include/audio_fx/reverb/Reverb.cpp)
target_include_directories(reverb PUBLIC ${PROJECT_SOURCE_DIR}/include)

# -------------------------------------------------
# Audio Engine Library
# -------------------------------------------------
add_library(audio_engine include/audio_engine/AudioEngine.cpp)
target_include_directories(audio_engine
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PORTAUDIO_INCLUDE_DIRS}
    PRIVATE
        ${PROJECT_SOURCE_DIR}
)

target_link_libraries(audio_engine PUBLIC 
    voice
    fs_instrument 
    midi_track
    clip_player 
    asound
    reverb
    ${PORTAUDIO_LIBRARIES})


# =================================================================================


# Find the package
find_package(Catch2 3 REQUIRED)

# Define the executable target
add_executable(SVFTest tests/SVFTest.cpp)
add_executable(plot_data tests/FilterPlotData.cpp)
add_executable(BiQuadTest tests/BiQuadTest.cpp)
add_executable(AudioTest tests/AudioEngineTest.cpp)
add_executable(ReverbTest tests/ReverbTest.cpp)
add_executable(MidiDAOTest tests/MIDIClipDAOTest.cpp)
add_executable(CompressorTest tests/CompressorTest.cpp)

# Link the required libraries to each executable
target_link_libraries(SVFTest PRIVATE oscillator svf Catch2::Catch2WithMain)
target_link_libraries(plot_data PRIVATE oscillator svf Catch2::Catch2WithMain)
target_link_libraries(BiQuadTest PRIVATE oscillator biquad Catch2::Catch2WithMain)
target_link_libraries(ReverbTest PRIVATE reverb Catch2::Catch2WithMain)
target_link_libraries(AudioTest PRIVATE audio_engine)
target_link_libraries(MidiDAOTest PRIVATE midi_track Catch2::Catch2WithMain)
target_link_libraries(CompressorTest PRIVATE oscillator compressor)

