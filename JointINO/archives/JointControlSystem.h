//######################
//JointControlSystem.h
//######################
//######################
//wrapper class around control system
class JointControlSystem
{
public:
	struct Joint
	{
	public:
		float p_thresholds[3]; //0=min, 1=max, 2=last
		float p_degreeRotation[4]; //0=min, 1=max, 2=current, 3=target		

		int p_id;

#define THRESH_min 	p_thresholds[0]
#define THRESH_max 	p_thresholds[1]
#define THRESH_last p_thresholds[2]

#define DEG_min 	p_degreeRotation[0]
#define DEG_max 	p_degreeRotation[1]
#define DEG_curr 	p_degreeRotation[2]
#define DEG_targ 	p_degreeRotation[3]
#define DEG_RANGE 	(DEG_max - DEG_min)

		inline Joint();
		inline Joint(int id, float thresholdMin, float thresholdMax, float degreesMin, float degreesMax);
		inline ~Joint();
		inline bool InRange(JointControlSystem* jcs); //the input sum is within the threshold range
		inline float GetDegrees(JointControlSystem* jcs); //current amount of degrees to move
	};

#define INITIAL_CYCLE_RATE 200
	float m_cycleRate; //cycles division rate -- divide degrees each cycle by this rate -- should be same as CPU
	float* m_coefficients; //ARRAY correlates directly with inputs -- same dimensions 

	int m_inputsLength;
	float* m_inputs; //array of floats with fixed size
	Joint** m_joints; //array of defined joints

	inline JointControlSystem(int inputCount, float* coefficients, float cycleRate = INITIAL_CYCLE_RATE);
	inline ~JointControlSystem();

	/// <summary>
	/// Load data into the current set of inputs for InRange() check
	/// </summary>
	/// <param name="nextInputs"></param>
	inline void UpdateStoredData(float* nextInputs);
};