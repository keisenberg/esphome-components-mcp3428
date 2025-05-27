#include "mcp3428_sensor.h"

#include "esphome/core/log.h"

namespace esphome {
namespace mcp3428 {

static const char *const TAG = "mcp3426/7/8.sensor";
static const uint8_t MEASUREMENT_INITIATE_MAX_TRIES = 10;

int32_t MCP3428Sensor::get_raw_reading() {
  uint32_t wait = 0;
  int32_t raw_value = 0;
  // initiate Measurement
  if (!this->parent_->request_measurement(this->multiplexer_, this->gain_, this->resolution_, wait)) {
    return raw_value;  // if sensor is busy there is no easy way the situation can be resolved in a synchronous manner
  }
  delay(wait);  // certainly not ideal but necessary when the result needs to be returned now

  bool success = this->parent_->poll_raw_result(raw_value);
  if (!success) {
    this->parent_->abandon_current_measurement();
  } else {
    this->status_clear_warning();
  }
  return raw_value;
}

void MCP3428Sensor::update() {
  this->set_retry(MEASUREMENT_TIME_16BIT_MS, MEASUREMENT_INITIATE_MAX_TRIES,
                  [this](const uint8_t remaining_initiate_attempts) {
                    uint32_t wait;
                    if (this->parent_->request_measurement(this->multiplexer_, this->gain_, this->resolution_, wait)) {
                      // measurement started, set timeout for retrieving value
                      this->set_timeout(wait, [this]() {
                        int32_t raw_value = 0;
                        bool success = this->parent_->poll_raw_result(raw_value);
                        if (success) {
                          ESP_LOGD(TAG, "'%s': Got Raw ADC Value=%d", this->get_name().c_str(), raw_value);
                          this->publish_state(raw_value);
                          this->status_clear_warning();
                        } else {
                          this->status_set_warning("No valid measurement returned");
                          this->parent_->abandon_current_measurement();
                        }
                      });
                      return RetryResult::DONE;
                    }
                    if (remaining_initiate_attempts == 0) {
                      this->status_set_warning("Could not initiate Measurement");
                    }
                    return RetryResult::RETRY;
                  });
}

void MCP3428Sensor::dump_config() {
  LOG_SENSOR("  ", "MCP3426/7/8 Sensor", this);
  ESP_LOGCONFIG(TAG, "    Multiplexer: Channel %u", this->multiplexer_ + 1);
  ESP_LOGCONFIG(TAG, "    Gain: %u", 0b0001 << this->gain_);
  ESP_LOGCONFIG(TAG, "    Resolution: %u", 12 + 2 * this->resolution_);
}

}  // namespace mcp3428
}  // namespace esphome
