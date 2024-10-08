#ifndef _CORE_COMMUNICATIONBACKEND_HPP
#define _CORE_COMMUNICATIONBACKEND_HPP

#include "core/ControllerMode.hpp"
#include "core/InputSource.hpp"
#include "state.hpp"

class CommunicationBackend {
  public:
    CommunicationBackend(
        InputState &inputs,
        InputSource **input_sources,
        size_t input_source_count
    );
    virtual ~CommunicationBackend() {};

    InputState &GetInputs();
    OutputState &GetOutputs();
    void ScanInputs();
    void ScanInputs(InputScanSpeed input_source_filter);

    virtual void UpdateOutputs();
    virtual CommunicationBackendId BackendId();
    virtual void SetGameMode(InputMode *gamemode);
    virtual InputMode *CurrentGameMode();
    void LimitOutputs(const uint32_t sampleSpacing, const bool travelTime);

    virtual void SendReport(bool isMelee) = 0;

  protected:
    InputState &_inputs;
    InputSource **_input_sources;
    size_t _input_source_count;
    OutputState _finalOutputs;
    OutputState _outputs;
    InputMode *_gamemode = nullptr;

  private:
    void ResetOutputs();
};

#endif
