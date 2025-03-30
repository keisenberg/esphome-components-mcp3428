#include "mcp3428_sensor.h"

#include "esphome/core/log.h"

namespace esphome {
namespace mcp3428 {

static const char *const TAG = "mcp3428.sensor";

void MCP3428Sensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MCP3428 sensor...");
  this->unit_of_measurement_ = "";  // Set unit to "" for raw ADC values
}

void MCP3428Sensor::dump_config() {
  ESP_LOGCONFIG(TAG, "MCP3428 Sensor:");
  ESP_LOGCONFIG(TAG, "  Channel: %d", this->multiplexer_ + 1);
  ESP_LOGCONFIG(TAG, "  Gain: %dx", 1 << this->gain_);
  ESP_LOGCONFIG(TAG, "  Resolution: %d bits", 12 + (this->resolution_ * 2));
  LOG_SENSOR("  ", "Raw ADC", this);
}

float MCP3428Sensor::sample() {
  uint32_t timeout_wait;
  if (!this->parent_->request_measurement(this->multiplexer_, this->gain_, this->resolution_, timeout_wait)) {
    this->status_set_warning();
    return NAN;
  }

  delay(timeout_wait);

  int32_t raw_value;
  if (!this->parent_->poll_result(raw_value)) {
    this->status_set_warning();
    return NAN;
  }

  this->status_clear_warning();
  return raw_value;
}

void MCP3428Sensor::update() {
  float raw_value = this->sample();
  if (!std::isnan(raw_value)) {
    this->publish_state(raw_value);
  }
}

}  // namespace mcp3428
}  // namespace esphome
