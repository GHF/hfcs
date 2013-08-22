//!
//! @file 		Pid.h
//! @author 	Geoffrey Hunter <gbmhunter@gmail.com> (www.cladlab.com), modified Xo Wang
//! @edited 	n/a
//! @date 		2012/10/01
//! @brief 		Header file for Pid.c
//! @details
//!				See README.rst



//===============================================================================================//
//====================================== HEADER GUARD ===========================================//
//===============================================================================================//

#ifndef __cplusplus
	#error Please build with C++ compiler
#endif

#ifndef PID_H
#define PID_H

//===============================================================================================//
//================================== PRECOMPILER CHECKS =========================================//
//===============================================================================================//


#define pidENABLE_FP_SUPPORT 0

//! @brief		Set to 1 if you want debug information printed
#define pidPRINT_DEBUG			0

//! @brief		Size (in bytes) of the debug buffer (only valid if
//!				pidPRINT_DEBUG is set to 1).
#define pidDEBUG_BUFF_SIZE		200

//===============================================================================================//
//======================================== NAMESPACE ============================================//
//===============================================================================================//

#if(pidENABLE_FP_SUPPORT == 1)
	#include "./FixedPoint/include/Fp.h"
#endif

#if(pidPRINT_DEBUG == 1)
#include <stdio.h>		// snprintf
#endif

namespace PidNs
{
	
	//! @brief		Base PID class. 
	class PidDataBase
	{
	
		public:
			//===============================================================================================//
			//=================================== PUBLIC TYPEDEFS ===========================================//
			//===============================================================================================//
			typedef enum			//!< Enumerates the controller direction modes
			{
				PID_DIRECT,			//!< Direct drive (+error gives +output)
				PID_REVERSE			//!< Reverse driver (+error gives -output)
			} ctrlDir_t;

			//! Used to determine wether the output shouldn't be accumulated (distance control),
			//! or accumulated (velocity control).
			typedef enum
			{
				DONT_ACCUMULATE_OUTPUT,
				ACCUMULATE_OUTPUT,
				DISTANCE_PID = DONT_ACCUMULATE_OUTPUT,
				VELOCITY_PID = ACCUMULATE_OUTPUT
			} outputMode_t;
		
			ctrlDir_t controllerDir;
			outputMode_t outputMode;	//!< The output mode (non-accumulating vs. accumulating) for the control loop
	};
	
	//! @brief		PID class that uses dataTypes for it's arithmetic
	template <class dataType, class floatType = float> class Pid : public PidDataBase
	{
		public:
		
			//! @brief 		Init function
			//! @details   	The parameters specified here are those for for which we can't set up 
			//!    			reliable defaults, so we need to have the user set them.
			void Init(
				dataType kp, 
				dataType ki,
				dataType kd, 
				ctrlDir_t controllerDir,
				outputMode_t outputMode,
				floatType samplePeriodMs,
				dataType minOutput,
				dataType maxOutput, 
				dataType setPoint);
			
			//! @brief 		Computes new PID values
			//! @details 	Call once per sampleTimeMs. Output is stored in the pidData structure.
			void Run(dataType input);
			
			void SetOutputLimits(dataType min, dataType max);
		
			//! @details	The PID will either be connected to a direct acting process (+error leads to +output, aka inputs are positive) 
			//!				or a reverse acting process (+error leads to -output, aka inputs are negative)
			void SetControllerDirection(ctrlDir_t controllerDir);
			
			//! @brief		Changes the sample time
			void SetSamplePeriod(floatType newSamplePeriodMs);
			
			//! @brief		This function allows the controller's dynamic performance to be adjusted. 
			//! @details	It's called automatically from the init function, but tunings can also
			//! 			be adjusted on the fly during normal operation
			void SetTunings(dataType kp, dataType ki, dataType kd);

			dataType GetKp();

			dataType GetKi();

			dataType GetKd();

			dataType GetZp();

			dataType GetZi();

			dataType GetZd();

			//! @brief		Prints debug information to the desired output
			void PrintDebug(const char* msg);

			//! @brief 		The set-point the PID control is trying to make the output converge to.
			dataType setPoint;			

			//! @brief		The control output. 
			//! @details	This is updated when Pid_Run() is called.
			dataType output;				
		
		private:
			//#if(pidPRINT_DEBUG == 1)
				char debugBuff[pidDEBUG_BUFF_SIZE];
			//#endif

			//! Time-step scaled proportional constant for quick calculation (equal to actualKp)
			dataType Zp;		
			
			//! Time-step scaled integral constant for quick calculation
			dataType Zi;	
			
			//! Time-step scaled derivative constant for quick calculation
			dataType Zd;					
			
			//! Actual (non-scaled) proportional constant
			dataType actualKp;			
			
			//! Actual (non-scaled) integral constant
			dataType actualKi;			
			
			//! Actual (non-scaled) derivative constant
			dataType actualKd;			
			
			//! Actual (non-scaled) proportional constant
			dataType prevInput;			
			
			//! The change in input between the current and previous value
			dataType inputChange;			
	
			dataType error;				//!< The error between the set-point and actual output (set point - output, positive
											//!< when actual output is lagging set-point
			
			//! @brief 		The output value calculated the previous time Pid_Run() was called.
			//! @details	Used in ACCUMULATE_OUTPUT mode.
			dataType prevOutput;		

			//! @brief		The sample period (in milliseconds) between successive Pid_Run() calls.
			//! @details	The constants with the z prefix are scaled according to this value.
			floatType samplePeriodMs;
											
			dataType pTerm;				//!< The proportional term that is summed as part of the output (calculated in Pid_Run())
			dataType iTerm;				//!< The integral term that is summed as part of the output (calculated in Pid_Run())
			dataType dTerm;				//!< The derivative term that is summed as part of the output (calculated in Pid_Run())
			dataType outMin;				//!< The minimum output value. Anything lower will be limited to this floor.
			dataType outMax;				//!< The maximum output value. Anything higher will be limited to this ceiling.	

			//! @brief		Counts the number of times that Run() has be called. Used to stop
			//!				derivative control from influencing the output on the first call.
			//! @details	Safely stops counting once it reaches 2^32-1 (rather than overflowing). 
			uint32_t numTimesRan;
	};

	template <class dataType, class floatType> void Pid<dataType, floatType>::Init(
		dataType kp, 
		dataType ki,
		dataType kd, 
		ctrlDir_t controllerDir, 
		outputMode_t outputMode, 
		floatType samplePeriodMs,
		dataType minOutput, 
		dataType maxOutput, 
		dataType setPoint)
	{

		SetOutputLimits(minOutput, maxOutput);		

		this->samplePeriodMs = samplePeriodMs;

	   SetControllerDirection(controllerDir);
		this->outputMode = outputMode;
		
		// Set tunings with provided constants
	   SetTunings(kp, ki, kd);
		this->setPoint = setPoint;
		prevInput = 0;
		prevOutput = 0;

		pTerm = floatType(0.0);
		iTerm = floatType(0.0);
		dTerm = floatType(0.0);
			
	}

	template <class dataType, class floatType> void Pid<dataType, floatType>::Run(dataType input)
	{
		// Compute all the working error variables
		//dataType input = *_input;
		
		error = setPoint - input;
		
		// Integral calcs
		
		iTerm += (Zi * error);
		// Perform min/max bound checking on integral term
		if(iTerm > outMax) 
			iTerm = outMax;
		else if(iTerm < outMin)
			iTerm = outMin;

		// DERIVATIVE CALS

		// Only calculate derivative if run once or more already.
		if(numTimesRan > 0)
		{
			inputChange = (input - prevInput);
			dTerm = -Zd*inputChange;
		}

		// Compute PID Output. Value depends on outputMode
		if(outputMode == DONT_ACCUMULATE_OUTPUT)
		{
			output = Zp*error + iTerm + dTerm;
		}
		else if(outputMode == ACCUMULATE_OUTPUT)
		{
			output = prevOutput + Zp*error + iTerm + dTerm;
		}
		
		// Limit output
		if(output > outMax) 
			output = outMax;
		else if(output < outMin)
			output = outMin;
		
		// Remember input value to next call
		prevInput = input;
		// Remember last output for next call
		prevOutput = output;
		  
		// Increment the Run() counter.
		if(numTimesRan < (2^(32))-1)
			numTimesRan++;
	}

	template <class dataType, class floatType> void Pid<dataType, floatType>::SetTunings(dataType kp, dataType ki, dataType kd)
	{
	   	if (kp<0 || ki<0 || kd<0) 
	   		return;
	 
	 	actualKp = kp; 
		actualKi = ki;
		actualKd = kd;
	   
	   // Calculate time-step-scaled PID terms
	   Zp = kp;

		// The next bit requires floatType->dataType casting functionality.
	   Zi = ki * (dataType)(samplePeriodMs/floatType(1000.0));
	   Zd = kd / (dataType)(samplePeriodMs/floatType(1000.0));
	 
	  if(controllerDir == PID_REVERSE)
	   {
	      Zp = (0 - Zp);
	      Zi = (0 - Zi);
	      Zd = (0 - Zd);
	   }

		#if(pidPRINT_DEBUG == 1)
			snprintf(debugBuff, 
				sizeof(debugBuff),
				"PID: Tuning parameters set. Kp = %.1f, Ki = %.1f, Kd = %f.1, "
				"Zp = %.1f, Zi = %.1f, Zd = %.1f, with sample period = %.1fms\r\n",
				actualKp,
				actualKi,
				actualKd,
				Zp,
				Zi,
				Zd,
				samplePeriodMs);
			PrintDebug(debugBuff);
		#endif
	}

	template <class dataType, class floatType> dataType Pid<dataType, floatType>::GetKp()
	{
		return actualKp;
	}

	template <class dataType, class floatType> dataType Pid<dataType, floatType>::GetKi()
	{
		return actualKi;
	}

	template <class dataType, class floatType> dataType Pid<dataType, floatType>::GetKd()
	{
		return actualKd;
	}

	template <class dataType, class floatType> dataType Pid<dataType, floatType>::GetZp()
	{
		return Zp;
	}

	template <class dataType, class floatType> dataType Pid<dataType, floatType>::GetZi()
	{
		return Zi;
	}

	template <class dataType, class floatType> dataType Pid<dataType, floatType>::GetZd()
	{
		return Zd;
	}
	
	template <class dataType, class floatType> void Pid<dataType, floatType>::SetSamplePeriod(floatType newSamplePeriodMs)
	{
	   if (newSamplePeriodMs > 0)
	   {
	      dataType ratio  = (dataType)newSamplePeriodMs
	                      / floatType(samplePeriodMs);
	      Zi *= ratio;
	      Zd /= ratio;
	      samplePeriodMs = newSamplePeriodMs;
	   }
	}

	template <class dataType, class floatType> void Pid<dataType, floatType>::SetOutputLimits(dataType min, dataType max)
	{
		if(min >= max) 
	   		return;
	   	outMin = min;
	   	outMax = max;
	 
	}

	template <class dataType, class floatType> void Pid<dataType, floatType>::SetControllerDirection(ctrlDir_t controllerDir)
	{
		if(controllerDir != controllerDir)
		{
	   		// Invert control constants
			Zp = (0 - Zp);
	    	Zi = (0 - Zi);
	    	Zd = (0 - Zd);
		}   
	   this->controllerDir = controllerDir;
	}

	template <class dataType, class floatType> void Pid<dataType, floatType>::PrintDebug(const char* msg)
	{
		// Support for multiple platforms
		#if(__linux)
			printf("%s", (const char*)msg);
		#endif
	}

} // namespace Pid

#endif // #ifndef PID_H

// EOF
