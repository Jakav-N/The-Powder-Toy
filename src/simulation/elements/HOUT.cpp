#include "simulation/ElementCommon.h"

int Element_HOUT_update(UPDATE_FUNC_ARGS);
int Element_HTIN_graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_HOUT()
{
	Identifier = "DEFAULT_PT_HOUT";
	Name = "HOUT";
	Colour = PIXPACK(0x660022);
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.85f;
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

	DefaultProperties.temp = 273.15f;
	HeatConduct = 0;
	Description = "One way heat transfer output.";

	Properties = TYPE_SOLID|PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_HOUT_update;
	Graphics = &Element_HTIN_graphics;
}

int Element_HOUT_update(UPDATE_FUNC_ARGS)
{
	int r, rx, ry, n;

	//Neighbor check loop, share tmp with other HOUT only
	int total_tmp = parts[i].tmp;
	int num_parts = 1;

	//Neighbor check loop, share tmp
	for (rx=-1; rx<2; rx++)
		for (ry=-1; ry<2; ry++)
			if (BOUNDS_CHECK && (rx || ry))
			{
				r = pmap[y+ry][x+rx];

				//Put out heat
				if (r && TYP(r) != PT_HOUT && TYP(r) != PT_HTIN) {
					for (n = 0; n < 10; i++) {
						if (parts[i].tmp > 0 && parts[ID(r)].temp < MAX_TEMP) {
							parts[i].tmp--;
							parts[ID(r)].temp += 10;
						} else {
							break;
						}
					}
				}

				if (!r || TYP(r) != PT_HOUT)
					continue;

				total_tmp = parts[i].tmp + parts[ID(r)].tmp;

				parts[i].tmp = total_tmp / 2;
				
				if (total_tmp % 2 == 1) {
					parts[ID(r)].tmp = total_tmp / 2 + 1;
				} else {
					parts[ID(r)].tmp = total_tmp / 2;
				}
				
			}

	return 0;
}
