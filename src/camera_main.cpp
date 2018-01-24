#include <iostream>
#include <memory>
#include "camera_sensor.hpp"
#include "camera_pipeline.hpp"
#include "camera_pipeline_interface.hpp"
#include "common.hpp"

int main(int argc, char** argv) {

  if (argc <= 2) {
    std::cout << "usage: " << argv[0] << " scenefile outfile <options>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "   --nonoise    Disable sensor noise (for debugging)" << std::endl;
    return 1;
  }
  const std::string infile(argv[1]);
  const std::string outfile(argv[2]);
  ArgParser parser(argc - 3, argv + 3);
  
  auto camera_sensor = CameraSensor::New(infile);
  if (not camera_sensor) {
    std::cout << "Error reading sensor data from " << infile << std::endl;
    return 0;
  }

  if (parser.HasArg("--nonoise"))
      camera_sensor->SetNoiseMagnitude(0.f);
  
  camera_sensor->SetLensCap(false);
  
  // BEGIN: CS348V STUDENTS MODIFY THIS CODE 
  // You can modify the CameraPipeline class, including the constructor.

  std::unique_ptr<CameraPipelineInterface> pipeline;
  pipeline.reset(new CameraPipeline(camera_sensor.get()));
  
  // END: CS348V STUDENTS MODIFY THIS CODE 
  
  auto image = pipeline->TakePicture();
  if (not image) {
    std::cout << "Could not take picture using camera pipeline" << std::endl;
    return 1;
  }
  
  if (not image->WriteToBmp(outfile)) {
    std::cout << "Error writing image to " << outfile << std::endl;
    return 1;
  }
  
  return 0;  
}
