#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

int main(){

	bool flag = false; // true = DEPTH, false = SKELETON

	ifstream in_nomi("nomi_dataset.txt");
	vector<string> v_nomi;

	//Controllo file
	if(!in_nomi.is_open())
		cout << "Errore apertura file input!" << endl;

	//Caricamento nomi del dataset
	string line;
	while(getline(in_nomi, line)){
		v_nomi.push_back(line);
	}
	cout << "Caricati " << v_nomi.size() << " nomi del dataset" << endl;

	if(flag){
		//Creazione nomi depth
		ofstream out_nomi("nomi_sdepth.txt");
		vector<string>::iterator it;
		for(it=v_nomi.begin();it!=v_nomi.end();++it){
			it->append("_sdepth.bin");
			out_nomi << *it << endl;
		}
		out_nomi.close();
	}
	else{
		//Creazione nomi azioni
		ofstream out_nomi("nomi_skeleton.txt");
		vector<string>::iterator it;
		for(it=v_nomi.begin();it!=v_nomi.end();++it){
			it->append("_skeleton3D.txt");
			out_nomi << *it << endl;
		}
		out_nomi.close();
	}

	//Chiusura file
	in_nomi.close();

	system("pause");
}