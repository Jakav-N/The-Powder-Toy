#include "simulation/ElementCommon.h"
#include "simulation/Simulation.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_IIVS()
{
	Identifier = "DEFAULT_PT_IIVS";
	Name = "IIVS";
	Colour = PIXPACK(0x707070);
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

	HeatConduct = 150;
	Description = "Very strong infectious iron ingot. Suggestion from younger sibling.";

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
}

static int update(UPDATE_FUNC_ARGS)
{
	int r, rx, ry, rt, nb, proton;

    //Equalizing properties
	if (sim->pv[y/CELL][x/CELL] > 10.0f) {
        sim->pv[y/CELL][x/CELL] -= 1.0f;
    } else if (sim->pv[y/CELL][x/CELL] < -10.0f) {
        sim->pv[y/CELL][x/CELL] += 1.0f;
    }

	if (parts[i].temp > 273.15f + 23.0f) {
        sim->pv[y/CELL][x/CELL] -= 1.0f;
    } else if (parts[i].temp > 273.15f + 21.0f) {
        sim->pv[y/CELL][x/CELL] += 1.0f;
    }

	for (rx=-3; rx<=3; rx++) {
		for (ry=-3; ry<=3; ry++) {
			if (BOUNDS_CHECK && (rx || ry)) {
				int r = pmap[y + ry][x + rx];
				rt = TYP(r);


                if (rt == PT_IIVS)
                    continue;
                
                if (rt) {
                    sim->part_change_type(ID(r), x + rx, y + ry, PT_IIVS);
                }
                
			}
		}
	}

	return 0;
}
