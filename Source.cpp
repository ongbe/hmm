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
#define num_e 3

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

//Funzione che dato file, estrae le terne (x,y,z) e le inserisce tutte in un vettore dell'azione
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

//Funzione che data l'azione (vettore di frame) riempie il vettore di features calcolate
void getfeatures(vector<vector<trip>> vaction, vector<vector<double>>& vfeatures){
	vector<double> feat;
	for(int i=0; i<vaction.size(); ++i){ //per ogni frame calcolo la feature
		double dist;

		//Distanza euclidea mani
		dist = sqrt(pow((getxyz(vaction, i, right_hand).x-getxyz(vaction, i, left_hand).x), 2) +
			pow((getxyz(vaction, i, right_hand).y-getxyz(vaction, i, left_hand).y), 2) +
			pow((getxyz(vaction, i, right_hand).z-getxyz(vaction, i, left_hand).z), 2));
		//Accumulo la feature
		feat.push_back(dist);

		//Distanza mano destra - testa
		dist = sqrt(pow((getxyz(vaction, i, right_hand).x-getxyz(vaction, i, head).x), 2) +
			pow((getxyz(vaction, i, right_hand).y-getxyz(vaction, i, head).y), 2) +
			pow((getxyz(vaction, i, right_hand).z-getxyz(vaction, i, head).z), 2));
		//Accumulo la feature
		feat.push_back(dist); 

		//Distanza mano sinistra - testa
		dist = sqrt(pow((getxyz(vaction, i, left_hand).x-getxyz(vaction, i, head).x), 2) +
			pow((getxyz(vaction, i, left_hand).y-getxyz(vaction, i, head).y), 2) +
			pow((getxyz(vaction, i, left_hand).z-getxyz(vaction, i, head).z), 2));
		//Accumulo la feature
		feat.push_back(dist); 

		//Memorizzo tutte le features calcolate
		vfeatures.push_back(feat);

		//Pulisco il vettore features per accumularne di nuove
		feat.clear();
	}
}

int main(int argc, char *argv[]){
	bool singolo = true; //funziona solo il singolo caricamento per ora
	bool train = false; //true: train, false: test
	string nome_file;
	ifstream in;
	vector<trip> v;
	vector<vector<trip>> vaction;

	cout << "ciao";

	if(train){	

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
						std::cout << "Aperto file: " << nome_file << endl;

						//Apro il file
						in.open(nome_file);
						if(!in.is_open())
							std::cout << "Errore apertura file!" << endl;

						//Ottengo tutto il contenuto del file -> vettore di triplette (x, y, z)
						getdata(in, v, vaction);

						//Ogni vaction contiene tanti elementi quanti frame memorizzati (ogni frame 20 triplette)
						std::cout << "Sono stati memorizzati " << vaction.size() << " frame" << endl;
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
			std::cout << "Aperto singolo file: " << nome_file << endl;

			//Apro il file
			in.open(nome_file);
			if(!in.is_open())
				std::cout << "Errore apertura file!" << endl;

			//Ottengo tutto il contenuto del file -> vettore di triplette (x, y, z)
			getdata(in, v, vaction);

			std::cout << "Sono stati memorizzati " << vaction.size() << " frame" << endl;
		}

		//Creazione del Feature Vector
		vector<vector<double>> vfeatures;

		std::cout << "Calcolo le features..." << endl;
		getfeatures(vaction, vfeatures);

		std::cout << "Il feature vector ha " << vfeatures.size() << " vettori di features, con " << vfeatures[0].size() << " features" << endl;



		//--------------------------------------------TRAINING HMM-----------------------------------------
		int num_stati = 8;
		int dim_fv = vfeatures[0].size(); //dimensione del singolo feature vector (tutti uguali)
		int num_gaus = 1;
		typedef vector<vector<double>>::iterator iter_vf;
		const iter_vf ivf_init = vfeatures.begin();
		const iter_vf ivf_final = vfeatures.end();

		//Nuovo HMM
		std::cout << "Costruisco un nuovo HMM..." << endl;
		CHMM_GMM *phmm = new CHMM_GMM(num_stati, dim_fv, num_gaus);

		//Inizializzo HMM
		std::cout << "Inizializzo il feature vector..." << endl;
		int num_iter;
		//phmm->RandomInit(); 
		Mat_<double> A_dopo_init;
		Mat_<double> si_prima_train;
		phmm->Init_Equi(ivf_init, ivf_final);
		A_dopo_init =  (phmm->m_A).clone(); 	
		si_prima_train = (phmm->m_pi).clone();

		//Addestramento HMM
		std::cout << "Training..." << endl << endl;
		int pniter;
		double plogprobinit, plogprobfinal;
		phmm->BaumWelch(ivf_init, ivf_final, &pniter, &plogprobinit, &plogprobfinal);
		std::cout << "Valore iniziale: " << plogprobinit << endl;
		std::cout << "Valore finale: " << plogprobfinal << endl;

		Mat_<double> A; //Matrice transizioni
		A = phmm->m_A;
		

		//--------------------------------------------TESTING TRAIN HMM-----------------------------------

		//LogLikelihood: ottengo score da HMM date le osservazioni
		double loglk = phmm->LogLikelihood(ivf_init, ivf_final, &A);
		std::cout << "logLikelihood ottenuta: " << loglk << endl;

		//Salvo HMM: ottengo un char significativo essendo il salvataggio in C (serve un char*)
		string prefix = "hmm_";
		stringstream suffix;
		suffix << "_" << num_stati << "_" << num_gaus;
		nome_file.erase(0, 8);
		nome_file.erase(11, nome_file.size());
		prefix.append(nome_file);
		prefix.append(suffix.str());
		char * cstr = new char [prefix.length()+1];
		strcpy_s(cstr, prefix.length()+1, prefix.c_str());

		//Salvo con un nome collegato all'azione considerata
		phmm->SaveToFile(cstr);

		//Salvo log valori HMM addestrato
		string nome_log = "log_train_";
		nome_log.append(prefix);
		nome_log.append(".txt");
		ofstream log(nome_log); 

		log << "Numero stati: " << phmm->m_iN << "\n" << "Numero gaussiane: " << phmm->m_iK << "\n" << "Numero features: " << phmm->m_iM << endl;
		log << "\nMatrice transizioni [dopo l'inizializzazione]:\n" << A_dopo_init << endl;
		log << "\nMatrice transizioni [dopo train]:\n" << phmm->m_A << endl;
		log << "\nStato iniziale [prima del train]:\n" << si_prima_train << endl;
		log << "\nStato finale:\n " << phmm->m_final << endl; 
		log << "\nNumero iterazioni train: " << pniter << endl;
		log << "Probabilità iniziale: " << plogprobinit << endl;
		log << "Probabilità finale: " << plogprobfinal << endl;
		log << "LogLikelihood ottenuta [test su stesse osservazioni del training]: " << loglk << endl;
		

		//Chiudo i file aperti
		in.close();
		log.close();
		std::system("pause");

	}
	//------------------------------------------------TESTING------------------------------------------
	else{
		std::cout << "Testing..." << endl;

		ifstream hmm_file("hmm.txt");
		ifstream test_file("test.txt");
		ofstream out_file("results.txt");

		vector<string> nomi_hmm;
		vector<string> nomi_test;
		string nome;

		//Ottengo nomi HMM da usare
		while(getline(hmm_file, nome))
			nomi_hmm.push_back(nome);

		std::cout << "Ci sono " << nomi_hmm.size() << " HMM da valutare" << endl;

		//Ottengo test dal dataset
		while(getline(test_file, nome))
			nomi_test.push_back(nome);

		std::cout << "Ci sono " << nomi_test.size() << " azioni per il testing" << endl << endl;

		for(int i=0; i<nomi_hmm.size(); ++i){

			//Ricavo numero stati e gaussiane (salvate in altro modo?): conversione char->int
			int num_stati, num_gaus;
			num_stati = (int)(nomi_hmm[i].at(16))-'0';
			num_gaus = (int)(nomi_hmm[i].at(18))-'0';

			//Carico l'HMM
			CHMM_GMM *hmm = new CHMM_GMM(num_stati, 3, num_gaus); 
			//Creo char* per aprire il file
			char * nome_hmm = new char [nomi_hmm[i].length()+1];
			strcpy_s(nome_hmm, nomi_hmm[i].length()+1, nomi_hmm[i].c_str());
			std::cout << "Carico: " << nomi_hmm[i] << endl;
			if(!(hmm->LoadFromFile(nome_hmm))){
				cout << "Errore caricamento HMM: " << nome_hmm << endl;
				out_file << "Errore caricamento HMM: " << nome_hmm << endl;
				continue;
			}
			//Salvo nel file
			out_file << nomi_hmm[i] << endl;

			//Ottengo matrice delle transizioni
			Mat_<double> A;
			A = hmm->m_A; 

			for(int k=0; k<nomi_test.size(); ++k){

				//Apro file con l'azione
				string prefix = "dataset/";
				prefix.append(nomi_test[k]);
				std::cout << "Valuto: " << prefix << endl;
				ifstream hmm_in(prefix);
				if(!hmm_in.is_open()){
					std::cout << "Errore apertura file azione!" << endl;
					out_file << "Errore apertura file azione" << endl;
					continue;
				}


				//Ottengo le terne (x, y, z)
				std::cout << "Leggo l'azione...";
				getdata(hmm_in, v, vaction);
				std::cout << "fatto" << endl;
				std::cout << "Sono stati memorizzati " << vaction.size() << " frame" << endl;

				//Ottengo le features
				cout << "Calcolo le features...";
				vector<vector<double>> vfeatures;
				getfeatures(vaction, vfeatures);
				cout << "fatto" << endl;
				std::cout << "Il feature vector ha " << vfeatures.size() << " vettori di features, con " << vfeatures[0].size() << " features" << endl;

				//Valuto l'HMM
				typedef vector<vector<double>>::iterator iter_vf;
				const iter_vf ivf_init = vfeatures.begin();
				const iter_vf ivf_final = vfeatures.end();
				double loglk;

				loglk = hmm->LogLikelihood(ivf_init, ivf_final, &A);
				cout << "\tlogLikelihood ottenuta: " << loglk << endl << endl;

				//Salvo il risultato nel file di log
				out_file << nomi_test[k] << "\t" << loglk << endl;

				//Pulisco i vettori utilizzati e non dichiarati ogni ciclo
				v.clear();
				vaction.clear();
			}
			out_file << endl << endl;
		}
		//Chiudo i file
		test_file.close();
		hmm_file.close();
		out_file.close();

		std::system("pause");
	}
}