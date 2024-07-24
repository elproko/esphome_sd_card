#include "esp32_camera_sd_card.h"

#ifdef USE_ESP_IDF

#include "math.h"
#include "esphome/core/log.h"

#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"

int constexpr SD_OCR_SDHC_CAP = (1 << 30);  // value defined in esp-idf/components/sdmmc/include/sd_protocol_defs.h

namespace esphome {
namespace esp32_camera_sd_card {

static const char *TAG = "esp32_camera_sd_card";

void Esp32CameraSDCardComponent::setup() {
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false, .max_files = 5, .allocation_unit_size = 16 * 1024};

  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

  if (this->mode_1bit_) {
    slot_config.width = 1;
  } else {
    slot_config.width = 4;
  }

  slot_config.clk = static_cast<gpio_num_t>(Utility::get_pin_no(this->clk_pin_));
  slot_config.cmd = static_cast<gpio_num_t>(Utility::get_pin_no(this->cmd_pin_));
  slot_config.d0 = static_cast<gpio_num_t>(Utility::get_pin_no(this->data0_pin_));

  if (!this->mode_1bit_) {
    slot_config.d1 = static_cast<gpio_num_t>(Utility::get_pin_no(this->data1_pin_));
    slot_config.d2 = static_cast<gpio_num_t>(Utility::get_pin_no(this->data2_pin_));
    slot_config.d3 = static_cast<gpio_num_t>(Utility::get_pin_no(this->data3_pin_));
  }

  // Enable internal pullups on enabled pins. The internal pullups
  // are insufficient however, please make sure 10k external pullups are
  // connected on the bus. This is for debug / example purpose only.
  slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

  auto ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &this->card_);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      this->init_error_ = "Failed to mount filesystem.";
    } else {
      this->init_error_ =  "Failed to initialize the card.";
#ifdef CONFIG_EXAMPLE_DEBUG_PIN_CONNECTIONS
      check_sd_card_pins(&config, pin_count);
#endif
    }
    ESP_LOGE(TAG, this->init_error_.c_str());

    mark_failed();
    return;
  }

#ifdef USE_TEXT_SENSOR
  if (this->sd_card_type_text_sensor_ != nullptr)
    this->sd_card_type_text_sensor_->publish_state(sd_card_type());
#endif

  update_sensors();
}

void Esp32CameraSDCardComponent::write_file(const char *path, const uint8_t *buffer, size_t len) {
  ESP_LOGV(TAG, "Writing file: %s\n", path);

  this->update_sensors();
}

void Esp32CameraSDCardComponent::append_file(const char *path, const uint8_t *buffer, size_t len) {
  ESP_LOGV(TAG, "Appending to file: %s", path);

  this->update_sensors();
}

bool Esp32CameraSDCardComponent::create_directory(const char *path) {
  ESP_LOGV(TAG, "Create directory: %s", path);

  this->update_sensors();
  return true;
}

bool Esp32CameraSDCardComponent::remove_directory(const char *path) {
  ESP_LOGV(TAG, "Remove directory: %s", path);

  this->update_sensors();
  return true;
}

bool Esp32CameraSDCardComponent::delete_file(const char *path) {
  ESP_LOGV(TAG, "Delete File: %s", path);

  this->update_sensors();
  return true;
}

std::vector<std::string> Esp32CameraSDCardComponent::list_directory(const char *path, uint8_t depth) {
  std::vector<std::string> list;
  list_directory_rec(path, depth, list);
  return list;
}

std::vector<std::string> &Esp32CameraSDCardComponent::list_directory_rec(const char *path, uint8_t depth,
                                                                         std::vector<std::string> &list) {
  ESP_LOGV(TAG, "Listing directory: %s\n", path);

  return list;
}

size_t Esp32CameraSDCardComponent::file_size(const char *path) { return -1; }

std::string Esp32CameraSDCardComponent::sd_card_type() const {
  if (this->card_->is_sdio) {
    return "SDIO";
  } else if (this->card_->is_mmc) {
    return "MMC";
  } else {
    return (this->card_->ocr & SD_OCR_SDHC_CAP) ? "SDHC/SDXC" : "SDSC";
  }
  return "UNKNOWN";
}

void Esp32CameraSDCardComponent::update_sensors() {
#ifdef USE_SENSOR
  if (this->used_space_sensor_ != nullptr)
    this->used_space_sensor_->publish_state(-1);
  if (this->total_space_sensor_ != nullptr)
    this->total_space_sensor_->publish_state(-1);

  for (auto &sensor : this->file_size_sensors_) {
    if (sensor.sensor != nullptr)
      sensor.sensor->publish_state(this->file_size(sensor.path));
  }
#endif
}

}  // namespace esp32_camera_sd_card
}  // namespace esphome

#endif  // USE_ESP_IDF