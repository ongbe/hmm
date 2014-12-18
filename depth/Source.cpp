#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <opencv2\opencv.hpp>

#include <iostream> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <direct.h>
#include <windows.h>

using namespace std;
using namespace cv;

int main(){

	ifstream in_nomi("nomi_sdepth.txt");

	//Carico nomi depth
	string line;
	string new_dir;

	while(getline(in_nomi, line)){

		string prefix = "C:/Users/Guido/Desktop/depth/";
		int num_frame, width, heigth;

		prefix.append(line);
		cout << "Carico: " << prefix << endl;
		ifstream in(prefix, ios::in | ios::binary);
		if(!in.is_open())
			cout << "Errore apertura file!" << endl;
		in.read((char*)&num_frame, sizeof(int));
		cout << "Numero frame: " << (int)num_frame << endl;
		in.read((char*)&width, sizeof(int));
		cout << "Larghezza: " << width << endl;
		in.read((char*)&heigth, sizeof(int));
		cout << "Altezza: " << heigth << endl;

		string directory = line.substr(line.find_first_of("a", 0)+1, 2); 
		if(directory.compare(new_dir) != 0){
			new_dir = directory;
			char dir[20]; strcpy_s(dir, new_dir.c_str());
			_chdir("C:/Users/Guido/Desktop/azioni");
			CreateDirectory(dir, NULL);
			cout << "Creo nuova cartella: " << new_dir << endl;
		}

		for(int i=0;i<num_frame;++i){
			Mat1i image(heigth, width);
			Mat1b norm_image(heigth, width);
			in.read((char*)image.data, image.rows*image.cols*sizeof(int));
			normalize(image, norm_image, 0, 255, NORM_MINMAX);
			//imshow("Immagine", norm_image);

			stringstream s;
			line.resize(12);
			line.append(to_string(i+1));
			s << "C:/Users/Guido/Desktop/azioni/" << new_dir << "/" << line << ".jpg";

			imwrite(s.str(), norm_image);
		}
		

		in.close();
		prefix.clear();
	}
	

	

	//Chiudo i file
	in_nomi.close();

	system("pause");
}