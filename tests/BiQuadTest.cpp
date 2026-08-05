#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "audio_source/oscillator/Oscillator.hpp"
#include "audio_fx/eq_filter/BiQuadFilter.hpp"
#include "misc/BodePlot.hpp"

BiQuadFilter filter(48000.0);

TEST_CASE("Biquad low pass coefficients")
{
    filter.setLowPass(1000.0, 0.707);
    auto c = filter.getCoefficients();

    REQUIRE(c.b0 == Catch::Approx(0.0039160766836994505));
    REQUIRE(c.b1 == Catch::Approx(0.007832153367398901));
    REQUIRE(c.b2 == Catch::Approx(0.0039160766836994505));
    REQUIRE(c.a1 == Catch::Approx(-1.8153179156742152));
    REQUIRE(c.a2 == Catch::Approx(0.8309822224090128));

}

TEST_CASE("Biquad high pass coefficients")
{
    filter.setHighPass(200.0, 0.7071);
    auto c = filter.getCoefficients();

    REQUIRE(c.b0 == Catch::Approx(0.9816580973135585));
    REQUIRE(c.b1 == Catch::Approx(-1.963316194627117));
    REQUIRE(c.b2 == Catch::Approx(0.9816580973135585));
    REQUIRE(c.a1 == Catch::Approx(-1.9629797472685724));
    REQUIRE(c.a2 == Catch::Approx(0.9636526419856617));
}

TEST_CASE("Biquad band pass coefficients")
{
    SECTION("Constant peak gain") {
        filter.setBandPass(1000.0, 5.0, BandPassMode::ConstantPeakGain);
        auto c = filter.getCoefficients();

        REQUIRE(c.b0 == Catch::Approx(0.01288444348727827));
        REQUIRE(c.b1 == Catch::Approx(0));
        REQUIRE(c.b2 == Catch::Approx(-0.01288444348727827));
        REQUIRE(c.a1 == Catch::Approx(-1.9573412921733742));
        REQUIRE(c.a2 == Catch::Approx(0.9742311130254434));
    }
    
    SECTION("Constant skirt gain") {
        filter.setBandPass(1000.0, 5.0, BandPassMode::ConstantSkirtGain);
        auto c = filter.getCoefficients();

        REQUIRE(c.b0 == Catch::Approx(0.01288444348727827 * 5.0));
        REQUIRE(c.b1 == Catch::Approx(0));
        REQUIRE(c.b2 == Catch::Approx(-0.01288444348727827 * 5.0));
        REQUIRE(c.a1 == Catch::Approx(-1.9573412921733742));
        REQUIRE(c.a2 == Catch::Approx(0.9742311130254434));
    }
}

TEST_CASE("Biquad notch coefficients")
{
    filter.setNotch(1000.0, 5.0);
    auto c = filter.getCoefficients();

    REQUIRE(c.b0 == Catch::Approx(0.9871155565127216));
    REQUIRE(c.b1 == Catch::Approx(-1.9573412921733742));
    REQUIRE(c.b2 == Catch::Approx(0.9871155565127216));
    REQUIRE(c.a1 == Catch::Approx(-1.9573412921733742));
    REQUIRE(c.a2 == Catch::Approx(0.9742311130254434));
}

TEST_CASE("Biquad Peaking EQ coefficients")
{
    filter.setPeaking(2000.0, 1.0, 6.0);
    auto c = filter.getCoefficients();

    REQUIRE(c.b0 == Catch::Approx(1.083528432));
    REQUIRE(c.b1 == Catch::Approx(-1.769718979));
    REQUIRE(c.b2 == Catch::Approx(0.748619473));
    REQUIRE(c.a1 == Catch::Approx(-1.769718979));
    REQUIRE(c.a2 == Catch::Approx(0.832147905));

    filter.setPeaking(5000.0, 1.0, 4.0);
    BodePlot::plot(filter);
}

// TEST_CASE("Biquad Peaking EQ coefficients")
// {

// }

TEST_CASE("Biquad Low Shelf coefficients")
{
    SECTION("Default shelf cut") {
        filter.setLowShelf(200.0, 1.0, 6.0);
        auto c = filter.getCoefficients();

        REQUIRE(c.b0 == Catch::Approx(1.006445578));
        REQUIRE(c.b1 == Catch::Approx(-1.968612352));
        REQUIRE(c.b2 == Catch::Approx(0.963120058));
        REQUIRE(c.a1 == Catch::Approx(-1.968850107));
        REQUIRE(c.a2 == Catch::Approx(0.969327881));
    }

    SECTION("High shelf cut") {
        filter.setLowShelf(200.0, 0.5, 6.0);
        auto c = filter.getCoefficients();

        REQUIRE(c.b0 == Catch::Approx(1.009138916));
        REQUIRE(c.b1 == Catch::Approx(-1.955555853));
        REQUIRE(c.b2 == Catch::Approx(0.947363898));
        REQUIRE(c.a1 == Catch::Approx(-1.955792031));
        REQUIRE(c.a2 == Catch::Approx(0.956266636));
    }
}

TEST_CASE("Biquad HIgh Shelf coefficients")
{
    filter.setHighShelf(8000.0, 1.0, -6.0);
    auto c = filter.getCoefficients();

    REQUIRE(c.b0 == Catch::Approx(0.636265806));
    REQUIRE(c.b1 == Catch::Approx(-0.275827680));
    REQUIRE(c.b2 == Catch::Approx(0.130429487));
    REQUIRE(c.a1 == Catch::Approx(-0.794619594));
    REQUIRE(c.a2 == Catch::Approx(0.285487207));

    filter.setHighShelf(1200.0, 1.0, 6.0);
}

TEST_CASE("Edge Cases")
{
    SECTION("Very low frequency")
    {
        filter.setHighPass(20.0, 0.707);
        auto c = filter.getCoefficients();

        REQUIRE(c.b0 == Catch::Approx(0.998150233));
        REQUIRE(c.b1 == Catch::Approx(-1.996300465));
        REQUIRE(c.b2 == Catch::Approx(0.998150233));
        REQUIRE(c.a1 == Catch::Approx(-1.996297045));
        REQUIRE(c.a2 == Catch::Approx(0.996303886));
    }

    SECTION("Near Nyquist")
    {
        filter.setHighPass(20000.0, 0.707);
        auto c = filter.getCoefficients();

        REQUIRE(c.b0 == Catch::Approx(0.049488004));
        REQUIRE(c.b1 == Catch::Approx(-0.098976008));
        REQUIRE(c.b2 == Catch::Approx(0.049488004));
        REQUIRE(c.a1 == Catch::Approx(1.279581945));
        REQUIRE(c.a2 == Catch::Approx(0.477533960));
    }

    SECTION("Narrow Bandwidth")
    {
        filter.setHighPass(1000.0, 50.0);
        auto c = filter.getCoefficients();

        REQUIRE(c.b0 == Catch::Approx(0.994424446));
        REQUIRE(c.b1 == Catch::Approx(-1.988848893));
        REQUIRE(c.b2 == Catch::Approx(0.994424446));
        REQUIRE(c.a1 == Catch::Approx(-1.980304906));
        REQUIRE(c.a2 == Catch::Approx(0.997392879));
    }
}

