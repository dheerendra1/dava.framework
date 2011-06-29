#include <iostream>
#include <Magick++.h>

#include <list>
#include <vector>
#include <sstream>
// crop result
struct CropRectBounds
{
	int minX1, minY1;
	int maxX2, maxY2;
};

struct ImageDesc
{
	Magick::Image * img;
	int width;
	int height;
};

static bool is_pixel_transparent(Magick::Image & img, int x, int y)
{
	return img.pixelColor(x,y).alphaQuantum() == 0;
}

/// Returns true with cropped image rectangle (x1,y1,x2,y2) through outBounds, if the outBounds is invalid, 
/// i.e. the image is fully transparent then it returns false. 
bool calculate_bounds(ImageDesc & imageDesc, CropRectBounds & outBounds)
{
	outBounds.minX1 = imageDesc.width;
	outBounds.minY1 = imageDesc.height;
	outBounds.maxX2 = -1;
	outBounds.maxY2 = -1;
	
	for(int y = 0; y < imageDesc.height; ++y)
		for(int x = 0; x < imageDesc.width; ++x)
		{
			if (!is_pixel_transparent(*imageDesc.img, x,y))
			{
				if (x < outBounds.minX1)
					outBounds.minX1 = x;
				
				if (y < outBounds.minY1)
					outBounds.minY1 = y;
				
				if (x > outBounds.maxX2)
					outBounds.maxX2 = x;
				
				if (y > outBounds.maxY2)
					outBounds.maxY2 = y;
			}
		} // x
	
	
	if (outBounds.maxX2 < 0)
		return false;
	
	return true;
}

#define BOOL_TO_SIGN(x) ((x)) ? (1) : (-1)

void crop_image(Magick::Image & img, int layerN)
{
//	CropRectBounds bounds;
//	ImageDesc desc;
//	
//	desc.img = &img;
//	desc.width = img.columns();
//	desc.height = img.rows();
	
	Magick::Geometry bbox = img.page();
	int xOff = (int)bbox.xOff();
	if (bbox.xNegative())
		xOff = -xOff;
	int yOff = (int)bbox.yOff();
	if (bbox.yNegative())
		yOff = -yOff;
	std::cout << "Frame Original x,y,w,h = "<<  xOff << ", "<< yOff << ", " << bbox.width() << ", " << bbox.height() << std::endl;
	std::cout << "Frame Original w,h = "<< img.columns() << ", " << img.rows() << std::endl;
//	
//	if (!calculate_bounds(desc, bounds))
//	{
//		bounds.minX1 = 0;
//		bounds.minY1 = 0;
//		bounds.maxX2 = 0;
//		bounds.maxY2 = 0;
//	}
//	
//	int ofsX = bounds.minX1;
//	int ofsY = bounds.minY1;
//	int w = bounds.maxX2 - bounds.minX1 + 1;
//	int h = bounds.maxY2 - bounds.minY1 + 1;
//	
//	std::cout << "offset = " << ofsX << ", " << ofsY << std::endl;
//	std::cout << "w,h = " << w << ", " << h << std::endl;
//	
//	//img.crop(Magick::Geometry(w,h, ofsX, ofsY));
//	
//	desc.width = img.columns();
//	desc.height = img.rows();
//	
//	std::cout << "image w,h = " << desc.width << ", " << desc.height << std::endl;
}

void process_psd(std::string imgName)
{

	std::vector<Magick::Image> layers;
	
	try {
		// Read a file into image object
		//image.read( imgName );
		Magick::readImages(&layers, imgName);
		
		
		
		// Crop the image to specified size (width, height, xOffset, yOffset)
		//image.crop( Geometry(100,100, 100, 100) );
		
		// Write the image to a file
		//image.write( "x.gif" );
	}
	catch( Magick::Exception &error_ )
    {
		std::cout << "Caught exception: " << error_.what() << std::endl;
		//return 1;
    }
	
	std::cout << "layers count = " << layers.size() << std::endl;
	
	std::string baseName = imgName.erase(imgName.rfind("."));

	int width = layers[0].columns();
	int height = layers[0].rows();
	
	for(int k = 1; k < layers.size(); ++k)
	{
		Magick::Image & currentLayer = layers[k];
		std::cout << "cropping layer " << k << std::endl;
		
		currentLayer.crop(Magick::Geometry(width,height, 0, 0));
		
		crop_image(currentLayer, k);
		std::stringstream sb;
		sb << baseName << k << ".png";
		currentLayer.write(sb.str());
	}
}


/////////////////
////////
int main (int argc, char * const argv[]) {
	Magick::InitializeMagick(argv[0]);
    // insert code here...
    std::cout << "psd_tool <src.psd>" << std::endl;
	std::cout << "Create pngs from psd layers and a txt description." << std::endl;
    
	if (argc > 1)
		process_psd(argv[1]);
	
	return 0;
}
