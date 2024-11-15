#pragma once
#include "esphome/core/component.h"
#include "esphome/components/web_server_base/web_server_base.h"

namespace esphome {
namespace sd_file_server {

class SDFileServer: public AsyncWebHandler, public Component {
public:
    SDFileServer(web_server_base::WebServerBase *);
    void setup() override;
    void dump_config() override;

    void start();

    void handleRequest(AsyncWebServerRequest *req) override;

protected:
    web_server_base::WebServerBase *base_;
    bool initialized_{false};
};

} // sd_file_server namspace
} // esphome namespace