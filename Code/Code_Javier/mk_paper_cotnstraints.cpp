#include "Data.h"


void Data::binaryvariables_paper_mk(GRBModel &model, GRBVar**** x)
{
	int i, j, k, s;
	for (i = 0; i < n_jobs + 1; i++)
	{
		x[i] = new GRBVar**[n_jobs + 1];
		for (j = 0; j < n_jobs + 1; j++)
		{
			x[i][j] = new GRBVar*[n_nurses];
			for (k = 0; k < n_nurses; k++)
			{
				x[i][j][k] = new GRBVar[n_skills];
				for (s = 0; s < n_skills; s++)
				{

					if (rl)
					{
						string st = "x(" + to_string(i) + "," + to_string(j) + "," + to_string(k) + "," + to_string(s) + ")";
						x[i][j][k][s] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, st);
					}
					else
					{
						string st = "x(" + to_string(i) + "," + to_string(j) + "," + to_string(k) + "," + to_string(s) + ")";
						x[i][j][k][s] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, st);
					}
				}
			}
		}
	}


}

void Data::linearvariables_paper_mk(GRBModel &model, GRBVar*** t, GRBVar** z)
{
	int i, k, s;
	//vector<double> B(n_jobs);
	//double num;
	for (i = 0; i < n_jobs + 1; i++)
	{
		t[i] = new GRBVar*[n_nurses];
		for (k = 0; k < n_nurses; k++)
		{
			t[i][k] = new GRBVar[n_skills];
			for (s = 0; s < n_skills; s++)
			{
				string st = "t(" + to_string(i) + "," + to_string(k) + "," + to_string(s) + ")";
				t[i][k][s] = model.addVar(0, INFINITY, 0.0, GRB_CONTINUOUS, st);
			}
		}
	}
	for (i = 0; i < n_jobs + 1; i++)
	{
		z[i] = new GRBVar[n_skills];
		for (s = 0; s < n_skills; s++)
		{
			string st = "z(" + to_string(i) + "," + to_string(s) + ")";
			z[i][s] = model.addVar(0, INFINITY, 0.0, GRB_CONTINUOUS, st);
		}
	}
}

void Data::objectivefunction_mk_paper(GRBModel &model,GRBVar Dp, GRBVar Tp, GRBVar Tmax)
{
	GRBLinExpr obj = 0;
	obj += lambda[0]*Dp + lambda[1]*Tp + lambda[2] * Tmax;
	model.setObjective(obj, GRB_MINIMIZE);
}
void Data::constraints2_paper_mk(GRBModel &model, GRBVar****x, GRBVar Dp)
{
	int i, j, k, s;
	GRBLinExpr Expr = 0;

	for (i = 0; i < n_jobs + 1; i++)
	{
		for (j = 0; j < n_jobs + 1; j++)
		{
			for (k = 0; k < n_nurses; k++)
			{
				for (s = 0; s < n_skills; s++)
				{
					if (offres[k][s] == 1)
					{
						if (i == 0)
						{
							if (j > 0 && demandes[j - 1][s] > 0 && offres[k][s] == 1)
							{
								Expr += od_depi[j - 1] * x[i][j][k][s];
							}
						}
						else if (j == 0)
						{
							if (i > 0)
							{
								Expr += od_depf[i - 1] * x[i][j][k][s];
							}
						}
						else if (demandes[j - 1][s] > 0)
						{
							Expr +=  od[i - 1][j - 1] * x[i][j][k][s];
						}
					}
				}
			}
		}
	}

	Expr -= Dp;
	model.addConstr(Expr == 0);
}

void Data::constraints3_paper_mk(GRBModel &model, GRBVar**z, GRBVar Tp)
{
	int i, s;
	GRBLinExpr Expr = 0;

	for (i = 1; i < n_jobs + 1; i++)
	{
		for (s = 0; s < n_skills; s++)
		{
			if (demandes[i-1][s] > 0)
			{
				Expr += z[i][s];
			}
		}
	}

	Expr -= Tp;
	model.addConstr(Expr == 0);
}

void Data::constraints4_paper_mk(GRBModel &model, GRBVar**z, GRBVar Tmax)
{
	int i, s;

	for (i = 1; i < n_jobs + 1; i++)
	{
		for (s = 0; s < n_skills; s++)
		{
			if (demandes[i-1][s] > 0)
			{
				GRBLinExpr Expr = 0;
				Expr += z[i][s];
				Expr -= Tmax;
				model.addConstr(Expr <= 0);
			}
		}
	}

}


void Data::constraints5_paper_mk(GRBModel &model, GRBVar****x)
{
	int i, k, s;

	for (k = 0; k < n_nurses; k++)
	{
		GRBLinExpr Expr = 0;

		for (s = 0; s < n_skills; s++)
		{
			for (i = 0; i < n_jobs + 1; i++)
			{
				if ((i==0 || demandes[i - 1][s] > 0) && offres[k][s] == 1)
				{
					Expr += x[0][i][k][s];
				}
			}
		}
		model.addConstr(Expr == 1);
	}
	for (k = 0; k < n_nurses; k++)
	{
		GRBLinExpr Expr = 0;

		for (s = 0; s < n_skills; s++)
		{
			for (i = 0; i < n_jobs + 1; i++)
			{
				if (offres[k][s] == 1)
				{
					Expr += x[i][0][k][s];
				}
			}
		}
		model.addConstr(Expr == 1);
	}
}


void Data::constraints6_paper_mk(GRBModel &model, GRBVar****x)
{
	int i, j, k, s;

	for (i = 1; i < n_jobs+1; i++)
	{
		for (k = 0; k < n_nurses; k++)
		{
			GRBLinExpr Expr = 0;

			for (s = 0; s < n_skills; s++)
			{
				for (j = 0; j < n_jobs + 1; j++)
				{
					if (offres[k][s] == 1)
					{
						if (j == 0)
						{
							Expr += x[i][0][k][s];
						}
						else if (demandes[j - 1][s] > 0)
						{
							Expr += x[i][j][k][s];
						}

						if (demandes[i - 1][s] > 0)
						{
							Expr -= x[j][i][k][s];
						}
					}
				}
			}
			model.addConstr(Expr == 0);
		}
	}
}

void Data::constraints7_paper_mk(GRBModel &model, GRBVar****x)
{
	int i, j, k, s;

	for (i = 1; i < n_jobs + 1; i++)
	{
		for (s = 0; s < n_skills; s++)
		{
			GRBLinExpr Expr = 0;

			for (k = 0; k < n_nurses; k++)
			{
				if (offres[k][s] == 1)
				{
					for (j = 0; j < n_jobs + 1; j++)
					{
						if (demandes[i - 1][s] > 0)
							Expr += x[j][i][k][s];
					}
				}
			}
			model.addConstr(Expr == demandes[i-1][s]);
		}
	}
}


void Data::constraints8_paper_mk(GRBModel &model, GRBVar****x, GRBVar***t)
{
	int Mbig = 10000;
	int i, j, k, s1, s2;
	for (i = 1; i < n_jobs + 1; i++)
	{
		for (j = 1; j < n_jobs + 1; j++)
		{
			for (k = 0; k < n_nurses; k++)
			{
				for (s1 = 0; s1 < n_skills; s1++)
				{
					for (s2 = 0; s2 < n_skills; s2++)
					{
						if (offres[k][s1] == 1 && offres[k][s2] == 1 && demandes[i-1][s1]==1 && demandes[j - 1][s2] == 1)
						{
							GRBLinExpr Expr = 0;
							Expr += t[i][k][s1] + dureedevisite[i-1][s1] + od[i-1][j-1] - t[j][k][s2]- Mbig*(1-x[i][j][k][s2]);
							model.addConstr(Expr <= 0);
						}
					}
				}
			}
		}
	}

	i = 0;
		for (j = 1; j < n_jobs + 1; j++)
		{
			for (k = 0; k < n_nurses; k++)
			{
				for (s1 = 0; s1 < n_skills; s1++)
				{
					for (s2 = 0; s2 < n_skills; s2++)
					{
						if (offres[k][s1] == 1 && offres[k][s2] == 1 && demandes[j - 1][s2] == 1)
						{
							GRBLinExpr Expr = 0;
							Expr += t[i][k][s1] + od_depi[j - 1] - t[j][k][s2] - Mbig * (1 - x[i][j][k][s2]);
							model.addConstr(Expr <= 0);
						}
					}
				}
			}
		}
/*
		for (i = 1; i < n_jobs + 1; i++)
		{
				for (k = 0; k < n_nurses; k++)
				{
					for (s1 = 0; s1 < n_skills; s1++)
					{
						
							if (offres[k][s1] == 1 && demandes[i - 1][s1] == 1)
							{
								GRBLinExpr Expr = 0;
								Expr += t[i - 1][k][s1] + dureedevisite[i - 1][s1] + od_depf[i - 1] - Mbig;
								model.addConstr(Expr <= 0);
							}
					}
				}
		}*/
}


void Data::constraints910_paper_mk(GRBModel &model, GRBVar*** t, GRBVar** z)
{
	int i, k, s;
	int ai, bi;

	vector<double> B(n_jobs);

	for (i = 1; i < n_jobs+1; i++)
	{
		for (k = 0; k < n_nurses; k++)
		{
			for (s = 0; s < n_skills; s++)
			{
				if (demandes[i-1][s] == 1 && offres[k][s] == 1)
				{
					ai = windowsc[i-1][0];
					GRBLinExpr Expr = 0;
					Expr += t[i][k][s] - ai;
					model.addConstr(Expr >= 0);
				}
			}
		}
	}

	for (i = 1; i < n_jobs+1; i++)
	{
		for (k = 0; k < n_nurses; k++)
		{
			for (s = 0; s < n_skills; s++)
			{
				if (demandes[i-1][s] == 1 && offres[k][s] == 1)
				{
					bi = windowsc[i-1][1];
					GRBLinExpr Expr = 0;
					Expr += t[i][k][s] - bi - z[i][s];
					model.addConstr(Expr <= 0);
				}
			}
		}
	}

}

void Data::constraints1112_paper_mk(GRBModel &model, GRBVar**** x, GRBVar*** t)
{
	int i, j, k1, k2, s, s1, s2;
	int BigM;

	for (i = 1; i < n_jobs + 1; i++)
	{
		BigM = 2 * windowsc[i - 1][1];

		int ni_skills = 0;
		for (s = 0; s < n_skills; s++)
		{
			if (demandes[i-1][s] == 1)
				ni_skills++;
		}

		if (ni_skills > 1)
		{
			for (k1 = 0; k1 < n_nurses; k1++)
			{
				for (s1 = 0; s1 < n_skills; s1++)
				{
					if (offres[k1][s1] == 1) // Solo para aquellas enfermeras que tienen el skill
					{
						for (k2 = 0; k2 < n_nurses; k2++)
						{
							for (s2 = s1+1; s2 < n_skills; s2++)
							{
								if (offres[k2][s2] == 1) // Solo para aquellas enfermeras que tienen el skill
								{
									if (demandes[i-1][s1] == 1 && demandes[i-1][s2] == 1) // Solo para las Skills requeridas en el paciente i
									{
										int n_min = gapmin[i-1];
										GRBLinExpr Expr = 0;
										Expr += t[i][k2][s2] - t[i][k1][s1];
										Expr -= n_min;
										Expr += BigM * 2;

										for (j = 0; j < n_jobs + 1; j++)
										{
											Expr -= BigM * x[j][i][k1][s1];
										}
										for (j = 0; j < n_jobs + 1; j++)
										{
											Expr -= BigM * x[j][i][k2][s2];
										}

										model.addConstr(Expr >= 0);
									}
								}

							}
						}
					}
				}
			}
		}
	}



	for (i = 1; i < n_jobs + 1; i++)
	{

		BigM = 2 * windowsc[i - 1][1];

		int ni_skills = 0;
		for (s = 0; s < n_skills; s++)
		{
			if (demandes[i-1][s] == 1)
				ni_skills++;
		}

		if (ni_skills > 1)
		{
			for (k1 = 0; k1 < n_nurses; k1++)
			{
				for (s1 = 0; s1 < n_skills; s1++)
				{
					if (offres[k1][s1] == 1) // Solo para aquellas enfermeras que tienen el skill
					{
						for (k2 = 0; k2 < n_nurses; k2++)
						{
							for (s2 = s1 + 1; s2 < n_skills; s2++)
							{
								if (offres[k2][s2] == 1) // Solo para aquellas enfermeras que tienen el skill
								{
									if (demandes[i-1][s1] == 1 && demandes[i-1][s2] == 1) // Solo para las Skills requeridas en el paciente i
									{
										int n_max = gap[i-1];
										GRBLinExpr Expr = 0;
										Expr += t[i][k2][s2] - t[i][k1][s1];
										Expr -= n_max;
										Expr -= BigM * 2;

										for (j = 0; j < n_jobs + 1; j++)
										{
											Expr += BigM * x[j][i][k1][s1];
										}
										for (j = 0; j < n_jobs + 1; j++)
										{
											Expr += BigM * x[j][i][k2][s2];
										}

										model.addConstr(Expr <= 0);
									}
								}

							}
						}
					}
				}
			}
		}
	}
}


void Data::write_solution_mk(GRBModel &model, GRBVar**** x, GRBVar*** t, GRBVar** z)
{
	int i, j, k,s;

	double value = model.get(GRB_DoubleAttr_ObjVal);
	double best_bound = model.get(GRB_DoubleAttr_ObjBound);

	fprintf(stderr, "Instance:\t%s\tObjValue:\t%.2f\tLB:\t%.2f", data.c_str(), value, best_bound);
	fprintf(stdout, "\n");

	for (k = 0; k < n_nurses; k++)
	{
		for (i = 0; i < n_jobs + 1; i++)
		{
			for (j = 0; j < n_jobs + 1; j++)
			{
				for (s = 0; s < n_skills; s++)
				{
					if (offres[k][s] == 1)
					{
						if (i == 0)
						{
							if (j > 0 && demandes[j - 1][s] > 0  && offres[k][s] == 1)
							{
								try
								{
									if (x[i][j][k][s].get(GRB_DoubleAttr_X) > 0.001)
									{
										fprintf(stdout, "x(%d,%d,%d,%d)\t%.2f\n", i, j,k,s, x[i][j][k][s].get(GRB_DoubleAttr_X));
									}
								}
								catch (GRBException e)
								{

								}
							}
						}
						else if (j == 0)
						{
							if (i > 0)
							{
								try
								{
									if (x[i][j][k][s].get(GRB_DoubleAttr_X) > 0.001)
									{
										fprintf(stdout, "x(%d,%d,%d,%d)\t%.2f\n", i, j, k, s, x[i][j][k][s].get(GRB_DoubleAttr_X));
									}
								}
								catch (GRBException e)
								{
								}
							}
						}
						else if (demandes[j - 1][s] > 0)
						{
							try
							{
								if (x[i][j][k][s].get(GRB_DoubleAttr_X) > 0.001)
								{
									fprintf(stdout, "x(%d,%d,%d,%d)\t%.2f\n", i, j, k, s, x[i][j][k][s].get(GRB_DoubleAttr_X));
								}
							}
							catch (GRBException e)
							{
							}
						}
					}
				}
			}
		}
	}


	for (i = 0; i < n_jobs + 1; i++)
	{
		for (k = 0; k < n_nurses; k++)
		{
			for (s = 0; s < n_skills; s++)
			{
				try
				{
					if (t[i][k][s].get(GRB_DoubleAttr_X) > 0.001)
					{
						fprintf(stdout, "t(%d,%d,%d)\t%.2f\n", i + 1, k, s, t[i][k][s].get(GRB_DoubleAttr_X));
					}
				}
				catch (GRBException e)
				{
				}
			}
		}
	}
	for (i = 0; i < n_jobs + 1; i++)
	{
		for (s = 0; s < n_skills; s++)
		{
			try
			{
				if (z[i][s].get(GRB_DoubleAttr_X) > 0.001)
				{
					fprintf(stdout, "z(%d,%d)\t%.2f\n", i + 1, s, z[i][s].get(GRB_DoubleAttr_X));
				}
			}
			catch (GRBException e)
			{
			}
		}

	}
}
