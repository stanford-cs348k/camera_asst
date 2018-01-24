#ifndef PIXEL_HPP_
#define PIXEL_HPP_

#include <cmath>

struct Float3Pixel {
  using T = float;

  Float3Pixel() = default;
  Float3Pixel(T r_in, T g_in, T b_in) : r(r_in), g(g_in), b(b_in) {}
  Float3Pixel(unsigned char r_in, unsigned char g_in, unsigned char b_in)
    : r(static_cast<T>(r_in)),
      g(static_cast<T>(g_in)),
      b(static_cast<T>(b_in)) {}
  Float3Pixel operator*(T a) const { return {r * a, g * a, b * a}; }
  Float3Pixel operator+(const Float3Pixel& that) const {
    return {r + that.r, g + that.g, b + that.b};
  }
  Float3Pixel operator-(const Float3Pixel& that) const {
    return {r - that.r, g - that.g, b - that.b};
  }
  Float3Pixel operator+(T a) const { return {r + a, g + a, b + a}; }
  Float3Pixel operator-(T a) const { return {r - a, g - a, b - a}; }
  Float3Pixel& operator+=(const Float3Pixel& that) {
    r += that.r;
    g += that.g;
    b += that.b;
    return *this;
  }
  Float3Pixel& operator-=(const Float3Pixel& that) {
    r -= that.r;
    g -= that.g;
    b -= that.b;
    return *this;
  }
  static Float3Pixel pow(const Float3Pixel& that, T power) {
    return {
        std::pow(that.r, power),
        std::pow(that.g, power),
        std::pow(that.b, power) };
  }

  // Helper functions to convert between RGB and YUV coordinates.
  static Float3Pixel RgbToYuv(const Float3Pixel& rgb) {
    Float3Pixel out;
    out.y = .299f * rgb.r + .587f * rgb.g + .114f * rgb.b;
    out.u = .492f * (rgb.b - out.y);
    out.v = .877f * (rgb.r - out.y);
    return out;
  }
  static Float3Pixel YuvToRgb(const Float3Pixel& yuv) {
    Float3Pixel out;
    out.r = yuv.y + 1.14f * yuv.v;
    out.g = yuv.y - .395f * yuv.u - .581f * yuv.v;
    out.b = yuv.y + 2.033f * yuv.u;
    return out;
  }

  union {
    T r = 0.;
    T y;
  };
  union {
    T g = 0.;
    T u;
  };
  union {
    T b = 0.;
    T v;
  };
};

struct FloatPixel {
  using T = float;

  FloatPixel() = default;
  FloatPixel(T i_in) : i(i_in) {}
  FloatPixel(unsigned char i_in) : i(static_cast<T>(i_in)) {}
  FloatPixel operator*(T a) const { return {i * a}; }
  FloatPixel operator+(const FloatPixel& that) const { return {i + that.i}; }
  FloatPixel operator-(const FloatPixel& that) const { return {i - that.i}; }
  FloatPixel& operator+=(const FloatPixel& that) {
    i += that.i;
    return *this;
  }
  FloatPixel& operator-=(const FloatPixel& that) {
    i -= that.i;
    return *this;
  }
  static FloatPixel pow(const FloatPixel& that, T power) {
    return { std::pow(that.i, power) };
  }

  T i = 0.;
};

typedef Float3Pixel RgbPixel;
typedef Float3Pixel YuvPixel;

#endif  // PIXEL_HPP_
