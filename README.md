# CS348K Assignment: Camera RAW Processing Pipeline #

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

This assignment will be handed in using Canvas:

 * Please hand in `camera_pipeline.cpp`, `camera_pipeline.hpp`, We should be able to build and run the code on the myth machines by dropping these files into a freshly checked out starter code tree.
 * Please also include a short writeup describing the techniques you employed to implement demoasicing and image-quality problems caused by noise and sensor defects.

# Part 2 (65 pts): Burst Mode Alignment for Denoising + Local Tone Mapping #

__Note:__
We've updated the scene assets from part 1 to include reference images produced using a simple reference implementation of the alignment and tone mapping algorithms you will implement in this part of the assignment. You should redownload the `scenes.tgz` file if you'd like to compare against these references. (Note: the reference solutions involve a basic implementation of the required techniques.  Motivated students will certainly be able to do better.)

### Due Monday October 22nd, 11:59pm ###

When implementing your solution to the first part of this assignment, you might have noticed a few visually objectional artifacts in your output. Consider the `taxi.bin` image:

![Noise visualization](http://cs348k.stanford.edu/fall18content/asst/taxi_noise_figure.png? "Noise visualization")

There are at least two issues you might notice here:
1. *Under-exposure:* To avoid over-exposing the bright sunset in the background, this image has been deliberately under-exposed. However, this means that regions of the the image which don't receive as much illumination (e.g. the front parts of the taxis) are very dark. 
2. *Noise:* Because the image was under-exposed, the effects of sensor noise are much more noticeable. In particular, the zoomed in region from the figure above shows how dominant the noise can become in dark regions.

In this part of the assignment, you'll correct for under-exposure using a form of local tone mapping called **exposure fusion** and deal with noise using a simplified version of the **HDR+ align and merge** algorithm.

## Local Tone Mapping

Tone mapping converts a high dynamic range image to a low dynamic range image (e.g., 8 bbits per pixel) that can be viewed on a (presumably low-dynamic range) display. 

For local tone mapping, you will implement a modified version of the [Exposure Fusion paper](http://ntp-0.cs.ucl.ac.uk/staff/j.kautz/publications/exposure_fusion.pdf) by Mertens et al. The key idea of exposure fusion is that, while it's very difficult to capture a single image where all parts of the image are well-exposed, it's possible to capture multiple exposures of the same image and then combine the well-exposed parts of each of these image to create a better image. 

In order to use this algorithm to tone map a single image, we are going to artifically create images under different exposure settings using our single input image. In particular, we are going to use two exposures: dark, which is the original unmodified image, and bright, which we create by multiplying the original image by a scale factor. This is similar to the approach taken in the HDR+ paper. The dark image will retain detail in the areas of the image with a lot of light (since it is under-exposed), while the bright image will retain detail in the areas with little light (since it is over-exposed).

Here's a sketch of the modified exposure fusion algorithm (you should read over the original paper to understand each step in more detail):
1. Convert output image from Part 1 of the assignment into grayscale
2. Create the two artifical exposure brackets from the grayscale images: dark and bright
3. Using a weighting function of your choice (section 3.1 in the paper), compute weights for both images
4. Compute a laplacian pyramid of both images and a gaussian pyramid of both weights
5. Blend the laplacian pyramids together using the gaussian pyramid of weights (section 3.2 in the paper)
6. Extract the exposure fused image from the blended pyramid and return it

For example, here's our reference pipeline's dark and bright images with their corresponding weights and the final output:
![Exposure Fusion](http://cs348k.stanford.edu/fall18content/asst/taxi_exposure_fusion_figure.png? "Exposure Fusion")

White in the weight images represent a high value, and black represents a low value. As you can see, the weights in the dark image select the well-exposed sky in the background while the bright image selects the brightened taxis in the foreground. 

This algorithm makes the image look much brigher in the dark regions without blowing out the already bright regions. But what about the noise? let's zoom back into that dark region we were looking at before:
![Tone Mapped Zoom](http://cs348k.stanford.edu/fall18content/asst/taxi_exposure_fusion_zoom_figure.png? "Exposure Fusion Zoom")

By boosting the dark regions (which are already prone to sensor noise), we have created even more objectional noise artifacts. Fortunately, burst mode alignment from the HDR+ paper exactly solves this problem and is the next sub-part of this assignment.

## Burst Mode Alignment for Denoising

In this sub-part of the assignment, you will write code to align and merge a burst of noisy images to produce a less noisy output image. The entry point to your code is the same as in the previous parts, but instead of calling `sensor_->GetSensorData`, you should call `sensor_->GetBurstSensorData`. This method reads a burst of RAW data from the sensor and returns it as a `std::vector` of bayered images. Your job is to implement a simplified version of the alignment and merging steps from the HDR+ paper to produce a denoised bayer image that can be processed by your existing camera pipeline code.

But first, why do we need this special alignment and merging step? What if we simply denoised by averaging together our burst of images? Let's try it:

![Average Denoising](http://cs348k.stanford.edu/fall18content/asst/taxi_averaging_figure.png#1 "Average Denoising")

The resulting image is absolutely less noisy, since by averaging images together the noise cancels itself out, but it is also very blurry because the input images were captured at different points in time. This is the motivation for the HDR+ image *alignment* and *merging* steps. Here is a sketch of an implementation, though feel free to make modifications or enhancements to this algorithm that you think can produce a better result:

__Alignment (section 4 in the HDR+ paper):__
The following is a suggestion for how to implement the alignment step:
1. Convert the stack of raw bayer images from `GetBurstSensorData` to grayscale by averaging together every 2x2 bayer grid (effectively downsampling by 2x).
2. Compute gaussian pyramids for each of these grayscale images (use your code from the exposure fusion sub-part).
3. For each of the images in the burst, perform a hierarchical alignment to the reference image (the first image in the burst) by following steps 4-6.
4. For each level of the gaussian pyramid, starting at the coarsest:
5. For each tile of the reference image at this level, find the closest matching tile in the image that is being matched against (we use the absolute difference between the tiles as a measure of distance)
6. Upsample the offsets to the next level and repeat step 5 using the upsampled offsets as starting points for the next search

The paper mentions many other additional steps: subpixel alignment using an L2 metric, a robust upsampling strategy for the alignment fields, varying search radii, fourier transforms for fast matching, etc. These can certainly improve your alignment, and we encourage you to attempt to implement them, but they are not necessary to achieve a decent image for this assignment.

__Merging (section 5 in the HDR+ paper):__
The merging algorithm in the HDR+ paper uses an advanced noise model and operates in the frequency domain. We encourage you to implement the merging step in whichever way you choose, but provide here a sketch of a relatively simple implementation which will produce decent results:
1. For overlapping tiles in the reference image (we use tiles of size 16 with stride 8): 
2. For each neighboring image, use the alignment offset to find the tile to merge.
3. Compute a merging weight for the tile from step 2 by comparing that tile to the reference image tile. In our implementation, we use the distance metric from the alignment step to compute an initial weight and then clamp all values below some minimum to 1 (full weight) and all values above some maximum distance to 0 (no weight) in order to throw out bad tile alignments which would blur the image if merged.
4. Merge the weighted neighboring image tile into the reference tile.
5. Repeat step 2-4 for all neighboring images.
6. Repeat step 1-5 for all overlapping tiles in the reference image.
7. Blend together the overlapping tiles using a raised cosine window (*Overlapped tiles* section in the paper).

With the power of your fully operational camera pipeline implementation, you should see something like this:

![Full Pipeline](http://cs348k.stanford.edu/fall18content/asst/taxi_merge_exposure_zoom_figure.png? "Full Pipeline")

__Test scenes:__
Your primary test scenes are a subset of the scenes in Part 1. Specifically: `taxi.bin`, `church.bin`, and `path.bin`. These secenes each have a burst of 3 images. For each of these scenes, we've provided a `SCENE_solution_part2.bmp` which is the output of a reference pipeline which would achieve full points for this part of the assignment, and `SCENE_google.bmp` which is the output of the full Google HDR+ pipeline (as you can see, it's just a *bit* better than our reference implementation!).

__Tips:__

* You may implement this assignment in any way you wish. We've provided a recommended sketch but feel free to improve upon our suggested algorithm.
* Implement utilities for generating Gaussian and Laplacian pyramids first as they will be used in all parts of this assignment.

## Handin ##

This assignment will be handed in using Canvas:

 * Please hand in `camera_pipeline.cpp`, `camera_pipeline.hpp`, We should be able to build and run the code on the myth machines by dropping these files into a freshly checked out starter code tree.
 * Please also include a short writeup describing the algorithm you implemented for exposure fusion and burst denoising.
