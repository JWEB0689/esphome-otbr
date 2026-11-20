import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor, sensor
from esphome.components.esp32 import add_idf_sdkconfig_option
from esphome.const import CONF_ID, ENTITY_CATEGORY_DIAGNOSTIC

DEPENDENCIES = ["openthread", "wifi", "api", "esp32"]
AUTO_LOAD = ["text_sensor", "sensor"]
ns = cg.esphome_ns.namespace("thread_credential_sharing")
ThreadCredentialSharing = ns.class_("ThreadCredentialSharing", cg.PollingComponent)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ThreadCredentialSharing),
    cv.Required("pairing_code"): text_sensor.text_sensor_schema(entity_category=ENTITY_CATEGORY_DIAGNOSTIC),
    cv.Required("state"): text_sensor.text_sensor_schema(entity_category=ENTITY_CATEGORY_DIAGNOSTIC),
    cv.Required("remaining"): sensor.sensor_schema(unit_of_measurement="s", accuracy_decimals=0, entity_category=ENTITY_CATEGORY_DIAGNOSTIC),
}).extend(cv.polling_component_schema("1s"))

async def to_code(config):
    add_idf_sdkconfig_option("CONFIG_OPENTHREAD_BORDER_AGENT_ENABLE", True)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    for key in ("pairing_code", "state"):
        sens = await text_sensor.new_text_sensor(config[key])
        cg.add(getattr(var, "set_" + key)(sens))
    sens = await sensor.new_sensor(config["remaining"])
    cg.add(var.set_remaining(sens))
