#include "camera_sensor.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include "common.hpp"

// static
std::unique_ptr<CameraSensor> CameraSensor::New(std::string filename) {
  FILE* f = fopen(filename.c_str(), "rb");
  if (not f) return nullptr;
  int num_planes;
  fread(&num_planes, sizeof(num_planes), 1, f);
  std::vector<CameraSensorImpl::SensorPlane> planes(num_planes);
  std::vector<Image<RgbPixel>*> perfect_images;
  int width;
  int height;
  fread(&width, sizeof(width), 1, f);
  fread(&height, sizeof(height), 1, f);
  T* const buffer = new T[num_planes * width * height];
  T* plane_buffer = buffer;
  float focus;
  std::vector<float> perfect_image_buffer(width * height * 3);
  for (int i = 0; i < num_planes; i++) {
    planes[i].buffer = plane_buffer;
    fread(&focus, sizeof(focus), 1, f);
    planes[i].focal_plane = focus;
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
  
  CameraSensorImpl::Opts opts;
  fread(&opts, sizeof(opts), 1, f);
  opts.noise_magnitude = .2f;
  
  // Print some debugging information
  //std::cout << "Noise mag: " << opts.noise_magnitude << std::endl;
  std::cout << "Read " << num_planes << " sensor planes of size ("
            << width << "," << height << ")" << std::endl;
  std::cout << "Focal planes : [";
  for (int i = 0; i < planes.size(); i++) {
      std::cout << planes[i].focal_plane;
      if (i < planes.size() -1 ) std::cout << ", ";
  }
  std::cout << "]" << std::endl;
  
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
  // Calculate min and max focal planes using planes_.
  min_focal_plane_ = std::numeric_limits<float>::max();
  max_focal_plane_ = std::numeric_limits<float>::min();
  for (const auto& plane : planes_) {
    min_focal_plane_ = std::min(min_focal_plane_, plane.focal_plane);
    max_focal_plane_ = std::max(max_focal_plane_, plane.focal_plane);
  }
  Random random(0);  // deterministic seed
  // Instance bright lines. About 1% of lines are boosted.
  const int num_bright_lines = height / 100;
  while (num_bright_lines > 0 && bright_lines_.size() < num_bright_lines) {
    const int row = random.UniformRandom<int>(0, height - 1);
    // Don't add the same line twice.
    if (In(bright_lines_, row)) continue;
    // Don't let adjacent lines be bright.
    if (In(bright_lines_, row + 1) || In(bright_lines_, row - 1)) continue;
    bright_lines_[row] =
        random.UniformRandom<T>(opts_.row_gain_min, opts_.row_gain_max);
  }
  // Instance dead pixels. About .1% of pixels will be dead.
  const int num_dead_pixels = width * height / 10000;
  while (num_dead_pixels > 0 && dead_pixels_.size() < num_dead_pixels) {
    const int row = random.UniformRandom<int>(0, height - 1);
    const int col = random.UniformRandom<int>(0, width - 1);
    dead_pixels_.insert({row, col});
  }
}

float CameraSensorImpl::GetFocalPlane() const {
  return planes_[active_sensor_plane_].focal_plane;
}

void CameraSensorImpl::SetFocalPlane(float focal_plane) {
  float min_distance = std::numeric_limits<float>::max();
  int index = -1;
  for (int i = 0; i < planes_.size(); i++) {
    const auto& plane = planes_[i];
    const auto distance = std::fabs(plane.focal_plane - focal_plane);
    if (distance < min_distance) {
      min_distance = distance;
      index = i;
    }
  }
  active_sensor_plane_ = index;
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
CameraSensorImpl::GetSensorData(
    int left, int top, int width, int height) const {

    Random noise;
    noise.Reseed();  // noise is "truly" random, and unique per shot
    
  std::unique_ptr<CameraSensorData<T>> data(
      new CameraSensorData<T>(width, height));

  const auto& plane = planes_[active_sensor_plane_];
  const T* const buff = plane.buffer;
  
  for (int row = 0; row < height; row++) {
    const T row_gain = GetOrDefault(bright_lines_, row, 0.f);
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
      data->data(row, col) += row_gain;
      
      // Add uniform random noise scales by noise_magnitude
      T value = data->data(row, col) + opts_.noise_magnitude * noise.UniformRandom<T>(-0.5f, 0.5f);
      // clamp to (0,1) range
      data->data(row, col) = std::max(0.0f, std::min(1.f, value));
    }
  }
  return data;
}
