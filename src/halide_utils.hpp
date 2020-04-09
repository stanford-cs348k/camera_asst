#pragma once


#ifdef __USE_HALIDE__
#include <memory>

#include "Halide.h"

#include "camera_sensor.hpp"
#include "image.hpp"
#include "pixel.hpp"

Halide::Buffer<float>
burstSensorDataToHalide(const std::vector<std::unique_ptr<CameraSensorData<float> > >& raw_data);

Halide::Buffer<float>
sensorDataToHalide(CameraSensorData<float>* raw_data,
    const int width,
    const int height);

std::unique_ptr<Image<RgbPixel>> rgbImageFromHalide(Halide::Buffer<float>& output);

#endif
