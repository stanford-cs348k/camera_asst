#include "image.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include "common.hpp"
#include "pixel.hpp"

template<typename Pixel>
Image<Pixel>::Image(int width, int height)
    : width_(width), height_(height), pixels_(new Pixel[width_ * height_]) {}

template<typename Pixel>
void Image<Pixel>::GammaCorrect(float gamma) {
  for (int row = 0; row < height_; row++) {
    for (int col = 0; col < width_; col++) {
      (*this)(row, col) = Pixel::pow((*this)(row, col), gamma);
    }
  }
}

template<typename Pixel>
std::unique_ptr<Image<Pixel>> Image<Pixel>::Clone() const {
  std::unique_ptr<Image> image(new Image(width_, height_));
  std::memcpy(image->pixels_, pixels_, sizeof(Pixel) * width_ * height_);
  return image;
}

template<>
bool Image<RgbPixel>::WriteToBmp(std::string filename) const {
  auto clamp = [] (RgbPixel::T v) {
    return static_cast<unsigned char>(Clamp(v, 0.f, 255.f));
  };
  const int filesize = 54 + 3 * width_ * height_;
  std::vector<unsigned char> buff(3 * width_ * height_);
  for (int j = 0; j < height_; j++) {
    const auto* scan_line = pixels_ + j * width_;
    for (int i = 0; i < width_; i++) {
      const int x = i;
      const int y = (height_ - 1) - j;
      buff[(x + y * width_) * 3 + 2] = clamp(scan_line[i].r);
      buff[(x + y * width_) * 3 + 1] = clamp(scan_line[i].g);
      buff[(x + y * width_) * 3 + 0] = clamp(scan_line[i].b);
    }
  }

  unsigned char file_header[14] =
      {'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0};
  unsigned char info_header[40] =
      {40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0};
  unsigned char pad[3] = { 0, 0, 0 };

  file_header[2] = (unsigned char)(filesize);
  file_header[3] = (unsigned char)(filesize >> 8);
  file_header[4] = (unsigned char)(filesize >> 16);
  file_header[5] = (unsigned char)(filesize >> 24);

  info_header[4] = (unsigned char)(width_);
  info_header[5] = (unsigned char)(width_ >> 8);
  info_header[6] = (unsigned char)(width_ >> 16);
  info_header[7] = (unsigned char)(width_ >> 24);
  info_header[8] = (unsigned char)(height_);
  info_header[9] = (unsigned char)(height_ >> 8);
  info_header[10] = (unsigned char)(height_ >> 16);
  info_header[11] = (unsigned char)(height_ >> 24);

  FILE* f = fopen(filename.c_str(), "wb");
  if (not f) return false;
  fwrite(file_header, sizeof(unsigned char), 14, f);
  fwrite(info_header, sizeof(unsigned char), 40, f);
  for (int i = 0; i < height_; i++) {
    fwrite(&(buff[0]) + (width_ * i * 3),
           3 * sizeof(unsigned char),
           width_,
           f);
    fwrite(pad, sizeof(unsigned char), (4 - (width_ * 3) % 4) % 4, f);
  }
  fclose(f);
  return true;
}

// static
template<>
std::unique_ptr<Image<RgbPixel>> Image<RgbPixel>::ReadFromBmp(
    std::string filename) {
  FILE* f = fopen(filename.c_str(), "rb");
  if (not f) return nullptr;
  unsigned char header[54];
  fread(header, sizeof(unsigned char), 54, f);  // read the 54 byte header
  // Get image height and width from header.
  const int width = *((int*)&header[18]);
  const int height = *((int*)&header[22]);
  std::unique_ptr<Image> image(new Image(width, height));
  unsigned char data[3];
  unsigned char padding[3];
  for (int row = height - 1; row >= 0; row--) {
    for (int col = 0; col < width; col++) {
      auto& pixel = (*image)(row, col);
      fread(data, sizeof(unsigned char), 3, f);
      pixel.b = static_cast<RgbPixel::T>(data[0]);
      pixel.g = static_cast<RgbPixel::T>(data[1]);
      pixel.r = static_cast<RgbPixel::T>(data[2]);
    }
    fread(padding, sizeof(unsigned char), (4 - (width * 3) % 4) % 4, f);
  }
  fclose(f);
  return image;
}

// Read/Write of images not supported for float images.
template<>
bool Image<FloatPixel>::WriteToBmp(std::string) const {
  return false;
}
// static
template<>
std::unique_ptr<Image<FloatPixel>> Image<FloatPixel>::ReadFromBmp(std::string) {
  return nullptr;
}

template class Image<RgbPixel>;
template class Image<FloatPixel>;
