#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/imgproc.hpp"
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
#include <dirent.h>
#include <vector>
#include <string>

using namespace cv;
using namespace std;


// Variables :
const int scale_max = 5;
int ratioCanny=3;
int alpha_slider;
int delta=255;
int scaleSobel=0;

int scaleLaplace=0;
int lowThreshold=0;
int const max_lowThreshold = 100;
int edgeThresh = 1;
int seuilSobel=127;
int seuilLaplace=127;

Mat image;
Mat imageRef;

// Prototypes des fonctions :
int contours_detect(Mat image);
int contours_corrects(Mat imageRef, Mat image);
void compare(Mat image, Mat imageRef, std::vector<float> &res);
Mat seuil_Img_Binaire(Mat img, int seuil);
Mat negative_image(Mat img);
Mat sobel_image(Mat image);
Mat laplace_image(Mat image);
Mat canny_image(Mat image);
void launchAlgo(Mat image);
static void on_trackbarSobel( int, void*);
static void on_trackbarLaplace( int, void*);
static void on_trackbarCanny( int, void*);
void on_trackbar( int, void* );
void create_trackbar();
void sobelOptim(Mat image,Mat imageRef, vector<float> &res);
void laplaceOptim(Mat image,Mat imageRef, vector<float> &res);
void cannyOptim(Mat image, Mat imageRef, vector<float> &res);
int recupererListeFichiers(std::string repertoire, std::vector<std::string> &fichiers);


int main( int argc, char **argv ) {

    vector<string> vect;
    vector<string> vectRef;
    FILE* file;
    file=fopen("resultat.txt","w+");
    recupererListeFichiers("images",vect);
    recupererListeFichiers("images/gt",vectRef);

    
    for (int i=0; i<vect.size(); i++) {
        if(!(vect[i].find("pgm")!=string::npos)){
            vect.erase(vect.begin()+i);
        }
    }
    vect.erase(vect.begin());
    
//    cv::namedWindow( "Display window",1 ); /* Create a window for display */
//    cv::namedWindow( "Sobel window",1 ); /* Create a window for display */
//    cv::namedWindow( "Laplace window",1 ); /* Create a window for display */
//    cv::namedWindow( "Canny window",1 ); /* Create a window for display */

//  fonction pour faire apparaitre les trackbars
//  create_trackbar();
    fprintf(file,";Sobel Performance; Sobel Taux FN;Sobel Taux FP; Laplace Performance; Laplace Taux FN;Laplace Taux FP; Canny Performance; Canny Taux FN;Canny Taux FP;\n");
    
    // on parcours toutes les images
    for (int i=0; i<vect.size(); i++) {
        vector<float> tampon;
        
        image = cv::imread( "images/"+vect[i], IMREAD_COLOR);
        imageRef = cv::imread( "images/gt/"+vect[i].substr(0, vect[i].size()-4)+"_gt_binary.pgm", IMREAD_GRAYSCALE); /* Read the file */
        
        if(!imageRef.data || !image.data){
            cout<<"error"<<endl;
            return -1;
        }
        cout<<"Image terminée : " << i<< endl;

        //on effectue un blur
        GaussianBlur( image, image, Size(3,3), 0, 0, BORDER_DEFAULT );
        // imshow("Display window",image);
        fprintf(file,"%s ;",vect[i].c_str());
        //calcul du meilleur sobel
        cout<<"Sobel en cours"<<endl;
        sobelOptim(image,imageRef,tampon);

        //on écrit dans le fichier les résultats
        for (int j=0; j<tampon.size(); j++) {
            fprintf(file,"%f    ;",tampon[j]);
        }
        
        //calcul du meilleur Laplace
        cout<<"Laplace en cours"<<endl;
        laplaceOptim(image,imageRef,tampon);
        for (int j=0; j<tampon.size(); j++) {
            fprintf(file,"%f    ;",tampon[j]);
        }

        //calcul du meilleur canny
        cout<<"Canny en cours"<<endl;    
        cannyOptim(image,imageRef,tampon);
        for (int j=0; j<tampon.size(); j++) {
            fprintf(file,"%f    ;",tampon[j]);
        }
        fprintf(file,"\n");
              
    }


    cv::waitKey(0);  /* Wait for a keystroke in the window */
    fclose(file);
    return 0;
}


// détection des contours de l'image
int contours_detect(Mat image) {

    int res=0;
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (image.at<uchar>(i, j)==0) {
                res++;
            }
        }
    }
    return res;
}

// détection des contours corrects de l'image
int contours_corrects(Mat imageRef, Mat image) {

    int res=0;
    bool haveRes=false;

    for (int i = 1; i < imageRef.rows-1; i++) {
        for (int j = 1; j < imageRef.cols-1; j++) {
            haveRes=false;
            if (imageRef.at<uchar>(i,j) == 0) {
                for(int k=-1; k<=1; k++) {
                    for(int l=-1; l<=1; l++) {
                        if(!haveRes && image.at<uchar>(i+k,j+l) == 0) {
                            res++;
                            haveRes=true;
                        }
                    }
                }
            }
        }
    }
    return res;
}


// calcul des différentes valeurs 
void compare(Mat image, Mat imageRef, std::vector<float> &res){
    
    float contDet=contours_detect(image);
    float contRed=contours_detect(imageRef);
    float contCor=contours_corrects(imageRef,image);
    float fauxPos=contDet-contCor;
    float fauxNeg=contRed-contCor;
    float performance=contCor/(contCor+fauxNeg+fauxPos);
    float tauxFP=fauxPos/(contCor+fauxNeg+fauxPos);
    float tauxFN=fauxNeg/(contCor+fauxNeg+fauxPos);

    res.clear();

    res.push_back(performance);
    res.push_back(tauxFN);
    res.push_back(tauxFP);
    
}


Mat seuil_Img_Binaire(Mat img, int seuil) {

    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            if (img.at<uchar>(i, j)<seuil) {
                img.at<uchar>(i, j)=0;
            } else {
                img.at<uchar>(i, j)=255;
            }
        }
    }

    return img;
}

Mat negative_image(Mat img) {

    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            img.at<uchar>(i, j) = 255 - img.at<uchar>(i, j);
        }
    }

    return img;
}


// algorithme de sobel
Mat sobel_image(Mat image) {

    Mat grad, newimage;
    int ddepth = CV_16S;
    
    if(image.empty()) {
        return image;
    }
    cvtColor( image, newimage, COLOR_BGR2GRAY );
    
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
    
    Sobel( newimage, grad_x, ddepth, 1, 0, 3, scaleSobel, delta, BORDER_DEFAULT );
    Sobel( newimage, grad_y, ddepth, 0, 1, 3, scaleSobel, delta, BORDER_DEFAULT );
    
    convertScaleAbs( grad_x, abs_grad_x );
    convertScaleAbs( grad_y, abs_grad_y );
    
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
  
    grad = seuil_Img_Binaire(grad, seuilSobel);
    return grad;
}

// algorithme de laplace
Mat laplace_image(Mat image) {

    Mat dst, newimage;
    int kernel_size = 3;
    int ddepth = CV_16S;   

    if( image.empty() ) { 
        return image; 
    }  
    
    cvtColor( image, newimage, COLOR_BGR2GRAY ); // Convert the image to grayscale
    Mat abs_dst;
    Laplacian( newimage, dst, ddepth, kernel_size, scaleLaplace, delta, BORDER_DEFAULT );
    
    convertScaleAbs( dst, abs_dst );
    abs_dst = seuil_Img_Binaire(abs_dst, seuilLaplace);

    return abs_dst;    
}


// algorithme de canny 
Mat canny_image(Mat image) {

    Mat newimage, dst;
    
    int kernel_size = 3;
    
    cvtColor( image, newimage, COLOR_BGR2GRAY );    
    Canny( newimage, newimage, lowThreshold, lowThreshold*ratioCanny, kernel_size );
    
    newimage = negative_image(newimage);
      
    return newimage;
}


// lance les différents algos
void launchAlgo(Mat image) {

    sobel_image(image);
    laplace_image(image);
    canny_image(image);
    
}


void on_trackbar( int, void* ) {
    launchAlgo(image);
}


static void on_trackbarSobel( int, void*) {
    sobel_image(image);
}


static void on_trackbarLaplace( int, void*) {
    laplace_image(image);
}


static void on_trackbarCanny( int, void*) {
    canny_image(image);
}

// créer les tracks bars
void create_trackbar() {

    createTrackbar( "Scale Sobel", "Display Sobel image", &scaleSobel, scale_max, on_trackbarSobel);
    on_trackbarSobel( scaleSobel, 0);
    
    createTrackbar( "Scale Laplace", "Display Laplace image", &scaleLaplace, 255, on_trackbarLaplace);
    on_trackbarLaplace( scaleLaplace, 0);

    createTrackbar( "Seuil Sobel", "Display Sobel image", &seuilSobel, 255, on_trackbarSobel);
    createTrackbar( "Seuil Laplace", "Display Laplace image", &seuilLaplace, 255, on_trackbarLaplace);

    createTrackbar( "lowThreshold Canny", "Display Canny image", &lowThreshold, 20, on_trackbarCanny);
    createTrackbar( "ratio Canny", "Display Canny image", &ratioCanny, 100, on_trackbarCanny);
    on_trackbarCanny( ratioCanny, 0);
    
    cv::waitKey(0);
}

// calcul du meilleur sobel
void sobelOptim(Mat image,Mat imageRef, vector<float> &res) {
    
    int bestScale=0;
    int bestSeuil=0;
    float bestPerf=0;
    vector<float> tampon;
    
    for (int i=0; i<=scale_max; i++) {
        for (int j=0; j<=255; j++) {
            scaleSobel=i;
            seuilSobel=j;
            compare(sobel_image(image),imageRef,tampon);
            if(tampon[0]>bestPerf){
                bestPerf=tampon[0];
                bestScale=i;
                bestSeuil=j;
            }
        }
    }
//    cout<<"bestscale="<<bestScale<<endl;
//     cout<<"bestseuil="<<bestSeuil<<endl;
    scaleSobel=bestScale;
    seuilSobel=bestSeuil;
    compare(sobel_image(image),imageRef,res);
}

// calcul du meilleur laplace
void laplaceOptim(Mat image,Mat imageRef, vector<float> &res) {
    
    int bestScale=0;
    int bestSeuil=0;
    float bestPerf=0;
    vector<float> tampon;
    
    for (int i=0; i<=scale_max; i++) {
        for (int j=0; j<=255; j++) {
            scaleLaplace=i;
            seuilLaplace=j;
            compare(laplace_image(image),imageRef,tampon);
            if(tampon[0]>bestPerf){
                bestPerf=tampon[0];
                bestScale=i;
                bestSeuil=j;
            }
        }
    }
//    cout<<"bestscale="<<bestScale<<endl;
//    cout<<"bestseuil="<<bestSeuil<<endl;
    scaleLaplace=bestScale;
    seuilLaplace=bestSeuil;
    compare(laplace_image(image),imageRef,res);
}

// calcul du meilleur canny
void cannyOptim(Mat image, Mat imageRef, vector<float> &res) {
    
    int bestlowThreshold=0;
    int bestRatioCanny=0;
    float bestPerf=0;
    vector<float> tampon;
    
    for (int i=0; i<=max_lowThreshold; i++) {
        for (int j=0; j<=5; j++) {
            lowThreshold=i;
            ratioCanny=j;
     
            compare(canny_image(image),imageRef,tampon);

            if(tampon[0]>bestPerf){
                bestPerf=tampon[0];
                bestlowThreshold=i;
                bestRatioCanny=j;
            }
        }
    }
//    cout<<"bestlowThreshold="<<bestlowThreshold<<endl;
//    cout<<"bestRatioCanny="<<bestRatioCanny<<endl;
    lowThreshold=bestlowThreshold;
    ratioCanny=bestRatioCanny;
    compare(canny_image(image),imageRef,res);
}

// recupère la liste des fichiers
int recupererListeFichiers(std::string repertoire, std::vector<std::string> &fichiers) {
    DIR *dp;
    struct dirent *dirp;
    
    if((dp  = opendir(repertoire.c_str())) == NULL) {
        std::cout << "Erreur à l'ouverture de " << repertoire << std::endl;
        return -1;
    }
    
    while ((dirp = readdir(dp)) != NULL) {
        fichiers.push_back(std:: string(dirp->d_name));
    }
    closedir(dp);
    return 0;
    
}

