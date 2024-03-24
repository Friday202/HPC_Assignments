#include <iostream>
#include <string>
#include <filesystem>

//#include "Image.h"
#include "ImageSeamCarver.h"

/*
* input arguments are: filename to image and reduction of pixels for width only
* example: \Images\720x480.png 80
*/

int main(int argc, char* argv[])
{
	std::string absoluteImgPath;
	int numOfSeams = 0;

	try
	{
		if (argc < 3)
		{			
			throw std::runtime_error("Not enough input arguments.");
		}
		std::string relativeImgPath = argv[1];	
		numOfSeams = std::stoi(argv[2]);
		std::string stringRemove = "/Images/";

		std::cout << relativeImgPath.substr(stringRemove.length(), relativeImgPath.length()) << " " << numOfSeams << " ";
		 
		// Get the current path and form absolute path
		absoluteImgPath = std::filesystem::current_path().string() + relativeImgPath;
	}
	catch (const std::exception& e)
	{	
		std::cerr << "Caught an exception: " << e.what() << std::endl;
	}

	//Image* image = new Image(absoluteImgPath, numOfSeams);
	//image->DisplayInformation(); 	
	//image->ShowGradientImage(absoluteImgPath + "_Gradient.png");
	//image->ShowCumulativeEnergyImage(absoluteImgPath + "_CumulativeEnergy.png");	
	//image->ShowCumulativeEnergyImageImproved(absoluteImgPath + "_CumulativeEnergyImproved.png");
	//image->RemoveSeam(); 
	//image->RemoveSeamImproved();
	//image->WriteImage(absoluteImgPath + "_ImgOut.png");
	//image->WriteImageImproved(absoluteImgPath + "_ImgOutNe2w.png");
	

	ImageSeamCarver* imageSC = new ImageSeamCarver(absoluteImgPath, numOfSeams);
	imageSC->RemoveSeam(); 
	imageSC->WriteImage(absoluteImgPath + "_ImgOutNew5.png");

	// Release memory 
	delete imageSC; 

	return 0; 
}