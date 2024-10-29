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

    if (inputs.lt1 && inputs.lt2 && inputs.mb2 && inputs.mb1) {
        outputs.triggerLDigital = true;
        outputs.triggerRDigital = true;
        outputs.a = true;
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
        outputs.leftStickY = 128 + (directions.y * 61);
        // L, R, LS, and MS + q3/4 = 7000 6875 (For vanilla shield drop. Gives 44.5
        // degree wavedash). Also used as default q3/4 diagonal if crouch walk option select is
        // enabled.
        if (directions.y == -1 && (shield_button_pressed || _cwos)) {
            outputs.leftStickX = 128 + (directions.x * 61);
            outputs.leftStickY = 128 + (directions.y * 56);
        }
    }

    // all modifiers = nothing
    if (!(inputs.lt1 == inputs.lt2 && inputs.lt2 == inputs.mb2)) {
        if (directions.diagonal) {
            if (!inputs.rf1) {
                // Comments are in order from shallowest to steepest
                // Conditionals are ordered to be easy to read the conditions
                // 7750 3000 - 21.16deg ("fuzzing safe" 20) - 62 24 - modX + modZ
                // 6875 3750 - 28.61deg ("fuzzing safe" 27) - 55 30 - modX
                // 6375 4625 - 35.96deg                     - 51 37 - modX + modY
                // 45deg - above these is angle off X-axis, below is angle off Y-axis
                ///////// 5125 7000 - 36.21deg                     - 41 56 - modZ
                // modZ and modX + modY are asymmetric for Y>0 to avoid 2f turnaround utilt/dtilt
                // coords modZ and modX + modY are asymmetric for Y<0 to avoid shield drop down
                // coords 5000 6750 - 36.53deg                     - 40 54 - modZ Y>0 5250 7125
                // - 36.38deg                     - 42 57 - modZ Y<0 modY and modX are asymmetric to
                // avoid shield drop down coords 3875 7125 - 28.54deg ("fuzzing safe" 27) - 31 57 -
                // modY 3000 7750 - 21.16deg ("fuzzing safe" 20) - 24 62 - modY + modZ
                if (inputs.lt1 && !inputs.lt2 && !inputs.mb2) { // modX
                    outputs.leftStickX = 128 + (directions.x * 55);
                    outputs.leftStickY = 128 + (directions.y * 30);
                } else if (!inputs.lt1 && inputs.lt2 && !inputs.mb2) { // modY
                    outputs.leftStickX = 128 + (directions.x * 31);
                    outputs.leftStickY = 128 + (directions.y * 57);
                } else if (!inputs.lt1 && !inputs.lt2 && inputs.mb2) { // modZ
                    if (directions.y == -1) {
                        outputs.leftStickX = 128 + (directions.x * 42);
                        outputs.leftStickY = 128 + (directions.y * 57);
                    } else {
                        outputs.leftStickX = 128 + (directions.x * 40);
                        outputs.leftStickY = 128 + (directions.y * 54);
                    }
                } else if (inputs.lt1 && inputs.lt2 && !inputs.mb2) { // modX + modY
                    outputs.leftStickX = 128 + (directions.x * 51);
                    outputs.leftStickY = 128 + (directions.y * 37);
                } else if (inputs.lt1 && !inputs.lt2 && inputs.mb2) { // modX + modZ
                    // manually override MX + MZ to MX angle if holding shield
                    // to comply with 27deg limitation and avoid being clamped
                    if (shield_button_pressed) {
                        outputs.leftStickX = 128 + (directions.x * 55);
                        outputs.leftStickY = 128 + (directions.y * 30);
                    } else {
                        outputs.leftStickX = 128 + (directions.x * 62);
                        outputs.leftStickY = 128 + (directions.y * 24);
                    }
                } else if (!inputs.lt1 && inputs.lt2 && inputs.mb2) { // modY + modZ
                    // manually override MY + MZ to MX angle if holding shield
                    // to comply with 27deg limitation and avoid being clamped
                    if (shield_button_pressed) {
                        outputs.leftStickX = 128 + (directions.x * 31);
                        outputs.leftStickY = 128 + (directions.y * 57);
                    } else {
                        outputs.leftStickX = 128 + (directions.x * 24);
                        outputs.leftStickY = 128 + (directions.y * 62);
                    }
                }
            } else {
                /* Extended Up B Angles */
                // Comments are in order from shallowest to steepest
                // Conditionals are ordered to be easy to read the conditions
                // 9250 3625 - 21.40deg                     - 74 29 - modX + modZ
                // 8750 4750 - 28.50deg ("fuzzing safe" 27) - 70 38 - modX
                // modX + modY and modZ are asymmetric for Y>0 to avoid no buffer jumpless up b
                // coords 7375 5375 - 36.09deg                     - 59 43 - modX + modY Y>0 8000
                // 5875 - 36.09deg                     - 64 47 - modX + modY Y<0 45deg - above these
                // is angle off X-axis, below is angle off Y-axis 5875 8000 - 36.29deg - 47 64 -
                // modZ 4750 8750 - 28.50deg ("fuzzing safe" 27) - 38 70 - modY 3625 9250 - 21.40deg
                // - 29 74 - modY + modZ

                if (inputs.lt1 && !inputs.lt2 && !inputs.mb2) { // modX
                    outputs.leftStickX = 128 + (directions.x * 70);
                    outputs.leftStickY = 128 + (directions.y * 38);
                } else if (!inputs.lt1 && inputs.lt2 && !inputs.mb2) { // modY
                    outputs.leftStickX = 128 + (directions.x * 38);
                    outputs.leftStickY = 128 + (directions.y * 70);
                } else if (!inputs.lt1 && !inputs.lt2 && inputs.mb2) { // modZ
                    outputs.leftStickX = 128 + (directions.x * 47);
                    outputs.leftStickY = 128 + (directions.y * 64);
                } else if (inputs.lt1 && inputs.lt2 && !inputs.mb2) { // modX + modY
                    if (directions.y == -1) {
                        outputs.leftStickX = 128 + (directions.x * 64);
                        outputs.leftStickY = 128 + (directions.y * 47);
                    } else {
                        outputs.leftStickX = 128 + (directions.x * 59);
                        outputs.leftStickY = 128 + (directions.y * 43);
                    }
                } else if (inputs.lt1 && !inputs.lt2 && inputs.mb2) { // modX + modZ
                    // manually override MX + MZ to MX angle if holding shield
                    // to comply with 27deg limitation and avoid being clamped
                    if (shield_button_pressed) {
                        outputs.leftStickX = 128 + (directions.x * 70);
                        outputs.leftStickY = 128 + (directions.y * 38);
                    } else {
                        outputs.leftStickX = 128 + (directions.x * 74);
                        outputs.leftStickY = 128 + (directions.y * 29);
                    }
                } else if (!inputs.lt1 && inputs.lt2 && inputs.mb2) { // modY + modZ
                    // manually override MY + MZ to MX angle if holding shield
                    // to comply with 27deg limitation and avoid being clamped
                    if (shield_button_pressed) {
                        outputs.leftStickX = 128 + (directions.x * 38);
                        outputs.leftStickY = 128 + (directions.y * 70);
                    } else {
                        outputs.leftStickX = 128 + (directions.x * 29);
                        outputs.leftStickY = 128 + (directions.y * 74);
                    }
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
                // Turnaround neutral B nerf
                if (inputs.rf1) {
                    outputs.leftStickX = 128 + (directions.x * 80);
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
    }

    // C-stick ASDI Slideoff angle overrides any other C-stick modifiers (such as
    // angled fsmash).
    if (directions.cx != 0 && directions.cy != 0) {
        // 5250 8500 = 42 68
        outputs.rightStickX = 128 + (directions.cx * 42);
        outputs.rightStickY = 128 + (directions.cy * 68);
    }

    /*
    // Horizontal SOCD overrides X-axis modifiers (for ledgedash maximum jump
    // trajectory).
    if (!_options.disable_ledgedash_socd_override && _horizontal_socd && !directions.vertical) {
        outputs.leftStickX = 128 + (directions.x * 80);
    }
    */

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
    if ((inputs.lt1 && inputs.lt2 && inputs.mb2) || inputs.nunchuk_c) {
        outputs.rightStickX = 128;
        outputs.rightStickY = 128;
    }

    // Nunchuk overrides left stick.
    if (inputs.nunchuk_connected) {
        outputs.leftStickX = inputs.nunchuk_x;
        outputs.leftStickY = inputs.nunchuk_y;
    }
}