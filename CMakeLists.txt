# BASIC SETUP
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(sub-xonscious VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)

include_directories(${PROJECT_SOURCE_DIR}/include)

# -------------------------------------------------
# Oscillator library
# -------------------------------------------------
add_library(oscillator include/audio_source/Oscillator.cpp)
target_include_directories(oscillator PUBLIC ${PROJECT_SOURCE_DIR}/include)

# -------------------------------------------------
# Audio Player Library
# -------------------------------------------------

add_library(audio_player include/audio_source/audio_clip/AudioPlayer.cpp)
target_include_directories(audio_player PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(audio_player PUBLIC sndfile)

# -------------------------------------------------
# Voice Library
# -------------------------------------------------
add_library(voice include/track/voice/Voice.cpp)
target_include_directories(voice PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(voice PUBLIC oscillator)

# -------------------------------------------------
# Audio Engine Library
# -------------------------------------------------
add_library(audio_engine include/audio_engine/AudioEngine.cpp)
target_include_directories(audio_engine
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PORTAUDIO_INCLUDE_DIRS}
)

target_link_libraries(audio_engine PUBLIC 
    voice 
    audio_player 
    asound
    reverb
    ${PORTAUDIO_LIBRARIES})

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
# Delay & Reverb library
# -------------------------------------------------
add_library(feedback_delay include/audio_fx/delay/FeedbackDelay.cpp)
target_include_directories(feedback_delay PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(reverb include/audio_fx/reverb/Reverb.cpp)
target_include_directories(reverb PUBLIC ${PROJECT_SOURCE_DIR}/include)


# =================================================================================


# Find the package
find_package(Catch2 3 REQUIRED)

# Define the executable target
add_executable(SVFTest tests/SVFTest.cpp)
add_executable(plot_data tests/FilterPlotData.cpp)
add_executable(BiQuadTest tests/BiQuadTest.cpp)
add_executable(AudioTest tests/AudioEngineTest.cpp)
add_executable(ReverbTest tests/ReverbTest.cpp)

# Link the required libraries to each executable
target_link_libraries(SVFTest PRIVATE oscillator svf Catch2::Catch2WithMain)
target_link_libraries(plot_data PRIVATE oscillator svf Catch2::Catch2WithMain)
target_link_libraries(BiQuadTest PRIVATE oscillator biquad Catch2::Catch2WithMain)
target_link_libraries(ReverbTest PRIVATE reverb Catch2::Catch2WithMain)
target_link_libraries(AudioTest PRIVATE audio_engine)

