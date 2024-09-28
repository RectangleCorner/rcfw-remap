#ifndef _MODES_MELEENERFS_HPP
#define _MODES_MELEENERFS_HPP

#include "core/ControllerMode.hpp"
#include "core/state.hpp"

#include <config.pb.h>

class MeleeNerfs : public ControllerMode {
  public:
    MeleeNerfs();
    void SetConfig(GameModeConfig &config, const MeleeOptions options, const bool cwos);

  protected:
    void UpdateDigitalOutputs(const InputState &inputs, OutputState &outputs);
    void UpdateAnalogOutputs(const InputState &inputs, OutputState &outputs);

  private:
    MeleeOptions _options;
    bool _cwos;
    bool _horizontal_socd;

    void HandleSocd(InputState &inputs);
};

#endif
