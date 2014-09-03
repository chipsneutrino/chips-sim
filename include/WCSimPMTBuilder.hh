/*
 * WCSimPMTBuilder.h
 *
 *  Created on: 3 Sep 2014
 *      Author: andy
 */

#ifndef WCSIMPMTBUILDER_H_
#define WCSIMPMTBUILDER_H_

#include <vector>
#include <map>

class WCSimPMTConfig;
class G4LogicalVolume;


class WCSimPMTBuilder {
public:
	WCSimPMTBuilder();
	virtual ~WCSimPMTBuilder();
	G4LogicalVolume * GetPMTLogicalVolume(WCSimPMTConfig config);
	void ConstructPMTs(std::vector<WCSimPMTConfig> configVec);

private:
	void ConstructPMT(WCSimPMTConfig config);
	std::map<std::string, G4LogicalVolume*> fPMTLogicalVolumes;
};

#endif /* WCSIMPMTBUILDER_H_ */
