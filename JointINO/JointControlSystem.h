#include "./Joint.h"

class JointControlSystem
{

public:
    enum gripType
    {
        GRIP_NONE,
        GRIP_FLAT,
        GRIP_MOUSE,
        GRIP_TRIPOD,
        GRIP_POINTER,
        GRIP_HOOK,
        GRIP_POWER
    };

    enum jointIndex
    {
        THUMB,
        INDEX,
        OTHER,
        WRIST_FLEX,
        WRIST_ROT
    };

    enum signalIndex
    {
        flex_carpi_rad,
        flex_carpi_uln,
        ext_carpi_rad,
        ext_carpi_uln,
        flex_digitorium,
        ext_digitorium,
        ext_digiti_minimi,
        ext_indicis
    };

    Joint joints[]; // array of defined joints

    JointControlSystem();
    ~JointControlSystem();

    void processSignals(float signal[]);
    int getGrip();
};