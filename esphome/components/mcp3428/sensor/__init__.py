import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ACCURACY_DECIMALS,
    CONF_DEVICE_CLASS,
    CONF_FORCE_UPDATE,
    CONF_ID,
    CONF_NAME,
    CONF_UPDATE_INTERVAL,
)

from .. import mcp3428_ns, MCP3428Component, MCP3428Multiplexer, MCP3428Gain, MCP3428Resolution

MCP3428Sensor = mcp3428_ns.class_(
    "MCP3428Sensor", sensor.Sensor, cg.PollingComponent
)

CONF_MCP3428_ID = "mcp3428_id"
CONF_MULTIPLEXER = "multiplexer"
CONF_GAIN = "gain"
CONF_RESOLUTION = "resolution"

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        MCP3428Sensor,
        accuracy_decimals=0,
        device_class=None,
        state_class=None,
    )
    .extend(
        {
            cv.GenerateID(CONF_MCP3428_ID): cv.use_id(MCP3428Component),
            cv.Required(CONF_MULTIPLEXER): cv.enum(MCP3428Multiplexer, upper=True),
            cv.Required(CONF_GAIN): cv.enum(MCP3428Gain, upper=True),
            cv.Required(CONF_RESOLUTION): cv.enum(MCP3428Resolution, upper=True),
        }
    )
    .extend(cv.polling_component_schema("1s"))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)

    cg.add(var.set_multiplexer(config[CONF_MULTIPLEXER]))
    cg.add(var.set_gain(config[CONF_GAIN]))
    cg.add(var.set_resolution(config[CONF_RESOLUTION]))

    cg.add(var.set_parent(cg.new_Pvariable(config[CONF_MCP3428_ID])))
