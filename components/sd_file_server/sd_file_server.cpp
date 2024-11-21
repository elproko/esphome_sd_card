#include "sd_file_server.h"
#include "esphome/core/log.h"
#include "esphome/components/network/util.h"

namespace esphome {
namespace sd_file_server {

static const char *TAG = "sd_file_server";

SDFileServer::SDFileServer(web_server_base::WebServerBase * base) : base_(base) {}

void SDFileServer::setup() {
  this->base_->init();
}

void SDFileServer::dump_config() {
    ESP_LOGCONFIG(TAG, "SD File Server:");
    ESP_LOGCONFIG(TAG, "  Address: %s:%u", network::get_use_address().c_str(), this->base_->get_port());
}

void SDFileServer::start() {
    this->base_->init();
    this->base_->add_handler(this);
    this->initialized_ = true;
}

void SDFileServer::handleRequest(AsyncWebServerRequest *req) {
  if (req->url() == "/") {
    auto *response = req->beginResponse(200, "text/plain", "Hello World!");
    response->addHeader("Content-Encoding", "gzip");
    req->send(response);
    return;
  }
}

} // sd_file_server namspace
} // esphome namespace