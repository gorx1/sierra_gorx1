#include "sierrachart.h"

SCDLLName("wstdb")

double GetWma(SCFloatArrayRef In, int StartIndex, int Length)
{
	double sum   = 0;
	double sum_w = 0;
	
	if (StartIndex < Length - 1 )
	{
		StartIndex = Length - 1;
	}
	
	if (StartIndex >= In.GetArraySize())
		return 0;
	
	for(int i = 0; i < Length; i++)
	{
		double w = Length - i;
		sum     += static_cast<double>(In[StartIndex - i]) * w;
		sum_w   += w;
	}

	return sum / sum_w;
}

double GetWvar(SCFloatArrayRef In, int StartIndex, int Length)
{	
	double sum   = 0;
	double sum_w = 0;
	double wm    = GetWma(In, StartIndex, Length);

	for(int i = 0; i < Length; i++)
	{ 
		if (StartIndex - i < 0)
			break;
		
		double w = Length - i;
		sum     += pow(static_cast<double>(In[StartIndex - i]) - wm, 2) * w;
		sum_w   += w;
	}

	return sum / sum_w;
}

void GetWstdev(SCFloatArrayRef In, float& Out, int StartIndex, int Length)
{
	double Wvar = 0;	

	Wvar = GetWvar(In, StartIndex, Length);

	if(Wvar < 0)
	{
		Out = 0;
	}
	else
	{
		Out = static_cast<float>(sqrt(Wvar));
	}
}

SCFloatArrayRef Wstdev(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
{	
	GetWstdev(In, Out[Index], Index, Length);
	
	return Out;
}

SCSFExport scsf_wstdb(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_UpperBand = sc.Subgraph[0];
	SCSubgraphRef Subgraph_MidBand   = sc.Subgraph[1];
	SCSubgraphRef Subgraph_LowerBand = sc.Subgraph[2];
	
	SCInputRef Input_InputData 		    = sc.Input[0];
	SCInputRef Input_Length 			= sc.Input[2];
	SCInputRef Input_StandardDeviations = sc.Input[3];

	SCFloatArrayRef array_stdev = sc.Subgraph[9].Arrays[1];
	
	if(sc.SetDefaults)
	{
		sc.GraphName = "WSTDB";

		sc.AutoLoop    = 1;
		sc.GraphRegion = 0;
		sc.ValueFormat = 3;  
		
		Subgraph_MidBand.Name 	      = "Basis";
		Subgraph_MidBand.DrawStyle 	  = DRAWSTYLE_LINE;
		Subgraph_MidBand.DrawZeros 	  = false;
		Subgraph_MidBand.PrimaryColor = RGB(255,0,255);

		Subgraph_LowerBand.Name 		= "Lower";
		Subgraph_LowerBand.DrawStyle    = DRAWSTYLE_LINE;
		Subgraph_LowerBand.DrawZeros    = false;
		Subgraph_LowerBand.PrimaryColor = RGB(255,255,0);

		Subgraph_UpperBand.Name 		= "Upper";
		Subgraph_UpperBand.DrawStyle    = DRAWSTYLE_LINE;
		Subgraph_UpperBand.DrawZeros 	= false;
		Subgraph_UpperBand.PrimaryColor = RGB(0,255,0);
		
		Input_InputData.Name = "Input Data";
		Input_InputData.SetInputDataIndex(SC_LAST);  
		
		Input_Length.Name = "Length";
		Input_Length.SetInt(25);
		Input_Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		Input_StandardDeviations.Name = "Multi";
		Input_StandardDeviations.SetFloat(2.0f);
		Input_StandardDeviations.SetFloatLimits(.001f, static_cast<float>(MAX_STUDY_LENGTH));

		return;
	}


	sc.DataStartIndex = Input_Length.GetInt()-1;	

	double basis = GetWma(sc.BaseDataIn[Input_InputData.GetInputDataIndex()], sc.Index, Input_Length.GetInt());
	
	Wstdev(sc.BaseDataIn[Input_InputData.GetInputDataIndex()], array_stdev, sc.Index, Input_Length.GetInt());
	
	Subgraph_MidBand[sc.Index]   = basis;
	Subgraph_UpperBand[sc.Index] = static_cast<float>(Subgraph_MidBand[sc.Index] + array_stdev[sc.Index] * Input_StandardDeviations.GetFloat());
	Subgraph_LowerBand[sc.Index] = static_cast<float>(Subgraph_MidBand[sc.Index] - array_stdev[sc.Index] * Input_StandardDeviations.GetFloat());
}
