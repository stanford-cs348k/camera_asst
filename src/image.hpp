#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include <memory>
#include <string>

template<typename Pixel> class Image {
 public:
  using PixelType = Pixel;

  Image(int width, int height);
  ~Image() { delete[] pixels_; }  // pixels_ is always new'ed.

  int width() const { return width_; }
  int height() const { return height_; }

  const Pixel& operator()(int row, int col) const {
    return pixels_[row * width_ + col];
  }
  Pixel& operator()(int row, int col) {
    return pixels_[row * width_ + col];
  }

  void GammaCorrect(float gamma);

  std::unique_ptr<Image> Clone() const;

  bool WriteToBmp(std::string filename) const;
  static std::unique_ptr<Image> ReadFromBmp(std::string filename);

 private:
  // Disallow copy and assign.
  Image(Image&);
  void operator=(const Image&);

  const int width_;
  const int height_;
  Pixel* const pixels_;
};

#endif  // IMAGE_HPP_
