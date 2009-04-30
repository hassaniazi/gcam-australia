/*
 * LEGAL NOTICE
 * This computer software was prepared by Battelle Memorial Institute,
 * hereinafter the Contractor, under Contract No. DE-AC05-76RL0 1830
 * with the Department of Energy (DOE). NEITHER THE GOVERNMENT NOR THE
 * CONTRACTOR MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
 * LIABILITY FOR THE USE OF THIS SOFTWARE. This notice including this
 * sentence must appear on any copies of this computer software.
 * 
 * EXPORT CONTROL
 * User agrees that the Software will not be shipped, transferred or
 * exported into any country or used in any manner prohibited by the
 * United States Export Administration Act or any other applicable
 * export laws, restrictions or regulations (collectively the "Export Laws").
 * Export of the Software may require some form of license or other
 * authority from the U.S. Government, and failure to obtain such
 * export control license may result in criminal liability under
 * U.S. laws. In addition, if the Software is identified as export controlled
 * items under the Export Laws, User represents and warrants that User
 * is not a citizen, or otherwise located within, an embargoed nation
 * (including without limitation Iran, Syria, Sudan, Cuba, and North Korea)
 *     and that User is not otherwise prohibited
 * under the Export Laws from receiving the Software.
 * 
 * All rights to use the Software are granted on condition that such
 * rights are forfeited if User fails to comply with the terms of
 * this Agreement.
 * 
 * User agrees to identify, defend and hold harmless BATTELLE,
 * its officers, agents and employees from all liability involving
 * the violation of such Export Laws, either directly or indirectly,
 * by User.
 */

/*! 
 * \file complex_carbon_printer.cpp
 * \ingroup Objects
 * \brief The ComplexCarbonPrinter class source file.
 * \author Jim Naslund
 */

#include "util/base/include/definitions.h"

#include "reporting/include/complex_carbon_printer.h"
#include "containers/include/region.h"
#include "util/base/include/util.h"
#include "ccarbon_model/include/carbon_box.h"
#include "ccarbon_model/include/carbon_box_model.h"
#include "ccarbon_model/include/npp.h"
#include "ccarbon_model/include/acarbon_flow.h"
#include "util/base/include/model_time.h"
#include "containers/include/scenario.h"

extern Scenario* scenario; // for modeltime

using namespace std;

/*! 
 * \brief Default Constructor
 * \param aRegionToPrint region to print.
 * \param aFile file to print to.
 */
ComplexCarbonPrinter::ComplexCarbonPrinter( const string& aRegionToPrint,
                                            ostream& aFile ):
mFile( aFile ),
mCorrectRegion( false ),
mRegionToPrint( aRegionToPrint ),
mTabs( Tabs() )
{
}

/*!
 * \brief Begin visiting a region with the graph printer.
 * \details Opens the graph and prints the header.
 * \param aRegion Region to visit.
 * \param aPeriod Period for which to visit.
 */
void ComplexCarbonPrinter::startVisitRegion( const Region* aRegion, const int aPeriod ){
    // Check if this is the region to print.
    if(aRegion->getName() == mRegionToPrint ){
        mCorrectRegion = true;
    }
    else {
        // Don't print this region.
        mCorrectRegion = false;
    }
}

/*!
 * \brief End visiting a region with the graph printer.
 * \details Closes the graph.
 * \param aRegion Region to visit.
 * \param aPeriod Period for which to visit.
 */
void ComplexCarbonPrinter::endVisitRegion( const Region* aRegion, const int aPeriod ){
}

void ComplexCarbonPrinter::startVisitCarbonCalc( const ICarbonCalc* aCarbonCalc,
                                                 const int aPeriod ){
    XMLWriteOpeningTag( "period", mFile, &mTabs, "", aPeriod );
    XMLWriteOpeningTag( CarbonBoxModel::getXMLNameStatic(), mFile, &mTabs );
}
void ComplexCarbonPrinter::endVisitCarbonCalc( const ICarbonCalc* aCarbonCalc,
                                               const int aPeriod ){
    XMLWriteClosingTag( CarbonBoxModel::getXMLNameStatic(), mFile, &mTabs );
    XMLWriteClosingTag( "period", mFile, &mTabs );
}

void ComplexCarbonPrinter::startVisitCarbonBox( const CarbonBox* aCarbonBox,
                                                const int aPeriod ){
    XMLWriteOpeningTag( CarbonBox::getXMLNameStatic(), mFile, &mTabs, aCarbonBox->getName() );
    XMLWriteElement( aCarbonBox->mStock->getStock( aPeriod ),
                     "carbon-stock", mFile, &mTabs );
    XMLWriteClosingTag( CarbonBox::getXMLNameStatic(), mFile, &mTabs ); 
}

void ComplexCarbonPrinter::startVisitCarbonFlow( const ACarbonFlow* aCarbonFlow,
                           const int aPeriod ){
   // mFile << mCurrentBox << "->" << aCarbonFlow->getTargetName()
   //       << "[label=\"" << aCarbonFlow->mFraction << "\"];" << endl;
}
