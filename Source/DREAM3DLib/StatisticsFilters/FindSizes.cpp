/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindSizes.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/PrivateFilters/FindBoundingBoxGrains.h"
#include "DREAM3DLib/PrivateFilters/FindGrainPhases.h"

const static float m_pi = M_PI;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSizes::FindSizes() :
AbstractFilter(),
m_GrainIds(NULL),
m_Volumes(NULL),
m_EquivalentDiameters(NULL),
m_NumCells(NULL)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSizes::~FindSizes()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1);

  GET_PREREQ_DATA(m, DREAM3D, FieldData, BiasedFields, ss, -301, bool, BoolArrayType, fields, 1);
  if(getErrorCondition() == -301)
  {
	setErrorCondition(0);
	FindBoundingBoxGrains::Pointer find_boundingboxfields = FindBoundingBoxGrains::New();
	find_boundingboxfields->setObservers(this->getObservers());
	find_boundingboxfields->setDataContainer(getDataContainer());
	if(preflight == true) find_boundingboxfields->preflight();
	if(preflight == false) find_boundingboxfields->execute();
	GET_PREREQ_DATA(m, DREAM3D, FieldData, BiasedFields, ss, -301, bool, BoolArrayType, fields, 1);
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Phases, ss, -302, int32_t, Int32ArrayType, fields, 1);
  if(getErrorCondition() == -302)
  {
	setErrorCondition(0);
	FindGrainPhases::Pointer find_grainphases = FindGrainPhases::New();
	find_grainphases->setObservers(this->getObservers());
	find_grainphases->setDataContainer(getDataContainer());
	if(preflight == true) find_grainphases->preflight();
	if(preflight == false) find_grainphases->execute();
	GET_PREREQ_DATA(m, DREAM3D, FieldData, Phases, ss, -301, int32_t, Int32ArrayType, fields, 1);
  }
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, ss, float, FloatArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, float,FloatArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, NumCells, ss, int32_t, Int32ArrayType, fields, 1);

  m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getEnsembleData(DREAM3D::EnsembleData::Statistics).get());
  if(m_StatsDataArray == NULL)
  {
	StatsDataArray::Pointer p = StatsDataArray::New();
	m_StatsDataArray = p.get();
	m_StatsDataArray->fillArrayWithNewStatsData(ensembles);
	m->addEnsembleData(DREAM3D::EnsembleData::Statistics, p);
  }

  setErrorMessage(ss.str());
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::execute()
{
  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }
  setErrorCondition(0);

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }


  if(m->getZPoints() > 1) find_sizes();
  if(m->getZPoints() == 1) find_sizes2D();
  notify("FindSizes Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::find_sizes()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->getTotalPoints();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float radcubed;
  float diameter;
  std::vector<float> avgdiam;
  std::vector<float> sddiam;
  std::vector<float> maxdiam;
  std::vector<float> mindiam;
  std::vector<float> binstepsize;
  std::vector<FloatArrayType::Pointer> binnumbers;
  std::vector<size_t> unbiasedcount;
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();

  avgdiam.resize(numensembles,0);
  sddiam.resize(numensembles,0);
  maxdiam.resize(numensembles,0);
  mindiam.resize(numensembles,1000000);
  binstepsize.resize(numensembles,0);
  binnumbers.resize(numensembles);
  unbiasedcount.resize(numensembles,0);

  DataArray<float>::Pointer m_GrainCounts = DataArray<float>::CreateArray(numgrains, "GrainCounts");
  float* graincounts = m_GrainCounts->GetPointer(0);


  // Initialize every element to 0.0
  for (size_t i = 0; i < numgrains * 1; i++)
  {
    graincounts[i] = 0.0f;
  }
  for (int j = 0; j < totalPoints; j++)
  {
    int gnum = m_GrainIds[j];
    graincounts[gnum]++;
  }
  float res_scalar = m->getXRes() * m->getYRes() * m->getZRes();
  float vol_term = (4.0/3.0)*m_pi;
  for (size_t i = 1; i < numgrains; i++)
  {
    m_NumCells[i] = graincounts[i];
    m_Volumes[i] = (graincounts[i] * res_scalar);
    radcubed = m_Volumes[i]/vol_term;
    diameter = 2.0f*powf(radcubed, 0.3333333333f);
    m_EquivalentDiameters[i] = diameter;
	if(m_BiasedFields[i] == false)
	{
		unbiasedcount[m_Phases[i]]++;
		avgdiam[m_Phases[i]] = avgdiam[m_Phases[i]] + logf(m_EquivalentDiameters[i]);
		if(m_EquivalentDiameters[i] > maxdiam[m_Phases[i]]) maxdiam[m_Phases[i]] = m_EquivalentDiameters[i];
		if(m_EquivalentDiameters[i] < mindiam[m_Phases[i]]) mindiam[m_Phases[i]] = m_EquivalentDiameters[i];
	}
  }
  for (size_t i = 1; i < numensembles; i++)
  {
	  avgdiam[i] = avgdiam[i]/float(unbiasedcount[i]);
	  statsDataArray[i]->setGrainSizeAverage(avgdiam[i]);
  }
  for (size_t i = 1; i < numgrains; i++)
  {
	if(m_BiasedFields[i] == false)
	{
		sddiam[m_Phases[i]] = sddiam[m_Phases[i]] + ((logf(m_EquivalentDiameters[i])-avgdiam[m_Phases[i]])*(logf(m_EquivalentDiameters[i])-avgdiam[m_Phases[i]]));
	}
  }
  for (size_t i = 1; i < numensembles; i++)
  {
	  sddiam[i] = sddiam[i]/float(unbiasedcount[i]);
	  sddiam[i] = sqrt(sddiam[i]);
	  statsDataArray[i]->setGrainSizeStdDev(sddiam[i]);
  }
  for (size_t i = 1; i < numensembles; i++)
  {
	  binnumbers[i] = FloatArrayType::CreateArray(10);
	  binnumbers[i]->SetName(DREAM3D::HDF5::BinNumber);
	  float stepsize = (maxdiam[i]-mindiam[i])/10.0;
	  statsDataArray[i]->setBinStepSize(stepsize);
	  statsDataArray[i]->setMaxGrainDiameter(maxdiam[i]);
	  statsDataArray[i]->setMinGrainDiameter(mindiam[i]);
	  for (size_t j = 0; j < 10; j++)
	  {
		  float value = mindiam[i] + (float(j)*stepsize);
		  binnumbers[i]->SetValue(j,value);
	  }
	  statsDataArray[i]->setBinNumbers(binnumbers[i]);
  }
}
void FindSizes::find_sizes2D()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->getTotalPoints();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float radsquared;
  float diameter;
  std::vector<float> avgdiam;
  std::vector<float> sddiam;
  std::vector<float> maxdiam;
  std::vector<float> mindiam;
  std::vector<float> binstepsize;
  std::vector<FloatArrayType::Pointer> binnumbers;
  std::vector<size_t> unbiasedcount;
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();

  avgdiam.resize(numensembles,0);
  sddiam.resize(numensembles,0);
  maxdiam.resize(numensembles,0);
  mindiam.resize(numensembles,1000000);
  binstepsize.resize(numensembles,0);
  binnumbers.resize(numensembles);
  unbiasedcount.resize(numensembles,0);

  DataArray<float>::Pointer m_GrainCounts = DataArray<float>::CreateArray(numgrains, "GrainCounts");
  float* graincounts = m_GrainCounts->GetPointer(0);


  for (size_t i = 0; i < numgrains; i++)
  {
      graincounts[i] = 0.0f;
  }
  for (int j = 0; j < totalPoints; j++)
  {
    int gnum = m_GrainIds[j];
    graincounts[gnum]++;
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    m_NumCells[i] = graincounts[i];
    m_Volumes[i] = (graincounts[i] * m->getXRes() * m->getYRes());
    radsquared = m_Volumes[i] / m_pi;
    diameter = (2 * sqrt(radsquared));
    m_EquivalentDiameters[i] = diameter;
	if(m_BiasedFields[i] == false)
	{
		unbiasedcount[m_Phases[i]]++;
		avgdiam[m_Phases[i]] = avgdiam[m_Phases[i]] + logf(m_EquivalentDiameters[i]);
		if(m_EquivalentDiameters[i] > maxdiam[m_Phases[i]]) maxdiam[m_Phases[i]] = m_EquivalentDiameters[i];
		if(m_EquivalentDiameters[i] < mindiam[m_Phases[i]]) mindiam[m_Phases[i]] = m_EquivalentDiameters[i];
	}
  }
  for (size_t i = 1; i < numensembles; i++)
  {
	  avgdiam[i] = avgdiam[i]/float(unbiasedcount[i]);
	  statsDataArray[i]->setGrainSizeAverage(avgdiam[i]);
  }
  for (size_t i = 1; i < numgrains; i++)
  {
	if(m_BiasedFields[i] == false)
	{
		sddiam[m_Phases[i]] = sddiam[m_Phases[i]] + ((logf(m_EquivalentDiameters[i])-avgdiam[m_Phases[i]])*(logf(m_EquivalentDiameters[i])-avgdiam[m_Phases[i]]));
	}
  }
  for (size_t i = 1; i < numensembles; i++)
  {
	  sddiam[i] = sddiam[i]/float(unbiasedcount[i]);
	  sddiam[i] = sqrt(sddiam[i]);
	  statsDataArray[i]->setGrainSizeStdDev(sddiam[i]);
  }
  for (size_t i = 1; i < numensembles; i++)
  {
	  binnumbers[i] = FloatArrayType::CreateArray(10);
	  binnumbers[i]->SetName(DREAM3D::HDF5::BinNumber);
	  float stepsize = (maxdiam[i]-mindiam[i])/10.0;
	  statsDataArray[i]->setBinStepSize(stepsize);
	  statsDataArray[i]->setMaxGrainDiameter(maxdiam[i]);
	  statsDataArray[i]->setMinGrainDiameter(mindiam[i]);
	  for (size_t j = 0; j < 10; j++)
	  {
		  float value = mindiam[i] + (float(j)*stepsize);
		  binnumbers[i]->SetValue(j,value);
	  }
	  statsDataArray[i]->setBinNumbers(binnumbers[i]);
  }
}

