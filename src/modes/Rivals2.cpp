#include "modes/Rivals2.hpp"

#define ANALOG_STICK_MIN 0
#define ANALOG_STICK_NEUTRAL 128
#define ANALOG_STICK_MAX 255

Rivals2::Rivals2() : ControllerMode() {}

void Rivals2::UpdateDigitalOutputs(const InputState &inputs, OutputState &outputs) {
    outputs.a = inputs.rt1;
    outputs.b = inputs.rf1;
    outputs.x = inputs.rf2;
    outputs.y = inputs.rf6;
    outputs.buttonR = inputs.rf3;
    if (inputs.nunchuk_connected) {
        // Lightshield with C button.
        if (inputs.nunchuk_c) {
            outputs.triggerLAnalog = 49;
        }
        outputs.triggerLDigital = inputs.nunchuk_z;
    } else {
        outputs.triggerLDigital = inputs.lf4;
    }
    outputs.triggerRDigital = inputs.rf5;

    // function layer
    if (inputs.lt2) {
        outputs.home = inputs.mb1;
    } else if (inputs.lt1) {
        outputs.select = inputs.mb1;
    } else {
        outputs.start = inputs.mb1;
    }

    outputs.leftStickClick = inputs.rf7;
    outputs.rightStickClick = inputs.rf8;
    outputs.buttonL = inputs.rf9; // LB available to be bound
    outputs.dpadLeft = inputs.mb3;
    outputs.dpadRight = inputs.mb4;
    outputs.dpadDown = inputs.mb5;
    outputs.dpadUp = inputs.mb6;

    // Activate D-Pad layer by holding Mod X + Mod Y.
    if ((inputs.lt1 && inputs.lt2) || inputs.mb2) {
        outputs.dpadUp = inputs.rt4;
        outputs.dpadDown = inputs.rt2;
        outputs.dpadLeft = inputs.rt3;
        outputs.dpadRight = inputs.rt5;
    } else { // dpad can be bound
        outputs.dpadUp = inputs.lt3;
        outputs.dpadDown = inputs.lt4;
        outputs.dpadLeft = inputs.lt5;
        outputs.dpadRight = inputs.lt6;
    }
}

void Rivals2::UpdateAnalogOutputs(const InputState &inputs, OutputState &outputs) {
    // Coordinate calculations to make modifier handling simpler.
    UpdateDirections(
        inputs.lf3, // Left
        inputs.lf1, // Right
        inputs.lf2, // Down
        inputs.rf4, // Up
        inputs.rt3, // C-Left
        inputs.rt5, // C-Right
        inputs.rt2, // C-Down
        inputs.rt4, // C-Up
        ANALOG_STICK_MIN,
        ANALOG_STICK_NEUTRAL,
        ANALOG_STICK_MAX,
        outputs
    );

    bool shield_button_pressed = inputs.lf4 || inputs.rf5;
    if (directions.diagonal) {
        if (directions.y == -1 && (shield_button_pressed)) {
            outputs.leftStickX = 128 + (directions.x * 109);
            outputs.leftStickY = 128 + (directions.y * 79);
        } else {
            outputs.leftStickX = 128 + (directions.x * 126);
            outputs.leftStickY = 128 + (directions.y * 127);
        }
    }
    // 48 total DI angles, 24 total Up b angles, 16 total airdodge angles

    if (inputs.lt1) {
        if (directions.horizontal) {
            outputs.leftStickX = 128 + (directions.x * 97);
        }

        if (directions.vertical) {
            outputs.leftStickY = 128 + (directions.y * 79);
        }

        /* Extra DI, Air Dodge, and Up B angles */
        if (directions.diagonal && (!inputs.rf1)) { // 80% mag
            outputs.leftStickX = 128 + (directions.x * 76);
            outputs.leftStickY = 128 + (directions.y * 59);

            if (inputs.rt1) { // angled ftilt
                // outputs.leftStickX = 128 + (directions.x * 60);
                // outputs.leftStickY = 128 + (directions.y * 53);
            }

            // Angles just for DI and Up B
            if (inputs.rt2) {
                outputs.leftStickX = 128 + (directions.x * 79);
                outputs.leftStickY = 128 + (directions.y * 54);
            }

            // Angles just for DI
            if (inputs.rt3) {
                outputs.leftStickX = 128 + (directions.x * 75);
                outputs.leftStickY = 128 + (directions.y * 57);
            }

            if (inputs.rt4) {
                outputs.leftStickX = 128 + (directions.x * 72);
                outputs.leftStickY = 128 + (directions.y * 60);
            }

            if (inputs.rt5) {
                outputs.leftStickX = 128 + (directions.x * 68);
                outputs.leftStickY = 128 + (directions.y * 63);
            }
        } else if (directions.diagonal && inputs.rf1 && !inputs.rf3) { // 100% mag
            outputs.leftStickX = 128 + (directions.x * 102);
            outputs.leftStickY = 128 + (directions.y * 64);

            // Angles just for DI and Up B
            if (inputs.rt2) {
                outputs.leftStickX = 128 + (directions.x * 98);
                outputs.leftStickY = 128 + (directions.y * 68);
            }

            // Angles just for DI
            if (inputs.rt3) {
                outputs.leftStickX = 128 + (directions.x * 94);
                outputs.leftStickY = 128 + (directions.y * 71);
            }

            if (inputs.rt4) {
                outputs.leftStickX = 128 + (directions.x * 90);
                outputs.leftStickY = 128 + (directions.y * 74);
            }

            if (inputs.rt5) {
                outputs.leftStickX = 128 + (directions.x * 84);
                outputs.leftStickY = 128 + (directions.y * 79);
            }
        } else if (directions.diagonal && (inputs.rf1 && inputs.rf3)) { // 60% mag
            outputs.leftStickX = 128 + (directions.x * 61);
            outputs.leftStickY = 128 + (directions.y * 38);

            // Angles just for DI and Up B
            if (inputs.rt2) {
                outputs.leftStickX = 128 + (directions.x * 59);
                outputs.leftStickY = 128 + (directions.y * 41);
            }
            // Angles just for DI
            if (inputs.rt3) {
                outputs.leftStickX = 128 + (directions.x * 57);
                outputs.leftStickY = 128 + (directions.y * 42);
            }

            if (inputs.rt4) {
                outputs.leftStickX = 128 + (directions.x * 53);
                outputs.leftStickY = 128 + (directions.y * 44);
            }

            if (inputs.rt5) {
                outputs.leftStickX = 128 + (directions.x * 51);
                outputs.leftStickY = 128 + (directions.y * 48);
            }
        }
    }

    if (inputs.lt2) {
        if (directions.horizontal) {
            outputs.leftStickX = 128 + (directions.x * 83);
        }

        if (directions.vertical) {
            outputs.leftStickY = 128 + (directions.y * 108);
        }

        /* Extra DI, Air Dodge, and Up B angles */
        if (directions.diagonal && (!inputs.rf1)) { // 80% mag
            outputs.leftStickX = 128 + (directions.x * 52);
            outputs.leftStickY = 128 + (directions.y * 82);

            // Angles just for DI and Up B
            if (inputs.rt2) {
                outputs.leftStickX = 128 + (directions.x * 54);
                outputs.leftStickY = 128 + (directions.y * 79);
            }

            // Angles just for DI
            if (inputs.rt3) {
                outputs.leftStickX = 128 + (directions.x * 57);
                outputs.leftStickY = 128 + (directions.y * 75);
            }

            if (inputs.rt4) {
                outputs.leftStickX = 128 + (directions.x * 60);
                outputs.leftStickY = 128 + (directions.y * 72);
            }

            if (inputs.rt5) {
                outputs.leftStickX = 128 + (directions.x * 63);
                outputs.leftStickY = 128 + (directions.y * 68);
            }
        } else if (directions.diagonal && inputs.rf1 && !inputs.rf3) { // 100% mag
            outputs.leftStickX = 128 + (directions.x * 64);
            outputs.leftStickY = 128 + (directions.y * 102);

            // Angles just for DI and Up B
            if (inputs.rt2) {
                outputs.leftStickX = 128 + (directions.x * 68);
                outputs.leftStickY = 128 + (directions.y * 98);
            }

            // Angles just for DI
            if (inputs.rt3) {
                outputs.leftStickX = 128 + (directions.x * 71);
                outputs.leftStickY = 128 + (directions.y * 94);
            }

            if (inputs.rt4) {
                outputs.leftStickX = 128 + (directions.x * 74);
                outputs.leftStickY = 128 + (directions.y * 90);
            }

            if (inputs.rt5) {
                outputs.leftStickX = 128 + (directions.x * 79);
                outputs.leftStickY = 128 + (directions.y * 84);
            }
        } else if (directions.diagonal && (inputs.rf1 && inputs.rf3)) { // 60% mag
            outputs.leftStickX = 128 + (directions.x * 39);
            outputs.leftStickY = 128 + (directions.y * 61);

            // Angles just for DI and Up B
            if (inputs.rt2) {
                outputs.leftStickX = 128 + (directions.x * 41);
                outputs.leftStickY = 128 + (directions.y * 59);
            }

            // Angles just for DI
            if (inputs.rt3) {
                outputs.leftStickX = 128 + (directions.x * 42);
                outputs.leftStickY = 128 + (directions.y * 57);
            }

            if (inputs.rt4) {
                outputs.leftStickX = 128 + (directions.x * 44);
                outputs.leftStickY = 128 + (directions.y * 53);
            }

            if (inputs.rt5) {
                outputs.leftStickX = 128 + (directions.x * 48);
                outputs.leftStickY = 128 + (directions.y * 51);
            }
        }
    }

    // Shut off C-stick when using D-Pad layer.
    if ((inputs.lt1 && inputs.lt2) || inputs.mb2) {
        outputs.rightStickX = 128;
        outputs.rightStickY = 128;
    }

    // Nunchuk overrides left stick.
    if (inputs.nunchuk_connected) {
        outputs.leftStickX = inputs.nunchuk_x;
        outputs.leftStickY = inputs.nunchuk_y;
    }
}
