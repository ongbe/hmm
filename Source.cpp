#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <iterator>

#include "gmmstd_gmm_tiny.h"
#include "gmmstd_hmm_GMM.h"

using namespace std;
using namespace gmmstd;

struct trip{
	double x;
	double  y;
	double  z;
};

//Numero azione
#define num_a 3
//Numero persona
#define num_s 1
//Numero istanza
#define num_e 1

//Numero azioni da caricare train (MAX 3)
#define num_a_train 1
//Numero persone da caricare train (MAX 12)
#define num_s_train 1
//Numero istanze da caricare train (MAX 3)
#define num_e_train 1

#define right_hand 12
#define left_hand 13
#define head 20

trip getxyz(vector<vector<trip>> vframe, int frame, int part ){
	trip t;
	t.x = vframe[frame][part-1].x; //il vettore inizia da 0: per frame ok tanto perchè uso for, la parte invece è definita
	t.y = vframe[frame][part-1].y;
	t.z = vframe[frame][part-1].z;

	return t;
}

//Funzione che dato file, estrae (x,y,z) e le inserisce tutte in un vettore dell'azione
void getdata(ifstream &in, vector<trip>& v, vector<vector<trip>>& vaction ){
	string line;

	while(getline(in, line)){
		trip tmp;
		double devNull;
			double t1,t2,t3;
			stringstream ss(line);

			ss >> t1 >> t2 >> t3 >> devNull;
			tmp.x=t1;
			tmp.y=t2;
			tmp.z=t3;

			v.push_back(tmp);

			if(v.size()==20){
							vaction.push_back(v);
							v.clear();
						}
	}
}


int main(){

	bool singolo = true;
	string nome_file;
	ifstream in;
	vector<trip> v;
	vector<vector<trip>> vaction;

	//--------------------------------------Acquisizione del dataset-------------------------------------
	if(!singolo){
		for(int i=0; i<num_a_train; ++i){

			stringstream ssi;
			ssi << setfill('0') << setw(2) << i+1;		

			for(int k=0; k<num_s_train; ++k){

				stringstream ssk;
				ssk << setfill('0') << setw(2) << k+1;

				for(int j=0; j<num_e_train; ++j){

					stringstream ssj;
					ssj << setfill('0') << setw(2) << j+1;

					//Creo nome file da aprire
					nome_file += "dataset/";
					nome_file += "a" + ssi.str();
					nome_file += "_s" + ssk.str();
					nome_file += "_e" + ssj.str();
					nome_file += "_skeleton3d.txt";
					cout << "Aperto file: " << nome_file << endl;

					//Apro il file
					in.open(nome_file);
					if(!in.is_open())
						cout << "Errore apertura file!" << endl;

					//Ottengo tutto il contenuto del file -> vettore di triplette (x, y, z)
					getdata(in, v, vaction);

					//Ogni vaction contiene tanti elementi quanti frame memorizzati (ogni frame 20 triplette)
					cout << "Sono stati memorizzati " << vaction.size() << " frame" << endl;
					nome_file.clear();
					ssj.str().clear();
				}
				ssk.clear();
			}	
			ssi.clear();
		}
	}
	else{// Versione per caricare un singolo file del dataset
		stringstream ssi;
		ssi << setfill('0') << setw(2) << num_a;
		stringstream ssk;
		ssk << setfill('0') << setw(2) << num_s;
		stringstream ssj;
		ssj << setfill('0') << setw(2) << num_e;

		//Creo nome del file da aprire
		nome_file += "dataset/";
		nome_file += "a" + ssi.str();
		nome_file += "_s" + ssk.str();
		nome_file += "_e" + ssj.str();
		nome_file += "_skeleton3d.txt";
		cout << "Aperto singolo file: " << nome_file << endl;

		//Apro il file
		in.open(nome_file);
		if(!in.is_open())
			cout << "Errore apertura file!" << endl;

		//Ottengo tutto il contenuto del file -> vettore di triplette (x, y, z)
		getdata(in, v, vaction);

		cout << "Sono stati memorizzati " << vaction.size() << " frame" << endl;
	}

	//--------------------------------Creazione del Feature Vector-------------------------------
	vector<double> feat;
	vector<vector<double>> vfeatures;

	cout << "Calcolo le features..." << endl;
	for(int i=0; i<vaction.size(); ++i){ //per ogni frame calcolo la feature
		double dist;

		//Distanza euclidea mani
		dist = sqrt(pow((getxyz(vaction, i, right_hand).x-getxyz(vaction, i, left_hand).x), 2) +
			pow((getxyz(vaction, i, right_hand).y-getxyz(vaction, i, left_hand).y), 2) +
			pow((getxyz(vaction, i, right_hand).z-getxyz(vaction, i, left_hand).z), 2));

		feat.push_back(dist);

		//Distanza mano destra - testa
		dist = sqrt(pow((getxyz(vaction, i, right_hand).x-getxyz(vaction, i, head).x), 2) +
			pow((getxyz(vaction, i, right_hand).y-getxyz(vaction, i, head).y), 2) +
			pow((getxyz(vaction, i, right_hand).z-getxyz(vaction, i, head).z), 2));

		feat.push_back(dist); 

		//Distanza mano sinistra - testa
		dist = sqrt(pow((getxyz(vaction, i, left_hand).x-getxyz(vaction, i, head).x), 2) +
			pow((getxyz(vaction, i, left_hand).y-getxyz(vaction, i, head).y), 2) +
			pow((getxyz(vaction, i, left_hand).z-getxyz(vaction, i, head).z), 2));

		feat.push_back(dist); 

		//Memorizzo tutte le features calcolate
		vfeatures.push_back(feat);

		feat.clear();
	}

	cout << "Il feature vector ha " << vfeatures.size() << " vettori di features, con " << vfeatures[0].size() << " features" << endl;



	//--------------------------------------------TRAINING HMM-----------------------------------------
	int num_stati = 8;
	int dim_fv = vfeatures[0].size(); //dimensione del singolo feature vector (tutti uguali)
	int num_gaus = 1;
	typedef vector<vector<double>>::iterator iter_vf;
	const iter_vf ivf_init = vfeatures.begin();
	const iter_vf ivf_final = vfeatures.end();
	
	//Nuovo HMM
	cout << "Costruisco un nuovo HMM..." << endl;
	CHMM_GMM *phmm = new CHMM_GMM(num_stati, dim_fv, num_gaus);

	//Inizializzo HMM
	cout << "Inizializzo il feature vector..." << endl;
	int num_iter;
	//phmm->RandomInit(); 
	phmm->Init_Equi(ivf_init, ivf_final);

	//Addestramento HMM
	cout << "Training..." << endl << endl;
	int pniter;
	double plogprobinit, plogprobfinal;
	phmm->BaumWelch(ivf_init, ivf_final, &pniter, &plogprobinit, &plogprobfinal);
	cout << "Valore iniziale: " << plogprobinit << endl;
	cout << "Valore finale: " << plogprobfinal << endl;
	

	//Ottengo valori HMM addestrato
	Mat_<double> A; //Matrice transizioni
	A = phmm->m_A;

	//--------------------------------------------TESTING HMM---------------------------------------
	
	//LogLikelihood: ottengo score da HMM date le osservazioni
	cout << "logLikelihood ottenuta: " << phmm->LogLikelihood(ivf_init, ivf_final, &A) << endl;

	//Salvo HMM: ottengo un char significativo essendo il salvataggio in C
	string prefix = "hmm_";
	nome_file.erase(0, 8);
	nome_file.erase(11, nome_file.size());
	prefix.append(nome_file);
	char * cstr = new char [prefix.length()+1];
	strcpy_s(cstr, prefix.length()+1, prefix.c_str());

	//Salvo con un nome collegato all'azione considerata
	phmm->SaveToFile(cstr);
	

	in.close();
	system("pause");

}