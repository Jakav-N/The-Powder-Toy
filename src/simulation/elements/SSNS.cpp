#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_SSNS()
{
	Identifier = "DEFAULT_PT_SSNS";
	Name = "SSNS";
	Colour = PIXPACK(0xbdb82f);
	MenuVisible = 1;
	MenuSection = SC_SENSOR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.96f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;

	Weight = 100;

	DefaultProperties.temp = 22.0f + 273.15f;
	HeatConduct = 0;
	Description = "Setting sensor, similar to other sensors. See wiki for more info.";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	DefaultProperties.tmp2 = 2;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	int rd = parts[i].tmp2;
	if (rd > 25) parts[i].tmp2 = rd = 25;
	parts[i].life = std::clamp(parts[i].life, 0, 4);

	bool doSerialization = false;
	bool doDeserialization = false;
	bool doSparking = false;
	int value = 0;

	for (int rx = -rd; rx < rd + 1; rx++)
		for (int ry = -rd; ry < rd + 1; ry++)
			if (x + rx >= 0 && y + ry >= 0 && x + rx < XRES && y + ry < YRES && (rx || ry))
			{
				int r = pmap[y + ry][x + rx];
				if (!r)
					r = sim->photons[y + ry][x + rx];
				if (!r)
					continue;

				//List of possible values
				int* const values[] = {&parts[ID(r)].ctype, &parts[ID(r)].tmp,
					&parts[ID(r)].tmp2, &parts[ID(r)].tmp3, &parts[ID(r)].tmp4};

				switch (parts[i].tmp)
				{
				case 1:
					// serialization into FILT
					if (TYP(r) != PT_SSNS && TYP(r) != PT_FILT && parts[ID(r)].life >= 0)
					{
						doSerialization = true;

						//Get the right value
						value = *values[parts[i].life];
					}
					break;
				case 3:
					// .life deserialization
					if (TYP(r) == PT_FILT)
					{
						doDeserialization = true;
						value = parts[ID(r)].ctype;
					}
					break;
				case 2:
					// Greater than mode
					if (*values[parts[i].life] > parts[i].ctype)
						doSparking = true;
					break;
				case 4:
					// Less than mode
					if (*values[parts[i].life] <= parts[i].ctype)
						doSparking = true;
					break;
				default:
					// Normal mode (equals)
					if (*values[parts[i].life] == parts[i].ctype)
						doSparking = true;
					break;
				}
			}

		

	for (int rx = -1; rx <= 1; rx++)
		for (int ry = -1; ry <= 1; ry++)
			if (BOUNDS_CHECK && (rx || ry))
			{
				int r = pmap[y + ry][x + rx];
				if (!r)
					r = sim->photons[y + ry][x + rx];
				if (!r)
					continue;
				int nx = x + rx;
				int ny = y + ry;
				// .life serialization.
				if (doSerialization)
				{
					while (TYP(r) == PT_FILT)
					{
						if (parts[i].ctype == 0) {
							parts[ID(r)].ctype = 0x10000000 + value;
						}
						
						nx += rx;
						ny += ry;
						if (nx < 0 || ny < 0 || nx >= XRES || ny >= YRES)
							break;
						r = pmap[ny][nx];
					}
				}
				// .life deserialization.
				else if (doDeserialization)
				{
					if (TYP(r) != PT_FILT) {
						//List of possible values
						int* const possibleValues[] = {&parts[ID(r)].ctype, &parts[ID(r)].tmp,
							&parts[ID(r)].tmp2, &parts[ID(r)].tmp3, &parts[ID(r)].tmp4};
						
						*possibleValues[parts[i].life] = value - 0x10000000;
					}
					
				}
				//Sparking nearby elements
				else if (doSparking)
				{
					int rt = TYP(r);
					if (sim->parts_avg(i, ID(r), PT_INSL) != PT_INSL)
					{
						if ((sim->elements[rt].Properties&PROP_CONDUCTS) && !(rt == PT_WATR || rt == PT_SLTW || rt == PT_NTCT || rt == PT_PTCT || rt == PT_INWR) && parts[ID(r)].life == 0)
						{
							parts[ID(r)].life = 4;
							parts[ID(r)].ctype = rt;
							sim->part_change_type(ID(r), x + rx, y + ry, PT_SPRK);
						}
					}
				}
			}

	return 0;
}
