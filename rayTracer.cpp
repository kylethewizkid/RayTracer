#include <math.h>
#include <iostream>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <time.h>
#include "Color.h"
#include "Canvas.h"
#include "objParse.cpp"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Ray.h"
#include "CImg.h"
#include "saveImg.cpp"
#include "rayTracer.h"

Color rayTracer::traceRay(Ray ray) {

using namespace std;

//global variables
vector<Triangle> objects; //things to push onto for obj parse



//To put command line parsings here
void commandLine(int argc, char *argv[]) {
	for (int i = 1; i < argc; ++i) {
	    if(i < argc && strcmp(argv[i], "-obj") == 0) {
	    	objParse(argv[i+1], objects);
	      	i += 1;
	    }
	    else { //error handling per last pg in spec
	    	cerr << "Bad command line input" << endl;
	    }
	}
}


int main (int argc, char *argv[]) {
  commandLine(argc, argv);
  cout << "rendering..." << endl;

  // THIS IS HOW TO SET UP A CAMERA
  Coord loc = Coord(1, 1, 1);
  Coord viewDir = Coord(2, 2, 2);
  Camera cam = Camera(loc, 1, viewDir);
  cout << "\n\nCAMERA PROPERTIES\n";
  cout << cam;

  // THIS IS HOW TO SET UP A LIGHT
  Color color = Color(3,3,3);
  Light light = Light(loc, color);
  cout << "\n\nLIGHT PROPERTIES\n";
  cout << light;

  // THIS IS HOW TO SET UP MATERIAL PROPERTIES
  Color color2 = Color(4,4,4);
  Color color3 = Color(5,5,5);
  Material mp = Material(loc, color, color2, color3);
  cout << "\n\nMATERIAL PROPERTIES\n";
  cout << mp;

  cimg_library::CImg<float> img = createImg(2, 2); // Creates a 2x2 Img
  Sample sample = Sample (0,0); // Instantiating a pixel @ 0,0 (origin @ top-left)
  Sample sample2 = Sample (1, 1); // Instantiating a pixel @ 1,1 (thus bottom-right)
  Color imgCol1 = Color(255,0,255);
  Color imgCol2 = Color(0, 255, 255); 
  editPixel(&img, sample, imgCol1); // Setting 0,0 to have be magenta.
  editPixel(&img, sample2, imgCol2); // Setting 1,1 to be cyan.
  saveImg(img); // Saving image to file result.png
  img.display(); // Displaying image
  vector<Coord> vertices;
          vector<double> faces, normals, textures;


  return 0;
}
};