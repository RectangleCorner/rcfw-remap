#ifndef _MODES_MELEEMODZ_HPP
#define _MODES_MELEEMODZ_HPP

#include "core/ControllerMode.hpp"
#include "core/state.hpp"

#include <config.pb.h>

class MeleeModZ : public ControllerMode {
  public:
    MeleeModZ();
    void SetConfig(GameModeConfig &config, const MeleeOptions options);

  protected:
    void UpdateDigitalOutputs(const InputState &inputs, OutputState &outputs);
    void UpdateAnalogOutputs(const InputState &inputs, OutputState &outputs);

  private:
    MeleeOptions _options;
    bool _horizontal_socd;

    void HandleSocd(InputState &inputs);
};

#endif
