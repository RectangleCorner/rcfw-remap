#ifndef _MODES_PROJECTM_HPP
#define _MODES_PROJECTM_HPP

#include "core/ControllerMode.hpp"
#include "core/state.hpp"

#include <config.pb.h>

// typedef struct {
//     bool true_z_press = false;
//     bool ledgedash_max_jump_traj = true;
// } ProjectMOptions;

class ProjectM : public ControllerMode {
  public:
    ProjectM();
    void SetConfig(GameModeConfig &config, const ProjectMOptions options, const bool true_z);

  private:
    ProjectMOptions _options;
    bool _true_z;
    bool _horizontal_socd;

    void HandleSocd(InputState &inputs);
    void UpdateDigitalOutputs(const InputState &inputs, OutputState &outputs);
    void UpdateAnalogOutputs(const InputState &inputs, OutputState &outputs);
};

#endif
