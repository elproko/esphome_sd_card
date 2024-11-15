#include "sd_file_server.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sd_file_server {

static const char *TAG = "sd_file_server";

SDFileServer::SDFileServer(web_server_base::WebServerBase * base) : base_(base) {}

void SDFileServer::setup() {}

void SDFileServer::dump_config() {
    ESP_LOGCONFIG(TAG, "SD File Server:");
}

void SDFileServer::start() {
    this->base_->init();
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