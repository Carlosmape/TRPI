#include <iostream>
#include <string.h>
#include <vector>
#include <stdio.h>

#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>

using namespace cv;
using namespace std;

struct HSVrange{
	int H_MIN;
	int H_MAX;
	int S_MIN;
	int S_MAX;
	int V_MIN;
	int V_MAX;
};

class Symbol {
	public:
		Mat img;
		string name;
};

class Trpi{
	private:
		//matrix storage for HSV image
		Mat HSV;
		//Matrix to store actual frame
		Mat camera;
		//matrix storage for binary threshold image
		Mat thresholded;
		
		VideoCapture cap;
		//Localización del objeto
		int x;
		int y;
		double areaant; //guarda el valor anterior del área de la persona, para saber si se aleja o acerca
		//max number of objects to be detected in frame
		int MAX_NUM_OBJECTS;
		//minimum and maximum object area
		int MIN_OBJECT_AREA;
		int MAX_OBJECT_AREA;
		
		HSVrange objectColor;
		Symbol symbols[7];
						
	public:
		Trpi(HSVrange colorGuia, int FRAME_WIDTH=640, int FRAME_HEIGHT=480){
			objectColor.H_MIN = colorGuia.H_MIN;
			objectColor.H_MAX = colorGuia.H_MAX;
			objectColor.S_MIN = colorGuia.S_MIN;
			objectColor.S_MAX = colorGuia.S_MAX;
			objectColor.V_MIN = colorGuia.V_MIN;
			objectColor.V_MAX = colorGuia.V_MAX;
			//localización del objeto
			x=0, y=0;
			areaant=0.0;
			
			//max number of objects to be detected in frame
			MAX_NUM_OBJECTS=50;
			//minimum and maximum object area
			MIN_OBJECT_AREA = 20*20;
			MAX_OBJECT_AREA = 2000;			//max area for the guide, if guide area is greater, PACA stops

			//abrimos la camara
			cap.open(0);
			//video capture objeto para adquirir los frames
			//Ajustamos la resolución de la cámara y habilitamos la exposicion automatica por si no la trae habilitada por defecto
			cap.set(CV_CAP_PROP_AUTO_EXPOSURE, 1);
			cap.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
			cap.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
			
			if (readRefImages(symbols) == -1) {
				printf("Error reading reference symbols\n");
			}
		}
		
		void morphOps(Mat &thresh){
			//create structuring element that will be used to "dilate" and "erode" image.
			//the element chosen here is a 3px by 3px rectangle

			Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
				//dilate with larger element so make sure object is nicely visible
			Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

			erode(thresh,thresh,erodeElement);
			erode(thresh,thresh,erodeElement);

			dilate(thresh,thresh,dilateElement);
			dilate(thresh,thresh,dilateElement);
		}
		string trackFilteredObject(int &x, int &y, Mat thresholded, Mat &cameraFeed){			
			string output= string();
			Mat temp;
			thresholded.copyTo(temp);
			//these two vectors needed for output of findContours
			vector< vector<Point> > contours;
			vector<Vec4i> hierarchy;
			//find contours of filtered image using openCV findContours function
			findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
			//use moments method to find our filtered object
			double refArea = 0.0;
			bool objectFound = false;
			int xtmp, ytmp = 0.0; /*VARIABLE TEMPORAL PARA SABER A DONDE SE MUEVE EL OBJETO*/
			if (hierarchy.size() > 0){ //si se ha encontrado algun objeto y además han pasado almenos 3milisegundos 
				int numObjects = hierarchy.size();
				//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
				if(numObjects<MAX_NUM_OBJECTS){
					for (int index = 0; index >= 0; index = hierarchy[index][0]) {
						Moments moment = moments((cv::Mat)contours[index]);
						double area = moment.m00;

						//if the area is less than 20 px by 20px then it is probably just noise
						//if the area is the same as the 3/2 of the image size, probably just a bad filter
						//we only want the object with the largest area so we safe a reference area each
						//iteration and compare it to the area in the next iteration.
						if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){ /*OBJECT FOUND*/
							xtmp = moment.m10/area;
							ytmp = moment.m01/area;
							objectFound = true;
							refArea = area;
								output+="0,"+intToString((double)(ytmp-y)/10)+","+intToString((areaant-area)/100.00)+",1,TRPI/";
							
							y=ytmp;
							x=xtmp;
							areaant = area;
						}
					}

				}else{
					output+="DEMASIADO RUIDO, AJUSTE EL FILTRO DE COLOR";
				}
			}
			return output;
		}



		void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center) {
			std::vector<cv::Point2f> top, bot;

			for (unsigned int i = 0; i < corners.size(); i++) {
				if (corners[i].y < center.y)
					top.push_back(corners[i]);
				else
					bot.push_back(corners[i]);
			}

			cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
			cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
			cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
			cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];

			corners.clear();
			corners.push_back(tl);
			corners.push_back(tr);
			corners.push_back(br);
			corners.push_back(bl);
		}

		int readRefImages(Symbol *symbols) {
			cout<<"Loading symbols ";
			symbols[0].img = imread("arrowL.jpg", CV_LOAD_IMAGE_GRAYSCALE);
			cout<<".";
			if (!symbols[0].img.data)
				return -1;
			threshold(symbols[0].img, symbols[0].img, 100, 255, 0);
			symbols[0].name = "0,-0.5,0.0,1,TRPI/";//Izquierda 90 grados = -0.5,0.0,0/

			symbols[1].img = imread("arrowT.jpg", CV_LOAD_IMAGE_GRAYSCALE);
			cout<<".";
			if (!symbols[1].img.data)
				return -1;
			threshold(symbols[1].img, symbols[1].img, 100, 255, 0);
			symbols[1].name = "0,0.0,0.0,0,TRPI/";//no funciona

			symbols[2].img = imread("arrowL45.jpg", CV_LOAD_IMAGE_GRAYSCALE);
			cout<<".";
			if (!symbols[2].img.data)
				return -1;
			threshold(symbols[2].img, symbols[2].img, 100, 255, 0);
			symbols[2].name = "0,-0.5,-0.5,1,TRPI/";//Izquierda 45 grados = -0.5,0.5,0/

			symbols[3].img = imread("arrowR45.jpg", CV_LOAD_IMAGE_GRAYSCALE);
			cout<<".";
			if (!symbols[3].img.data)
				return -1;
			threshold(symbols[3].img, symbols[3].img, 100, 255, 0);
			symbols[3].name = "0,0.5,0.5,1,TRPI/";//Derecha 45 grados = 0.5,0.5,0/

			symbols[4].img = imread("arrowStop.jpg", CV_LOAD_IMAGE_GRAYSCALE);
			cout<<".";
			if (!symbols[4].img.data)
				return -1;
			threshold(symbols[4].img, symbols[4].img, 100, 255, 0);
			symbols[4].name = "0,0.0,0.0,0,TRPI/";//STOP = 0.0,0.0,1/
			
			symbols[5].img = imread("arrowR.jpg", CV_LOAD_IMAGE_GRAYSCALE);
			cout<<".";
			if (!symbols[5].img.data)
				return -1;
			threshold(symbols[5].img, symbols[5].img, 100, 255, 0);
			symbols[5].name = "0,0.5,0.0,1,TRPI/"; //Derecha 90 grados = 0.5,0.0,0/
			cout<<" Done."<<endl;

			return 0;

		}

		string checkSymbols(Symbol * symbols,Mat &camera){	//Le pasamos los símbolos y el frame donde ha de detectarlos
			Mat new_image;	
			Mat greyImg;
			Mat canny_output;
			vector<vector<Point> > contours;
			vector<Point> approxRect;
			vector<Vec4i> hierarchy;
			
			cvtColor(camera, greyImg, CV_RGB2GRAY);

			GaussianBlur(greyImg, greyImg, Size(9, 9), 2, 2);

			/// Detect edges using canny
			Canny(greyImg, canny_output, 0, 0 * 3, 3);

			findContours(canny_output, contours, hierarchy, CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
			
			for (size_t i = 0; i < contours.size(); i++) {
				approxPolyDP(contours[i], approxRect,arcLength(Mat(contours[i]), true) * 0.05, true);
				if (approxRect.size() == 4) {
				float area = contourArea(contours[i]);

					if (area > 1000) {
						std::vector<cv::Point2f> corners;

						vector<Point>::iterator vertex;
						vertex = approxRect.begin();
						//vertex++;
						circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
						corners.push_back(*vertex);
						vertex++;
						circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
						corners.push_back(*vertex);
						vertex++;
						circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
						corners.push_back(*vertex);
						vertex++;
						circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
						corners.push_back(*vertex);

						Moments mu;
						mu = moments(contours[i], false);
						Point2f center(mu.m10 / mu.m00, mu.m01 / mu.m00);

						sortCorners(corners, center);

						// Define the destination image
						Mat correctedImg = ::Mat::zeros(195, 271, CV_8UC3);

						// Corners of the destination image
						std::vector<cv::Point2f> quad_pts;
						quad_pts.push_back(Point2f(0, 0));
						quad_pts.push_back(Point2f(correctedImg.cols, 0));
						quad_pts.push_back(Point2f(correctedImg.cols, correctedImg.rows));
						quad_pts.push_back(Point2f(0, correctedImg.rows));

						// Get transformation matrix
						Mat transmtx = getPerspectiveTransform(corners, quad_pts);

						// Apply perspective transformation
						warpPerspective(camera, correctedImg, transmtx, correctedImg.size());

						Mat correctedImgBin;

						cvtColor(correctedImg, correctedImgBin, CV_RGB2GRAY);

						//equalizeHist(correctedImgBin, correctedImgBin);

						correctedImgBin.copyTo(new_image);

						threshold(correctedImgBin, correctedImgBin, 140, 255, 0);

						double minVal,maxVal,medVal;

						minMaxLoc(new_image, &minVal, &maxVal);

						medVal=(maxVal-minVal)/2;

						threshold(new_image, new_image, medVal, 255, 0);

						Mat diffImg;

						int match, minDiff, diff;
						minDiff = 12000;
						match = -1;

						for (int i = 0; i < 6; i++) {
							//diffImg = symbols[i].img-correctedImgBin;

							bitwise_xor(new_image, symbols[i].img, diffImg,	noArray());

							diff = countNonZero(diffImg);
							if (diff < minDiff) {
								minDiff = diff;
								match = i;
							}
						}

						if (match != -1) {
							return symbols[match].name;
						}
					}
				}
			}
			return std::string();
		}

	void CannyThreshold(int, void*) {
	}

	string intToString(double number) {

		std::ostringstream ss;
		ss << number;
		return ss.str();
	}
	
	string seguirGuia(){
		while(1){
			cap >> camera;	//lee el siguiente frame
			
			//Guarda en HSV la imagen original transformada al esquema de colores HSV
			cvtColor(camera,HSV,COLOR_BGR2HSV);
			//Filtra el rango e olor en la imagen hsv y el resultado en threshold que indicará a presencia del guía
			inRange(HSV,Scalar(objectColor.H_MIN,objectColor.S_MIN,objectColor.V_MIN),Scalar(objectColor.H_MAX,objectColor.S_MAX,objectColor.V_MAX),thresholded);
			
			morphOps(thresholded);
			
			//pasamos threshold a la función para obtener su posición dentro de la imagen
			//y mostrar por pantalla la valocidad de movimiento y la dirección del guía
			string outputSymbols = checkSymbols(symbols,camera);
			string outputObject = trackFilteredObject(x,y,thresholded,camera);

			if (outputObject!=std::string() && outputSymbols!="0,0.0,0.0,0,TRPI/"){ //GUÍA a no ser que haya una señal STOP
				return outputObject;
			}else	if(outputSymbols!=std::string()){ //SI NO => SIGNO
				return outputSymbols;
			}
		}
		return "";
	}
	 
};
