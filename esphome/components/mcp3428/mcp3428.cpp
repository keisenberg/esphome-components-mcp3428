#include "mcp3428.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mcp3428 {

static const char *const TAG = "mcp3428";

void MCP3428Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MCP3428...");
  if (!this->write_bytes(0, nullptr, 0)) {
    this->mark_failed();
    return;
  }
}

void MCP3428Component::dump_config() {
  ESP_LOGCONFIG(TAG, "MCP3428:");
  LOG_I2C_DEVICE(this);
}

bool MCP3428Component::request_measurement(MCP3428Multiplexer multiplexer, MCP3428Gain gain,
                                        MCP3428Resolution resolution, uint32_t &timeout_wait) {
  uint8_t config = 0b10000000;  // RDY bit
  config |= (multiplexer & 0b11) << 5;
  config |= (gain & 0b11) << 1;
  config |= (resolution & 0b11) << 2;

  if (!this->write_bytes(0, &config, 1)) {
    return false;
  }

  this->prev_config_ = config;

  // Calculate timeout based on resolution (reduced times for faster operation)
  switch (resolution) {
    case MCP3428Resolution::MCP3428_12_BITS:
      timeout_wait = 2;  // 2ms for 12-bit
      break;
    case MCP3428Resolution::MCP3428_14_BITS:
      timeout_wait = 8;  // 8ms for 14-bit
      break;
    case MCP3428Resolution::MCP3428_16_BITS:
      timeout_wait = 30;  // 30ms for 16-bit
      break;
    default:
      timeout_wait = 30;  // Default to 16-bit timing
      break;
  }

  return true;
}

bool MCP3428Component::poll_result(int32_t &raw_value) {
  uint8_t data[3];
  if (!this->read_bytes(0, data, 3)) {
    return false;
  }

  // Check if conversion is ready
  if (data[2] & 0x80) {
    return false;
  }

  // Combine bytes based on resolution
  uint8_t resolution = (this->prev_config_ >> 2) & 0b00000011;
  switch (resolution) {
    case MCP3428Resolution::MCP3428_12_BITS:
      raw_value = ((int16_t)data[0] << 8) | data[1];
      raw_value = (raw_value << 4) >> 4;  // Sign extend 12-bit value
      break;
    case MCP3428Resolution::MCP3428_14_BITS:
      raw_value = ((int16_t)data[0] << 8) | data[1];
      raw_value = (raw_value << 2) >> 2;  // Sign extend 14-bit value
      break;
    case MCP3428Resolution::MCP3428_16_BITS:
      raw_value = ((int16_t)data[0] << 8) | data[1];
      break;
    default:
      return false;
  }

  return true;
}

}  // namespace mcp3428
}  // namespace esphome
