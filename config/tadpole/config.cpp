#include "comms/backend_init.hpp"
#include "config_defaults.hpp"
#include "core/CommunicationBackend.hpp"
#include "core/KeyboardMode.hpp"
#include "core/Persistence.hpp"
#include "core/mode_selection.hpp"
#include "core/pinout.hpp"
#include "core/state.hpp"
#include "input/DebouncedGpioButtonInput.hpp"
#include "input/NunchukInput.hpp"
#include "reboot.hpp"
#include "stdlib.hpp"

#include <config.pb.h>
#include <cstring> // display
#include <string> //display

Config config = default_config;
std::string dispCommBackend = "BACKEND";
std::string dispMode = "";
bool isMelee = false;
GpioButtonMapping button_mappings[] = {
    { BTN_LF1, 2  },
    { BTN_LF2, 3  },
    { BTN_LF3, 4  },
    { BTN_LF4, 5  },

    { BTN_LT1, 6  },
    { BTN_LT2, 7  },

    { BTN_MB1, 1  },
    { BTN_RT1, 14 },
    { BTN_RT2, 15 },
    { BTN_RT3, 13 },
    { BTN_RT4, 12 },
    { BTN_RT5, 16 },

    { BTN_RF1, 26 },
    { BTN_RF2, 21 },
    { BTN_RF3, 19 },
    { BTN_RF4, 17 },

    { BTN_RF5, 27 },
    { BTN_RF6, 22 },
    { BTN_RF7, 20 },
    { BTN_RF8, 18 },
};
const size_t button_count = sizeof(button_mappings) / sizeof(GpioButtonMapping);

DebouncedGpioButtonInput<button_count> gpio_input(button_mappings);

const Pinout pinout = {
    .joybus_data = 28,
    .mux = -1,
    .nunchuk_detect = -1,
    .nunchuk_sda = -1,
    .nunchuk_scl = -1,
};

CommunicationBackend **backends = nullptr;
size_t backend_count;
KeyboardMode *current_kb_mode = nullptr;

void setup() {
    static InputState inputs;

    // Create GPIO input source and use it to read button states for checking button holds.
    gpio_input.UpdateInputs(inputs);

    // Check bootsel button hold as early as possible for safety.
    if (inputs.rt2 && inputs.rt3 && inputs.rt4 && inputs.rt5) {
        reboot_bootloader();
    }

    // Turn on LED to indicate firmware booted.
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    // gpio_put(PICO_DEFAULT_LED_PIN, 1);

    // Attempt to load config, or write default config to flash if failed to load config.
    if (!persistence.LoadConfig(config)) {
        persistence.SaveConfig(config);
    }
    // Create array of input sources to be used.
    static InputSource *input_sources[] = {};
    size_t input_source_count = sizeof(input_sources) / sizeof(InputSource *);

    backend_count =
        initialize_backends(backends, inputs, input_sources, input_source_count, config, pinout);

    setup_mode_activation_bindings(config.game_mode_configs, config.game_mode_configs_count);
}

void loop() {
    select_mode(backends, backend_count, config);

    for (size_t i = 0; i < backend_count; i++) {
        backends[i]->SendReport(isMelee);
    }

    if (current_kb_mode != nullptr) {
        current_kb_mode->SendReport(backends[0]->GetInputs());
    }
}

/* Button inputs are read from the second core */

void setup1() {
    while (!backend_count || backends == nullptr) {
        delay(1);
    }

    // These have to be initialized after backends.
    CommunicationBackendId primary_backend_id = backends[0]->BackendId();
    switch (primary_backend_id) {
        case COMMS_BACKEND_DINPUT:
            dispCommBackend = "DINPUT";
            break;
        case COMMS_BACKEND_NINTENDO_SWITCH:
            dispCommBackend = "SWITCH";
            break;
        case COMMS_BACKEND_XINPUT:
            dispCommBackend = "XINPUT";
            break;
        case COMMS_BACKEND_GAMECUBE:
            dispCommBackend = "GCN";
            break;
        case COMMS_BACKEND_N64:
            dispCommBackend = "N64";
            break;
        case COMMS_BACKEND_UNSPECIFIED: // Fall back to configurator if invalid
                                        // backend selected.
        case COMMS_BACKEND_CONFIGURATOR:
        default:
            dispCommBackend = "CONFIG";
            dispMode = "MODE";
    }
}

void loop1() {
    if (backends != nullptr) {
        gpio_input.UpdateInputs(backends[0]->GetInputs());
        if (dispCommBackend != "CONFIG") {
            dispMode = backends[0]->CurrentGameMode()->GetConfig()->name;
        }
    }
    if (dispMode == "MELEE") {
        isMelee = true;
        // get exactly 2 khz input scanning
        const uint32_t interval = 500; // microseconds
        const uint32_t quarterInterval = interval / 4; // unit of 4 microseconds
        const uint32_t beforeMicros = micros();
        uint32_t afterMicros = beforeMicros;
        while ((afterMicros - beforeMicros) < interval) {
            afterMicros = micros();
        }

        gpio_input.UpdateInputs(backends[0]->GetInputs());
        for (size_t i = 0; i < backend_count; i++) {
            backends[i]->ScanInputs();
            backends[i]->UpdateOutputs();
            backends[i]->LimitOutputs(quarterInterval, config.travelTime);
        }
    } else {
        isMelee = false;
    }
}
