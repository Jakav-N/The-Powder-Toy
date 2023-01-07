#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_EBMB()
{
	Identifier = "DEFAULT_PT_EBMB";
	Name = "EBMB";
	Colour = PIXPACK(0x808080);
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

	DefaultProperties.temp = 22.0f + 273.15f;
	HeatConduct = 200;
	Description = "Element bomb. It blows up, creating a lot of its ctype.";

	Properties = TYPE_PART | PROP_LIFE_DEC | PROP_LIFE_KILL_DEC | PROP_NOCTYPEDRAW;

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

	CtypeDraw = &Element::ctypeDrawVInCtype;
}

static int update(UPDATE_FUNC_ARGS)
{
	int r, rx, ry;

	//Some code from CONV.cpp to set the ctype. It's slightly modified.
	int ctype = TYP(parts[i].ctype);
	if (!sim->IsElement(ctype) || ctype==PT_EBMB)
	{
		for (rx=-1; rx<2; rx++)
			for (ry=-1; ry<2; ry++)
				if (BOUNDS_CHECK)
				{
					r = sim->photons[y+ry][x+rx];
					if (!r)
						r = pmap[y+ry][x+rx];
					if (!r)
						continue;
					int rt = TYP(r);
					if (rt != PT_CLNE && rt != PT_PCLN &&
						rt != PT_STKM && rt != PT_PBCN &&
						rt != PT_STKM2 && rt != PT_CONV &&
						rt != PT_EBMB && rt < PT_NUM)
					{
						parts[i].ctype = rt;
						if (rt == PT_LIFE)
							parts[i].ctype |= PMAPID(parts[ID(r)].ctype);
					}
				}
	}
	else {

		if (parts[i].life == 0) {
			if (sim->pv[y/CELL][x/CELL] > 20.0f || (parts[i].vx + parts[i].vy) > 10.0f)
				parts[i].life = 30;
		
			for (rx = -1; rx <= 1; rx++)
					for (ry = -1; ry <= 1; ry++) {
						int rt = TYP(pmap[y + ry][x + rx]);

						if (rt == PT_EMBR || rt == PT_BOMB || rt == PT_DEST || rt == PT_SING || rt == PT_AMTR)
							parts[i].life = 30;
					}
		}
		
	
		if (parts[i].life)
			for (rx = -1; rx <= 1; rx++)
				for (ry = -1; ry <= 1; ry++) {
					r = pmap[y + ry][x + rx];

					if (TYP(r) == parts[i].ctype || TYP(r) == PT_EBMB)
						continue;
					else {
						sim->create_part(-1, x + rx, y + ry, parts[i].ctype);
						sim->pv[y/CELL][x/CELL]+=10.0f;
					}
				}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{	
	*colr = 128;
	*colg = 128;
	*colb = 128;

	if (ren->sim->IsElement(cpart->ctype)) {
		int ctype_color = ren->sim->elements[cpart->ctype].Colour;

		*colr = PIXR(ctype_color);
		*colg = PIXG(ctype_color);
		*colb = PIXB(ctype_color);
	}

	*pixel_mode |= PMODE_FLARE;
	return 0;
}
