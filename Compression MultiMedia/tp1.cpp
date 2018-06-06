#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;


Mat image;
Mat imageGray;
Mat imageBinaire;
Mat imageChromBlue;
Mat imageChromRed;
Mat imageReconstitued;

int main( int argc, char **argv ) {
    
    if ( argc != 2 ) {
       printf( "Usage: display_image ImageToLoadAndDisplay\n" );
       return -1;
    }

    image = cv::imread( argv[1] ); /* Read the file */

    if ( !image.data ) { /* Check for invalid input */
        printf("Could not open or find the image\n") ;
        return -1;
    }

    //montre l'image initial
    cv::namedWindow( "Display window"  );
    cv::imshow( "Display window", image );
    cv::waitKey(0);

    // affiche l'image en niveau de gris
    cv::cvtColor(image, imageGray, CV_BGR2GRAY);
    cv::imshow("Display window", imageGray);
    cv::waitKey(0);

    // affiche l'image en binaire
    cv::threshold(imageGray, imageBinaire, 127, 255, CV_THRESH_BINARY);
    cv::imshow("Display window", imageBinaire);
    //permet d'afficher la valeur de chaque pixel dans la console
    //cout << "M = " << endl << " " << imageBinaire << endl << endl;
    cv::waitKey(0);

    //créer une image de 512x512 de couleur jaune uni
    Mat matModif(512, 512, CV_8UC3, Scalar(0, 255, 255));
    
    // Vec3b intensity = M.at<Vec3b>(1, 1);
    // cout << intensity << endl;
    //modifie le coin haut gauche de l'image en rouge
    for(int i=0; i<100; i++) {
        for(int j=0; j<100; j++) {
            matModif.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
        }
    }
    // cout << "M = " << endl << " " << M << endl << endl;
    cv::imshow("Display window", matModif);
    cv::waitKey(0);

    //créer des images vides
    imageGray = Mat(image.rows, image.cols, CV_8UC3);
    imageChromBlue = Mat(image.rows, image.cols, CV_8UC3);
    imageChromRed = Mat(image.rows, image.cols, CV_8UC3);
    imageReconstitued = Mat(image.rows, image.cols, CV_8UC3);
    double blue, red, green, luminance, chrominanceBleu, chrominanceRouge;
    double blueR, redR, greenR;

    //parcourir tous les pixels de l'image afin d'obtenir les luminances/chrominances
    for(int i=0; i<image.rows; i++) {
        for(int j=0; j<image.cols; j++) {

            blue = image.at<Vec3b>(i, j).val[0];
            green = image.at<Vec3b>(i, j).val[1];
            red = image.at<Vec3b>(i, j).val[2];
            // cout << "Blue : " << blue << " Green : " << green << " Red : " << red << endl;

            // ////// norme CIE XYZ
            //calcul luminance
            luminance = 0.114 * blue + 0.299 * red + 0.587 * green ;
            imageGray.at<Vec3b>(i, j).val[0] = luminance;
            imageGray.at<Vec3b>(i, j).val[1] = luminance;
            imageGray.at<Vec3b>(i, j).val[2] = luminance;
            // calcul chrominance bleu
            chrominanceBleu = 0.564*(blue-luminance) + 128;
            imageChromBlue.at<Vec3b>(i, j).val[0] = chrominanceBleu;
            imageChromBlue.at<Vec3b>(i, j).val[1] = chrominanceBleu;
            imageChromBlue.at<Vec3b>(i, j).val[2] = chrominanceBleu;
            // calcul chromiannce rouge
            chrominanceRouge = 0.713*(red-luminance) + 128;
            imageChromRed.at<Vec3b>(i, j).val[0] = chrominanceRouge;
            imageChromRed.at<Vec3b>(i, j).val[1] = chrominanceRouge;
            imageChromRed.at<Vec3b>(i, j).val[2] = chrominanceRouge;
            
            // recalcul du RGB de l'image à partir des chroms/lums
            redR = luminance + 1.403 * (chrominanceRouge-128);
            greenR = luminance - 0.714 * (chrominanceRouge-128) - 0.344 * (chrominanceBleu -128);
            blueR = luminance + 1.773 * (chrominanceBleu-128);

            imageReconstitued.at<Vec3b>(i, j).val[0] = blueR;
            imageReconstitued.at<Vec3b>(i, j).val[1] = greenR;
            imageReconstitued.at<Vec3b>(i, j).val[2] = redR;



        }
    }
    cv::imshow("Display Luminance", imageGray);
    cv::imshow("Display Chrominance Bleu", imageChromBlue);
    cv::imshow("Display Chrominance Red", imageChromRed);
    cv::imshow("Display Reconstituée", imageReconstitued);
    cv::imshow("Display Origine", image);

    cv::waitKey(0);  /* Wait for a keystroke in the window */
    cv::destroyAllWindows();

    //ne fonctionne qu'avec la dim doit etant une puissance de 2
    float dimBlocRedim = 8;
    //re-echantillonage de l'image
    //parcours les pixels afin d'avoir les valeurs moyennes des blocs de dimBlocRedim
    for(int i=0; i<image.rows; i=i+dimBlocRedim) {
        for(int j=0; j<image.cols; j=j+dimBlocRedim) {
            luminance = 0; chrominanceBleu = 0; chrominanceRouge = 0;
            blue = 0; red = 0; green = 0; 
            // cout << i << j <<endl;
            for(int k=0; k<(dimBlocRedim); k++) {
                for(int l=0; l<(dimBlocRedim); l++) {
                    blue += image.at<Vec3b>(i+k, j+l).val[0];
                    green += image.at<Vec3b>(i+k, j+l).val[1];
                    red += image.at<Vec3b>(i+k, j+l).val[2];
                }
            }
            blue = blue/(dimBlocRedim*dimBlocRedim);
            red = red/(dimBlocRedim*dimBlocRedim);
            green = green/(dimBlocRedim*dimBlocRedim);
            luminance = (0.114 * blue + 0.299 * red + 0.587 * green);
            chrominanceBleu = (0.564*(blue-luminance) + 128);
            chrominanceRouge = (0.713*(red-luminance) + 128);
            for(int k=0; k<(dimBlocRedim); k++) {
                for(int l=0; l<(dimBlocRedim); l++) {
                    redR = luminance + 1.403 * (chrominanceRouge-128);
                    greenR = luminance - 0.714 * (chrominanceRouge-128) - 0.344 * (chrominanceBleu -128);
                    blueR = luminance + 1.773 * (chrominanceBleu-128);

                    imageReconstitued.at<Vec3b>(i+k, j+l).val[0] = blueR;
                    imageReconstitued.at<Vec3b>(i+k, j+l).val[1] = greenR;
                    imageReconstitued.at<Vec3b>(i+k, j+l).val[2] = redR; 
                }
            }

        }
    }
    cv::imshow("Display Origine", image);
    cv::imshow("Display Sous-Echantilonnée", imageReconstitued);
    

    cv::waitKey(0);  /* Wait for a keystroke in the window */
    return 0;
}
