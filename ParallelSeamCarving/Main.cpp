#include <iostream>
#include <string>
#include <filesystem>

#include "Image.h"

/*
* input arguments are: filename to image and reduction of pixels for width only
* example: \Images\720x480.png 12
*/

int main(int argc, char* argv[])
{
	std::string absoluteImgPath;
	int numOfPixels = 0; 

	try
	{
		if (argc < 3)
		{			
			throw std::runtime_error("Not enough input arguments.");
		}
		std::string relativeImgPath = argv[1];	
		numOfPixels = reinterpret_cast<int>(argv[2]); 

		// Get the current path and form absolute path
		absoluteImgPath = std::filesystem::current_path().string() + relativeImgPath;
	}
	catch (const std::exception& e)
	{	
		std::cerr << "Caught an exception: " << e.what() << std::endl;
	}

	Image* image = new Image(absoluteImgPath, numOfPixels);
	image->DisplayInformation(); 
	//image->WriteImage(absoluteImgPath + "_ImgOut.png");
	image->ShowGradientImage(absoluteImgPath + "_Gradient.png");
	image->ShowCumulativeEnergyImage(absoluteImgPath + "_CumulativeEnergy.png");

	// Release memory 
	delete image; 

	return 0; 
}