#include <math.h>
#include <iostream>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <time.h>
#include <opencv2/opencv.hpp>

int canvasX = 500; //CHANGE THESE!
int canvasY = 500; //CHANGE THESE!
#include "Camera.h"
#include "Tracer.h"
#include "Canvas.h"
#include "objParse.cpp"
#include "Coord.h"
#include "SaveImg.cpp"
#include "Sphere.h"
#include "Triangle.h"

using namespace cv;
using namespace std;

//************************
// GLOBAL VARIABLES
//************************
	vector<Shape*> all_shapes;
	vector<Triangle> objects; //things to push onto for obj parse
	vector<Light> lights;
	Material last_material = Material(Color(0,0,0), Color(0,0,0), Color(0,0,0), 0, Color(0,0,0)); //intialize to black so there's no garbage
	Coord camEye = Coord(0,0,2);
	Coord camLL = Coord(-1,-1,1);
	Coord camLR = Coord(1,-1,1);
	Coord camUL = Coord(-1,1,1);
	Coord camUR = Coord(1,1,1);
  Matrix transMatrix = Matrix();


// Main render loop
void render() {
	//SET UP CAVAS
	Canvas canvas = Canvas(canvasX, canvasY);
	//SET UP IMAGE
	cv::Mat img(canvasX,canvasY,CV_8UC3,Scalar(0,0,0));
 	//SET UP TRACER
	Tracer tracer = Tracer(all_shapes);       
	// //SET UP CAMERA through command line
	Camera camera = Camera(camEye, camLL, camUL, camLR, camUR, canvasX, canvasY);

	//RENDER LOOP FAST
	// 	while (canvas.getSample(&canvas.currSample)) {
	// 	Color color = Color(0,0,0);
	// 	Sample sample = canvas.currSample;
	// 	float u = (sample.x + 0.5) / canvasX ;
	// 	float v = (sample.y + 0.5) / canvasY;
	// 	Ray ray = camera.shootRay(u, v);
	// 	HitRecord hitRecord = tracer.hit(ray);
	// 	if (hitRecord.isHit) {
	// 		color = tracer.trace(hitRecord, lights, hitRecord.ray.direction);
	// 	}
	//     //clipping
	//     if (color.r > 1)
	//     	color.r = 1;
	//     if (color.g > 1)
	//     	color.g = 1; 
	//     if (color.b > 1)
	//     	color.b = 1;
	//     editPixel(&img, canvas.currSample, color); //writes to the image
	// }

	//RENDER LOOP for aliasing   
	while (canvas.getSample(&canvas.currSample)) {
		Color color = Color(0,0,0);
		Sample sample = canvas.currSample;
		int n = 3; //do 3x3 anti-aliasing
		for (int p = 0; p < n; p++) {
			for (int q = 0; q < n; q++) {
				float zetta = ((float) rand() / (RAND_MAX));
				float u = (sample.x + (p + zetta)/n) / canvasX ;
  				float v = (sample.y + (q + zetta)/n) / canvasY;
				Ray ray = camera.shootRay(u, v);
				HitRecord hitRecord = tracer.hit(ray);
				if (hitRecord.isHit) {
				    color = color + tracer.trace(hitRecord, lights, ray.direction);
				}  
			}
		}
		float scale = (float) 1/(n*n);
		color = color.scale(scale); //c = c/n^2
	    //clipping
	   if (color.r > 1)
	    	color.r = 1;
	    if (color.g > 1)
	    	color.g = 1; 
	    if (color.b > 1)
	    	color.b = 1;
	    editPixel(&img, canvas.currSample, color); //writes to the image
	}   


  saveImg(img); // Saving image to file result.png
  cv::namedWindow("RayTracer",cv::WINDOW_AUTOSIZE);
  cv::imshow("RayTracer", img);
  waitKey(-1);
  
}; 

//To put command line parsings here
void commandLine(istream& stream) {
	int argc = 0;
  string line;
  getline(stream, line); //line is the relevant data!
 	//split the line
  string buf;
  const char *l = buf.c_str();
  stringstream ss(line);
  vector<string> tokens;
  while (ss >> buf) {
    tokens.push_back(l);
  } //tokes[i] = tokens[i]

	for (int i = 0; i < tokens.size(); i++) { 
	  if (tokens[i].compare("cam") == 0) {
	    //cam ex ey ez llx lly llz lrx lry lrz ulx uly ulz urx ury urz
	    camEye = Coord(stof(tokens[i+1], NULL), stof(tokens[i+2], NULL), stof(tokens[i+3], NULL));
	    camLL = Coord(stof(tokens[i+4], NULL), stof(tokens[i+5], NULL), stof(tokens[i+6], NULL));
	    camLR = Coord(stof(tokens[i+7], NULL), stof(tokens[i+8], NULL), stof(tokens[i+9], NULL));
	    camUL = Coord(stof(tokens[i+10], NULL), stof(tokens[i+11], NULL), stof(tokens[i+12], NULL));
	    camUR = Coord(stof(tokens[i+13], NULL), stof(tokens[i+14], NULL), stof(tokens[i+15], NULL));
	    camEye = Transform::performTransform(camEye, transMatrix);
	    camLL = Transform::performTransform(camLL, transMatrix);
	    camLR = Transform::performTransform(camLR, transMatrix);
	    camUL = Transform::performTransform(camUL, transMatrix);
	    camUR = Transform::performTransform(camUR, transMatrix);
	    i += 15;
	  }
	  else if (tokens[i].compare("sph") == 0) {
	    Coord c = Coord(stof(tokens[i+1], NULL), stof(tokens[i+2], NULL), stof(tokens[i+3], NULL));
	      Sphere * sph = new Sphere(c, stof(tokens[i+4], NULL), last_material, transMatrix);
	    all_shapes.push_back(sph);
	    i += 4;
	  }
	  else if (tokens[i].compare("tri") == 0) {
	    Coord a = Coord(stof(tokens[i+1], NULL), stof(tokens[i+2], NULL), stof(tokens[i+3], NULL));
	    Coord b = Coord(stof(tokens[i+4], NULL), stof(tokens[i+5], NULL), stof(tokens[i+6], NULL));
	    Coord c = Coord(stof(tokens[i+7], NULL), stof(tokens[i+8], NULL), stof(tokens[i+9], NULL));
	    Triangle * tri = new Triangle(a, b, c, last_material, transMatrix);
	    all_shapes.push_back(tri);
	    i += 9;
	  }  
	  else if (tokens[i].compare("obj") == 0) {
	  	const char *obj = tokens[i+1].c_str();
	  	objParse(obj, &objects, &transMatrix);
	  	for (int i = 0; i < objects.size(); i++) {
	  		Triangle * objtri;
	  		if (objects[i].hasNormal) { //w defined normals
	    		objtri = new Triangle(objects[i].point1, objects[i].point2, objects[i].point3, objects[i].vn1, objects[i].vn2, objects[i].vn3, last_material, transMatrix);
	  		} 
	  		else {  
	    		objtri = new Triangle(objects[i].point1, objects[i].point2, objects[i].point3, last_material, transMatrix);
	  		}
	  		all_shapes.push_back(objtri);
	  	} 
	    i += 1; 
	  }
	  else if (tokens[i].compare("ltp") == 0) {
	  	//ltp px py pz r g b falloff
	    Coord pl = Coord(stof(tokens[i+1], NULL), stof(tokens[i+2], NULL), stof(tokens[i+3], NULL));
	    Color pl_c = Color(stof(tokens[i+4], NULL), stof(tokens[i+5], NULL), stof(tokens[i+6], NULL));
	    pl = Transform::performTransform(pl, transMatrix);
	    if (tokens[i+7].compare("0") == 0 || tokens[i+7].compare("1") == 0 || tokens[i+7].compare("2") == 0) {
	    	//if they specified falloff
	    	lights.push_back(Light(pl, pl_c, 2, stof(tokens[i+7], NULL)));
	    	i += 7;
	    }
	    else {
	    	lights.push_back(Light(pl, pl_c, 2, 0));
	    	i+= 6;
	    }
	  }
	  else if (tokens[i].compare("ltd") == 0) {
	    Coord dl = Coord(stof(tokens[i+1], NULL), stof(tokens[i+2], NULL), stof(tokens[i+3], NULL));
	    Color dl_c = Color(stof(tokens[i+4], NULL), stof(tokens[i+5], NULL), stof(tokens[i+6], NULL));
	    dl = Transform::performTransform(dl, transMatrix);
	    lights.push_back(Light(dl, dl_c, 1));
	    i += 6;
	  }
	  else if (tokens[i].compare("lta") == 0) {
	    Coord al = Coord(0,0,0); //lol
	    Color al_c = Color(stof(tokens[i+1], NULL), stof(tokens[i+2], NULL), stof(tokens[i+3], NULL));
	    al = Transform::performTransform(al, transMatrix);
	    lights.push_back(Light(al, al_c, 0));
	    i += 3;
	  }
	  else if (tokens[i].compare("mat") == 0) {
	    Color ka = Color(stof(tokens[i+1], NULL), stof(tokens[i+2], NULL), stof(tokens[i+3], NULL));
	    Color kd = Color(stof(tokens[i+4], NULL), stof(tokens[i+6], NULL), stof(tokens[i+6], NULL));
	    Color ks = Color(stof(tokens[i+7], NULL), stof(tokens[i+8], NULL), stof(tokens[i+9], NULL));
	    Color kr = Color(stof(tokens[i+11], NULL), stof(tokens[i+12], NULL), stof(tokens[i+13], NULL));
	    last_material = Material(ka, kd, ks, stof(tokens[i+10], NULL), kr);
	    i += 13;
	  }
	  else if (tokens[i].compare("xft") == 0) {
				Transform(TRANSLATION, stof(tokens[i+1], NULL),stof(tokens[i+2], NULL),stof(tokens[i+3], NULL));
	    transMatrix = Transform::calcTransMatrix();
	    i += 3;
	  }
	  else if (tokens[i].compare("xfr") == 0) {
				Transform newT = Transform(ROTATION, stof(tokens[i+1], NULL),stof(tokens[i+2], NULL),stof(tokens[i+3], NULL));
	    transMatrix = Transform::calcTransMatrix();
	    i += 3;
	  }
	  else if (tokens[i].compare("xfs") == 0) {
				Transform(SCALE, stof(tokens[i+1], NULL),stof(tokens[i+2], NULL),stof(tokens[i+3], NULL));
	    transMatrix = Transform::calcTransMatrix();
	    i += 3;
	  }
	  else if (tokens[i].compare("xfz") == 0) {
				transMatrix = Matrix();
	  }
	  else { //error handling per last pg in spec
	  	cerr << "Bad command line input: " << tokens[i] << endl;

	  }
	}
}

void readScene(char* scene) {
	ifstream sceneFile(scene);
	if (sceneFile.is_open()) {
    while (sceneFile.good()) {
      commandLine(sceneFile);
    }
	    sceneFile.close();
	}
  else {
    cout << "Please enter a valid .scene file!" << endl;
  }
}

int main (int argc, char *argv[]) {
  readScene(argv[1]);
  render();
  return 0;
}
