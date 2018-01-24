#ifndef CAMERA_PIPELINE_HPP_
#define CAMERA_PIPELINE_HPP_

#include <algorithm>
#include <limits>
#include <memory>
#include "camera_pipeline_interface.hpp"
#include "image.hpp"
#include "pixel.hpp"

class CameraPipeline : public CameraPipelineInterface {
 public:
    
  explicit CameraPipeline(CameraSensor* sensor)
    : CameraPipelineInterface(sensor) {}
    
 private:
  using T = typename CameraSensor::T;
  using CameraPipelineInterface::sensor_;

  std::unique_ptr<Image<RgbPixel>> ProcessShot() const override;
  void AutoFocus() override;

  // BEGIN: CS348V STUDENTS MODIFY THIS CODE
  //
  // You can add any necessary private member variables or functions.
  //
  // END: CS348V STUDENTS MODIFY THIS CODE  
};

#endif  // CAMERA_PIPELINE_HPP_
