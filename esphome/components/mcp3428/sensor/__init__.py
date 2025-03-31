import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_GAIN,
    CONF_MULTIPLEXER,
    CONF_RESOLUTION,
    STATE_CLASS_MEASUREMENT,
)
from .. import mcp3428_ns, MCP3428Component, CONF_MCP3428_ID

CODEOWNERS = ["@mdop"]
DEPENDENCIES = ["mcp3428"]

MCP3428Multiplexer = mcp3428_ns.enum("MCP3428Multiplexer")
MUX = {
    1: MCP3428Multiplexer.MCP3428_MULTIPLEXER_CHANNEL_1,
    2: MCP3428Multiplexer.MCP3428_MULTIPLEXER_CHANNEL_2,
    3: MCP3428Multiplexer.MCP3428_MULTIPLEXER_CHANNEL_3,
    4: MCP3428Multiplexer.MCP3428_MULTIPLEXER_CHANNEL_4,
}

MCP3428Gain = mcp3428_ns.enum("MCP3428Gain")
GAIN = {
    1: MCP3428Gain.MCP3428_GAIN_1,
    2: MCP3428Gain.MCP3428_GAIN_2,
    4: MCP3428Gain.MCP3428_GAIN_4,
    8: MCP3428Gain.MCP3428_GAIN_8,
}

MCP3428Resolution = mcp3428_ns.enum("MCP3428Resolution")
RESOLUTION = {
    12: MCP3428Resolution.MCP3428_12_BITS,
    14: MCP3428Resolution.MCP3428_14_BITS,
    16: MCP3428Resolution.MCP3428_16_BITS,
}

MCP3428Sensor = mcp3428_ns.class_(
    "MCP3428Sensor", sensor.Sensor, cg.PollingComponent
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(MCP3428Sensor),
        cv.Required(CONF_MCP3428_ID): cv.use_id(MCP3428Component),
        cv.Required(CONF_MULTIPLEXER): cv.enum(MUX, int=True),
        cv.Optional(CONF_GAIN, default=1): cv.enum(GAIN, int=True),
        cv.Optional(CONF_RESOLUTION, default=16): cv.enum(RESOLUTION, int=True),
        cv.Optional("name"): cv.string,
        cv.Optional("unit_of_measurement", default="count"): cv.string,
        cv.Optional("accuracy_decimals", default=0): cv.int_,
        cv.Optional("state_class", default=STATE_CLASS_MEASUREMENT): cv.string,
        cv.Optional("update_interval", default="60s"): cv.update_interval,
        cv.Optional("filters"): cv.ensure_list(cv.returning_lambda),
    }
).extend(cv.polling_component_schema("60s"))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await cg.register_parented(var, config[CONF_MCP3428_ID])

    cg.add(var.set_multiplexer(config[CONF_MULTIPLEXER]))
    cg.add(var.set_gain(config[CONF_GAIN]))
    cg.add(var.set_resolution(config[CONF_RESOLUTION]))
