#include "simulation/ElementCommon.h"

int Element_HTIN_update(UPDATE_FUNC_ARGS);
int Element_HTIN_graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_HTIN()
{
	Identifier = "DEFAULT_PT_HTIN";
	Name = "HTIN";
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

	DefaultProperties.temp = 0.0f;
	HeatConduct = 254;
	Description = "One way heat transfer input.";

	Properties = TYPE_SOLID|PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_HTIN_update;
	Graphics = &Element_HTIN_graphics;
}

int Element_HTIN_update(UPDATE_FUNC_ARGS)
{
	int r, rx, ry, total_tmp;

	//Neighbor check loop
	for (rx=-1; rx<2; rx++)
		for (ry=-1; ry<2; ry++)
			if (BOUNDS_CHECK && (rx || ry))
			{
				r = pmap[y+ry][x+rx];

				//Heat absorption
				if (r && parts[ID(r)].temp > 10.0f) {
					parts[i].tmp++;
					parts[ID(r)].temp -= 10;
				}

				if (!r || (TYP(r) != PT_HTIN && TYP(r) != PT_HOUT))
					continue;

				//Distribute tmp
				total_tmp = parts[i].tmp + parts[ID(r)].tmp;

				parts[i].tmp = total_tmp / 2;
				parts[ID(r)].tmp = total_tmp / 2;

				//Distribute the extra tmp, if any
				if (total_tmp % 2 == 1 && RNG::Ref().chance(1, 2)) {
					parts[i].tmp += 1;
				} else if (total_tmp % 2 == 1) {
					parts[ID(r)].tmp += 1;
				}
			}
	
	return 0;
}

int Element_HTIN_graphics(GRAPHICS_FUNC_ARGS)
{
	float gradient = (float)(cpart->tmp)/10;
	*colr = (int)(restrict_flt(gradient, 0.0f, 255.0f));

	return 0;
}
