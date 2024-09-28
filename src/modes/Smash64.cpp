#include "modes/Smash64.hpp"

#define ANALOG_STICK_MIN 48
#define ANALOG_STICK_NEUTRAL 128
#define ANALOG_STICK_MAX 208

Smash64::Smash64() : ControllerMode() {}

void Smash64::SetConfig(GameModeConfig &config) {
    InputMode::SetConfig(config);
}

void Smash64::HandleSocd(InputState &inputs) {
    InputMode::HandleSocd(inputs);
}

void Smash64::UpdateDigitalOutputs(const InputState &inputs, OutputState &outputs) {
    outputs.a = inputs.rt1;
    outputs.b = inputs.rf1;
    outputs.buttonR = inputs.lf4;
    if (inputs.nunchuk_connected) {
        outputs.triggerLDigital = inputs.nunchuk_z;
    } else {
        outputs.triggerLDigital = inputs.rf5;
    }
    outputs.triggerRDigital = inputs.rf3;
    outputs.start = inputs.mb1;

    // Activate D-Pad layer by holding Mod X + Mod Y or Nunchuk C button.
    if ((inputs.lt1 && inputs.lt2) || inputs.nunchuk_c || inputs.mb2) {
        outputs.dpadUp = inputs.rt4;
        outputs.dpadDown = inputs.rt2;
        outputs.dpadLeft = inputs.rt3;
        outputs.dpadRight = inputs.rt5;
    }
}

void Smash64::UpdateAnalogOutputs(const InputState &inputs, OutputState &outputs) {
    // Coordinate calculations to make modifier handling simpler.
    UpdateDirections(
        inputs.lf3, // Left
        inputs.lf1, // Right
        inputs.lf2, // Down
        inputs.rf4, // Up
        inputs.rt3, // C-Left
        inputs.rf6, // Y
        inputs.lt1, // mod x
        inputs.rf2, // x
        ANALOG_STICK_MIN,
        ANALOG_STICK_NEUTRAL,
        ANALOG_STICK_MAX,
        outputs
    );

    bool shield_button_pressed = inputs.rf3 || inputs.rf5;
    if (directions.diagonal) {
        // q1/2 = 7000 7000
        outputs.leftStickX = 128 + (directions.x * 56);
        outputs.leftStickY = 128 + (directions.y * 56);
        // L, R, LS, and MS + q3/4 = 7000 6875 (For vanilla shield drop. Gives 44.5
        // degree wavedash). Also used as default q3/4 diagonal if crouch walk option select is
        // enabled.
        if (directions.y == -1 && (shield_button_pressed)) {
            outputs.leftStickX = 128 + (directions.x * 56);
            outputs.leftStickY = 128 + (directions.y * 55);
        }
    }

    // Shut off C-stick when using D-Pad layer.
    if ((inputs.lt1 && inputs.lt2) || inputs.nunchuk_c || inputs.mb2) {
        outputs.rightStickX = 128;
        outputs.rightStickY = 128;
    }

    // Nunchuk overrides left stick.
    if (inputs.nunchuk_connected) {
        outputs.leftStickX = inputs.nunchuk_x;
        outputs.leftStickY = inputs.nunchuk_y;
    }
}
