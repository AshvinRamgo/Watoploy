#include "squarefactory.h"
#include "academicbuilding.h"
#include "residence.h"
#include "gym.h"
#include "nonownable.h"
#include <memory>
#include <string>
#include <vector>

using namespace std;

shared_ptr<Square> SquareFactory::createSquare(int position, const string& name) {
    //  Academic Buildings 
    if (name == "AL") return make_shared<AcademicBuilding>(position, name, 40, "Arts1", 50, 2, 10, 30, 90, 160, 250);
    if (name == "ML") return make_shared<AcademicBuilding>(position, name, 60, "Arts1", 50, 4, 20, 60, 180, 320, 450);

    if (name == "ECH") return make_shared<AcademicBuilding>(position, name, 100, "Arts2", 50, 6, 30, 90, 270, 400, 550);
    if (name == "PAS") return make_shared<AcademicBuilding>(position, name, 100, "Arts2", 50, 6, 30, 90, 270, 400, 550);
    if (name == "HH") return make_shared<AcademicBuilding>(position, name, 120, "Arts2", 50, 8, 40, 100, 300, 450, 600);

    if (name == "RCH") return make_shared<AcademicBuilding>(position, name, 140, "Eng", 100, 10, 50, 150, 450, 625, 750);
    if (name == "DWE") return make_shared<AcademicBuilding>(position, name, 140, "Eng", 100, 10, 50, 150, 450, 625, 750);
    if (name == "CPH") return make_shared<AcademicBuilding>(position, name, 160, "Eng", 100, 12, 60, 180, 500, 700, 900);

    if (name == "LHI") return make_shared<AcademicBuilding>(position, name, 180, "Health", 100, 14, 70, 200, 550, 750, 950);
    if (name == "BMH") return make_shared<AcademicBuilding>(position, name, 200, "Health", 100, 16, 80, 220, 600, 800, 1000);
    if (name == "OPT") return make_shared<AcademicBuilding>(position, name, 220, "Health", 150, 18, 90, 250, 700, 875, 1050);

    if (name == "EV1") return make_shared<AcademicBuilding>(position, name, 220, "Env", 150, 18, 90, 250, 700, 875, 1050);
    if (name == "EV2") return make_shared<AcademicBuilding>(position, name, 240, "Env", 150, 20, 100, 300, 750, 925, 1100);
    if (name == "EV3") return make_shared<AcademicBuilding>(position, name, 260, "Env", 150, 22, 110, 330, 800, 975, 1150);

    if (name == "PHYS") return make_shared<AcademicBuilding>(position, name, 260, "Sci1", 150, 22, 110, 330, 800, 975, 1150);
    if (name == "B1") return make_shared<AcademicBuilding>(position, name, 260, "Sci1", 150, 22, 110, 330, 800, 975, 1150);
    if (name == "B2") return make_shared<AcademicBuilding>(position, name, 280, "Sci1", 150, 24, 120, 360, 850, 1025, 1200);

    if (name == "EIT") return make_shared<AcademicBuilding>(position, name, 300, "Sci2", 200, 26, 130, 390, 900, 1100, 1275);
    if (name == "ESC") return make_shared<AcademicBuilding>(position, name, 300, "Sci2", 200, 26, 130, 390, 900, 1100, 1275);
    if (name == "C2") return make_shared<AcademicBuilding>(position, name, 320, "Sci2", 200, 28, 150, 450, 1000, 1200, 1400);

    if (name == "MC") return make_shared<AcademicBuilding>(position, name, 350, "Math", 200, 35, 175, 500, 1100, 1300, 1500);
    if (name == "DC") return make_shared<AcademicBuilding>(position, name, 400, "Math", 200, 50, 200, 600, 1400, 1700, 2000);

    //  Residences 
    if (name == "MKV") return make_shared<Residence>(position, name);
    if (name == "UWP") return make_shared<Residence>(position, name);
    if (name == "V1") return make_shared<Residence>(position, name);
    if (name == "REV") return make_shared<Residence>(position, name);

    //  Gyms 
    if (name == "PAC") return make_shared<Gym>(position, name);
    if (name == "CIF GYM") return make_shared<Gym>(position, name);

    //  Non-ownables 
    if (name == "COLLECT OSAP") return make_shared<CollectOSAP>(position);
    if (name == "COOP FEE") return make_shared<CoopFee>(position);
    if (name == "DC TIMS LINE") return make_shared<DCTimsLine>(position);
    if (name == "GO TO TIMS") return make_shared<GoToTims>(position);
    if (name == "GOOSE NESTING") return make_shared<GooseNesting>(position);
    if (name == "SLC") return make_shared<SLC>(position);
    if (name == "TUITION") return make_shared<Tuition>(position);
    if (name == "NEEDLES HALL") return make_shared<NeedlesHall>(position);

    //  Fallback 
    return nullptr;
}

shared_ptr<Square> SquareFactory::createSquareWithBoard(int position, const string& name, shared_ptr<Board> board) {
    // Special handling for SLC and NeedlesHall to pass board reference
    if (name == "SLC") return make_shared<SLC>(position, board);
    if (name == "NEEDLES HALL") return make_shared<NeedlesHall>(position, board);
    
    // For all other squares, use the regular createSquare method
    return createSquare(position, name);
}
