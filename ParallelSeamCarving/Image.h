#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <chrono>

#define TIMER_START \
    auto start_time = std::chrono::high_resolution_clock::now();

#define TIMER_END \
    auto end_time = std::chrono::high_resolution_clock::now(); \
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time); \
    std::cout << "Time taken: " << duration.count() << " milliseconds" << std::endl;

struct Pixel
{	
	Pixel() = default; 
	Pixel(uint8_t  red, uint8_t  green, uint8_t  blue, uint8_t  alpha) : R(red), G(green), B(blue), A(alpha), 
		computeR(red), computeG(green), computeB(blue) {}

	uint8_t  R;
	uint8_t  G;
	uint8_t  B;
	uint8_t  A;

	double  computeR;
	double  computeG;
	double  computeB;

	uint8_t Energy = 0; 

	// Never call this 
	void UpdatePixelValues()
	{
		R = computeR; 
		G = computeG;
		B = computeB;
	}
	
	Pixel operator +(const Pixel& other) const
	{
		return Pixel(this->computeR + other.computeR, this->computeG + other.computeG, this->computeB + other.computeB, this->A);
	}

	Pixel operator -(const Pixel& other) const
	{
		return Pixel(this->computeR - other.computeR, this->computeG - other.computeG, this->computeB - other.computeB, this->A);
	}

	Pixel operator *(const int& other) const
	{
		return Pixel(this->computeR * other, this->computeG * other, this->computeB * other, this->A);
	}
	
	Pixel operator * (const Pixel& other) const
	{
		return Pixel(this->computeR * other.computeR, this->computeG * other.computeG, this->computeB * other.computeB, this->A);
	}

};

class Image
{
public:
	Image(std::string filename, int numOfPixels);	
	~Image(); 

	/* C++ load image from file wrapper for C stlib */
	static Pixel* LoadImage(std::string filename, int& outWidth, int& outHeight, int& outChannelNum, int& outArraySize);

	/* C++ write image to file wrapper for C stlib */
	void WriteImage(std::string filename);

	/* Displays basic image information */
	void DisplayInformation(); 

	void ShowGradientImage(std::string filename);

private:
	void CalculateGradient(); 

	
	Pixel CalculateGradientX(std::vector<Pixel> pixelNeighbors);
	Pixel CalculateGradientY(std::vector<Pixel> pixelNeighbors);	

	double CalculateEnergy(Pixel& gradX, Pixel& gradY);

	std::vector<Pixel> GetPixelNeighbors(Pixel& ForPixel, int index);
	
private:
	int imgWidth = 0;
	int imgHeight = 0;
	int channelsNum = 0;
	int numOfPixels; 

	int arraySize = 0; 
	Pixel* imgArray = nullptr; 	

	const int dx[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	const int dy[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

};

