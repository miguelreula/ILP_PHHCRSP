#ifndef DATA_H_
#define DATA_H_
#include <set>
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <climits>   
#include <sstream>
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>
#include <list>
#include <sys/timeb.h>
#include <exception>
#include <stack>
#include <direct.h>
#include <vector>
#include <iterator>
#include <Windows.h>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include "data.h"
#pragma warning(disable:4786)
#pragma warning(disable:4996)

#include "gurobi_c++.h"
using namespace std;

//#define REFORCE
#define N_80
#define PRINT
#define OTHER_TREL
#define BOUNDS
#define SINGLE
#define REMOVE_TK
#define LAST_C
#define RL



//#undefine VIEJOS
class Data
{
private:
	int CPUmax; //Maximum CPU time. Default 3600
	int n_jobs; //For us, number of jobs. For Ait and Mk number of customers.
	int n_nurses; //NbVehicles - Number of nurses available
	int n_dupl; //Jobs duplicados, si un job tiene más de un skill se crea un job gemelo
	int n_skills; //NbServices - Number of different skills
	int preferences; //0 mk, 1 ait


	int skills_nexp;

	vector<vector<double>> pref; 	//CoutPref - Matrix of the preference scores between patients / carers
	vector<double> od_depi;
	vector<double> od_depf;
	vector<vector<double>> od_depi_to_nurse;
	vector<vector<double>> od_depf_to_nurse;
	vector<vector<double>> od; //od - od[i, j] as the distance between two patients.row / col 0 represent the nurses location(same for all of them)


	//TODO M for Ait

	vector<int> experts_rel;
	vector<int> experts;
	vector<int> experts_assig;
	vector<vector<int>> dureedevisite; //DureeDeVisite - Service time per skill->DureeDeVisite[job][skill]
	vector<vector<int>> demandes; //Demandes - Matrix with skills required by the client
	vector<vector<int>> offres; //Offres - For each nurse, what skills they have.Binary: Offres[nurse][skill]

	vector<vector<int>> windowsc; //WindowsC - For each job, WindowsC[job][0] is the start of the time window, WindowsC[job][1] the finishing time
	vector<vector<int>> windowsd; //WindowsD - WindowsD[nurse][0] time the nurse starts working, WindowsD[nurse][1] time the nurse finishes
	vector<vector<int>> cxyn;
	vector<vector<int>> cxyc;

	vector<int> gap; //gap - Gap between two services(for those that require two skills only!) if the gap is 0, it means they are simultaneous
	vector<int> gapmin; //gap - Gap between two services(for those that require two skills only!) if the gap is 0, it means they are simultaneous
	vector<vector<int>> rel_jobs;
	vector<vector<int>> gap_jobs;
	vector<vector<int>> gap_jobs_min;
	vector<vector<int>> matrix_skills;
	vector<int> skills;
	
	vector<double> lambda;

	string instance; //Nombre del txt con la instancia
	string data; //Nombre del txt con los datos
	string n_instance; //Nombre folder+instance

	bool export_lp; //Export lp or solve here if 1, export
	bool print; //by default, print true
	bool rl; //rl false by default
	bool html;
	bool v1, v2, v3;
	bool pre; //preproceso
	bool singleskill;
	bool difobj;

	int algorithm;
	int threads; //número de hilos modelos
	bool cci, ccf;

	bool feas = false;
public:
	Data();
	~Data(void);

	void read_instance();

	void setinstancesnames(char *argv[]);

	void setCPUmax(int);
	void setalgorithm(int);
	void setthreads(int num);
	void setfeas(bool num);
	void setexport_lp(bool);
	void setprint(bool);
	void set_rl(bool);
	void sethtml(bool);
	void setv1(bool num);
	void setcc1(bool num);
	void setcc2(bool num);
	void setv2(bool num);
	void setv3(bool num);
	void setdif(bool num);
	void setpre(bool num);

	int getpreferences();

	
	bool endsWithCaseInsensitive(std::string mainStr, std::string toMatch);

	bool startsWithCaseInsensitive(std::string mainStr, std::string toMatch);
	void duplicatejobs();
	void settimewindows(int);

	/*---- Mankowska ----*/
	int MIP_paper_mk();

	void binaryvariables_paper_mk(GRBModel &, GRBVar****);
	void linearvariables_paper_mk(GRBModel &, GRBVar***, GRBVar**);

	void objectivefunction_mk_paper(GRBModel &model, GRBVar Dp, GRBVar Tp, GRBVar Tmax);
	void constraints2_paper_mk(GRBModel &, GRBVar****, GRBVar);
	void constraints3_paper_mk(GRBModel &, GRBVar**, GRBVar);
	void constraints4_paper_mk(GRBModel &, GRBVar**, GRBVar);
	void constraints5_paper_mk(GRBModel &, GRBVar****);
	void constraints6_paper_mk(GRBModel &, GRBVar****);
	void constraints7_paper_mk(GRBModel &, GRBVar****);
	void constraints8_paper_mk(GRBModel &, GRBVar****, GRBVar***);
	void constraints910_paper_mk(GRBModel &model, GRBVar*** t, GRBVar** z);
	void constraints1112_paper_mk(GRBModel &model, GRBVar**** x, GRBVar*** t);

	void write_solution_mk(GRBModel &model, GRBVar**** x, GRBVar*** t, GRBVar** z);


	/*----Time dependent*/
	int MIP_timedependent();
};
#endif






























	////La informacion Load_factor, number y type se requiere para ser almacenada en el arbol, no se utiliza en cada matriz. La paso por referencia y cuando lea la bahía inicial la almaceno donde toque
	////Con esta funcion, además de obtener las "caracteristicas del problem" (disposicion inicial, nombre del fichero, S, H,...), obtengo un vector con el número de contenedores de bloqueo que hay en cada stack y el número total de estos contenedores y un vector con las alturas de los stacks.
	//void LeerDisposicionInicial(char *argv[], int&, int&, int&);
	//
	////Devuelve el nuevo vector con el número de contenedores de bloqueo de cada prioridad y, por referencia prioridad de mayor P de los contenedores de bloqueo.
	//void Matrix::NewBlockingNumber(int s_origen, int s_destino, vector<int> NewBlockingbystack, int& NewTotalBlockingNumber, vector<int> NewBlockingbytype, int& NewMaxBlockingtype);


	//Actualiza el M según el cálculo de Bortfeld de GB or GG
	//void ContarContenedoresMolestos_BortfeldForster();