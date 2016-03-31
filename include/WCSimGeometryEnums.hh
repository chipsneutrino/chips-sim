/*
 * WCSimGeometryEnums.hh
 *
 *  Created on: 24 Nov 2014
 *      Author: ajperch
 */

#ifndef WCSIMGEOMETRYENUMS_HH_
#define WCSIMGEOMETRYENUMS_HH_
#include <vector>
#include <iostream>
#include <cassert>

namespace WCSimGeometryEnums
{

	struct PhotodetectorLimit_t
	{
		enum Type{
			kUnknown = 0,
			kPercentCoverage = 1,
			kTotalNumber = 2,
			kZonalCoverage = 3
		};
		Type fType;

		PhotodetectorLimit_t(Type t = kUnknown) : fType(t){}
		PhotodetectorLimit_t(const PhotodetectorLimit_t &ep) : fType(ep.fType) {};
		PhotodetectorLimit_t(const std::string typeName) : fType(kUnknown)
		{
			if( typeName == "kUnknown" ){ fType = kUnknown; }
			else if( typeName == "kPercentCoverage" ){ fType = kPercentCoverage; }
			else if( typeName == "kTotalNumber" ){ fType = kTotalNumber; }
			else if( typeName == "kZonalCoverage" ){ fType = kZonalCoverage; }
			else{ fType = kUnknown; }
			if( fType == kUnknown )
			{
				std::cerr << "Error: Unknown PMT limit: \"" << typeName << "\"" << std::endl;
				assert(0);
			}
		}
		operator Type () const { return fType; };

		std::string AsString() const{
			return AsString(fType);
		}

		static std::vector<PhotodetectorLimit_t> GetAllTypes(){
			std::vector<PhotodetectorLimit_t> myVec;
			myVec.push_back(kPercentCoverage);
			myVec.push_back(kTotalNumber);
			myVec.push_back(kZonalCoverage);
			return myVec;
		}

    static std::vector<std::string> GetAllTypeNames(){
			std::vector<std::string> myVec;
      std::vector<PhotodetectorLimit_t> pmtVec = GetAllTypes();
      std::vector<PhotodetectorLimit_t>::const_iterator pmtIter = pmtVec.begin();
      while(pmtIter != pmtVec.end())
      {
        myVec.push_back(pmtIter->AsString());
        ++pmtIter;
      }
			return myVec;
		}

		static std::string AsString(Type type){
			std::string str("");
			if( type == kUnknown )  { str = "kUnknown"; }
			if( type == kPercentCoverage )  { str = "kPercentCoverage"; }
			if( type == kTotalNumber )  { str = "kTotalNumber"; }
			if( type == kZonalCoverage) { str = "kZonalCoverage"; }
			return str;
		};
	};

	struct DetectorRegion_t
	{
		enum Type{
			kUnknown = 0,
			kWall = 1,
			kTop = 2,
			kBottom = 3
		};
		Type fType;

		DetectorRegion_t(Type t = kUnknown) : fType(t){}
		DetectorRegion_t(const DetectorRegion_t &ep) : fType(ep.fType) {};
		DetectorRegion_t(const std::string typeName) : fType(kUnknown)
		{
			if( typeName == "kWall" ){ fType = kWall; }
			else if( typeName == "kTop" ){ fType = kTop; }
			else if( typeName == "kBottom" ){ fType = kBottom; }
			else{ fType = kUnknown; }
			if( fType == kUnknown )
			{
				std::cerr << "Error: Unknown detector location: \"" << typeName << "\"" << std::endl;
				assert(0);
			}
		}

		operator Type () const { return fType; };

		std::string AsString() const{
			return AsString(fType);
		}

		static std::vector<DetectorRegion_t> GetAllTypes(){
			std::vector<DetectorRegion_t> myVec;
			myVec.push_back(kWall);
			myVec.push_back(kTop);
			myVec.push_back(kBottom);
			return myVec;
		}
    
    static std::vector<std::string> GetAllTypeNames(){
			std::vector<std::string> myVec;
      std::vector<DetectorRegion_t> pmtVec = GetAllTypes();
      std::vector<DetectorRegion_t>::const_iterator pmtIter = pmtVec.begin();
      while(pmtIter != pmtVec.end())
      {
        myVec.push_back(pmtIter->AsString());
        ++pmtIter;
      }
			return myVec;
		}

		static std::string AsString(Type type){
			std::string str("");
			if( type == kUnknown )  { str = "kUnknown"; }
			if( type == kWall )  { str = "kWall"; }
			if( type == kTop )  { str = "kTop"; }
			if( type == kBottom )  { str = "kBottom"; }
			return str;
		};

	};


	struct PMTDirection_t
	{
		enum Type{
			kUnknown = 0,
			kInwards = 1,
			kOutwards = 2,
			kAngledUp = 3,
			kAngledDown = 4,
			kAngledUpstream = 5,
			kAngledDownstream = 6,
			kArbitrary = 7
		};
		Type fType;

		PMTDirection_t(Type t = kUnknown) : fType(t){}
		PMTDirection_t(const PMTDirection_t &ep) : fType(ep.fType) {};
		operator Type () const { return fType; };
		bool CanBuildWithoutAngles(){
			return (fType == kInwards || fType == kOutwards  || fType == kAngledUpstream );
		}

		std::string AsString() const{
			return AsString(fType);
		}

		static std::vector<PMTDirection_t> GetAllTypes(){
			std::vector<PMTDirection_t> myVec;
			myVec.push_back(kInwards);
			myVec.push_back(kOutwards);
			myVec.push_back(kAngledUp);
			myVec.push_back(kAngledDown);
			myVec.push_back(kAngledUpstream);
			myVec.push_back(kAngledDownstream);
			myVec.push_back(kArbitrary);
			return myVec;
		}
		
    static std::vector<std::string> GetAllTypeNames(){
			std::vector<std::string> myVec;
      std::vector<PMTDirection_t> pmtVec = GetAllTypes();
      std::vector<PMTDirection_t>::const_iterator pmtIter = pmtVec.begin();
      while(pmtIter != pmtVec.end())
      {
        myVec.push_back(pmtIter->AsString());
        ++pmtIter;
      }
			return myVec;
		}

		static std::string AsString(Type type){
			std::string str("");
			if( type == kUnknown )  { str = "kUnknown"; }
			if( type == kInwards )  { str = "kInwards"; }
			if( type == kOutwards )  { str = "kOutwards"; }
			if( type == kAngledUp )  { str = "kAngledUp"; }
			if( type == kAngledDown )  { str = "kAngledDown"; }
			if( type == kAngledUpstream )  { str = "kAngledUpstream"; }
			if( type == kAngledDownstream )  { str = "kAngledDownstream"; }
			if( type == kArbitrary )  { str = "kArbitrary"; }
			return str;
		};

		static PMTDirection_t FromString(std::string string)
		{
			Type type = kUnknown;
			if( string == "kUnknown" )  { type = kUnknown; }
			if( string == "kInwards" )  { type = kInwards; }
			if( string == "kOutwards" )  { type = kOutwards; }
			if( string == "kAngledUp" )  { type = kAngledUp; }
			if( string == "kAngledDown" )  { type = kAngledDown; }
			if( string == "kAngledUpstream" )  { type = kAngledUpstream; }
			if( string == "kAngledDownstream" )  { type = kAngledDownstream; }
			if( string == "kArbitrary" )  { type = kArbitrary; }
			if( type == kUnknown ) { std::cerr << "Error: unknown PMT direction type \"" << string << "\"" << std::endl; assert(0); }
			return PMTDirection_t(type);
		}
	};

}



#endif /* WCSIMGEOMETRYENUMS_HH_ */
