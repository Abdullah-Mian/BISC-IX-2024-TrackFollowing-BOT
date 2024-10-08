#ifndef CAMERA_INDEX_H
#define CAMERA_INDEX_H
// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
#define LED_GPIO_NUM 4


#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "img_converters.h"
#include "Arduino.h"

#define PART_BOUNDARY "123456789000000000000987654321"
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

typedef struct {
  httpd_req_t *req;
  size_t len;
} jpg_chunking_t;

static size_t jpg_encode_stream(void *arg, size_t index, const void *data, size_t len) {
  jpg_chunking_t *j = (jpg_chunking_t *)arg;
  if (!index) {
    j->len = 0;
  }
  if (httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK) {
    return 0;
  }
  j->len += len;
  return len;
}

void detectColor(camera_fb_t *fb) {
  if (!fb) return;

  digitalWrite(LED_GPIO_NUM, LOW);  // Turn off LED for color detection

  uint8_t *buf = fb->buf;
  size_t len = fb->len;

  // Convert JPEG to RGB
  uint8_t *rgb_buffer = (uint8_t *)malloc(fb->width * fb->height * 3);
  if (!rgb_buffer) {
    Serial.println("Failed to allocate memory for RGB buffer");
    return;
  }

  bool converted = fmt2rgb888(buf, len, PIXFORMAT_JPEG, rgb_buffer);

  if (!converted) {
    Serial.println("Failed to convert image to RGB");
    free(rgb_buffer);
    return;
  }

  long red_sum = 0, green_sum = 0, blue_sum = 0;
  for (int i = 0; i < fb->width * fb->height * 3; i += 3) {
    red_sum += rgb_buffer[i];
    green_sum += rgb_buffer[i + 1];
    blue_sum += rgb_buffer[i + 2];
  }

  int total_pixels = fb->width * fb->height;
  int red_avg = red_sum / total_pixels;
  int green_avg = green_sum / total_pixels;
  int blue_avg = blue_sum / total_pixels;

  Serial.printf("Color range detected - R: %d, G: %d, B: %d\n", red_avg, green_avg, blue_avg);

  if (red_avg > green_avg && red_avg > blue_avg) {
    Serial.println("Dominant color: Red");
  } else if (green_avg > red_avg && green_avg > blue_avg) {
    Serial.println("Dominant color: Green");
  } else if (blue_avg > red_avg && blue_avg > green_avg) {
    Serial.println("Dominant color: Blue");
  } else {
    Serial.println("No dominant color detected");
  }

  free(rgb_buffer);
}

static esp_err_t capture_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;

  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  size_t fb_len = 0;
  if (fb->format == PIXFORMAT_JPEG) {
    fb_len = fb->len;
    res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
  } else {
    jpg_chunking_t jchunk = { req, 0 };
    res = frame2jpg_cb(fb, 80, jpg_encode_stream, &jchunk) ? ESP_OK : ESP_FAIL;
    httpd_resp_send_chunk(req, NULL, 0);
    fb_len = jchunk.len;
  }
  esp_camera_fb_return(fb);
  return res;
}
static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t *_jpg_buf = NULL;
  char *part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if (res != ESP_OK) {
    return res;
  }

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  while (true) {
    digitalWrite(LED_GPIO_NUM, HIGH);  // Turn on LED for capture
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      delay(100);
      detectColor(fb);  // Perform color detection for each frame

      if (fb->format != PIXFORMAT_JPEG) {
        bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
        esp_camera_fb_return(fb);
        fb = NULL;
        if (!jpeg_converted) {
          Serial.println("JPEG compression failed");
          res = ESP_FAIL;
        }
      } else {
        _jpg_buf_len = fb->len;
        _jpg_buf = fb->buf;
      }
    }

    if (res == ESP_OK) {
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if (fb) {
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if (_jpg_buf) {
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    if (res != ESP_OK) {
      break;
    }

    // Add a small delay to slow down the stream and allow time for color detection output
    delay(200);
  }

  return res;
}

void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;

  httpd_uri_t index_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = stream_handler,
    .user_ctx = NULL
  };

  httpd_uri_t capture_uri = {
    .uri = "/capture",
    .method = HTTP_GET,
    .handler = capture_handler,
    .user_ctx = NULL
  };

  Serial.printf("Starting web server on port: '%d'\n", config.server_port);
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &capture_uri);
  }

  config.server_port += 1;
  config.ctrl_port += 1;
  Serial.printf("Starting stream server on port: '%d'\n", config.server_port);
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &index_uri);
  }
}

#endif  // CAMERA_INDEX_H