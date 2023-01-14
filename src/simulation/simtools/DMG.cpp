#include "simulation/ToolCommon.h"

static int perform(Simulation * sim, Particle * cpart, int x, int y, int brushX, int brushY, float strength);

void SimTool::Tool_DMG()
{
	Identifier = "DEFAULT_TOOL_DMG";
	Name = "DMG";
	Colour = PIXPACK(0xeb4205);
	Description = "Heats the targeted element.";
	Perform = &perform;
}

static int perform(Simulation * sim, Particle * cpart, int x, int y, int brushX, int brushY, float strength)
{
	if(!cpart)
		return 0;
    
    cpart->temp += 3000.0f;
    
    if (cpart->temp > 11000.0f) {
        cpart->temp = 0.0f;
    }

    sim->pv[y/CELL][x/CELL] += 97.0f;

    if (sim->pv[y/CELL][x/CELL] > 1000.0f) {
        sim->pv[y/CELL][x/CELL] = -1000.0f;
    }
	
	return 1;
}
