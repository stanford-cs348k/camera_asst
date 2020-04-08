#include "halide_utils.hpp"

#ifdef __USE_HALIDE__

Halide::Buffer<float>
burstSensorDataToHalide(const std::vector<std::unique_ptr<CameraSensorData<float> > >& raw_data) {

  int nframes = raw_data.size();

  assert(nframes > 0);

  int width = raw_data.at(0)->width();
  int height = raw_data.at(0)->height();

  Halide::Buffer<float>
    input(width, height, nframes);
 
  for (int frame = 0; frame < nframes; frame++) {
    for (int row = 0; row < height; row++) {
      for (int col = 0; col < width; col++) {
        input(col, row, frame) = raw_data.at(frame)->data(row, col);
      }
    }
  }

  return input;
}

Halide::Buffer<float>
sensorDataToHalide(CameraSensorData<float>* raw_data,
    const int width,
    const int height) {

  Halide::Buffer<float> input(width, height);
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      input(col, row) = raw_data->data(row, col);
    }
  }
  return input;
}

std::unique_ptr<Image<RgbPixel>> rgbImageFromHalide(Halide::Buffer<float>& output) {
  std::unique_ptr<Image<RgbPixel>> image(new Image<RgbPixel>(output.width(), output.height()));

  for (int row = 0; row < output.height(); row++) {
    for (int col = 0; col < output.width(); col++) {

      // Note: Halide uses the col, row, channel convention
      float r = output(col, row, 0);
      float g = output(col, row, 1);
      float b = output(col, row, 2);

      auto& pixel = (*image)(row, col);
      pixel.r = r;
      pixel.g = g;
      pixel.b = b;
    }
  }

  return image;
}

#endif
