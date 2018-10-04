# CS348K Assignment: Camera RAW Processing #

In this assignment you will implement a simple RAW image processing pipeline for the camera of the world's hottest smartphone, the kPhone 348. Your job is to process the data coming off the device's sensor to produce the highest quality image you can. The assignment is split into two phases:

 * Phase 1, you'll process a single RAW image to demoaisc the image and correct for sensor defects.
 * Phase 2, you'll extend your implementation to process a burst of images from a scene featuring a wide range of intensities.  You'll align and merge the images of the burst to reduce noise, and then perform local tone mapping to produce a compelling HDR result.  

## Getting Started ##

Grab the assignment starter code.

    git clone git@github.com:stanford-cs348k/camera_asst.git

To run the assignment, you will also need to download the scene datasets, located at <http://cs348k.stanford.edu/fall18content/asst/scenes.tgz>.

For example, on the Linux SU myth machines:

    wget http://cs348k.stanford.edu/fall18content/asst/scenes.tgz
    tar -xvf scenes.tgz

__Build Instructions__

The codebase uses a simple `Makefile` as the build system. To build the starter code, run `make` from the top level directory. The assignment source code is in `src/`, and object files and binaries will be generated into `build/` and `bin/` respectively.

__Running the starter code:__

Now you can run the camera. Just run:

    ./bin/kcamera MY_SCENES_DIR/taxi.bin output.bmp

(where `MY_SCENES_DIR` is where you extracted the scene datasets into) The camera will "take a picture" and output the result of processing the RAW sensor data from the sensor to `output.bmp`. The starter code just copies the sensor data verbatim into the red, green, and blue channels of the output image. (so the output is just a visualization of the RAW data from the sensor).  So for a scene that looks like the image at left, you should see output that looks a bit like this.

 ![RAW Example](http://cs348k.stanford.edu/fall18content/asst/taxi_figure.jpg "RAW Data visualization")

## Part 1 (35 points) ##

### Due Monday October 8th, 11:59pm ###

In the first part of the assignment you must process the raw image data to produce an RGB image that, simply put, looks as good as you can make it. The entry point to your code should be `CameraPipeline::ProcessShot()` in `camera_pipeline.cpp`.  This method reads RAW data from the sensor, and outputs an RGB image.

You will need to do the following in this function:

* Demosaic the interleaved RGB channels in the raw sensor data. You may use any technique you wish.
* Denoise the image and correct for any sensor defects (such as dead pixels that always read out as white).  You may use any techniques you wish.
* Rescale the floating point pixel values to 8-bit integer values in the 0-255 range.  We recommend that you first do this by remapping the floating point range (0,1) to the range (0,255).  We'll address more intelligent forms of "tone mapping" in Part 2 of the assignment. 

__Test scenes:__
* Your primary test scenes are taken directly from Google's [HDR+ dataset](https://hdrplusdata.org/dataset.html). These scenes have been chosen to stress different aspects of real-world images. They are: `taxi.bin` (busy street scene with low-light), `hand.bin` (close-up shot with lots of texture), `church.bin` (high dynamic range), and `path.bin` (complex, high-resolution scene). For each of these scenes, we've provided a `SCENE_solution_part1.bmp` which is the output of a reference pipeline which would achieve full points for this part of the assignment, and `SCENE_google.bmp` which is the output of the full Google HDR+ pipeline (as you can see, it's just a *bit* better than our reference implementation!).
* In addition, we've provided some helpful debugging scenes, such as: `black.bin` (an all black image), `gray.bin` (a 50% gray image, for which pixels without defects should be [128,128,128]), `stripe.bin` (a tough case for demosaicing), `color.bin`, and `stanford.bin`)

__Tips:__

* You may implement this assignment in any way you wish.  Certainly, you will have to demosaic the image to recover RGB values. The techniques you employ for handling noise, bad pixels, defect pixels are up to you.
* We guarantee that pixel defects (stuck pixels, pixels with extra sensitivity) are static defects that are the same for every photograph taken for a given scene.  Note that while the overall noise statistics of the sensor are the same per photograph, the perturbation of individual pixel values due to noise varies per photograph (that's the nature of noise!).
* You may assume that RAW data read from the sensor is linear in incident light, and in the range [0, 1].
* The following is the Bayer filter pattern used on the kPhone's sensor.  Pixel (0,0) is the top-left of the image.
![Bayer Array](http://graphics.stanford.edu/courses/cs348v-18-winter/asst_images/asst2/bayer.jpg "Bayer color filter array for the kPhone 348V")
* You should start with basic linear interpolation demosicing as [discussed in class](http://cs348k.stanford.edu/fall18/lecture/camerapipeline/slide_059).  However, we encourage you to attempt more advanced demosiacing solution as discussed [in lecture](http://cs348k.stanford.edu/fall18/lecture/camerapipeline/slide_067), or in [this paper that's listed under the course recommended readings page](https://ieeexplore.ieee.org/document/1407714).

## Description of the Starter Code ##

Much of the scaffolding code (reading and writing data, storing images, etc.) is provided for you. Your changes should only go in regions marked like:

    // BEGIN: CS348K STUDENTS MODIFY THIS CODE 
    ...
    // END: CS348K STUDENTS MODIFY THIS CODE 

Only two files contain such regions:
* `camera_pipeline.hpp` and `camera_pipeline.cpp` where you can customize the `CameraPipeline` class (while maintaining the same API), and implement the camera pipeline itself.

The driver code for this assignment (containing `main()`) is located in `camera_main.cpp`.

__CameraSensor__ is a class which presents the same interface as a real camera sensor. It has methods like `SetLensCap()` and `GetSensorData()`.  From inside `CameraPipeline` class you can access the sensor via the local member variable `sensor_`.

__CameraSensorData__ is a wrapper around the raw camera sensor data. It has a method `data(row, col)` which returns the floating point intensity at index `(row, col)` in the sensor array. This value will always be between 0 and 1. This is the object returned by `CameraSensor::GetSensorData()`.

__CameraPipeline__ holds your implementation of the `CameraPipelineInterface` interface, whose job it is to take in a `CameraSensor` object and output a processed RGB image.

__Image__ is an image container for both RGB- and YUV-space images. It is nothing more than a wrapper on top of a buffer of pixels. It has one important function, which is `operator ()(row, col)`, which returns the pixel at row `row` and column `col`. The pixel is returned by reference, so it can be modified directly.  

__Pixel__ (in `Pixel.hpp`) is struct of convenience routines for 3-channel pixels. You may find the routines `RgbToYuv` and `YuvToRgb` helpful.

__Tips__:

* Most of the detail of the starter code has been abstracted away for you and effectively your entire implementation of RAW processing can go in to the `CameraPipeline::ProcessShot()` function.
* For the relevant classes, look at the `.hpp` files to understand the public API. 
* If you are having trouble using some of the provided functionality, feel free to implement your own versions. We just ask that you do not modify existing classes, but add new ones if necessary.
* The starter code makes heavy use of `std::unique_ptr`.  If you are not familiar with `std::unique_ptr`, see <https://shaharmike.com/cpp/unique-ptr/>. 

## Grading ##

Phase 1 of the assigment will be graded on image quality. A reasonable implementation will address the challenges of demosaicing sensor output, correcting pixel defects and removing noise.  We don't have a numeric definition of good since there is no precise right answer here... it's a photograph, you'll know a reasonably looking one when you see it!  We encourage you to start with simple algorithms, get them to work, and then if there is time, attempt to improve image quality to move to more advanced techniques.

## Handin ##

This assignment will be handed in using Canvas:  <http://canvas.stanford.edu>

 * Please hand in `camera_pipeline.cpp`, `camera_pipeline.hpp`, We should be able to build and run the code on the myth machines by dropping these files into a freshly checked out starter code tree.
 * Please also include a short writeup describing the techniques you employed to implement demoasicing and image-quality problems caused by noise and sensor defects.

# Part 2 (65 pts): Burst Mode Alignment for Denoising + Local Tone Mapping #

Part 2 of the assignment will be released on Monday October 8th and be due on Monday October 22nd.
