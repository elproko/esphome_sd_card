import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import web_server_base
from esphome.components.web_server_base import CONF_WEB_SERVER_BASE_ID
from esphome.const import (
    CONF_ID,
    CONF_PORT
)
from esphome.core import coroutine_with_priority, CORE

AUTO_LOAD = ["web_server_base"]
DEPENDENCIES = ["sd_mmc_card"]

sd_file_server_ns = cg.esphome_ns.namespace("sd_file_server")
SDFileServer = sd_file_server_ns.class_("SDFileServer", cg.Component)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SDFileServer),
            cv.GenerateID(CONF_WEB_SERVER_BASE_ID): cv.use_id(
                web_server_base.WebServerBase
            ),
            cv.Optional(CONF_PORT, default=80): cv.port,
        }
    ).extend(cv.COMPONENT_SCHEMA),
)

async def to_code(config):
    paren = await cg.get_variable(config[CONF_WEB_SERVER_BASE_ID])
    
    var = cg.new_Pvariable(config[CONF_ID], paren)
    await cg.register_component(var, config)
    
    cg.add_define("USE_SD_CARD_WEBSERVER")
    cg.add_define("USE_SD_CARD_WEBSERVER_PORT", config[CONF_PORT])