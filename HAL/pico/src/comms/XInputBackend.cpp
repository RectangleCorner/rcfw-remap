#include "comms/XInputBackend.hpp"

#include "core/CommunicationBackend.hpp"
#include "core/state.hpp"

#include <Adafruit_USBD_XInput.hpp>

XInputBackend::XInputBackend(
    InputState &inputs,
    InputSource **input_sources,
    size_t input_source_count
)
    : CommunicationBackend(inputs, input_sources, input_source_count),
      _xinput() {
    Serial.end();
    _xinput.begin();
    Serial.begin(115200);

    TinyUSBDevice.setID(0x0738, 0x4726);
}

CommunicationBackendId XInputBackend::BackendId() {
    return COMMS_BACKEND_XINPUT;
}

void XInputBackend::SendReport(bool isMelee) {
    if (!isMelee) {
        ScanInputs(InputScanSpeed::SLOW);
        ScanInputs(InputScanSpeed::MEDIUM);
    }

    while (!_xinput.ready()) {
        tight_loop_contents();
    }

    if (!isMelee) {
        ScanInputs(InputScanSpeed::FAST);

        UpdateOutputs();
    }

    if (!isMelee) {
        // Digital outputs
        _report.a = _outputs.a;
        _report.b = _outputs.b;
        _report.x = _outputs.x;
        _report.y = _outputs.y;
        _report.lb = _outputs.buttonL;
        _report.rb = _outputs.buttonR;
        _report.lt = _outputs.triggerLDigital ? 255 : _outputs.triggerLAnalog;
        _report.rt = _outputs.triggerRDigital ? 255 : _outputs.triggerRAnalog;
        _report.start = _outputs.start;
        _report.back = _outputs.select;
        _report.home = _outputs.home;
        _report.dpad_up = _outputs.dpadUp;
        _report.dpad_down = _outputs.dpadDown;
        _report.dpad_left = _outputs.dpadLeft;
        _report.dpad_right = _outputs.dpadRight;
        _report.ls = _outputs.leftStickClick;
        _report.rs = _outputs.rightStickClick;

        _report.lx = (_outputs.leftStickX - 128) * 65535 / 255 + 128;
        _report.ly = (_outputs.leftStickY - 128) * 65535 / 255 + 128;
        _report.rx = (_outputs.rightStickX - 128) * 65535 / 255 + 128;
        _report.ry = (_outputs.rightStickY - 128) * 65535 / 255 + 128;
    } else {
        // Digital outputs
        _report.a = _finalOutputs.a;
        _report.b = _finalOutputs.b;
        _report.x = _finalOutputs.x;
        _report.y = _finalOutputs.y;
        _report.lb = _finalOutputs.buttonL;
        _report.rb = _finalOutputs.buttonR;
        _report.lt = _finalOutputs.triggerLDigital ? 255 : _finalOutputs.triggerLAnalog;
        _report.rt = _finalOutputs.triggerRDigital ? 255 : _finalOutputs.triggerRAnalog;
        _report.start = _finalOutputs.start;
        _report.back = _finalOutputs.select;
        _report.home = _finalOutputs.home;
        _report.dpad_up = _finalOutputs.dpadUp;
        _report.dpad_down = _finalOutputs.dpadDown;
        _report.dpad_left = _finalOutputs.dpadLeft;
        _report.dpad_right = _finalOutputs.dpadRight;
        _report.ls = _finalOutputs.leftStickClick;
        _report.rs = _finalOutputs.rightStickClick;

        _report.lx = (_finalOutputs.leftStickX - 128) * 65535 / 255 + 128;
        _report.ly = (_finalOutputs.leftStickY - 128) * 65535 / 255 + 128;
        _report.rx = (_finalOutputs.rightStickX - 128) * 65535 / 255 + 128;
        _report.ry = (_finalOutputs.rightStickY - 128) * 65535 / 255 + 128;
    }

    _xinput.sendReport(&_report);
}