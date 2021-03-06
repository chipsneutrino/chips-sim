/*
 * WCSimPMTBuilder.h
 *
 *  Created on: 3 Sep 2014
 *      Author: andy
 */

#pragma once

#include <vector>
#include <map>

class WCSimPMTConfig;
class WCSimWCSD;
class G4LogicalVolume;

class WCSimGeantPMTWrapper
{
public:
	WCSimGeantPMTWrapper();
	WCSimGeantPMTWrapper(G4LogicalVolume *PMTLogicalVolume, G4LogicalVolume *glassFaceLogicalVolume);
	void Delete();
	void Reset();
	G4LogicalVolume *GetPMTLogicalVolume() const;
	G4LogicalVolume *GetGlassFaceLogicalVolume() const;

private:
	G4LogicalVolume *fPMTLogicalVolume;
	G4LogicalVolume *fGlassFaceLogicalVolume;
};

class WCSimPMTBuilder
{
public:
	WCSimPMTBuilder();
	virtual ~WCSimPMTBuilder();
	G4LogicalVolume *GetPMTLogicalVolume(WCSimPMTConfig config);
	G4LogicalVolume *GetGlassFaceLogicalVolume(WCSimPMTConfig config);
	void ConstructPMTs(std::vector<WCSimPMTConfig> configVec);
	void SetSensitiveDetector(WCSimWCSD *sensDet);
	void Reset();

private:
	void ConstructPMT(WCSimPMTConfig config);
	std::map<std::string, WCSimGeantPMTWrapper> fPMTLogicalVolumes;
};
