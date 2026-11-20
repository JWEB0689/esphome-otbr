#pragma once
#include "esphome/core/component.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome::thread_credential_sharing {
class ThreadCredentialSharing : public PollingComponent {
 public:
  void setup() override;
  void update() override;
  void on_shutdown() override;
  void start();
  void stop();
  void set_pairing_code(text_sensor::TextSensor *value) { code_ = value; }
  void set_state(text_sensor::TextSensor *value) { state_ = value; }
  void set_remaining(sensor::Sensor *value) { remaining_ = value; }
 protected:
  void clear_code_();
  text_sensor::TextSensor *code_{nullptr};
  text_sensor::TextSensor *state_{nullptr};
  sensor::Sensor *remaining_{nullptr};
  uint32_t started_{0};
  bool active_{false};
  bool discovery_{false};
};
}
