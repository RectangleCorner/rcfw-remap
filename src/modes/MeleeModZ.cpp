#include "modes/MeleeModZ.hpp"

#define ANALOG_STICK_MIN 48
#define ANALOG_STICK_NEUTRAL 128
#define ANALOG_STICK_MAX 208

MeleeModZ::MeleeModZ() : ControllerMode() {
    _horizontal_socd = false;
}

void MeleeModZ::SetConfig(GameModeConfig &config, const MeleeOptions options, const bool cwos) {
    InputMode::SetConfig(config);
    _options = options;
    _cwos = cwos;
}
void MeleeModZ::HandleSocd(InputState &inputs) {
    _horizontal_socd = inputs.lf3 && inputs.lf1;
    InputMode::HandleSocd(inputs);
}

void MeleeModZ::UpdateDigitalOutputs(const InputState &inputs, OutputState &outputs) {
    outputs.a = inputs.rt1;
    outputs.b = inputs.rf1;
    outputs.x = inputs.rf2;
    outputs.y = inputs.rf6;
    outputs.buttonR = inputs.rf3;
    if (inputs.nunchuk_connected) {
        outputs.triggerLDigital = inputs.nunchuk_z;
    } else {
        outputs.triggerLDigital = inputs.lf4;
    }
    outputs.triggerRDigital = inputs.rf5;
    outputs.start = inputs.mb1;

    if ((inputs.lt1 && inputs.lt2 && inputs.mb2) || inputs.nunchuk_c) {
        outputs.dpadUp = inputs.rt4;
        outputs.dpadDown = inputs.rt2;
        outputs.dpadLeft = inputs.rt3;
        outputs.dpadRight = inputs.rt5;
    }
}

void MeleeModZ::UpdateAnalogOutputs(const InputState &inputs, OutputState &outputs) {
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

    bool shield_button_pressed = inputs.lf4 || inputs.rf5 || inputs.rf7 || inputs.rf8;

    if (directions.diagonal) {
        // q1/2 = 7000 7000
        // outputs.leftStickX = 128 + (directions.x * 56);
        // outputs.leftStickY = 128 + (directions.y * 61);
        outputs.leftStickX = 128 + (directions.x * 56);
        outputs.leftStickY = 128 + (directions.y * 56);
        // L, R, LS, and MS + q3/4 = 7000 6875 (For vanilla shield drop. Gives 44.5
        // degree wavedash). Also used as default q3/4 diagonal if crouch walk option select is
        // enabled.
        if (directions.y == -1 && _cwos) {
            outputs.leftStickX = 128 + (directions.x * 57);
            outputs.leftStickY = 128 + (directions.y * 55);
        }
    }

    // all modifiers = nothing
    if (!(inputs.lt1 == inputs.lt2 && inputs.lt2 == inputs.mb2)) {
        if (directions.diagonal) {
            if (!inputs.rf1) {
                // Comments are in order from shallowest to steepest
                // Conditionals are ordered to be easy to read the conditions
                // 7375 3125 - 22.96deg - 59 25 - modX + modZ
                // 6750 3500 - 27.41deg - 54 28 - modX
                // 6375 4625 - 35.96deg - 51 37 - modX + modY
                // 45deg - above these is angle off X-axis, below is angle off Y-axis
                // 5125 7000 - 36.21deg - 41 56 - modZ
                // 3625 7000 - 27.38deg - 29 56 - modY
                // 3250 7625 - 23.09deg - 26 61 - modY + modZ

                if (inputs.lt1 && !inputs.lt2 && !inputs.mb2) { // modX
                    outputs.leftStickX = 128 + (directions.x * 54);
                    outputs.leftStickY = 128 + (directions.y * 28);
                } else if (!inputs.lt1 && inputs.lt2 && !inputs.mb2) { // modY
                    outputs.leftStickX = 128 + (directions.x * 29);
                    outputs.leftStickY = 128 + (directions.y * 56);
                } else if (!inputs.lt1 && !inputs.lt2 && inputs.mb2) { // modZ
                    outputs.leftStickX = 128 + (directions.x * 41);
                    outputs.leftStickY = 128 + (directions.y * 56);
                } else if (inputs.lt1 && inputs.lt2 && !inputs.mb2) { // modX + modY
                    outputs.leftStickX = 128 + (directions.x * 51);
                    outputs.leftStickY = 128 + (directions.y * 37);
                } else if (inputs.lt1 && !inputs.lt2 && inputs.mb2) { // modX + modZ
                    outputs.leftStickX = 128 + (directions.x * 59);
                    outputs.leftStickY = 128 + (directions.y * 25);
                } else if (!inputs.lt1 && inputs.lt2 && inputs.mb2) { // modY + modZ
                    outputs.leftStickX = 128 + (directions.x * 26);
                    outputs.leftStickY = 128 + (directions.y * 61);
                }
            } else {
                /* Extended Up B Angles */
                // Comments are in order from shallowest to steepest
                // Conditionals are ordered to be easy to read the conditions
                // 9125 3875 - 23.01deg - 73 31 - modX + modZ
                // 8750 4500 - 27.22deg - 70 36 - modX
                // 7250 5250 - 35.91deg - 58 42 - modX + modY
                // 45deg - above these is angle off X-axis, below is angle off Y-axis
                // 5750 7875 - 36.14deg - 46 63 - modZ
                // 4625 8750 - 27.86deg - 37 70 - modY
                // 3875 9125 - 23.01deg - 31 73 - modY + modZ
                // 5125 7000 - 36.21deg - 41 56 - modZ
                // 3625 7000 - 27.38deg - 29 56 - modY
                // 3250 7625 - 23.09deg - 26 61 - modY + modZ

                if (inputs.lt1 && !inputs.lt2 && !inputs.mb2) { // modX
                    outputs.leftStickX = 128 + (directions.x * 70);
                    outputs.leftStickY = 128 + (directions.y * 36);
                } else if (!inputs.lt1 && inputs.lt2 && !inputs.mb2) { // modY
                    outputs.leftStickX = 128 + (directions.x * 37);
                    outputs.leftStickY = 128 + (directions.y * 70);
                } else if (!inputs.lt1 && !inputs.lt2 && inputs.mb2) { // modZ
                    outputs.leftStickX = 128 + (directions.x * 46);
                    outputs.leftStickY = 128 + (directions.y * 63);
                } else if (inputs.lt1 && inputs.lt2 && !inputs.mb2) { // modX + modY
                    outputs.leftStickX = 128 + (directions.x * 58);
                    outputs.leftStickY = 128 + (directions.y * 42);
                } else if (inputs.lt1 && !inputs.lt2 && inputs.mb2) { // modX + modZ
                    outputs.leftStickX = 128 + (directions.x * 73);
                    outputs.leftStickY = 128 + (directions.y * 31);
                } else if (!inputs.lt1 && inputs.lt2 && inputs.mb2) { // modY + modZ
                    outputs.leftStickX = 128 + (directions.x * 31);
                    outputs.leftStickY = 128 + (directions.y * 73);
                }
            }
        }
        // modZ does nothing on non-diagonal
        // modX + modY should do nothing in this case, similar to legacy behaviour
        else if (inputs.lt1 != inputs.lt2) { // ModZ does nothing on non-diagonal
            if (inputs.lt1) {
                // MX + Horizontal (even if shield is held) = 6625 = 53
                if (directions.horizontal) {
                    outputs.leftStickX = 128 + (directions.x * 53);
                }
                // MX + Vertical (even if shield is held) = 5375 = 43
                // y=-0.5500 (44) is solo nana ice block, so we reduce this by one
                // MX + Vertical (even if shield is held) = 5250 = 42
                if (directions.vertical) {
                    outputs.leftStickY = 128 + (directions.y * 42);
                }
            }
            if (inputs.lt2) {
                // MY + Horizontal (even if shield is held) = 3375 = 27
                if (directions.horizontal) {
                    outputs.leftStickX = 128 + (directions.x * 27);
                }
                // MY + Vertical (even if shield is held) = 7375 = 59
                if (directions.vertical) {
                    outputs.leftStickY = 128 + (directions.y * 59);
                }
            }
        }

        // some logic should be applied regardless of diagonal, do that here
        if (inputs.lt1) {
            // Angled fsmash
            if (directions.cx != 0) {
                // 8500 5250 = 68 42
                outputs.rightStickX = 128 + (directions.cx * 68);
                outputs.rightStickY = 128 + (directions.y * 42);
            }
        }

        if (inputs.lt2) {
            // Turnaround neutral B nerf
            if (inputs.rf1) {
                outputs.leftStickX = 128 + (directions.x * 80);
            }
        }
    }

    // C-stick ASDI Slideoff angle overrides any other C-stick modifiers (such as
    // angled fsmash).
    if (directions.cx != 0 && directions.cy != 0) {
        // 5250 8500 = 42 68
        outputs.rightStickX = 128 + (directions.cx * 42);
        outputs.rightStickY = 128 + (directions.cy * 68);
    }

    // Horizontal SOCD overrides X-axis modifiers (for ledgedash maximum jump
    // trajectory).
    if (!_options.disable_ledgedash_socd_override && _horizontal_socd && !directions.vertical) {
        outputs.leftStickX = 128 + (directions.x * 80);
    }

    if (inputs.rf7) {
        outputs.triggerRAnalog = 49;
    }
    if (inputs.rf8) {
        outputs.triggerRAnalog = 94;
    }

    if (outputs.triggerLDigital) {
        outputs.triggerLAnalog = 140;
    }
    if (outputs.triggerRDigital) {
        outputs.triggerRAnalog = 140;
    }

    // Shut off C-stick when using D-Pad layer.
    if (inputs.lt1 && inputs.lt2 && inputs.mb2) {
        outputs.rightStickX = 128;
        outputs.rightStickY = 128;
    }

    // Nunchuk overrides left stick.
    if (inputs.nunchuk_connected) {
        outputs.leftStickX = inputs.nunchuk_x;
        outputs.leftStickY = inputs.nunchuk_y;
    }
}
