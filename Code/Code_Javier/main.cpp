#include "Data.h"
#pragma warning(disable:4786)
#pragma warning(disable:4996)


#include <sys/timeb.h>

 int main(int argc, char *argv[])
{
	Data problem;

	char **agv;

	//Cómo introducir la instancia
	//1ºPrimero introducir la carpeta
	//2ºLuego nombre de instancia

	problem.setinstancesnames(argv);
	problem.read_instance();

	// -e //Export lp. Por defecto no se exporta
	// -s //Silence. Por defecto se printea el resultado. 
	// -r //Linear relaxation
	// -c 1//número de hilos. Si no se pone nada, se deja por defecto.
	// -m 0//Modelo que resolver
	// -t 60 change time limit to 60
	// -w 24 // Quiere decir que el TW de la enfermera se excede a 24 horas (24*60)
	// -f Da primera solución factible que encuentra

	int algorithm=-2;
	int type_algorithm;
	int twe = 0;
	for (agv = argv + 3, argc--; argc > 2 && agv[0][0] == '-'; --argc, ++agv)
	{
		switch (agv[0][1])
		{
		default:
			break;
		case 'e':
			problem.setexport_lp(true);
			break;
		case 'r': //Linear Relaxation
			problem.set_rl(true);
			break;
		case 's':
			problem.setprint(false);
			break;
		case 't':
			problem.setCPUmax((int)atoi(agv[1]));
			++agv;
			--argc;
			break;
		case 'w':
			twe = (int)atoi(agv[1]);
			problem.settimewindows(twe);
			++agv;
			--argc;
			break;
		case 'c':
			problem.setthreads((int)atoi(agv[1]));
			++agv;
			--argc;
			break;
		case 'm':
			algorithm = (int)atoi(agv[1]);
			++agv;
			--argc;
			break;
		case 'f':
			problem.setfeas(true);
			break;
		}
	}

	problem.setalgorithm(algorithm);
	if (algorithm == -2)
	{
		fprintf(stderr, "Se requiere el algoritmo correcto\n");
		return 0;
	}
	   
	//0 -> Mk

	switch (algorithm)
	{
		default:
			break;
		case 0:
			problem.MIP_paper_mk();
			break;
		case 1:
			problem.MIP_timedependent();
			break;
	}

	return 0;
}
































