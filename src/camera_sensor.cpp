#include "camera_sensor.hpp"
#include "common.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <limits>
#include <cassert>

namespace {
template <typename T>
void ReadBIN(const std::string& path, std::vector<CameraSensorImpl::SensorPlane> &planes,
             std::vector<Image<RgbPixel> *> &perfect_images,
             CameraSensorImpl::Opts& opts,
             int &width, int &height,
             T*& buffer) {
  FILE* f = fopen(path.c_str(), "rb");
  assert(f);
  int num_planes;
  fread(&num_planes, sizeof(num_planes), 1, f);
  planes.resize(num_planes);
  fread(&width, sizeof(width), 1, f);
  fread(&height, sizeof(height), 1, f);
  buffer = new T[num_planes * width * height];
  T* plane_buffer = buffer;
  float focus;
  std::vector<float> perfect_image_buffer(width * height * 3);
  for (int i = 0; i < num_planes; i++) {
    planes[i].buffer = plane_buffer;
    fread(&focus, sizeof(focus), 1, f);
    fread(plane_buffer, sizeof(T), width * height, f);
    plane_buffer += width * height;
    fread(&(perfect_image_buffer[0]), sizeof(float), width * height * 3, f);
    const int channel_stride = width * height;
    std::unique_ptr<Image<RgbPixel>> perfect_image(
        new Image<RgbPixel>(width, height));
    for (int row = 0; row < height; row++) {
      for (int col = 0; col < width; col++) {
        auto& pixel = (*perfect_image)(row, col);
        const int pixel_index = width * row + col;
        pixel.r = perfect_image_buffer[pixel_index];
        pixel.g = perfect_image_buffer[pixel_index + channel_stride];
        pixel.b = perfect_image_buffer[pixel_index + 2 * channel_stride];
      }
    }
    perfect_images.push_back(perfect_image.release());
  }
  fread(&opts, sizeof(opts), 1, f);
  opts.noise_magnitude = .05f;
}
}
// static
std::unique_ptr<CameraSensor> CameraSensor::New(std::string filename) {
  {
    // Check if file exists
    std::ifstream infile(filename);
    if (!infile.good()) {
      return nullptr;
    }
  }

  std::vector<CameraSensorImpl::SensorPlane> planes;
  std::vector<Image<RgbPixel>*> perfect_images;
  int width;
  int height;
  CameraSensorImpl::Opts opts;
  T* buffer;
  ReadBIN<T>(filename, planes, perfect_images, opts, width, height, buffer);
  
  // Print some debugging information
  //std::cout << "Noise mag: " << opts.noise_magnitude << std::endl;
  std::cout << "Read " << planes.size() << " sensor planes of size ("
            << width << "," << height << ")" << std::endl;
  
  return std::unique_ptr<CameraSensor>(
      new CameraSensorImpl(
          width, height, buffer, planes, perfect_images, opts));
}

CameraSensorImpl::CameraSensorImpl(int width,
                                   int height,
                                   const T* buffer,
                                   std::vector<SensorPlane> planes,
                                   std::vector<Image<RgbPixel>*> perfect_images,
                                   Opts opts)
  : width_(width),
    height_(height),
    buffer_(buffer),
    planes_(planes),
    perfect_images_(perfect_images),
    opts_(opts) {
  Random random(0);  // deterministic seed
  // Instance dead pixels. About .1% of pixels will be dead.
  const int num_dead_pixels = width * height / 10000;
  while (num_dead_pixels > 0 && dead_pixels_.size() < num_dead_pixels) {
    const int row = random.UniformRandom<int>(0, height - 1);
    const int col = random.UniformRandom<int>(0, width - 1);
    dead_pixels_.insert({row, col});
  }
}

std::unique_ptr<Image<RgbPixel>> CameraSensorImpl::GetPerfectImage(
    int left, int top, int width, int height) const {
  std::unique_ptr<Image<RgbPixel>> image(new Image<RgbPixel>(width, height));
  const auto& perfect_image = *(perfect_images_[active_sensor_plane_]);
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      (*image)(row, col) = perfect_image(row + top, col + left);
    }
  }
  return image;
}

std::unique_ptr<CameraSensorData<typename CameraSensorImpl::T>>
CameraSensorImpl::GetSensorData(int left, int top, int width,
                                int height) const {

  Random noise;
  noise.Reseed(); // noise is "truly" random, and unique per shot

  std::unique_ptr<CameraSensorData<T>> data(
      new CameraSensorData<T>(width, height));

  const auto &plane = planes_[active_sensor_plane_];
  const T *const buff = plane.buffer;

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      const std::array<int, 2> index = {row, col};
      if (In(dead_pixels_, index)) {
        data->data(row, col) = opts_.dead_pixel_value;
        continue;
      }
      if (lens_cap_) {
        data->data(row, col) = 0.f;
      } else {
        data->data(row, col) = buff[(top + row) * width_ + (left + col)];
      }
      
      // Add uniform random noise scales by noise_magnitude
      T value = data->data(row, col) + opts_.noise_magnitude * noise.UniformRandom<T>(-0.5f, 0.5f);
      // clamp to (0,1) range
      data->data(row, col) = std::max(0.0f, std::min(1.f, value));
    }
  }
  return data;
}

std::vector<std::unique_ptr<CameraSensorData<typename CameraSensorImpl::T>>>
CameraSensorImpl::GetBurstSensorData(
    int left, int top, int width, int height) const {
  std::vector<std::unique_ptr<CameraSensorData<T>>> data;
  int temp_index = active_sensor_plane_;
  for (int p = 0; p < planes_.size(); ++p) {
    active_sensor_plane_ = p;
    data.emplace_back(std::move(GetSensorData(left, top, width, height)));
  }
  active_sensor_plane_ = temp_index;
  return data;
}
