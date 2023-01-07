#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_CRST()
{
	Identifier = "DEFAULT_PT_CRST";
	Name = "CRST";
	Colour = PIXPACK(0xf72d69);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;

	Weight = 30;

	DefaultProperties.temp = 22.0f + 273.15f;
	HeatConduct = 29;
	Description = "Crazy crystal. A crystal that infects things like a virus.";

	Properties = TYPE_PART | PROP_LIFE_DEC | PROP_LIFE_KILL_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 273.15f + 1000.0f;
	HighTemperatureTransition = PT_LAVA;

    DefaultProperties.tmp = 180;

	Update = &update;
	Graphics = &graphics;
}

//Takes a hue, between 0-360, and a saturation and value each between 0 and 1.
int* hsvToRgb (int hue, float saturation, float value) {
    float C = saturation * value;
    float X = C * (1 - abs(fmod((float)(hue) / 60.0f, 2) - 1));
    float m = value - C;
    float r, g, b;

    if (hue >= 0 && hue < 60) {
        r = 0.9f, g = X, b = 0; //C instead of 0.9
    } else if (hue >= 60 && hue < 120) {
        r = X, g = 0.9f, b = 0;
    } else if (hue >= 120 && hue < 180) {
        r = 0, g = 0.9f, b = X;
    } else if (hue >= 180 && hue < 240) {
        r = 0, g = X, b = 0.9f;
    } else if (hue >= 240 && hue < 300) {
        r = X, g = 0, b = 0.9f;
    } else {
        r = 0.9f, g = 0, b = X;
    }

    int R = (int)((r + m) * 255);
    int G = (int)((g + m) * 255);
    int B = (int)((b + m) * 255);
    static int color[3] = {R, G, B};


    return color;
}

/*void rgbFromHue (int hue) {
    //float C = 0.9f;
    //float X = 0.9 * (1 - abs(fmod((float)(hue) / 60.0f, 2) - 1)) + 0.1;

    //TODO: Finish these calculations.
    //float before_abs = fmod((float)(hue) / 60.0f, 2) - 1;
    float before_abs = ((float)(hue % 120)) / 60.0f - 1.0f;
    float X = 0.9 * (1 - (before_abs > 0 ? before_abs : -before_abs) - 1) + 0.1;
    //float m = 0.1f;
    float r, g, b;

    if (hue >= 0 && hue < 60) {
        r = 1.0f, g = X, b = 0.1f; //C instead of 0.9
    } else if (hue >= 60 && hue < 120) {
        r = X, g = 1.0f, b = 0.1f;
    } else if (hue >= 120 && hue < 180) {
        r = 0.1f, g = 1.0f, b = X;
    } else if (hue >= 180 && hue < 240) {
        r = 0.1f, g = X, b = 1.0f;
    } else if (hue >= 240 && hue < 300) {
        r = X, g = 0.1f, b = 1.0f;
    } else {
        r = 1.0f, g = 0.1f, b = X;
    }

    const int color[3] = {(int)(r * 255), (int)(g * 255), (int)(b * 255)};

    //return &color;
}

int* abcdef (int hue) {
    static int color[3] = {(int)(hue * 255), (int)(hue * 255), (int)(hue * 255)};

    return color;
}*/

static int update(UPDATE_FUNC_ARGS)
{
    int rng_value = RNG::Ref().between(1,8);

    //1/4 chance, grow
    if (rng_value <= 2) {
        //Growing

        //Get rx and ry (copied from QRTZ.cpp)
        int rnd = RNG::Ref().gen() % 0x3FF;
		int rx = (rnd%3) - 1;
		rnd >>= 2;
		int ry = (rnd%3) - 1;
        rnd >>= 2;

        if (BOUNDS_CHECK && (rx || ry)) {
            int new_particle = sim->create_part(-1, x + rx, y + ry, PT_CRST);

            if (new_particle > -1) {
                parts[new_particle].tmp = std::clamp(parts[i].tmp + (rnd % 32) - 16, 0, 360);
            }
        }
    }

	//Cooling (somewhat slow)
	if (parts[i].temp > 23.0f + 273.15f) {
		parts[i].temp -= 1.0f;
	}

	return 0;
}


static int graphics(GRAPHICS_FUNC_ARGS)
{
	*pixel_mode |= PMODE_FLARE;

    int hue = cpart->tmp;

    float X = 0.9f * (1 - abs(fmod((float)(hue) / 60.0f, 2) - 1));
    float r, g, b;

    if (hue >= 0 && hue < 60) {
        r = 1.0f, g = X, b = 0.1f;
    } else if (hue >= 60 && hue < 120) {
        r = X, g = 1.0f, b = 0.1f;
    } else if (hue >= 120 && hue < 180) {
        r = 0.1f, g = 1.0f, b = X;
    } else if (hue >= 180 && hue < 240) {
        r = 0.1f, g = X, b = 1.0f;
    } else if (hue >= 240 && hue < 300) {
        r = X, g = 0.1f, b = 1.0f;
    } else {
        r = 1.0f, g = 0.1f, b = X;
    }

    *colr = (int)(r * 255);
    *colb = (int)(g * 255);
    *colg = (int)(b * 255);
	return 0;
}
