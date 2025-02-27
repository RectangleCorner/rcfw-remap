#include "core/CommunicationBackend.hpp"

#include "core/ControllerMode.hpp"
#include "core/InputSource.hpp"
#include "core/state.hpp"
#include "modes/HaxLimits.hpp"
#include "modes/MeleeLimits.hpp"

#include <config.pb.h>

CommunicationBackend::CommunicationBackend(
    InputState &inputs,
    InputSource **input_sources,
    size_t input_source_count
)
    : _inputs(inputs) {
    _gamemode = nullptr;
    _input_sources = input_sources;
    _input_source_count = input_source_count;
}

InputState &CommunicationBackend::GetInputs() {
    return _inputs;
}

OutputState &CommunicationBackend::GetOutputs() {
    return _outputs;
}

void CommunicationBackend::ScanInputs() {
    for (size_t i = 0; i < _input_source_count; i++) {
        _input_sources[i]->UpdateInputs(_inputs);
    }
}

void CommunicationBackend::ScanInputs(InputScanSpeed input_source_filter) {
    for (size_t i = 0; i < _input_source_count; i++) {
        InputSource *input_source = _input_sources[i];
        if (input_source->ScanSpeed() == input_source_filter) {
            input_source->UpdateInputs(_inputs);
        }
    }
}

void CommunicationBackend::ResetOutputs() {
    _outputs = OutputState();
}

void CommunicationBackend::UpdateOutputs() {
    ResetOutputs();
    if (_gamemode != nullptr) {
        _gamemode->UpdateOutputs(_inputs, _outputs);
    }
}

CommunicationBackendId CommunicationBackend::BackendId() {
    return COMMS_BACKEND_UNSPECIFIED;
}

void CommunicationBackend::SetGameMode(InputMode *gamemode) {
    _gamemode = gamemode;
}

InputMode *CommunicationBackend::CurrentGameMode() {
    return _gamemode;
}

void CommunicationBackend::LimitOutputs(const uint32_t sampleSpacing, const bool travelTime) {
    if (travelTime) {
        limitOutputs(sampleSpacing, AB_A /*doesn't matter*/, _inputs, _outputs, _finalOutputs);
    } else {
        limitOutputsHax(sampleSpacing, AB_A /*doesn't matter*/, _inputs, _outputs, _finalOutputs);
    }
}
