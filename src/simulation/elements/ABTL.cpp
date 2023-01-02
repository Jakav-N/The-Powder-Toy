#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_ABTL()
{
	Identifier = "DEFAULT_PT_ABTL";
	Name = "ABTL";
	Colour = PIXPACK(0x3a34eb);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	Advection = 0.6f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.95f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;

	Weight = 30;

	DefaultProperties.temp = 10.0f + 273.15f;
	HeatConduct = 29;
	Description = "Ablator. Creates a large portion of water and explodes when it touches something.";

	Properties = TYPE_PART | PROP_LIFE_DEC | PROP_LIFE_KILL_DEC;

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
	
	if (parts[i].life)
		for (rx = -2; rx <= 2; rx++)
			for (ry = -2; ry <= 2; ry++) {
				r = pmap[y + ry][x + rx];

				if (TYP(r) == PT_WATR || TYP(r) == PT_WTRV || TYP(r) == PT_ABTL)
					continue;
				else if (TYP(r) != PT_DMND && TYP(r) != PT_INDE) {
					if (TYP(r) != PT_NONE) {
						sim->kill_part(ID(r));
					}

					sim->create_part(-3, x + rx, y + ry, PT_WATR);
					sim->pv[y/CELL][x/CELL]+=5.0f;
				}
			}

	//Cooling
	if (parts[i].temp > 32.0f + 273.15f) {
		parts[i].temp -= 10.0f;
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	*pixel_mode |= PMODE_FLARE;
	return 1;
}
