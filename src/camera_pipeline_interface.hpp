#pragma once

#include <memory>
#include "camera_sensor.hpp"
#include "image.hpp"
#include "pixel.hpp"

// This is the interface for the virtual camera pipeline. The pipeline takes in
// a sensor, and provides the capability of "taking a picture" using that
// sensor, through the function TakePicture().
class CameraPipelineInterface {
 public:
  explicit CameraPipelineInterface(CameraSensor* sensor) : sensor_(sensor) {}

  // Takes a picture using the input sensor. The high-level process for "taking
  // a picture" involves a general processing pass (demosaic, denoise, tone map,
  // color correction, etc.). This high-level process is common for all
  // implementations of CameraPipelineInterface. however, the exact processing
  // algorithms (ProcessShot()) are defined by the specific implementation.
  std::unique_ptr<Image<RgbPixel>> TakePicture() {
    return ProcessShot();
  }

 protected:
  CameraSensor* const sensor_;

 private:
  using T = typename CameraSensor::T;

  // Implementations need to implement these two functions:
  // Implementations of ProcessShot() should take the raw sensor data in
  // @raw_data and output a clean final image.
  virtual std::unique_ptr<Image<RgbPixel>> ProcessShot() const = 0;
};
