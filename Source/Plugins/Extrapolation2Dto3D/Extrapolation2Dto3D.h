

#ifndef Extrapolation2Dto3D_H_
#define Extrapolation2Dto3D_H_

#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/MXATypes.h>

#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/AbstractPipeline.h"
#include "Extrapolation2Dto3DFunc.h"



/**
* @class Extrapolation2Dto3D Extrapolation2Dto3D.h AIM/Extrapolation2Dto3D/Extrapolation2Dto3D.h
* @brief This class serves as the main entry point to execute the Extrapolation2Dto3D
* pipeline
* @author 
* @author 
* @date 
* @version 1.0
*/
class DREAM3DLib_EXPORT Extrapolation2Dto3D : public AbstractPipeline
{
  public:
    MXA_SHARED_POINTERS(Extrapolation2Dto3D);
    MXA_TYPE_MACRO(Extrapolation2Dto3D);

    MXA_STATIC_NEW_MACRO(Extrapolation2Dto3D);

    virtual ~Extrapolation2Dto3D();

    /**
    * @brief Main method to run the operation
    */
    virtual void execute();

  protected:
    Extrapolation2Dto3D();

  private:
    Extrapolation2Dto3DFunc::Pointer m;

    Extrapolation2Dto3D(const Extrapolation2Dto3D&);    // Copy Constructor Not Implemented
    void operator=(const Extrapolation2Dto3D&);  // Operator '=' Not Implemented
};



#endif /* Extrapolation2Dto3D_H_ */
