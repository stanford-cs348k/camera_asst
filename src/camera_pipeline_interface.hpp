#ifndef CAMERA_PIPELINE_INTERFACE_HPP_
#define CAMERA_PIPELINE_INTERFACE_HPP_

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
  // a picture" involves running an autofocus algorithm, followed by a general
  // processing pass (demosaic, denoise, tone map, etc.). This high-level
  // process is common for all implementations of CameraPipelineInterface;
  // however, the specific autofocus algorithm (AutoFocus()) and processing
  // algorithms (ProcessShot()) are defined by the specific implementation.
  std::unique_ptr<Image<RgbPixel>> TakePicture() {
    AutoFocus();
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

  // Implementations of AutoFocus() should select an optimal focus using data
  // obtained from the input camera sensor.
  virtual void AutoFocus() = 0;
};

#endif  // CAMERA_PIPELINE_INTERFACE_HPP_
