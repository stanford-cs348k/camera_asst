# CS348V Assignment 2: RAW Processing for the kPhone 348V #

__Due: Wed Feb 7th, 11:59pm (100 total pts)__

In this assignment you will implement a simple RAW image processing pipeline for the camera of the world's hottest smartphone, the kPhone 348V. Your job is to process the data coming off the device's sensor to produce the highest quality image you can. In addition to implementing the necessary image processing to produce a good image, you are also responsible for controlling the focus of the camera.

## Getting Started ##

Grab the assignment starter code.

    git clone git@github.com:stanford-cs348v/asst2.git

To run the assignment, you will need to download the scene datasets, located at http://graphics.stanford.edu/courses/cs348v-18-winter/asst/asst2/scenes.tar.

__Build Instructions__

The codebase uses a simple `Makefile` as the build system. To build the starter code, run `make` from the top level directory. The assignment source code is in `src/`, and object files and binaries will be populated in `build/` and `bin/` respectively.

__Running the starter code:__

Now you can run the camera. Just run:

    ./bin/kcamera SCENES_DIR/sanmiguel.bin output.bmp

The camera will "take a picture" and output results of processing the RAW sensor data from the sensor to `output.bmp`. The starter code just copies the sensor data verbatim into the red, green, and blue channels of the output image. (so the output is just a visualization of the RAW data from the sensor).  So for a scene that looks like the image at left, you should see output that looks a bit like this.

 ![Example](http://graphics.stanford.edu/courses/cs348v-18-winter/asst_images/asst2/sanmiguel_example.jpg "San Miguel RAW data")

## Assignment Components ##

### Part 1: Basic RAW Processing (40 pts) ###

In the first part of the assignment you need to process the raw image data to produce an RGB image that, simply put, looks at good as you can make it. The entry point to your code should be `CameraPipeline::ProcessShot()` in `camera_pipeline.cpp`.  This method reads RAW data from the sensor, and output an RGB image.

You will need to do the following in this function:

* Demosaic the interleaved RGB channels in the raw sensor data. You may use any technique you wish.
* Denoise and correct for any sensor defects (such as dead pixels and bright lines that exist in the output).  You may use any techniques you wish.

__Tips:__

* In this part of the assignment it is likely best to start by working on images the don't require auto-focusing. These include: `black.bin` (an all black image), `gray.bin` (a 50% gray image, for which pixels without defects should be [128,128,128]), `stripe.bin` (a tough case for demosaicing), `color.bin`, `stanford.bin`, and `sanmiguel.bin`)
* You may implement this assignment in any way you wish.  Certainly, you will have to demosaic the image to recover RGB values. The techniques you employ for handling noise, bad pixels, defect pixels, and vignetting artifacts are up to you.
* We guarantee that pixel defects (stuck pixels, pixels with extra sensitivity) are static defects that are the same for every photograph taken by the camera (for a given scene -- it can vary between scenes). (Hint: What does this suggest a simple way to "calibrate" your camera for static defects?) Note that while the overall noise statistics of the sensor are the same per photograph, the perturbation of individual pixel values due to noise varies per photograph (that's the nature of noise!).
* You may assume that RAW input data from the sensor will be in the range [0, 1].
* The following is the Bayer filter pattern used on the kPhone's sensor.  Pixel (0,0) is the top-left of the image.
![Bayer Array](http://graphics.stanford.edu/courses/cs348v-18-winter/asst_images/asst2/bayer.jpg "Bayer color filter array for the kPhone 348V")

### Part 2: Tone Adjustment (40 pts) ###

You'll notice that some of the test scenes, like the San Miguel scene, have both very bright regions.  In this part of the assignment, you'll improve the aesthetics of your output images by performing a form of local tone adjustment.  Tone adjustment can result is more pleasing images by preserving detail in both the darkest and brightest regions of the images.

You can test your tone adjustment algorithm in isolation by using `CameraSensor::GetPerfectImage()` which provides a "perfect" version of the scene (imagine a black-box oracle which could denoise and demosaic perfectly). For example, here is a comparison of a naive method (simple gamma correction) vs. Exposure Fusion on a "perfect" image:
![Gamma Correction](http://graphics.stanford.edu/courses/cs348v-18-winter/asst_images/asst2/breakfast-lamps-gamma.bmp "Tone adustument using gamma correction") ![Exposure Fusion](http://graphics.stanford.edu/courses/cs348v-18-winter/asst_images/asst2/breakfast-lamps-ll.bmp "Tone adustument using Exposure Fusion")

We'd like to to implement one of two algorithms for tone adjustment: [Local Laplacian Filtering](https://people.csail.mit.edu/sparis/publi/2011/siggraph/), or a simple variant of [Exposure Fusion](https://mericam.github.io/exposure_fusion/index.html).  Although researchers have now established its [theoretical interpretations](https://dl.acm.org/citation.cfm?id=2629645), Local Laplacian filtering is a conceptually simple algorithm that performs image manipulation using two data structures we discussed in class: the Gaussian pyramid and the Laplacian pyramid.

* Regardless of which algorithm you use, the first step of this part of the assignment is to implement construction of Gaussian and Laplacian pyramids for the luminance channel of your image. (You will want to convert your result from part 1 into YUV format, and compute single channel Gaussian and Laplacian pyramids on Y.)  We discussed both of these algorithms in class.

* Given these data structures performing advanced image adjustments like tone manipulation and detail enhancement is surprisingly simple.

If you pursure a Local Laplacian filtering approach, we urge you to first read the paper [Local Laplacian Filters: Edge-aware Image Processing with a Laplacian Pyramid](https://people.csail.mit.edu/sparis/publi/2011/siggraph/) (Paris et al. 2011) to understand the idea of Local Laplacian filtering. However, we suggest you implement the faster version of the algorithm that is presented in Section 3 of [this later paper](https://dl.acm.org/citation.cfm?id=2629645), and explained in [these helpful notes](http://graphics.stanford.edu/courses/cs348v-18-winter/asst/asst2/local_laplacian_notes.pdf) by Raj.

If you pursue the exposure fusion approach, first read [Mertens et. al 2007](https://mericam.github.io/exposure_fusion/index.html), then take a look at the paragraphs under the heading _"Dynamic Range Compression"_ in Section 6 of the [Google HDR+ paper](Burst Photography for High Dynamic Range and Low-light Imaging on Mobile Cameras) we read in class.  This describes a simple tone adjustment algorithm that is based on Exposure Fusion, but only requires two input images (a low exposure and high exposure image) that you can derive from the RAW image provided to you by the sensor in this assignment. 

### Part 3: Autofocus (20 pts) ###

In the final of the assignment you will implement a simple autofocus algorithm. Based on analysis of regions of the sensor (notice that `sensor_->GetSensorData()` can return a crop window of the full sensor), you should design an algorithm that sets the camera's focus via a call to `sensor_->SetFocus()`.

As you're probably well aware from your own life experiences, it can be very frustrating when camera takes a long time to focus, causing you to miss a great action shot.  Therefore, a good implementation of autofocus should try and make its focusing decision quickly by analyzing as few pixels as possible. Although we are not grading based on the performance of your autofocus implementation, it can be fun to design an algorithm that quickly converges to a good solution. You can gather statistics on how many crop windows requested, how many pixels requested, etc. The method `sensor_->GetMinFocalPlane()` and `sensor_->GetMinFocalPlane()` are convenient to obtain the range of focal planes the current scene admits.

## Provided Code ##

Much of the scaffolding code (reading and writing data, storing images, etc.) is provided for you. Your changes should only go in regions marked like:

    // BEGIN: CS348V STUDENTS MODIFY THIS CODE 
    ...
    // END: CS348V STUDENTS MODIFY THIS CODE 

Only two files contain such regions:
* `camera_pipeline.hpp` and `camera_pipeline.cpp` where you can customize the `CameraPipeline` class (while maintaining the same API), and implement the camera pipeline itself.

The driver code for this assignment (containing `main()`) is located in `camera_main.cpp`.

__CameraSensor__ is a class which presents the same interface as a real camera sensor. It has methods like `SetLensCap()`, `SetFocalPlane()`, and `GetSensorData()`.  From inside `CameraPipeline` class you can access the sensor via the local member variable `sensor_`.

__CameraSensorData__ is a wrapper around the raw camera sensor data. It has a method `data(row, col)` which returns the floating point intensity at index `(row, col)` in the sensor array. This is the object returned by `CameraSensor::GetSensorData()`.

__CameraPipeline__ holds your implementation of the `CameraPipelineInterface` interface, whose job it is to take in a `CameraSensor` object and output a processed RGB image.

__Image__ is an image container for both RGB- and YUV-space images. It is nothing more than a wrapper on top of a buffer of pixels. It has one important function, which is `operator ()(row, col)`, which returns the pixel at row `row` and column `col`. The pixel is returned by reference, so it can be modified directly.  

__Pixel__ (in `Pixel.hpp`) is struct of convenience routines for 3-channel pixels. You may find the routines `RgbToYuv` and `YuvToRgb` helpful.

__Tips__:

* Most of the detail of the stater code has been abstracted away for you and effectively your entire implementation of RAW processing and autofocus can go in to the `CameraPipeline::ProcessShot()` function.
* For the relevant classes, look at the `.hpp` files to understand the public API. 
* If you are having trouble using some of the provided functionality, feel free to implement your own versions. We just ask that you do not modify existing classes, but add new ones if necessary.
* The starter code makes heavy use of `std::unique_ptr`.  If you are not familiar with `std::unique_ptr`, see <https://shaharmike.com/cpp/unique-ptr/>. 

## Grading ##

This assignment is not graded on wall-clock performance, only image quality. (However, we reserve the right to grudgingly take points off if your autofocus implementation is shockingly brute force or naive.) A reasonable implementation will address the challenges of demosaicing sensor output, correcting pixel defects, removing noise, and implementing tone mapping using Local Laplacian filtering, and ultimately produce a good quality image. (We don't have a numeric definition of good since there is no precise right answer...it's a photograph, you'll know a good one when you see it.)

To verify that you have actually implemented the "auto" part of autofocus, we will test your autofocus algorithm on at least one test case that has not been given to you. The composition/framing of the grading scenes will be straightforward (i.e. the foreground object will be easy to find for a reasonable auto-focus algorithm).

## Handin ##

This assignment will be handed in using Canvas.

#### Code Handin ####

Please hand in `camera_pipeline.cpp`, `camera_pipeline.hpp`, We should be able to build and run the code on the myth machines by dropping these files into a freshly checked out starter code tree.

### Writeup Handin ###

Please include a short writeup describing your implementation. In this writeup, specifically address the following:

* Describe the techniques you employed to address image-quality problems caused by noise and sensor defects.
* Describe your autofocus algorithm:
 * What regions of the screen did you choose to focus on (pun definitely intended)? Why?
 * Did you have to combat any problems caused by sensor noise in your autofocus algorithm?
* If applicable, describe any additional optimizations or features you added to the camera pipeline.
