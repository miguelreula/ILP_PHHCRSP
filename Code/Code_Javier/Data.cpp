#include "Data.h"
#define PRINT

std::string trim(const std::string& str,
	const std::string& whitespace = " \t")
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

std::string reduce(const std::string& str,
	const std::string& fill = " ",
	const std::string& whitespace = " \t")
{
	// trim first
	auto result = trim(str, whitespace);

	// replace sub ranges
	auto beginSpace = result.find_first_of(whitespace);
	while (beginSpace != std::string::npos)
	{
		const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
		const auto range = endSpace - beginSpace;

		result.replace(beginSpace, range, fill);

		const auto newStart = beginSpace + fill.length();
		beginSpace = result.find_first_of(whitespace, newStart);
	}

	return result;
}

Data::Data()
{
	CPUmax = 3600;

	export_lp = false;
	print = true;
	n_dupl = 0;
	rl = false;
	html = false;
	threads = -1;
	v1 = false;
	v2 = false;
	v3 = false;
	pre = 1;
	singleskill = true;

	for (int i = 0; i < 3; i++) { lambda.push_back(0); }
	lambda[0] = double(1)/double(3);
	lambda[1] = double(1) / double(3);
	lambda[2] = double(1) / double(3);
	difobj = false;

	cci = true;
	ccf = true;
	feas = false;
}

Data::~Data(void)
{
}

int Data::getpreferences() { return preferences; }

void Data::read_instance()
{

	const char* ninstance = n_instance.c_str();
	ifstream ficheroi;
	ficheroi.open(ninstance);

	string phrase;
	bool fi = false; //Fin de fichero//
	std::vector<std::string> elems;
	std::stringstream s;
	std::string item;
	char delim;
	string Value = "";

	int c_nurse, c_job, c_skill, c_job1, c_job2;
	vector<vector<int>> list_dupl;
	vector<int> temp_dupl;
	int r_skills;
	c_nurse = 0;


	bool flag_skills = false;
	bool flag_duracion = false;
	bool flag_pref = false;
	bool flag_dini = false;
	bool flag_dfi = false;
	bool flag_distances = false;
	bool flag_nurses = false;
	bool flag_gap = false;
	bool flag_twn = false;
	bool flag_twc = false;
	bool flag_cxyn = false;
	bool flag_cxyc = false;

	if (!ficheroi)
	{
		printf("Problemas al leer la instancia, no se encuentra el archivo ");
		exit(4);
	}
	else
	{
		delim = '=';
		c_job1 = 0;
		c_job2 = 0;
		while (getline(ficheroi, phrase) && !fi)
		{
			elems.clear();
			item.clear();
			s.clear();
			s.str(phrase);

			while (std::getline(s, item, delim))
			{
				if (!item.empty())
				{
					elems.push_back(item);
				}
			}

			//En elems está la frase
			if (elems.size() > 0)
			{	
				if (strcmp(elems[0].c_str(), "];") == 0)
				{
					flag_skills = false;
					flag_duracion = false;
					flag_pref = false;
					flag_dini = false;
					flag_dfi = false;
					flag_distances = false;
					flag_nurses = false;
					flag_gap = false;
					flag_twn = false;
					flag_twc = false;
					flag_cxyn = false;
					flag_cxyc = false;
					delim = '=';

				}
				else if (startsWithCaseInsensitive(elems[0].c_str(), "NbCustomers") != 0)
				{	// NbVehicles - Number of nurses available 
					// WindowsD - WindowsD[nurse][0] time the nurse starts working, WindowsD[nurse][1] time the nurse finishes

					n_jobs = atoi(elems[1].c_str());

					vector<int> temp(2);
					vector<double> temp3(n_jobs);
					for (int i = 0; i < n_jobs; i++)
					{
						gap.push_back(0);
						gapmin.push_back(0);
						windowsc.push_back(temp);
						od.push_back(temp3);
						od_depi.push_back(0);
						od_depf.push_back(0);
						cxyc.push_back(temp);
					}
				}
				else if (startsWithCaseInsensitive(elems[0].c_str(), "NbSynch") != 0)
				{	// NbVehicles - Number of nurses available 
					// WindowsD - WindowsD[nurse][0] time the nurse starts working, WindowsD[nurse][1] time the nurse finishes

					n_dupl = atoi(elems[1].c_str());
				}
				else if (startsWithCaseInsensitive(elems[0].c_str(), "NbNurses") != 0)
				{	// NbVehicles - Number of nurses available 
					// WindowsD - WindowsD[nurse][0] time the nurse starts working, WindowsD[nurse][1] time the nurse finishes

					n_nurses = atoi(elems[1].c_str());

					vector<int> temp(2);
					vector<double> temp2(n_nurses);

					for (int i = 0; i < n_nurses; i++)
					{
						windowsd.push_back(temp);
						cxyn.push_back(temp);
					}
					for (int i = 0; i < n_jobs; i++)
					{
						pref.push_back(temp2);
						od_depf_to_nurse.push_back(temp2);
						od_depi_to_nurse.push_back(temp2);
					}
				}
				else if (startsWithCaseInsensitive(elems[0].c_str(), "NbServices") != 0)
				{
					// NbServices - Number of different skills 
					// Offres - For each nurse, what skills they have.Binary: Offres[nurse][skill] 
					// Demandes - Matrix with skills required by the client
					// DureeDeVisite - Service time per skill->DureeDeVisite[job][skill]

					n_skills = atoi(elems[1].c_str());
					vector<int> temp(n_skills);
					for (int i = 0; i < n_nurses; i++)
					{
						offres.push_back(temp);
					}
					for (int i = 0; i < n_jobs; i++)
					{
						demandes.push_back(temp);
						dureedevisite.push_back(temp);
					}
				}
				else if (startsWithCaseInsensitive(elems[0].c_str(), "NbPref") != 0)
				{	// NbVehicles - Number of nurses available 
					// WindowsD - WindowsD[nurse][0] time the nurse starts working, WindowsD[nurse][1] time the nurse finishes

					preferences = atoi(elems[1].c_str());
				}
				else if (strcmp(elems[0].c_str(), "Skills") == 0)
				{
					c_nurse = 0;
					flag_nurses = true;
					delim = '\t';
				}
				else if (flag_nurses)
				{
					int num;
					for (int i = 0; i < elems.size(); i++)
					{
						num = atoi(elems[i].c_str());
						offres[c_nurse][i] = num;
					}
					c_nurse++;
				}
				else if (strcmp(elems[0].c_str(), "Demands") == 0)
				{
					c_job = 0;
					flag_skills = true;
					delim = '\t';
				}
				else if (flag_skills)
				{
					int num;
					for (int i = 0; i < elems.size(); i++)
					{
						num = atoi(elems[i].c_str());
						demandes[c_job][i] = num;
					}
					c_job++;
				}
				else if (strcmp(elems[0].c_str(), "Duration") == 0)
				{
					c_job = 0;
					flag_duracion = true;
					delim = '\t';
				}
				else if (flag_duracion)
				{
					int num;
					for (int i = 0; i < elems.size(); i++)
					{
						num = atoi(elems[i].c_str());
						dureedevisite[c_job][i] = num;
					}
					c_job++;
				}
				else if (strcmp(elems[0].c_str(), "Pref") == 0)
				{
					c_job = 0;
					flag_pref = true;
					delim = '\t';
				}
				else if (flag_pref)
				{
					double num;
					for (int i = 0; i < elems.size(); i++)
					{
						num = atof(elems[i].c_str());
						pref[c_job][i] = num;
					}
					c_job++;
				}
				else if (strcmp(elems[0].c_str(), "d_ini_to_nurses") == 0)
				{
					c_job = 0;
					flag_dini = true;
					delim = '\t';
				}
				else if (flag_dini)
				{
					double num;
					for (int i = 0; i < elems.size(); i++)
					{
						num = atof(elems[i].c_str());
						od_depi_to_nurse[c_job][i] = num;
					}
					od_depi[c_job] = num;
					c_job++;
				}
				else if (strcmp(elems[0].c_str(), "d_fi_to_nurses") == 0)
				{
					c_job = 0;
					flag_dfi = true;
					delim = '\t';
				}
				else if (flag_dfi)
				{
					double num;
					for (int i = 0; i < elems.size(); i++)
					{
						num = atof(elems[i].c_str());
						od_depf_to_nurse[c_job][i] = num;
					}
					od_depf[c_job] = num;
					c_job++;
				}
				else if (strcmp(elems[0].c_str(), "d_customers") == 0)
				{
					c_job = 0;
					flag_distances = true;
					delim = '\t';
				}
				else if (flag_distances)
				{
					double num;
					for (int i = 0; i < elems.size(); i++)
					{
						num = atof(elems[i].c_str());
						od[c_job][i] = num;
					}
					c_job++;
				}
				else if (strcmp(elems[0].c_str(), "gap_min_max") == 0)
				{
					c_job = 0;
					flag_gap = true;
					delim = '\t';
				}
				else if (flag_gap)
				{
					int num;
					num = atoi(elems[0].c_str());
					gapmin[c_job] = num;
					int num2;
					num2 = atoi(elems[1].c_str());
					gap[c_job]=num2;
					c_job++;
				}
				else if (strcmp(elems[0].c_str(), "TW_customers") == 0)
				{
					c_job = 0;
					flag_twc = true;
					delim = '\t';
				}
				else if (flag_twc)
				{
					int num;
					num = atoi(elems[0].c_str());
					windowsc[c_job][0] = num;
					int num2;
					num2 = atoi(elems[1].c_str());
					windowsc[c_job][1] = num2;
					c_job++;
				}
				else if (strcmp(elems[0].c_str(), "cxy_customer") == 0)
				{
					c_job = 0;
					flag_cxyc = true;
					delim = '\t';
				}
				else if (flag_cxyc)
				{
					int num;
					num = atoi(elems[0].c_str());
					cxyc[c_job][0] = num;
					int num2;
					num2 = atoi(elems[1].c_str());
					cxyc[c_job][1] = num2;
					c_job++;
				}
				else if (strcmp(elems[0].c_str(), "TW_customers") == 0)
				{
					c_job = 0;
					flag_twc = true;
					delim = '\t';
				}
				else if (flag_twc)
				{
					int num;
					num = atoi(elems[0].c_str());
					windowsc[c_job][0] = num;
					int num2;
					num2 = atoi(elems[1].c_str());
					windowsc[c_job][1] = num2;
					c_job++;
				}
				else if (strcmp(elems[0].c_str(), "TW_nurses") == 0)
				{
					c_nurse = 0;
					flag_twn = true;
					delim = '\t';
				}
				else if (flag_twn)
				{
					int num;
					num = atoi(elems[0].c_str());
					windowsd[c_nurse][0] = num;
					int num2;
					num2 = atoi(elems[1].c_str());
					windowsd[c_nurse][1] = num2;
					c_nurse++;
				}
				else if (strcmp(elems[0].c_str(), "cxy_nurses") == 0)
				{
					c_nurse = 0;
					flag_cxyn = true;
					delim = '\t';
				}
				else if (flag_cxyn)
				{
					int num;
					num = atoi(elems[0].c_str());
					cxyn[c_nurse][0] = num;
					int num2;
					num2 = atoi(elems[1].c_str());
					cxyn[c_nurse][1] = num2;
					c_nurse++;
				}
			}
		}
	}

	int num_skill;
	for (int k = 0; k < n_nurses; k++)
	{
		num_skill = 0;
		for (int s = 0; s < n_skills; s++)
		{
			if (offres[k][s] == 1)
			{
				num_skill++;
			}
		}
		if (num_skill > 1)
		{
			singleskill = false;
			break;
		}
	}




}

void Data::setinstancesnames(char *argv[])
{
	string folder = argv[1]; //Folder
	data = argv[2]; //File
	instance = "Result_" + data; //out instance
	n_instance = "./" + folder + "/" + data;

}
void Data::setprint(bool num)
{
	print = num;
}
void Data::sethtml(bool num)
{
	html = num;
}
void Data::setalgorithm(int num)
{
	algorithm = num;
}
void Data::setexport_lp(bool num)
{
	export_lp = num;
}
void Data::set_rl(bool num)
{
	rl = num;
}
void Data::setthreads(int num)
{
	threads = num;
}

void Data::settimewindows(int num)
{
	for (int k = 0; k < n_nurses; k++)
	{
		windowsd[k][1] = num * 60;
	}
}
void Data::setv1(bool num)
{
	v1 = num;
}
void Data::setcc1(bool num)
{
	cci = num;
}
void Data::setcc2(bool num)
{
	ccf = num;
}
void Data::setv2(bool num)
{
	v2 = num;
}
void Data::setv3(bool num)
{
	v3= num;
}
void Data::setdif(bool num)
{
	difobj = num;
}
void Data::setCPUmax(int num)
{
	CPUmax = num;
}
void Data::setpre(bool num)
{
	pre = num;
}
void Data::setfeas(bool num)
{
	feas = num;
}
bool Data::endsWithCaseInsensitive(std::string mainStr, std::string toMatch)
{
	auto it = toMatch.begin();
	return mainStr.size() >= toMatch.size() &&
		std::all_of(std::next(mainStr.begin(), mainStr.size() - toMatch.size()), mainStr.end(), [&it](const char & c) {
		return ::tolower(c) == ::tolower(*(it++));
	});
}
bool Data::startsWithCaseInsensitive(std::string mainStr, std::string toMatch)
{
	// Convert mainStr to lower case
	std::transform(mainStr.begin(), mainStr.end(), mainStr.begin(), ::tolower);
	// Convert toMatch to lower case
	std::transform(toMatch.begin(), toMatch.end(), toMatch.begin(), ::tolower);

	if (mainStr.find(toMatch) == 0)
		return true;
	else
		return false;
}
void Data::duplicatejobs()
{
	int i, j, h, s;

	vector<vector<int>> list_dupl;
	vector<int> temp;
	int r_skills;

	n_dupl = 0;
	for (i = 0; i < n_jobs; i++)
	{
		temp.clear();
		r_skills = 0;
		for (s = 0; s < n_skills; s++)
		{
			if (demandes[i][s] >0)
			{
				r_skills= r_skills + demandes[i][s];
				if (r_skills > 1)
				{
					temp.push_back(n_jobs + n_dupl);
					n_dupl++;
				}
			}
		}
		if (r_skills > 1)
		{
			//Si entro aquí, ese nodo se tiene que duplicar
			temp.push_back(i);
			list_dupl.push_back(temp);
		}
	}

	//Aquí tengo una lista con los que tengo que duplicar y también se cuántos nodos más se va a crear. Empiezo a crearlos.
	if (n_dupl > 0)
	{
		// Actualizo el tamaño de los vectores [pref,demandes,duredevisite, windowsc,od,od_depi,od_depf]
		vector<double> temp_ndouble(n_nurses);
		vector<int> temp_skills(n_skills);
		vector<int> temp_2(2);
		vector<double> temp_jobs(n_jobs);
		for (i = 0; i < n_dupl; i++)
		{
			pref.push_back(temp_ndouble);
			demandes.push_back(temp_skills);
			dureedevisite.push_back(temp_skills);
			windowsc.push_back(temp_2);
			od_depi.push_back(0);
			od_depf.push_back(0);
			od.push_back(temp_jobs);
			cxyc.push_back(temp_2);
		}
		vector<int> temp_all(n_jobs + n_dupl);
		//Falta por actualizar tamaño de od por completo.
		//Aún faltaría también lo de los gaps. Vamos a usar dos matrices, una rel_matrix con 1 si dos nodos están relacionados y 0 si no lo están, y otra gap_matrix que nos diga el gap en los que están relacionados.
		for (i = 0; i < n_jobs + n_dupl; i++)
		{
			rel_jobs.push_back(temp_all);
			gap_jobs.push_back(temp_all);
			gap_jobs_min.push_back(temp_all);
			for (j = 0; j < n_dupl; j++)
			{
				od[i].push_back(0);
			}
		}

		//Ahora sí que podemos actualizar los datos en esas estructuras.
		//Vamos a tener que cambiar la entrada de demandas
		int c_job, c_clon;
		for (i = 0; i < list_dupl.size(); i++)
		{
			//Estoy en el primer nodo que tiene descendencia
			c_job = list_dupl[i][list_dupl[i].size() - 1];
			r_skills = -1;
			for (s = 0; s < n_skills; s++)
			{
				if (demandes[c_job][s] > 0)
				{
					r_skills=r_skills+demandes[c_job][s]; //La primera se tiene que quedar porque es la del nodo. El resto no
					if (r_skills > 0)
					{

						c_clon = list_dupl[i][r_skills - 1];
						if (demandes[c_job][s] == 2)
						{
							demandes[c_job][s] = 1;
							demandes[c_clon][s] = 1;

							dureedevisite[c_clon][s] = dureedevisite[c_job][s];
						}
						else
						{
							demandes[c_job][s] = 0;
							demandes[c_clon][s] = 1;

							dureedevisite[c_clon][s] = dureedevisite[c_job][s];
							dureedevisite[c_job][s] = 0;
						}
						//Aquí es donde se produce los cambios. Esa demanda pasa a ser del cliente que toca.
						rel_jobs[c_job][c_clon] = 1;
						gap_jobs[c_job][c_clon] = gap[c_job];
						gap_jobs_min[c_job][c_clon] = gapmin[c_job];

						//Lo estoy clonando
						pref[c_clon] = pref[c_job];
						windowsc[c_clon] = windowsc[c_job];
						od_depi[c_clon] = od_depi[c_job];
						od_depf[c_clon] = od_depf[c_job];
						od[c_clon] = od[c_job];
						cxyc[c_clon] = cxyc[c_job];
						for (h = 0; h < n_jobs + n_dupl; h++)
						{
							od[h][c_clon] = od[h][c_job];
						}
					}
				}
			}
		}
	}
	else
	{
		//En este modelo voy a usar esto. Si no hay duplicados, no hay gaps ni relaciones.
		vector<int> temp_all(n_jobs);
		for (i = 0; i < n_jobs; i++)
		{
			rel_jobs.push_back(temp_all);
			gap_jobs.push_back(temp_all);
			gap_jobs_min.push_back(temp_all);
		}
	}

	n_jobs = n_jobs + n_dupl;

}

