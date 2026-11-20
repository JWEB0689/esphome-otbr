#include "thread_credential_sharing.h"
#include "esphome/components/openthread/openthread.h"
#include "esphome/components/wifi/wifi_component.h"
#include "esphome/core/application.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include <openthread/border_agent_ephemeral_key.h>
#include <openthread/thread.h>
#include "esp_openthread_meshcop_mdns.h"

namespace esphome::thread_credential_sharing {
static const char *const TAG = "thread_sharing";
static constexpr uint32_t WINDOW_MS = 600000;

void ThreadCredentialSharing::setup() {
  code_->publish_state("");
  state_->publish_state("Closed");
  remaining_->publish_state(0);
}
void ThreadCredentialSharing::clear_code_() {
  active_ = false;
  code_->publish_state("");
  remaining_->publish_state(0);
}
void ThreadCredentialSharing::start() {
  stop_pending_ = false;
  if (!wifi::global_wifi_component->is_connected()) {
    state_->publish_state("Wi-Fi disconnected");
    return;
  }
  auto lock = openthread::InstanceLock::try_acquire(10);
  if (!lock) { state_->publish_state("Thread not ready"); return; }
  auto *instance = lock.get_instance();
  if (otThreadGetDeviceRole(instance) < OT_DEVICE_ROLE_CHILD) {
    state_->publish_state("Thread not attached");
    return;
  }
  if (!discovery_) {
    if (esp_openthread_publish_meshcop_mdns(App.get_name().c_str()) != ESP_OK) {
      state_->publish_state("Discovery setup failed");
      return;
    }
    discovery_ = true;
  }
  otBorderAgentEphemeralKeyStop(instance);
  clear_code_();
  otBorderAgentEphemeralKeySetEnabled(instance, true);
  otBorderAgentEphemeralKeyTap tap{};
  otError error = otBorderAgentEphemeralKeyGenerateTap(&tap);
  if (error == OT_ERROR_NONE)
    error = otBorderAgentEphemeralKeyStart(instance, tap.mTap, WINDOW_MS, 0);
  if (error != OT_ERROR_NONE) {
    state_->publish_state("Start failed");
    ESP_LOGE(TAG, "Start failed: %s", otThreadErrorToString(error));
    return;
  }
  started_ = millis();
  active_ = true;
  code_->publish_state(tap.mTap);
  for (auto &c : tap.mTap) c = 0;
  state_->publish_state("Sharing");
  remaining_->publish_state(WINDOW_MS / 1000);
  ESP_LOGI(TAG, "Credential sharing opened for 10 minutes");
}
void ThreadCredentialSharing::stop() {
  stop_pending_ = true;
  auto lock = openthread::InstanceLock::try_acquire(10);
  if (!lock) { state_->publish_state("Stop pending"); return; }
  otBorderAgentEphemeralKeyStop(lock.get_instance());
  stop_pending_ = false;
  clear_code_();
  state_->publish_state("Closed");
}
void ThreadCredentialSharing::update() {
  if (stop_pending_) { stop(); return; }
  if (!active_) return;
  if (!wifi::global_wifi_component->is_connected() || millis() - started_ >= WINDOW_MS) {
    stop();
    return;
  }
  auto lock = openthread::InstanceLock::try_acquire(0);
  if (!lock) return;
  auto state = otBorderAgentEphemeralKeyGetState(lock.get_instance());
  if (state == OT_BORDER_AGENT_STATE_STOPPED || state == OT_BORDER_AGENT_STATE_DISABLED) {
    clear_code_();
    state_->publish_state("Closed");
    return;
  }
  state_->publish_state(otBorderAgentEphemeralKeyStateToString(state));
  remaining_->publish_state((WINDOW_MS - (millis() - started_)) / 1000);
}
void ThreadCredentialSharing::on_shutdown() { stop(); }
}
