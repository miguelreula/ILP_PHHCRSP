#include "Data.h"
int Data::MIP_paper_mk()
{
	struct timeb t_after, t_ini;
	double total_time;

	try {
		/* declare variables */
		GRBEnv env = GRBEnv();
		GRBModel model = GRBModel(env);

		GRBVar ****x = new GRBVar***[n_jobs + 1];
		GRBVar ***t = new GRBVar**[n_jobs + 1];
		GRBVar **z = new GRBVar*[n_jobs + 1];

		GRBVar Dp;
		string s = "Dp";
		Dp = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, s);
		GRBVar Tp;
		s = "Tp";
		Tp = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, s);
		GRBVar Tmax;
		s = "Tmax";
		Tmax = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, s);

		// Variables////////////////////////////////////////
		binaryvariables_paper_mk(model, x);
		linearvariables_paper_mk(model, t, z);
		model.update();

		// Objective Function////////////////////////////////////////
		objectivefunction_mk_paper(model, Dp,Tp,Tmax);
		model.update();

		//// Constraints///////////////////////////////////////////////
		constraints2_paper_mk(model, x, Dp);
		constraints3_paper_mk(model, z, Tp);
		constraints4_paper_mk(model, z, Tmax);
		constraints5_paper_mk(model, x);
		constraints6_paper_mk(model, x);
		constraints7_paper_mk(model, x);
		constraints8_paper_mk(model,x,t);
		constraints910_paper_mk(model, t, z);
		constraints1112_paper_mk(model, x, t);
		model.update();

		if (export_lp)
		{
			mkdir("Results");
			string lp_name;
			lp_name = data.substr(0, data.find(".txt"));
			lp_name = lp_name + ".lp";
			lp_name = "./Results/lpfgurob__" + lp_name;
			model.write(lp_name.c_str());
		}
		else
		{
			model.getEnv().set(GRB_DoubleParam_TimeLimit, CPUmax);
			// Run the optimizion

			if (rl)
			{
				model.getEnv().set(GRB_IntParam_Presolve, 0);
				model.getEnv().set(GRB_DoubleParam_NodeLimit, 0);
			}

			if (threads > -1)
			{
				model.getEnv().set(GRB_IntParam_Threads, threads);
			}

			ftime(&t_ini);
			model.optimize();
			ftime(&t_after);

			total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;

			int status = model.get(GRB_IntAttr_Status);
			int num_sol = model.get(GRB_IntAttr_SolCount);
			int number_var = model.get(GRB_IntAttr_NumVars);
			int number_varbin = model.get(GRB_IntAttr_NumBinVars);
			int number_const = model.get(GRB_IntAttr_NumConstrs);

			fprintf(stdout, "Status:\t%d\n", status);
			if (status == GRB_OPTIMAL || (status == GRB_TIME_LIMIT && num_sol > 0))
			{
				if (rl)
				{
					mkdir("Results");
					FILE * pFile;
					pFile = fopen("./Results/rl_solutions.txt", "a+");

					if (pFile != NULL)
					{
						fprintf(pFile, "\nAlgorithm:\ts%d\tStatus:\t%d\t", algorithm, status);
						fprintf(pFile, "Instance:\t%s\tObjValue:\t%.2f\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), model.get(GRB_DoubleAttr_ObjVal), total_time, threads, pre);
						fprintf(pFile, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d", number_varbin, number_var - number_varbin, number_var, number_const);
						fclose(pFile);
					}
				}

				std::cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << std::endl;

				try
				{
					fprintf(stdout, "Solution:\t%.2f\n", model.get(GRB_DoubleAttr_ObjVal));
					if (print)
					{
						fprintf(stderr, "Algorithm:\ts%d\tStatus:\t%d\t", algorithm, status);

						write_solution_mk(model, x, t,z);
						fprintf(stderr, "\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", total_time, threads, pre);

						fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
					}
				}
				catch (GRBException e)
				{
					fprintf(stderr, "Algorithm:\ts%d\tStatus:\t%d\t", algorithm, status);

					fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), total_time, threads, pre);
					fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
				}
			}
			else if (status == GRB_INFEASIBLE)
			{

				fprintf(stderr, "Algorithm:\ts%d\tStatus:\t%d\t", algorithm, status);
				fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), total_time, threads, pre);
				fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
			}
			else
			{
				double best_bound = model.get(GRB_DoubleAttr_ObjBound);
				fprintf(stderr, "Algorithm:\ts%d\tStatus:\t%d\t", algorithm, status);
				fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t%.2f\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), best_bound, total_time, threads, pre);
				fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
			}
		}

		// Free environment/vars
		delete[] x;
		delete[] t;
		delete[] z;
	}
	catch (GRBException e)
	{
		std::cout << "Error code = "
			<< e.getErrorCode()
			<< std::endl;
		std::cout << e.getMessage() << std::endl;
	}
	catch (...)
	{
		std::cout << "Exception during optimization"
			<< std::endl;
	}
	return 1;
}



int Data::MIP_timedependent()
{
	struct timeb t_after, t_ini;
	double total_time;

	try {
		/* declare variables */
		GRBEnv env = GRBEnv();
		GRBModel model = GRBModel(env);

		GRBVar ****x = new GRBVar***[n_jobs + 1];
		GRBVar ***t = new GRBVar**[n_jobs + 1];
		GRBVar **z = new GRBVar*[n_jobs + 1];

		GRBVar Dp;
		string s = "Dp";
		Dp = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, s);
		GRBVar Tp;
		s = "Tp";
		Tp = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, s);
		GRBVar Tmax;
		s = "Tmax";
		Tmax = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, s);

		// Variables////////////////////////////////////////
		binaryvariables_paper_mk(model, x);
		linearvariables_paper_mk(model, t, z);
		model.update();

		// Objective Function////////////////////////////////////////
		objectivefunction_mk_paper(model, Dp, Tp, Tmax);
		model.update();

		//// Constraints///////////////////////////////////////////////
		constraints2_paper_mk(model, x, Dp);
		constraints3_paper_mk(model, z, Tp);
		constraints4_paper_mk(model, z, Tmax);
		constraints5_paper_mk(model, x);
		constraints6_paper_mk(model, x);
		constraints7_paper_mk(model, x);
		constraints8_paper_mk(model, x, t);
		constraints910_paper_mk(model, t, z);
		constraints1112_paper_mk(model, x, t);
		model.update();

		if (export_lp)
		{
			mkdir("Results");
			string lp_name;
			lp_name = data.substr(0, data.find(".txt"));
			lp_name = lp_name + ".lp";
			lp_name = "./Results/lpfgurob__" + lp_name;
			model.write(lp_name.c_str());
		}
		else
		{
			model.getEnv().set(GRB_DoubleParam_TimeLimit, CPUmax);
			// Run the optimizion

			if (rl)
			{
				model.getEnv().set(GRB_IntParam_Presolve, 0);
				model.getEnv().set(GRB_DoubleParam_NodeLimit, 0);
			}

			if (threads > -1)
			{
				model.getEnv().set(GRB_IntParam_Threads, threads);
			}

			ftime(&t_ini);
			model.optimize();
			ftime(&t_after);

			total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;

			int status = model.get(GRB_IntAttr_Status);
			int num_sol = model.get(GRB_IntAttr_SolCount);
			int number_var = model.get(GRB_IntAttr_NumVars);
			int number_varbin = model.get(GRB_IntAttr_NumBinVars);
			int number_const = model.get(GRB_IntAttr_NumConstrs);

			fprintf(stdout, "Status:\t%d\n", status);
			if (status == GRB_OPTIMAL || (status == GRB_TIME_LIMIT && num_sol > 0))
			{
				if (rl)
				{
					mkdir("Results");
					FILE * pFile;
					pFile = fopen("./Results/rl_solutions.txt", "a+");

					if (pFile != NULL)
					{
						fprintf(pFile, "\nAlgorithm:\ts%d\tStatus:\t%d\t", algorithm, status);
						fprintf(pFile, "Instance:\t%s\tObjValue:\t%.2f\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), model.get(GRB_DoubleAttr_ObjVal), total_time, threads, pre);
						fprintf(pFile, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d", number_varbin, number_var - number_varbin, number_var, number_const);
						fclose(pFile);
					}
				}

				std::cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << std::endl;

				try
				{
					fprintf(stdout, "Solution:\t%.2f\n", model.get(GRB_DoubleAttr_ObjVal));
					if (print)
					{
						fprintf(stderr, "Algorithm:\ts%d\tStatus:\t%d\t", algorithm, status);

						write_solution_mk(model, x, t, z);
						fprintf(stderr, "\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", total_time, threads, pre);

						fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
					}
				}
				catch (GRBException e)
				{
					fprintf(stderr, "Algorithm:\ts%d\tStatus:\t%d\t", algorithm, status);

					fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), total_time, threads, pre);
					fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
				}
			}
			else if (status == GRB_INFEASIBLE)
			{

				fprintf(stderr, "Algorithm:\ts%d\tStatus:\t%d\t", algorithm, status);
				fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), total_time, threads, pre);
				fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
			}
			else
			{
				double best_bound = model.get(GRB_DoubleAttr_ObjBound);
				fprintf(stderr, "Algorithm:\ts%d\tStatus:\t%d\t", algorithm, status);
				fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t%.2f\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), best_bound, total_time, threads, pre);
				fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
			}
		}

		// Free environment/vars
		delete[] x;
		delete[] t;
		delete[] z;
	}
	catch (GRBException e)
	{
		std::cout << "Error code = "
			<< e.getErrorCode()
			<< std::endl;
		std::cout << e.getMessage() << std::endl;
	}
	catch (...)
	{
		std::cout << "Exception during optimization"
			<< std::endl;
	}
	return 1;
}


