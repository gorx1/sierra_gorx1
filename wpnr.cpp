#include "sierrachart.h"

using namespace std;

SCDLLName("wpnr")

double get_wpnr(SCFloatArrayRef src, int index, int len, int n)
{
	vector<pair<double, double>> pairs;
	double weights_sum = 0.0;
	
	for (int i = 0; i < len; i++)
	{	
		double w = len - i;
		pairs.push_back(make_pair(static_cast<double>(src[index - i]), w));
		weights_sum += w;
	}
	
	sort(pairs.begin(), pairs.end());
	
	double sum = 0.0;
	vector<double> weights_cum;
	
	for (int i = 0; i < len; i++)
	{
		sum += pairs[i].second;
		weights_cum.push_back(sum);
	}
	
	double wpnr = 0.0;

	for (int i = 0; i < len; i++)
	{
		if (weights_cum[i] >= weights_sum / 100 * n)
		{
			wpnr = pairs[i].first;
			break;
		}
	}
	
	return wpnr;
}

SCFloatArrayRef wpnr(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length, int Percentile)
{	
	Out[Index] = static_cast<float>(get_wpnr(In, Index, Length, Percentile));
	
	return Out;
}

SCSFExport scsf_wpnr(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_Wpnr = sc.Subgraph[0];
	
	SCInputRef Input_InputData  = sc.Input[0];
	SCInputRef Input_Length 	= sc.Input[2];
	SCInputRef Input_Percentile = sc.Input[3];
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "WPNR";

		sc.AutoLoop    = 1;
		sc.GraphRegion = 0;
		sc.ValueFormat = 3;  
		
		Subgraph_Wpnr.Name 	       = "WPNR";
		Subgraph_Wpnr.DrawStyle    = DRAWSTYLE_LINE;
		Subgraph_Wpnr.DrawZeros    = false;
		Subgraph_Wpnr.PrimaryColor = RGB(255,0,255);
		
		Input_InputData.Name = "Input Data";
		Input_InputData.SetInputDataIndex(SC_LAST);  
		
		Input_Length.Name = "Length";
		Input_Length.SetInt(25);
		Input_Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		Input_Percentile.Name = "Perc";
		Input_Percentile.SetInt(50);
		Input_Percentile.SetIntLimits(0, 100);

		return;
	}
	
	sc.DataStartIndex = Input_Length.GetInt()-1;	

	wpnr(sc.BaseDataIn[Input_InputData.GetInputDataIndex()], Subgraph_Wpnr, sc.Index, Input_Length.GetInt(), Input_Percentile.GetInt());
}
