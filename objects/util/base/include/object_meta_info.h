/*
 * object_meta_info.h
 * Created: 03/02/2007
 * Version: 04/20/2007
 *
 * This software, which is provided in confidence, was prepared by employees
 * of Pacific Northwest National Laboratory operated by Battelle Memorial
 * Institute. Battelle has certain unperfected rights in the software
 * which should not be copied or otherwise disseminated outside your
 * organization without the express written authorization from Battelle.
 * All rights to the software are reserved by Battelle.   Battelle makes no
 * warranty, express or implied, and assumes no liability or responsibility
 * for the use of this software.
 */

#if !defined( __OBJECT_META_INFO_H )
#define __OBJECT_META_INFO_H    // prevent multiple includes

// include files ***********************************************************

#include "util/base/include/xml_pair.h"
#include <xercesc/dom/DOMNodeList.hpp>

// namespaces **************************************************************

namespace ObjECTS {

// class: TObjectMetaInfo **************************************************

template <class T = double>
class TObjectMetaInfo
{
public :

   typedef T   value_type;

   //! Default constructor
   TObjectMetaInfo(void) : mName(), mValue() {}
   /*! Copy constructor
    *  \param other the instance to copy
    */
   TObjectMetaInfo(const TObjectMetaInfo<T>& other)
      : mName( other.mName ), mValue( other.mValue ) {}

   //! Destructor
   virtual ~TObjectMetaInfo(void) {}

   /*! Assignment operator
    *  \param other the instance to copy
    *  \return *this
    */
   TObjectMetaInfo<T>& operator = (const TObjectMetaInfo<T>& other)
   {
      if ( &other != this )
      {
         mName  = other.mName;
         mValue = other.mValue;
      }
      return *this;
   }

   /*! Get the name
    *  \return the name
    */
   virtual const std::string& getName( void ) const { return mName; }

   /*! Get the value
    *  \return the value
    */
   virtual const value_type& getValue( void ) const { return mValue; }

   /*! Get the XML tag name
    *  \return the XML tag name
    */
   static const std::string& getXMLNameStatic( void );

   /*! Set the name
    *  \param aName the name to set
    */
   virtual void setName( const std::string& aName ) { mName = aName; }

   /*! Set the value
    *  \param aValue the value to set
    */
   virtual void setValue( const value_type& aValue ) { mValue = aValue; }

  /*! Parse XML from the specified node
    *  \param apNode The current node of a DOM tree.
    *  \return Whether the parse completed successfully.
    */
   virtual bool XMLParse( const xercesc::DOMNode* apNode );
   /*! \brief Serialize the object to an output stream in an XML format.
    *  \details Function which writes out all data members of an object which are
    *           necessary to duplicate a model run. This should not include
    *           internal state variables, only variables that were read-in or
    *           changed by calibration.
    *  \param aOut Stream into which to write.
    *  \param aTabs Object which controls formatting of the file.
    */

   virtual void toInputXML(
      std::ostream& aOut,
      Tabs*         aTabs ) const;

   /*! \brief Serialize the object to an output stream in an XML format.
    *  \details Function which writes out all data members of an object which are
    *           necessary to duplicate a model run. This should not include
    *           internal state variables, only variables that were read-in or
    *           changed by calibration.
    *  \param aPeriod the period for which to report.
    *  \param aOut Stream into which to write.
    *  \param aTabs Object which controls formatting of the file.
    */
   virtual void toDebugXML(
      const int     aPeriod,
      std::ostream& aOut,
      Tabs*         aTabs ) const;

private :

   std::string mName;
   value_type  mValue;
};

// TObjectMetaInfoGetXMLName ***********************************************

/*! Get the XML tag name
 *  \return the XML tag name
 */
inline const std::string& TObjectMetaInfoGetXMLName( void )
{
   static const std::string XMLName = "object-meta-info";
   return XMLName;
}

// TObjectMetaInfo<T>::getXMLNameStatic ************************************

/*! Get the XML tag name
 *  \return the XML tag name
 */
template <class T>
inline const std::string& TObjectMetaInfo<T>::getXMLNameStatic( void )
{
   return TObjectMetaInfoGetXMLName();
}

// TObjectMetaInfo<T>::XMLParse ********************************************

/*! Parse XML from the specified node
 *  \param aNode The current node of a DOM tree.
 *  \return Whether the parse completed successfully.
 */
template <class T>
inline bool TObjectMetaInfo<T>::XMLParse( const xercesc::DOMNode* apNode )
{
   typedef XMLPair<T> value_pair_type;

   if ( !apNode || apNode->getNodeType() != xercesc::DOMNode::ELEMENT_NODE )
   {
      return false;
   }
   XMLSize_t numParsed = 0;

   // get the name attribute
   setName( XMLHelper<std::string>::getAttr( apNode, "name" ) );
   if ( getName().length() )
   {
      ++numParsed;
   }

   // get all the children.
   xercesc::DOMNodeList* pNodeList = apNode->getChildNodes();
   XMLSize_t             n         = pNodeList ? pNodeList->getLength() : 0;

   for ( XMLSize_t i = 0; i != n; ++i )
   {
      const xercesc::DOMNode* pCurr = pNodeList->item( i );
      if ( !pCurr )
      {
         return false;
      }

      const std::string nodeName =
         XMLHelper<std::string>::safeTranscode( pCurr->getNodeName() );

      if( nodeName == "#text" )
      {
         continue;
      }
      else if( nodeName == "value" )
      {
         value_pair_type np;
         if ( !np.parse( pCurr ) )
         {
            return false;
         }
         setValue( np.getValue() );
         ++numParsed;
      }
      else
      {
         ILogger& mainLog = ILogger::getLogger( "main_log" );
         mainLog.setLevel( ILogger::WARNING );
         mainLog << "Unrecognized text string: " << nodeName
            << " found while parsing "
            << getXMLNameStatic() << "." << std::endl;
         return false;
      }
   }

   return numParsed == 2;
}


// TObjectMetaInfo<T>::toInputXML ******************************************

/*! \brief Serialize the object to an output stream in an XML format.
 *  \details Function which writes out all data members of an object which are
 *           necessary to duplicate a model run. This should not include
 *           internal state variables, only variables that were read-in or
 *           changed by calibration.
 *  \param aOut Stream into which to write.
 *  \param aTabs Object which controls formatting of the file.
 */
template <class T>
inline void TObjectMetaInfo<T>::toInputXML( std::ostream& aOut, Tabs* aTabs ) const
{
   XMLWriteOpeningTag( getXMLNameStatic(), aOut, aTabs, mName );
   XMLWriteElement( mValue, "value", aOut, aTabs );
   XMLWriteClosingTag( getXMLNameStatic(), aOut, aTabs );
}

// TObjectMetaInfo<T>::toDebugXML ******************************************

/*! \brief Serialize the object to an output stream in an XML format.
 *  \details Function which writes out all data members of an object which are
 *           necessary to duplicate a model run. This should not include
 *           internal state variables, only variables that were read-in or
 *           changed by calibration.
 *  \param aPeriod the period for which to report.
 *  \param aOut Stream into which to write.
 *  \param aTabs Object which controls formatting of the file.
 */
template <class T>
inline void TObjectMetaInfo<T>::toDebugXML( const int aPeriod,
   std::ostream& aOut, Tabs* aTabs ) const
{
   XMLWriteOpeningTag( getXMLNameStatic(), aOut, aTabs, mName );
   XMLWriteElement( mValue, "value", aOut, aTabs );
   XMLWriteClosingTag( getXMLNameStatic(), aOut, aTabs );
}

}  // namespace ObjECTS

#endif   // __OBJECT_META_INFO_H

// end of object_meta_info.h ***********************************************

