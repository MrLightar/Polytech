#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <time.h>
#include <math.h>

// ----- IMPORTANT -----
// Pour compiler et utiliser ce code, il faut installer les librairies OpenCV

using namespace cv;
using namespace std;

// ----- Prototypes des fonctions -----
void createBinaryImage();
void createCorreledImage();
Mat convertFileToImage(string name, Mat img);
void createBinaryImage();
void writeRLE();
void decompressRLE();
int get_file_size(std::string filename);
string toBinary(int n);
long toDecimal(string val);

// ----- Variables globales -----
#define blocSize 4          //taile du bloc
Mat image, imageReconst;    // images qui seront affichées
int imageDim = 512;         // dimensions des images
int probUnApresUn = 90;     // probabilité qu'après un 1 se trouve un autre 1
int probZeroApresZero = 90; // probabilité qu'après un 0 se trouve un autre 0


int main(void) {

    
    // createBinaryImage();
     
    createCorreledImage();

    image = convertFileToImage("image.txt", image);

    //affichage de l'image généré de façon corélée
    cv::namedWindow("Display window" );
    cv::imshow("Display window", image );
    //enregistre l'image
    cv::imwrite( "image_result.pgm", image );
    cv::waitKey(0);

    writeRLE();
    decompressRLE();

    //Calcul le taux de compression avec le RLE en fonction des tailles des fichiers
    float tauxCompression, filesize1, filesize2;
    filesize1 = get_file_size("imageRLE.txt");
    filesize2 = get_file_size("image.txt");
    tauxCompression = ((float) (filesize1 / (float) filesize2));
    cout << "Taille image initiale : " << filesize2 << "   | Taille après RLE : " << filesize1 << endl;
    cout << "Taux de Compression RLE : " << tauxCompression << "       Corrélation : " << probUnApresUn << endl;
    
    //affiche l'image reconstituée suite à la décompression du RLE
    imageReconst = convertFileToImage("imageDecompress.txt", imageReconst);
    cv::imshow("Display window", image );
    cv::imshow("Display window Reconstituée", imageReconst );
    
    cv::waitKey(0);

    // for()

    return 0;
}

// Créer une image Binaire dont les zéros et uns sont répartis de manièe non-corrélées
// Le résultat est sauvegardé dans un fichier texte 
void createBinaryImage() {

    ofstream myfile;

    srand(time(NULL));
    myfile.open ("image.txt");
    for(int i = 0; i < imageDim; i++) {
        for(int j = 0; j<imageDim; j++) {
            if((rand()%100) >= 90) {
                myfile << 1;
            } else {
                myfile << 0;
            }
        }
    }
    myfile.close();
}


// Créer une image corrélée
// Le résultat est sauvegardé dans un fichier texte
void createCorreledImage() {

    ofstream myfile;
    bool firstBit = true;
    int precBit;

    srand(time(NULL));
    myfile.open ("image.txt");

    for(int i = 0; i < imageDim; i++) {
        for(int j = 0; j<imageDim; j++) {
            if(firstBit) { // cas du premier bit dont les prob de 0 ou 1 sont équiprobables
                if((rand()%100+1) > 50) {
                    myfile << 1;
                    precBit = 1;
                } else {
                    myfile << 0;
                    precBit = 0;
                }
                firstBit = false;
            } else {
                if(precBit == 1) {
                    //probabilité d'avoir un 1 après en avoir eu 1
                    if((rand()%100+1) <= probUnApresUn) {
                        myfile << 1;
                    } else {
                        myfile << 0;
                        precBit = 0;
                    }
                } else {
                    //probabilité d'avoir un 0 après en avoir eu 0
                    if((rand()%100+1) <= probZeroApresZero) {
                        myfile << 0;
                    } else {
                        myfile << 1;
                        precBit = 1;
                    }
                }
            }
            
        }
    }
    myfile.close();

}

// Récupère un fichier txt d'une image qui a été créer par une des fonctions
// name : nom du fichier devant être converti en image
// img : image sur laquellle la fonction travaillera
// return : image depuis le fichier
Mat convertFileToImage(string name, Mat img) {
    
    const char * nom = name.c_str();
    // ifstream myfile("image.txt", ios::in);
    ifstream myfile(nom, ios::in);
    img = Mat(imageDim, imageDim, CV_8UC1, Scalar(255));

    if(myfile) {
        char caractere;
        int i = 0;
        int j = 0, k = 0;
        while(myfile.get(caractere)) {
            i++;
            k = i%imageDim; //récupère la colonne de l'image
            if(caractere == '0') {
                img.at<uchar>(j, k) = 0;
            } else {
                img.at<uchar>(j, k) = 255;
            }            
            if(k==(imageDim-1)) {
                j++;
            }
        }
        myfile.close();
    } else {
        cout << "Erreur à l'ouverture du fichier" << endl;
    }

    return img;

    
}


// Utilise la compression RLE avec codage blocs pour bits corrélés.
// La taille des blocs est définissable préalablement dans le code
void writeRLE() {

    ifstream myfile("image.txt", ios::in);
    ofstream myfileRLE;
    myfileRLE.open ("imageRLE.txt");

    if(myfile) {
        char precCarac;
        char caractere;
        int nbOccur = 0;
        // ecrit le 1er bit du fichier pour le RLE
        myfile.get(caractere);
        myfileRLE << caractere;
        precCarac = caractere;
        nbOccur++;

        // tant que le fichier contient un caractere
        while(myfile.get(caractere)) {
            if((caractere == precCarac)) { //si caractere similaire au precedent augmenter l'occurrence
                nbOccur++;
            } else {
                // ecriture sous format RLE
                if(precCarac == '0' ||  precCarac == '1') {
                    if(nbOccur >= (pow(2,blocSize)-1)) { //gère si le bloc n'est pas assez grand
                        while(nbOccur >= (pow(2,blocSize)-1)) {
                            myfileRLE << toBinary((pow(2,blocSize)-1));
                            nbOccur -= (pow(2,blocSize)-2);
                        }
                    }
                    myfileRLE << toBinary(nbOccur); //ecrit dans le fichier
                    nbOccur = 1;
                }                
            }
            precCarac = caractere;
        }

        //Répétition du code pour ajouter les derniers bits
        if(precCarac == '0' ||  precCarac == '1') {
            if(nbOccur >= (pow(2,blocSize)-1)) {
                while(nbOccur >= (pow(2,blocSize)-1)) {
                    myfileRLE << toBinary((pow(2,blocSize)-1));
                    nbOccur -= (pow(2,blocSize)-2);
                }
            }
            myfileRLE << toBinary(nbOccur);
            nbOccur = 1;
        } 
        //fin de la répétition
        myfile.close();
    } else {
        cout << "Erreur à l'ouverture du fichier" << endl;
    }
}


// Permet d'obtenir la taille d'un fichier
// filename : nom du fichier
// return : la taille du fichier en int
int get_file_size(std::string filename) {
    FILE *p_file = NULL;

    p_file = fopen(filename.c_str(),"rb");
    fseek(p_file,0,SEEK_END);
    int size = ftell(p_file);
    fclose(p_file);

    return size;
}


// converti une valeur pour obtenir son écriture en binaire en fonction de la blocsize
// ex : si blosize = 8, on aura pour 7 : 00000111
// n : valeur à convertir
// return : une chaine de caractère correspondant à la valeur en binaire 
string toBinary(int n) {

    string binary = std::bitset<blocSize>(n).to_string(); //to binary

    return binary;
}


// converti une écriture binaire pour obtenir son écriture en décimal en fonction de la blocsize
// ex : si blosize = 8, on aura pour 00000111 : 7
// val : valeur binaire à convertir
// return : un long correspondant à la valeur décimal du binaire 
long toDecimal(string val) {

    unsigned long decimal = std::bitset<blocSize>(val).to_ulong();

    return decimal;
}


// Effectue la décompression du fichier RLE créer à partir du codage précédent
// La taille des blocs est définissable préalablement dans le code
void decompressRLE() {
    
    ifstream myfile("imageRLE.txt", ios::in);
    ofstream myfileDecompress;
    myfileDecompress.open ("imageDecompress.txt");
    
    if(myfile) {
        char writableCarac, caractere;
        string bloc;
        int i = 0;
        myfile.get(writableCarac);

        // tant que le fichier contient un caractere
        while(myfile.get(caractere)) {
            i++;
            bloc += caractere; //ajoute le caractère

            if(i == blocSize) {
                if(toDecimal(bloc) == (pow(2,blocSize)-1)) {
                    for(int j = 0; j<(pow(2,blocSize)-2); j++) {
                        myfileDecompress << writableCarac;
                    }  
                } else {
                    for(int j = 0; j<toDecimal(bloc); j++) {
                        myfileDecompress << writableCarac;
                    }
                    if(writableCarac == '0') {
                        writableCarac = '1';
                    } else {
                        writableCarac = '0';
                    }
                }
                i=0;
                bloc = "";
            }
            
        }
        myfile.close();
    } else {
        cout << "Erreur à l'ouverture du fichier" << endl;
    }
}