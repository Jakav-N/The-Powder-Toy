#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_STAR()
{
	Identifier = "DEFAULT_PT_STAR";
	Name = "STAR";
	Colour = PIXPACK(0xFFFF00);
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
	Hardness = 0;

	Weight = 100;

	HeatConduct = 186;
	Description = "Star. Creates heat.";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;
	
	Update = &update;
	Graphics = &graphics;
	Create = &create;
}

static int update(UPDATE_FUNC_ARGS)
{
	int r, rx, ry;
	bool doExplosion = false;

	for (rx = -1; rx <= 1; rx++)
		for (ry = -1; ry <= 1; ry++) {
			if (rx || ry) {
				r = pmap[y + ry][x + rx];
				
				if (TYP(r) == PT_WATR || TYP(r) == PT_WTRV || TYP(r) == PT_ABTL || TYP(r) == PT_NONE)
					continue;
				
				parts[i].life = 20;
				break;
			}
		}


	sim->pv[y/CELL][x/CELL] += 0.2f;

	//Heating (to exactly 9000 degrees C)
	if (parts[i].temp <= 8990.0f + 273.15f)
		parts[i].temp += 10.0f;

	if (parts[i].temp < 9000.0f + 273.15f && parts[i].temp >= 8990.0f + 273.15f)
		parts[i].temp = 9000.0f + 273.15f;

	return 0;
}

static void create(ELEMENT_CREATE_FUNC_ARGS) {
	sim->parts[i].tmp = RNG::Ref().between(1, 256);
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	*pixel_mode |= PMODE_FLARE;

	//Yellow coloring in the surrounding area
	*firer = (int)(256);
	*fireg = (int)(256);
	*fireb = cpart->tmp;
	*firea = 50;
	*pixel_mode |= FIRE_ADD;

	return 1;
}
