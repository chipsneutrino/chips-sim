#include "WCSimDetectorConstruction.hh"
#include "WCSimPmtInfo.hh"
#include "WCSimPMTConfig.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Vector3D.hh"
#include "globals.hh"
#include "G4VisAttributes.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "CLHEP/Units/SystemOfUnits.h"

#include <sstream>
#include <iomanip>

using std::setw;
// These routines are object registration routines that you can pass
// to the traversal code.

void WCSimDetectorConstruction::PrintGeometryTree(G4VPhysicalVolume *aPV, int aDepth, int replicaNo,
												  const G4Transform3D &aTransform)
{
	for (int levels = 0; levels < aDepth; levels++)
		G4cout << " ";
	G4cout << aPV->GetName() << " Level:" << aDepth << " Pos:" << aTransform.getTranslation() << " Rot:"
		   << aTransform.getRotation().getTheta() / CLHEP::deg << "," << aTransform.getRotation().getPhi() / CLHEP::deg << ","
		   << aTransform.getRotation().getPsi() / CLHEP::deg << G4endl;
}

void WCSimDetectorConstruction::GetWCGeom(G4VPhysicalVolume *aPV, int aDepth, int replicaNo,
										  const G4Transform3D &aTransform)
{

	// Grab mishmash of useful information from the tree while traversing it
	// This information will later be written to the geometry file
	// (Alternatively one might define accessible constants)

	//aah original line-> if ((aPV->GetName() == "WCBarrel")){
	if ((aPV->GetName() == "WCBarrel") || (aPV->GetName() == "physiMBTank"))
	{ //last condition is the Mailbox Physical Tank name.
		// Stash info in data member
		WCOffset = G4ThreeVector(aTransform.getTranslation().getX() / CLHEP::cm, aTransform.getTranslation().getY() / CLHEP::cm,
								 aTransform.getTranslation().getZ() / CLHEP::cm);
	}

	//aah---original line->  if ((aPV->GetName() == "WCCapPMTGlass") || (aPV->GetName() == "WCPMTGlass")){
	//if ((aPV->GetName().contains("PMTGlass"))){    //aah--new version

	//(JF) all volumes have the same name now
	//    if ((aPV->GetName() == "WCPMTGlass"){
	// Assume all the PMTs are the same size
	// G4Sphere* solidWCPMT = (G4Sphere *)aPV->GetLogicalVolume()->GetSolid();
	//G4double outerRadius = solidWCPMT->GetOuterRadius()/CLHEP::cm;

	// Stash info in data member
	// AH Need to store this in CM for it to be understood by SK code
	//    WCPMTSize = WCPMTRadius/CLHEP::cm;// I think this is just a variable no if needed
	WCPMTSize = fPMTConfigs[0].GetRadius() / CLHEP::cm; // I think this is just a variable no if needed
														//  }
														// (JF) None of this is needed anymore.  Values are calculated when writing file
														/*
			 WCCylInfo[0] = 0;//aah  in case not a cylinder at all--eg Mailbox

			 if ((aPV->GetName() == "WCPMTGlass")){

			 //    G4Tubs* CylInfo[0]
			 //    G4double innerRadius = solidWCBSCell->GetInnerRadius()/CLHEP::cm;
			 // M Fechner : it is no longer a cylinder

			 G4double innerRadius = sqrt(pow(aTransform.getTranslation().getX()/CLHEP::cm,2) + pow(aTransform.getTranslation().getY()/CLHEP::cm,2));
			 //    G4double innerRadius = aTransform.getTranslation().getZ()/CLHEP::cm;
			 // Stash info in data member
			 WCCylInfo[0] = innerRadius;
			 }
			 */

	// Note WC can be off-center... get both extremities
	static G4float zmin = 100000, zmax = -100000.;
	static G4float xmin = 100000, xmax = -100000.;
	static G4float ymin = 100000, ymax = -100000.;
	if (aDepth == 0)
	{ // Reset for this traversal
		xmin = 100000, xmax = -100000.;
		ymin = 100000, ymax = -100000.;
		zmin = 100000, zmax = -100000.;
	}

	// aah ->old version
	//if ((aPV->GetName() == "WCPMTGlass")){
	if ((aPV->GetName() == "WCCapBlackSheet") || (aPV->GetName() == "GlassFaceWCPMT"))
	{
		//	if ((aPV->GetName().contains("BlackSheet")) || (aPV->GetName()=="WCCapPMT") || (aPV->GetName().contains("PMTGlass_"))){//aah new version--does effectively the same thing, finds max and min z maybe less efficiently since more volumes may be found, but it is done only once per run.
		G4float x = aTransform.getTranslation().getX() / CLHEP::cm;
		G4float y = aTransform.getTranslation().getY() / CLHEP::cm;
		G4float z = aTransform.getTranslation().getZ() / CLHEP::cm;

		if (x < xmin)
		{
			xmin = x;
		}
		if (x > xmax)
		{
			xmax = x;
		}

		if (y < ymin)
		{
			ymin = y;
		}
		if (y > ymax)
		{
			ymax = y;
		}

		if (z < zmin)
		{
			zmin = z;
		}
		if (z > zmax)
		{
			zmax = z;
		}

		WCCylInfo[0] = xmax - xmin;
		WCCylInfo[1] = ymax - ymin;
		WCCylInfo[2] = zmax - zmin;
		//      G4cout << "determin hight: " << zmin << "  " << zmax << " " << aPV->GetName()<<" " << z  << G4endl;
	}
}

void WCSimDetectorConstruction::DescribeAndRegisterPMT(G4VPhysicalVolume *aPV, int aDepth, int replicaNo,
													   const G4Transform3D &aTransform)
{
	static std::string replicaNoString[20];

	std::stringstream depth;
	std::stringstream pvname;

	depth << replicaNo;
	pvname << aPV->GetName();

	replicaNoString[aDepth] = pvname.str() + "-" + depth.str();

	//aah original line->
	//if ((aPV->GetName() == "GlassFaceWCPMT"))
	if (aPV->GetName().contains("WCPMT_")) // PMTs have the name WCPMT_<pmt_name>
										   //	if ((aPV->GetName().contains("PMTGlass")))//aah I believe this works for all
	{

		std::string tubeName = aPV->GetName();
		tubeName = tubeName.substr(6);
		//    std::cout << "== Found PMT " << totalNumPMTs << ": " << tubeName  << std::endl;

		// First increment the number of PMTs in the tank.
		totalNumPMTs++;

		// Put the location of this tube into the location map so we can find
		// its ID later.  It is coded by its tubeTag string.
		// This scheme must match that used in WCSimWCSD::ProcessHits()

		std::string tubeTag;
		//    G4LogicalVolume *mother = aPV->GetMotherLogical();
		//    if (mother != NULL) {
		// Prepend name of mother if it exists to distinguish different
		// PMT hierarchies
		//      tubeTag += mother->GetName();
		//      tubeTag += ":";
		//    }

		//    tubeTag += aPV->GetName();
		for (int i = 0; i <= aDepth; i++)
			tubeTag += ":" + replicaNoString[i];
		//  G4cout << tubeTag << G4endl;

		// Since we get the PMT object, need to then add the geometry address
		// of the glasss such that we can use it later in the sensitive detector.
		tubeTag += ":GlassFaceWCPMT-0";
		tubeLocationMap[tubeTag] = totalNumPMTs;

		// Record where tube is in the cylinder
		// (JF) This distinction was useful for 2km detector
		// not so much for DUSEL
		/*
		 //aah original
		 if (aPV->GetName() == "WCPMTGlass"){
		 //if ((aPV->GetName()=="WCPMTGlass") || (aPV->GetName().contains("D_PMTGlass")))
		 //aah  also find the Mailbox pmt's on side wall (LxD_PMTGlass or WxD_PMTGlass!)

		 tubeCylLocation[totalNumPMTs] = wall;
		 }
		 else
		 {   // It's a cap pmt
		 // Get which endcap
		 if ((aPV->GetName() == "WCCapPMTGlass")) //aah
		 {									//aah
		 //// Cap tubes could be at either of two possible depths..
		 //// Get the depth of the cap, up either 3 or 4
		 ////      G4int capdepth = (aDepth==6 ? aDepth-4 : aDepth-3);
		 //// M F : NOT TRUE ANY MORE : WE REMOVED THE CAP CELLS , THE
		 //// DEPTHS ARE DIFFERENT !!!

		 G4int capdepth = 2;
		 G4int endcap = std::atoi(replicaNoString[capdepth].c_str());
		 //      G4cout << "endcap " << endcap << "\n";
		 //// If the cap is replica 1, it's endcap1.  If replica 0, it's endcap 2
		 tubeCylLocation[totalNumPMTs] = (endcap ? endcap1: endcap2);
		 }  //aah
		 else	//aah
		 {		//aah
		 if ((aPV->GetName() == "LXW_PMT_plus"))tubeCylLocation[totalNumPMTs]=endcap1;	//top plane  aah
		 if ((aPV->GetName() == "LXW_PMT_minus"))tubeCylLocation[totalNumPMTs]=endcap2;   //bottom plane  aah
		 }				//aah
		 }
		 */

		// Put the transform for this tube into the map keyed by its ID
		tubeIDMap[totalNumPMTs] = aTransform;
		tubeNameMap[totalNumPMTs] = tubeName;
		tubeTagMap[totalNumPMTs] = tubeTag;

		// G4cout <<  "depth " << depth.str() << G4endl;
		//     G4cout << "tubeLocationmap[" << tubeTag  << "]= " << tubeLocationMap[tubeTag] << "\n";
		//G4cout << "tubeCylLocation[" << totalNumPMTs  << "]= " << tubeCylLocation[totalNumPMTs] << "\n";

		// Print
		//     G4cout << "Tube: "<<std::setw(4) << totalNumPMTs << " " << tubeTag
		//     	   << " Pos:" << aTransform.getTranslation()/CLHEP::cm
		//     	   << " Rot:" << aTransform.getRotation().getTheta()/CLHEP::deg
		//     	   << "," << aTransform.getRotation().getPhi()/CLHEP::deg
		//     	   << "," << aTransform.getRotation().getPsi()/CLHEP::deg
		//     	   << G4endl;
	}
}

// Utilities to do stuff with the info we have found.

// Output to WC geometry text file
void WCSimDetectorConstruction::DumpGeometryTableToFile()
{
	// Open a file
	geoFile.open("geofile.txt", std::ios::out);

	geoFile.precision(2);
	geoFile.setf(std::ios::fixed);

	// (JF) Get first tube transform for filling in detector radius
	// the height is still done with WCCylInfo above
	G4Transform3D firstTransform = tubeIDMap[2];
	innerradius = sqrt(
		pow(firstTransform.getTranslation().getX() / CLHEP::cm, 2) + pow(firstTransform.getTranslation().getY() / CLHEP::cm, 2));

	if (isMailbox == false)
	{
		geoFile << setw(8) << innerradius;
		geoFile << setw(8) << WCCylInfo[2];
	}
	else
	{
		geoFile << setw(8) << 0;
		geoFile << setw(8) << 0;
	}
	geoFile << setw(10) << totalNumPMTs;
	geoFile << setw(8) << WCPMTSize << setw(4) << G4endl;

	geoFile << setw(8) << WCOffset(0) << setw(8) << WCOffset(1) << setw(8) << WCOffset(2) << G4endl;

	G4double maxZ = 0.0; // used to tell if pmt is on the top/bottom cap
	G4double minZ = 0.0; // or the barrel
	G4int cylLocation;

	// clear before add new stuff in
	for (int i = 0; i < fpmts.size(); i++)
	{
		delete fpmts.at(i);
	}
	fpmts.clear();

	// Firstly, count the number of vetos. Need to do this so that
	// we can put them as the final PMTs. This will really help things remain simple in
	// the reconstruction algorithms.
	fVetoPMTs = 0;
	for (int tubeID = 1; tubeID <= totalNumPMTs; tubeID++)
	{
		G4Transform3D newTransform = tubeIDMap[tubeID];
		// Get tube orientation vector
		G4Vector3D nullOrient = G4Vector3D(0, 0, 1);
		G4Vector3D pmtOrientation = newTransform * nullOrient;
		// Count it if it is a veto
		if (pmtOrientation.z() == 1)
		{
			++fVetoPMTs;
		}
	}

	// Create the PMT veto with the require number of elements.
	// We need to do this so that the tube ID will correspond to the index + 1 later on.
	for (unsigned int i = 0; i < totalNumPMTs; ++i)
	{
		fpmts.push_back(0x0);
	}

	// A new hash map to re-order the PMTs. This is needed by
	// the sensitive detector class later in the simulation.
	hash_map<std::string, int, hash<std::string>> newLocHashMap;

	// Grab the tube information from the tubeID Map and dump to file.
	int pmtNo = 0; // Will increment before using so first PMT is 1.
	int vetoNo = 0;
	for (int tubeID = 1; tubeID <= totalNumPMTs; tubeID++)
	{
		G4Transform3D newTransform = tubeIDMap[tubeID];

		// Get tube orientation vector
		G4Vector3D nullOrient = G4Vector3D(0, 0, 1);
		G4Vector3D pmtOrientation = newTransform * nullOrient;
		//cyl_location cylLocation = tubeCylLocation[tubeID];
		std::string pmtName = tubeNameMap[tubeID];

		// Figure out if pmt is on top/bottom or barrel
		// print key: 0-top, 1-barrel, 2-bottom
		if (pmtOrientation.z() == 1) //veto
		{
			cylLocation = 3;
		}
		else if (pmtOrientation.z() < 1.0 && pmtOrientation.z() >= 0.1) //bottom
		{
			cylLocation = 2;
		}
		else if (pmtOrientation.z() >= -1.0 && pmtOrientation.z() <= -0.1) //top
		{
			cylLocation = 0;
		}
		else if ((pmtOrientation.z() < 0.1 && pmtOrientation.z() > -0.1)) //barrel
		{
			cylLocation = 1;
		}
		else 
		{
			cylLocation = 1;
		}

		double tubeNumber;
		if (cylLocation != 3)
		{
			++pmtNo;
			tubeNumber = pmtNo;
		}
		else
		{
			++vetoNo;
			tubeNumber = totalNumPMTs - fVetoPMTs + vetoNo;
		}
		// Fill a new hash map based on the pmt ordering we need.
		newLocHashMap[tubeTagMap[tubeID]] = tubeNumber;

		geoFile.precision(9);
		geoFile << setw(4) << tubeNumber << " " << setw(8) << newTransform.getTranslation().getX() / CLHEP::cm << " "
				<< setw(8) << newTransform.getTranslation().getY() / CLHEP::cm << " " << setw(8)
				<< newTransform.getTranslation().getZ() / CLHEP::cm << " " << setw(7) << pmtOrientation.x() << " " << setw(7)
				<< pmtOrientation.y() << " " << setw(7) << pmtOrientation.z() << " " << setw(3) << cylLocation
				<< G4endl;

		WCSimPmtInfo *new_pmt = new WCSimPmtInfo(cylLocation, newTransform.getTranslation().getX() / CLHEP::cm,
												 newTransform.getTranslation().getY() / CLHEP::cm, newTransform.getTranslation().getZ() / CLHEP::cm,
												 pmtOrientation.x(), pmtOrientation.y(), pmtOrientation.z(), tubeNumber, pmtName);

		fpmts[tubeNumber - 1] = new_pmt;
	}

	tubeLocationMap = newLocHashMap;
	geoFile.close();
}

// Code for traversing the geometry tree.  This code is very general you pass
// it a function and it will call the function with the information on each
// object it finds.
//
// The traversal code comes from a combination of me/G4Lab project &
// from source/visualization/modeling/src/G4PhysicalVolumeModel.cc
//
// If you are trying to understand how passing the function works you need
// to understand pointers to member functions...
//
// Also notice that DescriptionFcnPtr is a (complicated) typedef.
//

void WCSimDetectorConstruction::TraverseReplicas(G4VPhysicalVolume *aPV, int aDepth, const G4Transform3D &aTransform,
												 DescriptionFcnPtr registrationRoutine)
{
	// Recursively visit all of the geometry below the physical volume
	// pointed to by aPV including replicas.

	G4ThreeVector originalTranslation = aPV->GetTranslation();
	G4RotationMatrix *pOriginalRotation = aPV->GetRotation();

	if (aPV->IsReplicated())
	{
		EAxis axis;
		G4int nReplicas;
		G4double width, offset;
		G4bool consuming;

		aPV->GetReplicationData(axis, nReplicas, width, offset, consuming);

		for (int n = 0; n < nReplicas; n++)
		{
			switch (axis)
			{
			default:
			case kXAxis:
				aPV->SetTranslation(G4ThreeVector(-width * (nReplicas - 1) * 0.5 + n * width, 0, 0));
				aPV->SetRotation(0);
				break;
			case kYAxis:
				aPV->SetTranslation(G4ThreeVector(0, -width * (nReplicas - 1) * 0.5 + n * width, 0));
				aPV->SetRotation(0);
				break;
			case kZAxis:
				aPV->SetTranslation(G4ThreeVector(0, 0, -width * (nReplicas - 1) * 0.5 + n * width));
				aPV->SetRotation(0);
				break;
			case kRho:
				//Lib::Out::putL("GeometryVisitor::visit: WARNING:");
				//Lib::Out::putL(" built-in replicated volumes replicated");
				//Lib::Out::putL(" in radius are not yet properly visualizable.");
				aPV->SetTranslation(G4ThreeVector(0, 0, 0));
				aPV->SetRotation(0);
				break;
			case kPhi:
			{
				G4RotationMatrix rotation;
				rotation.rotateZ(-(offset + (n + 0.5) * width));
				// Minus Sign because for the physical volume we need the
				// coordinate system rotation.
				aPV->SetTranslation(G4ThreeVector(0, 0, 0));
				aPV->SetRotation(&rotation);
			}
			break;

			} // axis switch

			DescribeAndDescendGeometry(aPV, aDepth, n, aTransform, registrationRoutine);

		} // num replicas for loop
	}	  // if replicated
	else
		DescribeAndDescendGeometry(aPV, aDepth, aPV->GetCopyNo(), aTransform, registrationRoutine);

	// Restore original transformation...
	aPV->SetTranslation(originalTranslation);
	aPV->SetRotation(pOriginalRotation);
}

void WCSimDetectorConstruction::DescribeAndDescendGeometry(G4VPhysicalVolume *aPV, int aDepth, int replicaNo,
														   const G4Transform3D &aTransform, DescriptionFcnPtr registrationRoutine)
{
	// Calculate the new transform relative to the old transform

	G4Transform3D *transform = new G4Transform3D(*(aPV->GetObjectRotation()), aPV->GetTranslation());

	G4Transform3D newTransform = aTransform * (*transform);
	delete transform;

	// Call the routine we use to print out geometry descriptions, make
	// tables, etc.  The routine was passed here as a paramater.  It needs to
	// be a memeber function of the class

	(this->*registrationRoutine)(aPV, aDepth, replicaNo, newTransform);

	int nDaughters = aPV->GetLogicalVolume()->GetNoDaughters();

	for (int iDaughter = 0; iDaughter < nDaughters; iDaughter++)
		TraverseReplicas(aPV->GetLogicalVolume()->GetDaughter(iDaughter), aDepth + 1, newTransform,
						 registrationRoutine);
}

G4double WCSimDetectorConstruction::GetGeo_Dm(G4int i)
{
	if (i >= 0 && i <= 2)
	{
		return WCCylInfo[i];
	}
	else if (i == 3)
	{
		return innerradius;
	}
	else
	{
		return 0;
	}
}
