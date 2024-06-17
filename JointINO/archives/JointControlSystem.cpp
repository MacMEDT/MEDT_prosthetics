#include ".\JointControlSystem.h"

JointControlSystem::JointControlSystem(int inputCount, float* coefficients, float cycleRate)
{
	m_cycleRate = cycleRate;
	m_coefficients = coefficients;
	m_inputs = new float[inputCount] { 0 };
	m_inputsLength = inputCount;

	//starting values
	//===============
	//threshold min =0.4
	//threshold max =0.6
	//degree min=0
	//degree max=180
	m_joints = new Joint*[m_inputsLength];
}
JointControlSystem::~JointControlSystem() 
{
	//delete &m_cycleRate;
	//delete &m_inputsLength;
	delete[] m_inputs;
	delete[] m_coefficients;
}

void JointControlSystem::UpdateStoredData(float* nextInputs) 
{
	m_inputs = nextInputs;
}

JointControlSystem::Joint::Joint()
{
	p_id = -1;
	THRESH_min = 0;
	THRESH_max = 1;
	DEG_min = 0;
	DEG_max = 360;
	DEG_curr = DEG_min + (DEG_RANGE / 2); //set point in middle of valid range			
}
//id, thresholdmin, thresholdmax, degreesmin, degreesmax
JointControlSystem::Joint::Joint(int id, float thresholdMin, float thresholdMax, float degreesMin, float degreesMax)
{
	p_id = id;
	THRESH_min = thresholdMin;
	THRESH_max = thresholdMax;
	DEG_min = degreesMin;
	DEG_max = degreesMax;
	DEG_curr = DEG_min + (DEG_RANGE / 2); //set point in middle of valid range			
}
JointControlSystem::Joint::~Joint()
{
}

//true if (input sum * coeff) within threshold
bool JointControlSystem::Joint::InRange(JointControlSystem* jcs)
{
	THRESH_last = 0; //reset sum for each iteration
	
	//TODO: why do we have the array? Only check for current joint, per cycle
	//for (int i = 0; i < m_inputsLength; i++)
	//{
	
	if (THRESH_min > jcs->m_inputs[p_id]) THRESH_min = jcs->m_inputs[p_id]; //assign min
	if (THRESH_max < jcs->m_inputs[p_id]) THRESH_max = jcs->m_inputs[p_id]; //assign max

	//Multivariate regression
	//https://docs.google.com/document/d/1K0lEID0azMEU90q1zjZloJze-vb1MGexAW7cJyY2Yz4/edit
	THRESH_last = jcs->m_inputs[p_id] * jcs->m_coefficients[p_id]; //TODO: use coefficient here?
	//}
	return THRESH_last > THRESH_min && THRESH_last < THRESH_max;
}

//TODO: degree format ---- we can format the return value in two ways, either iterative, each cycles movements or total rotation from starting point
///return current degrees -- InRange() must be called to read valid data
float JointControlSystem::Joint::GetDegrees(JointControlSystem* jcs)
{
	//TODO: limit the threshold, dynamic and fixed actuation range?
	//	typically 5 degrees
	float threshLast = THRESH_last;
	float threshNorm = (THRESH_last - THRESH_min) / THRESH_max; //normalized
	float degrees = DEG_RANGE * threshNorm; 
	float cycleDegrees = degrees / jcs->m_cycleRate;

	//TODO: unused? Remove, refactor?
	DEG_targ = DEG_min + degrees; //TODO: determine if we can even use ---- cached value 

	//prevent overextension
	if (cycleDegrees > DEG_min && cycleDegrees < DEG_max)
	{
		DEG_curr += cycleDegrees; //+,- shouldn't matter
		return cycleDegrees; //DEG_curr
	}
	return 0;
};

