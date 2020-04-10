# Stanford CS348K Assignment 1: A Burst-Mode Camera RAW Processing Pipeline #

In this assignment you will implement a simple RAW image processing pipeline for the camera of the world's hottest smartphone, the kPhone 348. Your job is to process the data coming off the device's sensor to produce the highest quality image you can. The assignment is split into two parts.

 * In part 1, you'll process a single RAW image to demoaisc the image and correct for sensor defects.
 * In part 2, you'll extend your implementation to process a burst of images from a scene featuring a wide range of intensities.  You'll align and merge the images of the burst to reduce noise, and then perform local tone mapping to produce a compelling HDR result.  Part 2 involves more implementation effort than part 1.

## Getting Started ##

Grab the assignment starter code.

    git clone git@github.com:stanford-cs348k/camera_asst.git

To run the assignment, you will also need to download the scene datasets, located at <http://cs348k.stanford.edu/spring20content/asst/asst1scenes.tgz>.

For example:

    wget http://cs348k.stanford.edu/fall20content/asst/asst1scenes.tgz
    tar -xvf asst1scenes.tgz

This is a large 640MB download since it contains burst sequences for several high-resolution captures. If you seek to work on Myth (and not a personal machine), you are going to need a larger AFS quota.  Please let the staff know if you require this, and we'll help you make the request to Stanford IT.

__Build Instructions__

The codebase uses a simple `Makefile` as the build system. To build the starter code, run `make` from the top level directory. The assignment source code is in `src/`, and object files and binaries will be generated into `build/` and `bin/` respectively.

You have the option of implementing this assignment either in C++ or in [Halide](https://halide-lang.org/). Halide is a domain-specific language for image processing embedded in C++. (Note: major parts of the real HDR+ pipeline at Google are implemented in Halide.)
We have provided stubs of the pipeline for both vanilla C++ and Halide. Students who are interested
in getting the highest performance possible will probably be better off using Halide, since it
allows you to quickly experiment with many different loop schedules for a program.  Please see the [Halide site](https://halide-lang.org/) for [tutorials about the language](https://halide-lang.org/tutorials/tutorial_introduction.html), and this [Youtube video](https://www.youtube.com/watch?v=3uiEyEKji0M).

__Using Halide__

To use Halide you will need to build and install it from the link here: <https://github.com/halide/Halide>. Halide requires LLVM 8.0 or greater and a c++17 compiler. Once Halide is compiled set the environment variable `USE_HALIDE=1` and set environment variable `HALIDE_PATH` to point to the Halide source code directory on your machine.
We have provided some helper functions to load and store data from Halide buffers in the
file `halide_utils.hpp`.

__Running the starter code:__

Now you can run the camera. Just run:

    ./bin/kcamera MY_SCENES_DIR/taxi.bin output.bmp

(where `MY_SCENES_DIR` is where you extracted the scene datasets into) The camera will "take a picture" and output the result of processing the RAW sensor data from the sensor to `output.bmp`. The starter code just copies the sensor data verbatim into the red, green, and blue channels of the output image. (so the output is just a visualization of the RAW data from the sensor).  So for a scene that looks like the image at left, you should see output that looks a bit like this.

 ![RAW Example](http://cs348k.stanford.edu/fall18content/asst/taxi_figure.jpg "RAW Data visualization")

# Part 1 (30 points): Basic Camera RAW Pipeline ##

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

The driver code for this assignment (containing `main()`) is located in `camera_main.cpp`. Students who choose to use Halide for this assignment will find some Halide helper functions for loading and storing data in `./src/halide_utils.hpp`.

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

# Part 2 (70 points): Local Tone Mapping + Burst Mode Alignment for Denoising #

When implementing your solution to the first part of this assignment, you might have noticed visual artifacts in your output. Consider the `taxi.bin` image:

![Noise visualization](http://cs348k.stanford.edu/fall18content/asst/taxi_noise_figure.png? "Noise visualization")

There are at least two issues you might notice here:
1. *Under-exposure:* The scene exhibits high dynamic range, so to avoid over-exposing the bright sunset in the background, the image has been deliberately under-exposed. As a result, regions of the the image which don't receive as much illumination (e.g. the front parts of the taxis) are very dark. 
2. *Noise:* Because the image was under-exposed, the effects of sensor noise are much more noticeable. In particular, the zoomed in region from the figure above shows how dominant the noise can become in dark regions.

In this part of the assignment, you'll address high dynamic range using a local tone mapping algorithm called **exposure fusion**. Then you will reduce noise in the tone mapped output by **aligning and merging** a sequence of underexposed shots as discussed in [Burst Photography for High Dynamic Range and Low-light Imaging on Mobile Cameras](https://ai.google/research/pubs/pub45586).

## Local Tone Mapping via Exposure Fusion

Tone mapping converts a high dynamic range image (with greater than 8 bits of information per channel) to a low dynamic range image (e.g., 8 bits per channel) that can be viewed on a low-dynamic range display.  In a local tone mapping algorithm, different parts of the image are exposed differently so that detail is retained in both very bright and dark regions.   

In this assignment, we'd like you to implement a modified version of [Exposure Fusion](http://ntp-0.cs.ucl.ac.uk/staff/j.kautz/publications/exposure_fusion.pdf) as described by Mertens et al. The key idea of exposure fusion is that, while it is difficult to capture a single image where all parts of the image are well-exposed, it's possible to capture multiple exposures of the same scene and then combine the well-exposed parts of each of these images to create a satisfying high dynamic range photo.

Recall that the pixel data you receive from the sensor via `GetSensorData()` is represented as a 32-bit floating point value between 0 and 1. (Even though the mantissa of a single-precision float number is 23 bits, the data is from Google HDR+'s dataset, acquired via a Pixel phone, so the actual precision of these values is about 10 bits.)  Rather than take multiple exposures with the camera as described in the paper, you'll first *virtually* create two 8-bit exposures from the high-precision input.  

Your specific solution is allowed to differ (see further detail in the "Dynamic Range Compression" part of Section 6 of the HDR+ paper for heuristics), but one basic approach would create the following two virtual exposures after processing the data with your pipeline from part 1 of the assignment (but before conversion to 8-bit values): 

 * *dark*, which is a grayscale version of the RGB image after basic RAW processing.  (RGB to YUV conversion to get grayscale)
 * *bright* is formed by multiplying dark by a scale factor.

The dark image will retain detail in the areas of the image with a lot of light (since it is largely under-exposed), while the bright image will retain detail in the areas with little light (since the digital gain applied by the scale factor brightens dark parts of the image).

Exposure fusion then computes a per-pixel weight that selects between the bright and dark images.  You can either see 
Section 3.1 of Mertens et al. for example heuristics, or the much simpler version described in the "Dynamic Range Compression" part of Section 6 of the HDR+ paper.  The *Laplacian pyramids* of the dark and bright images (not initial image pixels) are blended together according to this weight. Last, the resulting Laplacian pyramid is flatted to get a merged grayscale image.

This modified grayscale image is then combined with the UV channels of the original pre-tone mapped RGB image to get a modified result.  

In summary, here's a sketch of the modified exposure fusion algorithm (you should read over the original paper to understand each step in more detail):
1. Convert output image from Part 1 of the assignment into grayscale
2. Create the two artifical exposure brackets from the grayscale images: dark and bright
3. Using a weighting function of your choice (section 3.1 in the paper), compute weights for both images
4. Compute a Laplacian pyramid of both images and a Gaussian pyramid of the weights
5. Blend the Laplacian pyramids together using the Gaussian pyramid of weights (section 3.2 in the paper)
6. Extract the exposure fused image from the blended pyramid (flatten the pyramid) and use this as the Y channel of the final output image.

For example, here's our reference pipeline's dark and bright images with their corresponding weights and the final output:
![Exposure Fusion](http://cs348k.stanford.edu/fall18content/asst/taxi_exposure_fusion_figure.png? "Exposure Fusion")

White in the weight images represent a high value, and black represents a low value. As you can see, the weights in the dark image select the well-exposed sky in the background while the bright image selects the brightened taxis in the foreground. 

__Note__: Be careful about whether you perform local tone mapping operations in linear intensity space (on luminance) or in a non-linear perceptual space (luma). The role of local tone mapping is to mimic how a human would perceive a scene if they were there in person. If we are using heuristics to select the exposure of different parts of the scene based on what we think would look good to a human, does it make more sense to apply these heuristics on luminance or luma values? How do you convert from luminance to luma?  How does one convert luma back to luminance?

This algorithm makes the image look much brighter in the dark regions without blowing out the already bright regions. But what about the noise? let's zoom back into that dark region we were looking at before:
![Tone Mapped Zoom](http://cs348k.stanford.edu/fall18content/asst/taxi_exposure_fusion_zoom_figure.png? "Exposure Fusion Zoom")

Notice that while this algorithm produces a result where there is detail in all regions, by boosting the dark regions (which are already prone to sensor noise), we have accentuated noise artifacts. Fortunately, burst mode alignment from the HDR+ paper solves this problem and is the next sub-part of this assignment.

## Reducing Noise by Aligning an Image Burst

In this sub-part of the assignment, you will write code to align and merge a burst of (potentially noisy) sensor captures to produce a less noisy output image. The entry point to your code is the same as in the previous parts, but instead of calling `sensor_->GetSensorData()`, you should call `sensor_->GetBurstSensorData()`. This method reads a burst of RAW data from the sensor and returns it as a `std::vector` of bayered images. Your job is to implement a simplified version of the alignment and merging steps from the HDR+ paper to produce a denoised bayer image that can be processed by your existing camera pipeline code. __Note: the align/merge algorithm is used to produce a new (higher bit depth) pre-demosaiced RAW image that should then be passes through the rest of your RAW processing pipeline (including local tone mapping).

To illustrate why aligning the burst is necessary, consider the output of simply summing each capture in burst, as shown below:

![Average Denoising](http://cs348k.stanford.edu/fall18content/asst/taxi_averaging_figure.png#1 "Average Denoising")

The resulting image is certainly less noisy, since summation increases signal to noise ratio in dark regions.  However, the result is also now blurry because the input images were captured at different points in time. This is the motivation for the HDR+ image *alignment* and *merging* steps. Here is a sketch of a simple implementation, though feel free to make modifications or enhancements to this algorithm that you think can produce a better result:

__Alignment (Section 4 in the HDR+ paper):__
The following is a suggestion for how to implement the alignment step:
1. Convert the stack of raw bayer images from `GetBurstSensorData()` to grayscale by averaging together every 2x2 bayer grid (effectively downsampling by 2x).
2. Compute Gaussian pyramids for each of these grayscale images (You should be able to use your code from the earlier exposure fusion part of the assignment.).
3. For each of the images in the burst, perform a hierarchical alignment to the reference image (the first image in the burst) by following steps 4-6.
4. For each level of the Gaussian pyramid, starting at the coarsest:
5. For each tile of the reference image at this level, find the closest matching tile in the image that is being matched against (we use the absolute difference between the tiles as a measure of distance)
6. Upsample the offsets to the next level and repeat step 5 using the upsampled offsets as starting points for the next search

The paper mentions many other additional steps: subpixel alignment using an L2 metric, a robust upsampling strategy for the alignment fields, varying search radii, Fourier transforms for fast matching, etc. These can certainly improve your alignment, and we encourage interested students to attempt to implement some of these more advanced techniques, but they are not necessary to achieve a reasonable output image for this assignment.

__Merging (Section 5 in the HDR+ paper):__

The merging algorithm in the HDR+ paper uses an advanced noise model and operates in the frequency domain. We encourage you to implement the merging step in whichever way you choose, but below we provide a sketch of a basic implementation which will produce decent results:
1. For overlapping tiles in the reference image (Our reference uses tiles of size 16 with stride 8): 
2. For each neighboring image, use the alignment offset to find the tile to merge.
3. Compute a merging weight for the tile from step 2 by comparing that tile to the reference image tile. In our implementation, we use the distance metric from the alignment step to compute an initial weight and then clamp all values below some minimum to 1 (full weight) and all values above some maximum distance to 0 (no weight) in order to throw out bad tile alignments which would blur the image if merged.
4. Merge the weighted neighboring image tile into the reference tile.
5. Repeat step 2-4 for all neighboring images.
6. Repeat step 1-5 for all overlapping tiles in the reference image.
7. Blend together the overlapping tiles using a raised cosine window (*Overlapped tiles* section in the paper).

Now that your implementation can align/merge a burst of RAW images, and then apply exposure fusion to the result, you should obtain a result that looks something like this (obviously results will vary based on algorithms used):

![Full Pipeline](http://cs348k.stanford.edu/fall18content/asst/taxi_merge_exposure_zoom_figure.png? "Full Pipeline")

__Test scenes:__
Your primary test scenes are a subset of the scenes in Part 1. Specifically: `taxi.bin`, `church.bin`, and `path.bin`. These scenes each have a burst of 3 images. For each of these scenes, we've provided a `SCENE_solution_part2.bmp` which is the output of a reference pipeline which would achieve full points for this part of the assignment, and `SCENE_google.bmp` which is the output of the full Google HDR+ pipeline (as you can see, it's just a *bit* better than our reference implementation!).

__Tips:__

* You may implement this assignment in any way you wish. We've provided a recommended sketch of the basic algorithms, but feel free to improve upon our suggested algorithm.  (The algorithms described in the reference readings are certainly more advanced than the baseline approaches described here.)
* Implement utility functions for generating Gaussian and Laplacian pyramids first, as they will be used in all parts of this assignment.

## Grading ##

The assignment will be graded on image quality. Your implementation should contain an approach for demosaicing, correcting sensor defects / noise, aligning/merging images in a burst, and a valid implementation of exposure fusion.  You may adjust/improve algorithms however you seek.  We encourage you to start with simple algorithms, get them to work, and then if there is time, attempt to improve image quality to move to more advanced techniques.  

## Handin ##

This assignment will be handed in using Canvas:

 * Please hand in `camera_pipeline.cpp`, `camera_pipeline.hpp`, We should be able to build and run the code on the myth machines by dropping these files into a freshly checked out starter code tree.
 * Please also include a short writeup describing the algorithm you implemented for exposure fusion and burst denoising.
